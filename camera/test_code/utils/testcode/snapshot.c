/* =============================================================================
*             Texas Instruments OMAP(TM) Platform Software
*  (c) Copyright Texas Instruments, Incorporated.  All Rights Reserved.
*
*  Use of this software is controlled by the terms and conditions found
*  in the license agreement under which this software has been supplied.
* ============================================================================*/

#include <sys/types.h>
#include <sys/stat.h>
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

/* For parameter parser */
#include <getopt.h>

#define DEFAULT_PREVIEW_PIXFMT 		"YUYV"
#define DEFAULT_PREVIEW_WIDTH 		640
#define DEFAULT_PREVIEW_HEIGHT 		480
#define DEFAULT_PREVIEW_FPS 		30

#define DEFAULT_CAPTURE_PIXFMT 		"RAW10"
#define DEFAULT_CAPTURE_WIDTH 		1024
#define DEFAULT_CAPTURE_HEIGHT 		768
#define DEFAULT_CAPTURE_FPS 		10

#define DSS_STREAM_START_FRAME		3

static void usage(void)
{
	printf("Usage:\n");
	printf("\tsnapshot <options>\n");
	printf("\t-d <device-node>\n"
	       "\t\tCamera device node to open (default: /dev/video0)\n");
	printf("\t-p <pixelFormat>\n"
	       "\t\tPixel format to use."
			"(default: " DEFAULT_PREVIEW_PIXFMT ")\n\n"
	       "\t\t\tSupported:\n"
	       "\t\t\t\tYUYV, UYVY, RAW10\n");
	printf("\t-w <width>\n");
	printf("\t\tLCD preview width (default: 640)\n");
	printf("\t-h <height>\n");
	printf("\t\tLCD preview height (default: 480)\n");
	printf("\t-v <device-node>\n"
	       "\t\tVideo device node to open (default: /dev/video1)\n");
}

