/* =============================================================================
*             Texas Instruments OMAP(TM) Platform Software
*  (c) Copyright Texas Instruments, Incorporated.  All Rights Reserved.
*
*  Use of this software is controlled by the terms and conditions found
*  in the license agreement under which this software has been supplied.
* ============================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <linux/ioctl.h>
#include <sys/mman.h>
#include <linux/videodev2.h>
#include <linux/errno.h>
#include <errno.h>
#include <string.h>
#include <sys/time.h>
#include <mach/isp_user.h>
#include "kbget.h"

#define VIDIOC_S_OMAP2_ROTATION		_IOW('V', 3, int)
#define VIDEO_DEVICE1 "/dev/video1"
#define VIDEO_DEVICE2 "/dev/video2"

#define DEFAULT_PIXEL_FMT 		"YUYV"
#define DEFAULT_VIDEO_SIZE 		"VGA"
#define DEFAULT_VIDEO_FPS 		30
#define DSS_STREAM_START_FRAME	3


static void usage(void)
{
	printf("streaming [camDevice] [pixelFormat] [<sizeW> <sizeH>] [(vid)]"
				"[<SnapsizeW> <SnapsizeH>]\n");
	printf("   To start streaming capture of 1000 frames\n");
	printf("   [camDevice] Camera device to be open\n\t 1:Micron sensor "
				"2:OV3640 3:IMX046\n");
	printf("   [pixelFormat] set the pixelFormat to use. \n\tSupported: "
		"YUYV, UYVY, RGB565, RGB555, RGB565X, RGB555X, RAW10 \n");
	printf("   [sizeW] Set the video width\n");
	printf("   [sizeH] Set the video heigth\n");
	printf("\tOptionally size can be specified using standard name sizes"
							"(VGA,PAL,etc)\n");
	printf("\tIf size is NOT specified QCIF used as default\n");
	printf("   [vid] is the video pipeline to be used. Valid vid is 1 "
							"(default) or 2\n");
	printf("   [SnapsizeW] Set the snapshot width\n");
	printf("   [SnapsizeH] Set the snapshot heigth\n");
}

static void dump_sensor_info(int cfd)
{
	struct omap34xxcam_sensor_info sens_info;
	int ret;

	printf("Getting Sensor Info...\n");
	ret = ioctl(cfd, VIDIOC_PRIVATE_OMAP34XXCAM_SENSOR_INFO,
				&sens_info);
	if (ret < 0) {
		printf("VIDIOC_PRIVATE_OMAP34XXCAM_SENSOR_INFO not supported!");
	} else {
		printf("  Sensor xclk:       %d Hz\n", sens_info.current_xclk);
		printf("  Max Base size:     %d x %d\n",
			sens_info.full_size.width,
			sens_info.full_size.height);
		printf("  Current Base size: %d x %d\n",
			sens_info.active_size.width,
			sens_info.active_size.height);
	}
}

int main(int argc, char *argv[])
{
	struct screen_info_struct {
		int fd;
		char *data;
		int width;
		int height;
	} screen_info ;
	struct buffers {
		void *start;
		size_t length;
	} *vbuffers, *cbuffers;
	void *src_start;
	struct v4l2_capability capability;
	struct v4l2_format cfmt, vfmt;
	struct v4l2_requestbuffers creqbuf, vreqbuf;
	struct v4l2_buffer cfilledbuffer, vfilledbuffer;
	struct v4l2_control control;
	int cfd, vfd, i, ret, input;
	int memtype = V4L2_MEMORY_USERPTR;
	int quit_flag = 0, snap_flag = 0;
	int index = 1, vid = 1, set_video_img = 0;
	int device = 1;
	char *pixelFmt;
	char *strmsizeW = NULL, *strmsizeH = NULL;
	char *snapsizeW, *snapsizeH;


	if ((argc > 1) && (!strcmp(argv[1], "?"))) {
		usage();
		return 0;
	}

	if (argc > index) {
		device = atoi(argv[index]);
		index++;
	}

	cfd = open_cam_device(O_RDWR, device);
	if (cfd <= 0) {
		printf("Could not open the cam device\n");
		return -1;
	}


	/********************************************************************/
	/* Parse video size */

	if (argc > index) {
		pixelFmt = argv[index];
		index++;
		if (argc > index) {
			ret = validateSize(argv[index]);
			if (ret == 0) {
				strmsizeW = argv[index];
			} else {
				index++;
				if (argc > (index)) {
					strmsizeW = argv[index-1];
					strmsizeH = argv[index];
				} else {
					printf("Invalid size\n");
					usage();
					return -1;
				}
			}
			index++;
		} else {
			printf("Setting QCIF as video size, default value\n");
			strmsizeW = DEFAULT_VIDEO_SIZE;
			index++;
		}
	} else {
		printf("Setting pixel format and video size with default "
					"values\n");
		pixelFmt = DEFAULT_PIXEL_FMT;
		strmsizeW = DEFAULT_VIDEO_SIZE;
		if (ret < 0)
			return -1;
	}


	if (argc > index) {
		vid = atoi(argv[index]);
		if ((vid != 1) && (vid != 2)) {
				printf("vid has to be 1 or 2!\n ");
				return -1;
		}
	}


	/********************************************************************/
	/* Parse Snapshot size */

	index += 2;
	if (argc > index) {
		snapsizeW = argv[index-1];
		snapsizeH = argv[index];
	} else {
		printf("Invalid size\n");
		usage();
		return -1;
	}


	/********************************************************************/
	/* Video: Open handle to DSS */

	vfd = open((vid == 1) ? VIDEO_DEVICE1 : VIDEO_DEVICE2, O_RDWR);
	if (vfd <= 0) {
		printf("Could no open the device %s\n",
			(vid == 1) ? VIDEO_DEVICE1 : VIDEO_DEVICE2);
		vid = 0;
	} else
		printf("openned %s for rendering\n",
			(vid == 1) ? VIDEO_DEVICE1 : VIDEO_DEVICE2);

	/********************************************************************/
	/* Video: Query Capability */

	if (ioctl(vfd, VIDIOC_QUERYCAP, &capability) == -1) {
		perror("dss VIDIOC_QUERYCAP");
		return -1;
	}
	if (capability.capabilities & V4L2_CAP_STREAMING)
		printf("The video driver is capable of Streaming!\n");
	else {
		printf("The video driver is not capable of "
							"Streaming!\n");
		return -1;
	}


restart_streaming:
	printf("Restart streaming...\n");

	/********************************************************************/
	/* Camera: Set Frame rate to 30fps */

	printf("Set Camera frame rate to %ufps...\n", DEFAULT_VIDEO_FPS);
	ret = setFramerate(cfd, DEFAULT_VIDEO_FPS);
	if (ret < 0) {
		printf("Error setting framerate = %d\n", DEFAULT_VIDEO_FPS);
		return -1;
	}

	/********************************************************************/
	/* Camera: Set Format & Size */

	printf("Set Camera format & size...\n");
	if (strmsizeH == NULL)
		ret = cam_ioctl(cfd, pixelFmt, strmsizeW);
	else
		ret = cam_ioctl(cfd, pixelFmt, strmsizeW, strmsizeH);
	if (ret < 0) {
		usage();
		return -1;
	}


	/********************************************************************/
	/* Camera: Query Capability */

	printf("Query Camera Capabilities...\n");
	if (ioctl(cfd, VIDIOC_QUERYCAP, &capability) < 0) {
		perror("cam VIDIOC_QUERYCAP");
		return -1;
	}
	if (capability.capabilities & V4L2_CAP_STREAMING)
		printf("The Camera driver is capable of Streaming!\n");
	else {
		printf("The Camera driver is not capable of Streaming!\n");
		return -1;
	}

	/********************************************************************/
	/* Camera: Get Pixel Format */

	cfmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	ret = ioctl(cfd, VIDIOC_G_FMT, &cfmt);
	if (ret < 0) {
		perror("cam VIDIOC_G_FMT");
		return -1;
	}
	printf("Camera Image width = %d, Image height = %d, size = %d\n",
			cfmt.fmt.pix.width,
			cfmt.fmt.pix.height,
			cfmt.fmt.pix.sizeimage);

	/********************************************************************/
	/* Video: Get Pixel Format */

	vfmt.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
	ret = ioctl(vfd, VIDIOC_G_FMT, &vfmt);
	if (ret < 0) {
		perror("dss VIDIOC_G_FMT");
		return -1;
	}
	printf("Video Image width = %d, Image height = %d, size = %d\n",
			vfmt.fmt.pix.width, vfmt.fmt.pix.height,
			vfmt.fmt.pix.sizeimage);

	/********************************************************************/
	/* Check if size and pixel formats match */

	if ((cfmt.fmt.pix.width != vfmt.fmt.pix.width) ||
		(cfmt.fmt.pix.height != vfmt.fmt.pix.height) ||
		(cfmt.fmt.pix.sizeimage !=
				vfmt.fmt.pix.sizeimage)) {
		printf("image sizes don't match!\n");
		set_video_img = 1;
	}
	if (cfmt.fmt.pix.pixelformat !=
				vfmt.fmt.pix.pixelformat) {
		printf("pixel formats don't match!\n");
		set_video_img = 1;
	}

	if (set_video_img) {
		printf("set video image the same as camera image...\n");
		vfmt.fmt.pix.width = cfmt.fmt.pix.width;
		vfmt.fmt.pix.height = cfmt.fmt.pix.height;
		vfmt.fmt.pix.sizeimage = cfmt.fmt.pix.sizeimage;
		vfmt.fmt.pix.pixelformat = cfmt.fmt.pix.pixelformat;
		ret = ioctl(vfd, VIDIOC_S_FMT, &vfmt);

		if (ret < 0) {
			perror("dss VIDIOC_S_FMT");
			return -1;
		}

		printf("New Image & Video sizes, after "
		"equaling:\nCamera Image width = %d, Image "
		"height = %d, size = %d\n",
		cfmt.fmt.pix.width, cfmt.fmt.pix.height,
		cfmt.fmt.pix.sizeimage);
		printf("Video Image width = %d, Image "
				"height = %d, size = %d\n",
				vfmt.fmt.pix.width,
				vfmt.fmt.pix.height,
				vfmt.fmt.pix.sizeimage);

		if ((cfmt.fmt.pix.width != vfmt.fmt.pix.width) ||
			(cfmt.fmt.pix.height != vfmt.fmt.pix.height) ||
			(cfmt.fmt.pix.sizeimage != vfmt.fmt.pix.sizeimage) ||
			(cfmt.fmt.pix.pixelformat !=
				vfmt.fmt.pix.pixelformat)) {
			printf("can't make camera and video image "
				"compatible!\n");
			return 0;
		}
	}

	/********************************************************************/
	/* Video: Request number of available buffers */

	vreqbuf.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
	vreqbuf.memory = V4L2_MEMORY_MMAP;
	vreqbuf.count = 4;
	if (ioctl(vfd, VIDIOC_REQBUFS, &vreqbuf) == -1) {
		perror("dss VIDEO_REQBUFS");
		return;
	}
	printf("Video Driver allocated %d buffers when 4 are "
			"requested\n", vreqbuf.count);

	/********************************************************************/
	/* Video: Query buffer & mmap buffer address */

	vbuffers = calloc(vreqbuf.count, sizeof(*vbuffers));
	for (i = 0; i < vreqbuf.count; ++i) {
		struct v4l2_buffer buffer;
		buffer.type = vreqbuf.type;
		buffer.index = i;
		if (ioctl(vfd, VIDIOC_QUERYBUF, &buffer) == -1) {
			perror("dss VIDIOC_QUERYBUF");
			return;
		}
		vbuffers[i].length = buffer.length;
		vbuffers[i].start = mmap(NULL, buffer.length, PROT_READ
					| PROT_WRITE, MAP_SHARED,
					 vfd,
					buffer.m.offset);
		if (vbuffers[i].start == MAP_FAILED) {
			perror("dss mmap");
			return;
		}
		printf("Video Buffers[%d].start = %x  length = %d\n",
			i, vbuffers[i].start, vbuffers[i].length);
	}

	/********************************************************************/
	/* Camera: Request number of available buffers */

	creqbuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	creqbuf.memory = memtype;
	creqbuf.count = 4;
	printf("Requesting %d buffers of type %s\n", creqbuf.count,
		(memtype == V4L2_MEMORY_USERPTR) ? "V4L2_MEMORY_USERPTR" :
							"V4L2_MEMORY_MMAP");
	if (ioctl(cfd, VIDIOC_REQBUFS, &creqbuf) < 0) {
		perror("cam VIDEO_REQBUFS");
		return -1;
	}
	printf("Camera Driver allowed buffers reqbuf.count = %d\n",
		creqbuf.count);

	/********************************************************************/
	/* Camera: Query for each buffer & mmap buffer addresses */

	cbuffers = calloc(creqbuf.count, sizeof(*cbuffers));

	for (i = 0; i < creqbuf.count; ++i) {
		struct v4l2_buffer buffer;
		buffer.type = creqbuf.type;
		buffer.memory = creqbuf.memory;
		buffer.index = i;
		if (ioctl(cfd, VIDIOC_QUERYBUF, &buffer) < 0) {
			perror("cam VIDIOC_QUERYBUF");
			return -1;
		}
		if (memtype == V4L2_MEMORY_USERPTR) {
			buffer.flags = 0;
			buffer.m.userptr = (unsigned int)vbuffers[i].start;
			buffer.length = vbuffers[i].length;
		} else {
			cbuffers[i].length = buffer.length;
			cbuffers[i].start = vbuffers[i].start;
			printf("Mapped Buffers[%d].start = %x  length = %d\n",
				i, cbuffers[i].start, cbuffers[i].length);

			buffer.m.userptr = (unsigned int)cbuffers[i].start;
			buffer.length = cbuffers[i].length;
		}

		if (ioctl(cfd, VIDIOC_QBUF, &buffer) < 0) {
			perror("cam VIDIOC_QBUF");
			return -1;
		}
	}

	dump_sensor_info(cfd);

	printf("Streaming %d x %d...\n",
			cfmt.fmt.pix.width,
			cfmt.fmt.pix.height);

	/********************************************************************/
	/* Start Camera streaming */

	if (ioctl(cfd, VIDIOC_STREAMON, &creqbuf.type) < 0) {
		perror("cam VIDIOC_STREAMON");
		return -1;
	}

	/********************************************************************/
	/* Start streaming loop */

	cfilledbuffer.type = creqbuf.type;
	vfilledbuffer.type = vreqbuf.type;
	i = 0;
	while (1) {
		/* De-queue the next avaliable buffer */
		while (ioctl(cfd, VIDIOC_DQBUF, &cfilledbuffer) < 0)
			perror("cam VIDIOC_DQBUF");

		vfilledbuffer.index = cfilledbuffer.index;
		vfilledbuffer.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
		vfilledbuffer.memory = V4L2_MEMORY_MMAP;
		vfilledbuffer.m.userptr =
			(unsigned int)(vbuffers[cfilledbuffer.index].start);
		vfilledbuffer.length = cfilledbuffer.length;
		if (ioctl(vfd, VIDIOC_QBUF, &vfilledbuffer) < 0) {
			perror("dss VIDIOC_QBUF");
			return -1;
		}
		i++;

		if (i == DSS_STREAM_START_FRAME) {
			/* Turn on streaming for video */
			if (ioctl(vfd, VIDIOC_STREAMON, &vreqbuf.type)) {
				perror("dss VIDIOC_STREAMON");
				return -1;
			}
		}

		if (i >= DSS_STREAM_START_FRAME) {
			/* De-queue the previous buffer from video driver */
			if (ioctl(vfd, VIDIOC_DQBUF, &vfilledbuffer)) {
				perror("dss VIDIOC_DQBUF");
				return;
			}
		}

		if (kbhit()) {
			input = getch();
			if (input == '2') {
				control.id = V4L2_CID_FOCUS_RELATIVE;
				control.value = -1;
				if (ioctl(cfd, VIDIOC_S_CTRL, &control) == -1)
					perror("Error CID_FOCUS_RELATIVE: ");
				printf("Lens position: -1\n");
			} else if (input == '3') {
				control.id = V4L2_CID_FOCUS_RELATIVE;
				control.value = 1;
				if (ioctl(cfd, VIDIOC_S_CTRL, &control) == -1)
					perror("Error CID_FOCUS_RELATIVE: ");
				printf("Lens position: +1\n");
			} else if (input == '4') {
				control.id = V4L2_CID_FOCUS_RELATIVE;
				control.value = -5;
				if (ioctl(cfd, VIDIOC_S_CTRL, &control) == -1)
					perror("Error CID_FOCUS_RELATIVE: ");
				printf("Lens position: -5\n");
			} else if (input == '5') {
				control.id = V4L2_CID_FOCUS_RELATIVE;
				control.value = 5;
				if (ioctl(cfd, VIDIOC_S_CTRL, &control) == -1)
					perror("Error CID_FOCUS_RELATIVE: ");
				printf("Lens position: +5\n");
			} else if (input == 'c') {
				snap_flag = 1;
			} else if (input == 'q') {
				quit_flag = 1;
			}
		}

		if (quit_flag | snap_flag) {
			printf("Cancelling the streaming capture...\n");

			creqbuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
			if (ioctl(cfd, VIDIOC_STREAMOFF, &creqbuf.type) < 0) {
				perror("cam VIDIOC_STREAMOFF");
				return -1;
			}

			if (ioctl(vfd, VIDIOC_STREAMOFF, &vreqbuf.type) == -1) {
				perror("dss VIDIOC_STREAMOFF");
				return -1;
			}

			break;
		}

		if (i >= DSS_STREAM_START_FRAME) {
			cfilledbuffer.index = vfilledbuffer.index;
			while (ioctl(cfd, VIDIOC_QBUF, &cfilledbuffer) < 0)
				perror("cam VIDIOC_QBUF");
		}
	}

	/********************************************************************/
	/* Cleanup */

	/* Dequeue camera buffers */
	for (i = 0; i < creqbuf.count; ++i) {
		struct v4l2_buffer buffer;
		buffer.type = creqbuf.type;
		buffer.memory = creqbuf.memory;
		buffer.index = i;
		if (ioctl(cfd, VIDIOC_QUERYBUF, &buffer) < 0) {
			perror("cam VIDIOC_QUERYBUF");
			return -1;
		}

		buffer.flags = 0;
		buffer.m.userptr = (unsigned int)vbuffers[i].start;
		buffer.length = vbuffers[i].length;
		printf("Camera DQBUF %i\n", i);
		if (ioctl(cfd, VIDIOC_DQBUF, &buffer) < 0)
			perror("cam VIDIOC_DQBUF");
	}


	/* Unmap video buffers */
	for (i = 0; i < vreqbuf.count; i++) {
		if (vbuffers[i].start)
			munmap(vbuffers[i].start, vbuffers[i].length);
	}
	free(vbuffers);


	for (i = 0; i < creqbuf.count; i++) {
		if (cbuffers[i].start) {
			if (memtype == V4L2_MEMORY_USERPTR)
				free(cbuffers[i].start);
			else
				munmap(cbuffers[i].start, cbuffers[i].length);
		}
	}
	free(cbuffers);

	/********************************************************************/
	/* Take snapshot ? */

	if (snap_flag) {
		snapshot(cfd, pixelFmt, snapsizeW, snapsizeH);
		snap_flag = 0;
		goto restart_streaming;
	}


	close(vfd);
	close(cfd);
}