static void display_keys(void)
{
	printf("Keys:\n");
	printf("  c - Snapshot (Capture to file)\n");
	printf("  2 - Lens FOCUS_RELATIVE -1\n");
	printf("  3 - Lens FOCUS_RELATIVE +1\n");
	printf("  4 - Lens FOCUS_RELATIVE -5\n");
	printf("  5 - Lens FOCUS_RELATIVE +5\n");
	printf("  q - Quit\n");
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

int main(int argc, char **argv)
{
	struct buffers {
		void *start;
		size_t length;
	} *vbuffers, *cbuffers;
	struct v4l2_capability capability;
	struct v4l2_format cfmt, vfmt;
	struct v4l2_requestbuffers creqbuf, vreqbuf;
	struct v4l2_buffer cfilledbuffer, vfilledbuffer;
	struct v4l2_control control;
	int cfd, vfd, i, ret, input;
	int memtype = V4L2_MEMORY_USERPTR;
	int quit_flag = 0, snap_flag = 0;
	int set_video_img = 0;
	char *camdev = NULL;
	char *viddev = NULL;
	int prvw = DEFAULT_PREVIEW_WIDTH, prvh = DEFAULT_PREVIEW_HEIGHT;
	int prvfps = DEFAULT_PREVIEW_FPS;
	char *prvpix = DEFAULT_PREVIEW_PIXFMT;
	int capw = DEFAULT_CAPTURE_WIDTH, caph = DEFAULT_CAPTURE_HEIGHT;
	int capfps = DEFAULT_CAPTURE_FPS;
	char *cappix = DEFAULT_CAPTURE_PIXFMT;
	int c;

	opterr = 0;

	while (1) {
		static struct option long_options[] = {
			{"camdev",	required_argument,	0, 'c'},
			{"pixprv",	required_argument,	0, 'p'},
			{"wprv",	required_argument,	0, 'w'},
			{"hprv",	required_argument,	0, 'h'},
			{"fpsprv",	required_argument,	0, 'f'},
			{"pixcap",	required_argument,	0, 'q'},
			{"wcap",	required_argument,	0, 'x'},
			{"hcap",	required_argument,	0, 'y'},
			{"fpscap",	required_argument,	0, 'g'},
			{"viddev",	required_argument,	0, 'v'},
			{0, 0, 0, 0}
		};
		int option_index = 0;

		c = getopt_long_only(argc, argv, "c:p:w:h:f:q:x:y:g:v:",
				long_options, &option_index);

		/* Detect the end of the options. */
		if (c == -1)
			break;

		switch (c) {
		case 'c':
			camdev = optarg;
			break;
		case 'p':
			prvpix = optarg;
			break;
		case 'w':
			prvw = atoi(optarg);
			break;
		case 'h':
			prvh = atoi(optarg);
			break;
		case 'f':
			prvfps = atoi(optarg);
			break;
		case 'q':
			cappix = optarg;
			break;
		case 'x':
			capw = atoi(optarg);
			break;
		case 'y':
			caph = atoi(optarg);
			break;
		case 'g':
			capfps = atoi(optarg);
			break;
		case 'v':
			viddev = optarg;
			break;
		case '?':
			if ((optopt == 'c') ||
			    (optopt == 'p') ||
			    (optopt == 'w') ||
			    (optopt == 'h') ||
			    (optopt == 'f') ||
			    (optopt == 'q') ||
			    (optopt == 'x') ||
			    (optopt == 'y') ||
			    (optopt == 'g') ||
			    (optopt == 'v'))
				fprintf(stderr,
					"Option -%c requires an argument.\n",
					optopt);
			else if (isprint(optopt))
				fprintf(stderr,
					"Unknown option `-%c'.\n",
					optopt);
			else
				fprintf(stderr,
					"Unknown option character `\\x%x'.\n",
					optopt);
			return 1;
		default:
			abort();
		}
	}

	cfd = open(camdev, O_RDWR);
	if (cfd <= 0) {
		printf("Could not open the cam device %s\n", camdev);
		return -1;
	}

	printf("Openned %s for capturing\n", viddev);

	/********************************************************************/
	/* Video: Open handle to DSS */

	vfd = open(viddev, O_RDWR);
	if (vfd <= 0) {
		printf("Could no open the device %s\n", viddev);
		return -1;
	}

	printf("Openned %s for rendering\n", viddev);

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

	printf("Set Camera frame rate to %ufps...\n", DEFAULT_PREVIEW_FPS);
	ret = setFramerate(cfd, prvfps);
	if (ret < 0) {
		printf("Error setting framerate = %d\n", DEFAULT_PREVIEW_FPS);
		return -1;
	}

	/********************************************************************/
	/* Camera: Set Format & Size */

	printf("Set Camera format & size...\n");

	/* get the current format of the video capture */
	cfmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	ret = ioctl(cfd, VIDIOC_G_FMT, &cfmt);
	if (ret < 0) {
		perror("VIDIOC_G_FMT");
		return -1;
	}

	cfmt.fmt.pix.width = prvw;
	cfmt.fmt.pix.height = prvh;

	if (!strcmp(prvpix, "YUYV"))
		cfmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
	else if (!strcmp(prvpix, "UYVY"))
		cfmt.fmt.pix.pixelformat = V4L2_PIX_FMT_UYVY;
	else if (!strcmp(prvpix, "RAW10"))
		cfmt.fmt.pix.pixelformat = V4L2_PIX_FMT_SGRBG10;
	else {
		printf("unsupported pixel format!\n");
		return -1;
	}

	ret = ioctl(cfd, VIDIOC_S_FMT, &cfmt);
	if (ret < 0) {
		perror("cam VIDIOC_S_FMT");
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
			(cfmt.fmt.pix.height != vfmt.fmt.pix.height)/* ||
			(cfmt.fmt.pix.sizeimage != vfmt.fmt.pix.sizeimage) */||
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

	display_keys();

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

	/* Unmap video buffers */
	for (i = 0; i < vreqbuf.count; i++) {
		if (vbuffers[i].start)
			munmap(vbuffers[i].start, vbuffers[i].length);
	}
	free(vbuffers);

	/* Cleanup structure that holds info about camera buffers */
	free(cbuffers);

	/********************************************************************/
	/* Take snapshot ? */

	if (snap_flag) {
		snapshot(cfd, cappix, capw, caph);
		snap_flag = 0;
		goto restart_streaming;
	}


	close(vfd);
	close(cfd);
}

/********************************************************************/
/********************************************************************/

int snapshot(int cfd, char *pixelFmt, int w, int h, int fps)
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
	char filename[16];
	int file_is_yuv = 0, file_is_raw = 0;

	printf("\nTaking snapshot...\n");

	/********************************************************************/
	/* Set frame rate */

	ret = setFramerate(cfd, fps);
	if (ret < 0) {
		printf("Error setting framerate");
		return -1;
	}


	/********************************************************************/
	/* Set snapshot frame format */

	/* get the current format of the video capture */
	cfmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	ret = ioctl(cfd, VIDIOC_G_FMT, &cfmt);
	if (ret < 0) {
		perror("VIDIOC_G_FMT");
		return -1;
	}

	cfmt.fmt.pix.width = w;
	cfmt.fmt.pix.height = h;

	if (!strcmp(pixelFmt, "YUYV")) {
		cfmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
		file_is_yuv = 1;
	} else if (!strcmp(pixelFmt, "UYVY")) {
		cfmt.fmt.pix.pixelformat = V4L2_PIX_FMT_UYVY;
		file_is_yuv = 1;
	} else if (!strcmp(pixelFmt, "RAW10")) {
		cfmt.fmt.pix.pixelformat = V4L2_PIX_FMT_SGRBG10;
		file_is_raw = 1;
	} else {
		printf("unsupported pixel format!\n");
		return -1;
	}

	ret = ioctl(cfd, VIDIOC_S_FMT, &cfmt);
	if (ret < 0) {
		perror("cam VIDIOC_S_FMT");
		return -1;
	}

	/********************************************************************/
	/* Open image output file */

	sprintf(filename, "snap%04X.%s", snap_count,
		file_is_yuv ? "yuv" : (file_is_raw ? "raw" : "dat"));

	/* Create a file with 644 permissions */
	fd_save = creat(filename,
			O_RDWR | S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
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