/********************************************************************/
/********************************************************************/

int snapshot(int cfd, char *pixelFmt, char *w, char *h)
{
	struct {
		void *start;
		void *start_aligned;
		size_t length;
	} *cbuffers;

	static unsigned int snap_count = 1;
	struct v4l2_capability capability;
	struct v4l2_format cfmt;
	struct v4l2_requestbuffers creqbuf;
	struct v4l2_buffer cfilledbuffer;
	int i, ret, count = 1, memtype = V4L2_MEMORY_USERPTR;
	int fd_save = 0;
	int index = 1;
	char filename[16];

	printf("\nTaking snapshot...\n");

	/********************************************************************/
	/* Set frame rate */

	ret = setFramerate(cfd, 10);
	if (ret < 0) {
		printf("Error setting framerate");
		return -1;
	}


	/********************************************************************/
	/* Set snapshot frame format */

	ret = cam_ioctl(cfd, pixelFmt, w, h);
	if (ret < 0) {
		printf("Error setting snapshot frame format");
		return -1;
	}


	/********************************************************************/
	/* Open image output file */

	sprintf(filename, "snap%04X.yuv", snap_count);

	fd_save = creat(filename, O_RDWR);
	if (fd_save <= 0) {
		printf("Can't create file %s\n", filename);
		return -1;
	} else {
		printf("The captured frames will be saved into: %s\n",
			filename);
	}

	/********************************************************************/
	/* Camera: Get format */

	cfmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	ret = ioctl(cfd, VIDIOC_G_FMT, &cfmt);
	if (ret < 0) {
		perror("VIDIOC_G_FMT");
		return -1;
	}
	printf("Camera Image width = %d, Image height = %d, size = %d\n",
		 cfmt.fmt.pix.width, cfmt.fmt.pix.height,
		 cfmt.fmt.pix.sizeimage);

	/********************************************************************/
	/* Camera: Request pointers to buffers */

	creqbuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	creqbuf.memory = memtype;
	creqbuf.count = count;

	printf("Requesting %d buffers of type %s\n", creqbuf.count,
		(memtype == V4L2_MEMORY_USERPTR) ? "V4L2_MEMORY_USERPTR" :
						"V4L2_MEMORY_MMAP");
	if (ioctl(cfd, VIDIOC_REQBUFS, &creqbuf) < 0) {
		perror("VIDEO_REQBUFS");
		return -1;
	}
	printf("Camera Driver allowed buffers reqbuf.count = %d\n",
		creqbuf.count);
	if (creqbuf.count != count)
		count = creqbuf.count;

	/********************************************************************/
	/* Camera: Allocate user memory, and queue each buffer */

	cbuffers = calloc(creqbuf.count, sizeof(*cbuffers));

	for (i = 0; i < creqbuf.count; ++i) {
		struct v4l2_buffer buffer;
		buffer.type = creqbuf.type;
		buffer.memory = creqbuf.memory;
		buffer.index = i;
		if (ioctl(cfd, VIDIOC_QUERYBUF, &buffer) < 0) {
			perror("VIDIOC_QUERYBUF");
			return -1;
		}

		/* V4L2_MEMORY_USERPTR */
		cbuffers[i].length = buffer.length;
		if (cbuffers[i].length & 0xfff) {
			cbuffers[i].length =
				(cbuffers[i].length & 0xfffff000) + 0x1000;
		}
		cbuffers[i].start = malloc(cbuffers[i].length);
		cbuffers[i].start_aligned = cbuffers[i].start;
		if ((unsigned int)cbuffers[i].start_aligned & 0xfff) {
			cbuffers[i].start_aligned =
			(void *)((unsigned int)cbuffers[i].start_aligned +
			(0x1000 -
			((unsigned int)cbuffers[i].start_aligned & 0xfff)));
		}
		buffer.length = cbuffers[i].length;
		buffer.m.userptr = (unsigned int)cbuffers[i].start_aligned;
		printf("User Buffer [%d].start = %x  length = %d\n",
			 i, cbuffers[i].start, cbuffers[i].length);

		 if (ioctl(cfd, VIDIOC_QBUF, &buffer) < 0) {
			perror("CAMERA VIDIOC_QBUF");
			return -1;
		}
	}

	cfilledbuffer.type = creqbuf.type;
	cfilledbuffer.memory = memtype;

	dump_sensor_info(cfd);

	/********************************************************************/
	/* Camera: turn on streaming  */

	if (ioctl(cfd, VIDIOC_STREAMON, &creqbuf.type) < 0) {
		perror("VIDIOC_STREAMON");
		return -1;
	}

	/********************************************************************/
	/* Camera: Queue/Dequeue one time  */

	i = 0;
	while (1) {
		/* De-queue the next avaliable buffer */
		while (ioctl(cfd, VIDIOC_DQBUF, &cfilledbuffer) < 0) {
			perror("VIDIOC_DQBUF");
			printf(" ERROR HAS OCCURED\n");
		}

		i++;

		if (i == count) {
			printf("Cancelling the streaming capture...\n");
			creqbuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
			if (ioctl(cfd, VIDIOC_STREAMOFF, &creqbuf.type) < 0) {
				perror("VIDIOC_STREAMOFF");
				return -1;
			}
			printf("Done\n");
			break;
		}

		while (ioctl(cfd, VIDIOC_QBUF, &cfilledbuffer) < 0)
			perror("CAM VIDIOC_QBUF");
	}

	/********************************************************************/
	/* Save buffer to file  */

	printf("Captured %d frames!\n", i);
	printf("Start writing to file\n");
	for (i = 0; i < count; i++)
		write(fd_save, cbuffers[i].start_aligned,
			cfmt.fmt.pix.width * cfmt.fmt.pix.height * 2);
	printf("Completed writing to file\n");
	close(fd_save);

	/********************************************************************/
	/* Camera: Cleanup  */

	for (i = 0; i < creqbuf.count; i++) {
		if (cbuffers[i].start) {
			/* V4L2_MEMORY_USERPTR */
			free(cbuffers[i].start);
		}
	}

	free(cbuffers);
	snap_count++;

	return 1;
}
