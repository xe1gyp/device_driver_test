/* ========================================================================
*             Texas Instruments OMAP(TM) Platform Software
*  (c) Copyright Texas Instruments, Incorporated.  All Rights Reserved.
*
*  Use of this software is controlled by the terms and conditions found
*  in the license agreement under which this software has been supplied.
* ========================================================================= */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <linux/videodev2.h>
#include <string.h>
#include <mach/isp_user.h>
#include "absLayer.h"

#define VIDEO_DEVICE1 "/dev/video1"
#define VIDEO_DEVICE2 "/dev/video2"

#define DEFAULT_PIXEL_FMT "YUYV"
#define DEFAULT_VIDEO_SIZE "QCIF"

static void usage(void)
{
	printf("streaming_abs [camDevice] [absLayerGroup] [absLayerOption] "
			"[absLayerMode] [pixelFormat] [<sizeW> <sizeH>]"
			" [(vid)] [<count>] [<file>]\n");
	printf("   To start streaming capture of 1000 frames\n");
	printf("   [camDevice] Camera device to be open\n\t 1:Micron sensor "
					"2:OV sensor\n");
	printf("   [absLayerGroup] CCDC or PREVIEW\n");
	printf("   [absLayerOption] Abstraction Layer option to be updated\n");
	printf("   [absLayerMode] 1: Before streamon(Default)\n");
	printf("   		  2: After streamon\n");
	printf("   [pixelFormat] set the pixelFormat to use. \n\tSupported: "
		"YUYV, UYVY, RGB565, RGB555, RGB565X, RGB555X, SGRBG10,"
		" SRGGB10, SBGGR10, SGBRG10 \n");
	printf("   [sizeW] Set the video width\n");
	printf("   [sizeH] Set the video heigth\n");
	printf("\tOptionally size can be specified using standard name sizes"
						"(VGA,PAL,etc)\n");
	printf("\tIf size is NOT specified QCIF used as default\n");
	printf("   [vid] is the video pipeline to be used. Valid vid is 1 "
						"(default) or 2\n");
	printf("   [count] amount of frames to be displayed/saved\n");
	printf("   [file] Optionally the captured image can be saved to file "
						"<file>\n");
}

int main(int argc, char *argv[])
{
	struct {
		void *start;
		size_t length;
	} *vbuffers, *cbuffers;
	struct v4l2_capability capability;
	struct v4l2_format cformat, vformat;
	struct v4l2_requestbuffers creqbuf, vreqbuf;
	struct v4l2_buffer cfilledbuffer, vfilledbuffer;
	int vfd, cfd;
	int i, ret, count = -1, memtype = V4L2_MEMORY_USERPTR;
	int index = 1, vid = 1, set_video_img = 0;
	int device = 1;
	char *pixelFmt;
	char *absGroup, *absOption;
	int absMode = 1;
	int framerate = 30;

	if ((argc > 1) && (!strcmp(argv[1], "?"))) {
		usage();
		return 0;
	}

	if (argc > index) {
		device = atoi(argv[index]);
		index++;
	}

	if (argc > index) {
		absGroup = argv[index];
		ret = absLayerValidateGroup(absGroup);
		if (ret <= 0) {
			printf("Wrong Abs Layer Group\n");
			usage();
			return 0;
		}
		index++;
	}

	if (argc > index) {
		absOption = argv[index];
		index++;
	}

	if (argc > index) {
		absMode = atoi(argv[index]);
		if (absMode < 0 || absMode > 2) {
			printf("Wrong Abstraction Update Mode\n");
			usage();
			return 0;
		}
		index++;
	}

	cfd = open_cam_device(O_RDWR, device);
	if (cfd <= 0) {
		printf("Could not open the cam device\n");
		return -1;
	}

	framerate = 30;

	if (argc > index) {
		pixelFmt = argv[index];
		index++;
		if (argc > index) {
			ret = validateSize(argv[index]);
			if (ret == 0) {
				ret = cam_ioctl(cfd, pixelFmt, argv[index]);
				if (ret < 0) {
					usage();
					return -1;
				}
			} else {
				index++;
				if (argc > (index)) {
					ret = cam_ioctl(cfd, pixelFmt,
						argv[index-1], argv[index]);
					if (ret < 0) {
						usage();
						return -1;
					}
				} else {
					printf("Invalid size\n");
					usage();
					return -1;
				}
			}
			index++;
		} else {
			printf("Setting QCIF as video size, default value\n");
			ret = cam_ioctl(cfd, pixelFmt, DEFAULT_VIDEO_SIZE);
			if (ret < 0)
				return -1;
		}
	} else {
		printf("Setting pixel format and video size with default "
						"values\n");
		ret = cam_ioctl(cfd, DEFAULT_PIXEL_FMT, DEFAULT_VIDEO_SIZE);
		if (ret < 0)
			return -1;
	}
	ret = setFramerate(cfd, framerate);
	if (ret < 0) {
		printf("Error setting framerate = %d\n", framerate);
		return -1;
	}

	if (argc > index) {
		vid = atoi(argv[index]);
		if ((vid != 1) && (vid != 2)) {
			printf("vid has to be 1 or 2!\n ");
			return 0;
		}
		index++;
	}

	if (argc > index)
		count = atoi(argv[index]);
	printf("Frames: %d\n", count);
	index++;

	if (count >= 1000 || count <= 0)
		count = -1;

	vfd = open((vid == 1) ? VIDEO_DEVICE1 : VIDEO_DEVICE2, O_RDWR);
	if (vfd <= 0) {
		printf("Could no open the device %s\n",
			(vid == 1) ? VIDEO_DEVICE1 : VIDEO_DEVICE2);
		vid = 0;
	} else
		printf("openned %s for rendering\n",
			(vid == 1) ? VIDEO_DEVICE1 : VIDEO_DEVICE2);

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

	if (ioctl(cfd, VIDIOC_QUERYCAP, &capability) < 0) {
		perror("cam VIDIOC_QUERYCAP");
		return -1;
	}
	if (capability.capabilities & V4L2_CAP_STREAMING)
		printf("The driver is capable of Streaming!\n");
	else {
		printf("The driver is not capable of Streaming!\n");
		return -1;
	}

	cformat.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	ret = ioctl(cfd, VIDIOC_G_FMT, &cformat);
	if (ret < 0) {
		perror("cam VIDIOC_G_FMT");
		return -1;
	}
	printf("Camera Image width = %d, Image height = %d, size = %d\n",
			cformat.fmt.pix.width, cformat.fmt.pix.height,
					cformat.fmt.pix.sizeimage);

	vformat.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
	ret = ioctl(vfd, VIDIOC_G_FMT, &vformat);
	if (ret < 0) {
		perror("dss VIDIOC_G_FMT");
		return -1;
	}
	printf("Video Image width = %d, Image height = %d, size = %d\n",
		vformat.fmt.pix.width, vformat.fmt.pix.height,
		vformat.fmt.pix.sizeimage);

	if ((cformat.fmt.pix.width != vformat.fmt.pix.width) ||
	    (cformat.fmt.pix.height != vformat.fmt.pix.height)) {
		printf("image sizes don't match!\n");
		set_video_img = 1;
	}
	if (cformat.fmt.pix.pixelformat !=
	    vformat.fmt.pix.pixelformat) {
		printf("pixel formats don't match!\n");
		set_video_img = 1;
	}

	if (set_video_img) {
		printf("set video image the same as camera image ..\n");
		vformat.fmt.pix.width = cformat.fmt.pix.width;
		vformat.fmt.pix.height = cformat.fmt.pix.height;
		vformat.fmt.pix.sizeimage = cformat.fmt.pix.sizeimage;
		vformat.fmt.pix.pixelformat =
					cformat.fmt.pix.pixelformat;
		ret = ioctl(vfd, VIDIOC_S_FMT, &vformat);
		if (ret < 0) {
			perror("dss VIDIOC_S_FMT");
			return -1;
		}
		printf("New Image & Video sizes, after "
			"equaling:\nCamera Image width = %d, "
			"Image height = %d, size = %d\n",
			cformat.fmt.pix.width, cformat.fmt.pix.height,
			cformat.fmt.pix.sizeimage);
		printf("Video Image width = %d, Image height "
			"= %d, size = %d\n",
			vformat.fmt.pix.width, vformat.fmt.pix.height,
			vformat.fmt.pix.sizeimage);

		if ((cformat.fmt.pix.width != vformat.fmt.pix.width) ||
		    (cformat.fmt.pix.height !=
		     vformat.fmt.pix.height) ||
		    (cformat.fmt.pix.pixelformat !=
		     vformat.fmt.pix.pixelformat)) {
			printf("can't make camera and video image "
				"compatible!\n");
			return 0;
		}
	}

	vreqbuf.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
	vreqbuf.memory = V4L2_MEMORY_MMAP;
	vreqbuf.count = 4;
	if (ioctl(vfd, VIDIOC_REQBUFS, &vreqbuf) == -1) {
		perror("dss VIDEO_REQBUFS");
		return;
	}
	printf("Video Driver allocated %d buffers when 4 are "
			"requested\n", vreqbuf.count);

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
		vbuffers[i].start = mmap(NULL, buffer.length,
					 PROT_READ | PROT_WRITE,
					 MAP_SHARED,
					 vfd,
					 buffer.m.offset);
		if (vbuffers[i].start == MAP_FAILED) {
			perror("dss mmap");
			return;
		}
		printf("Video Buffers[%d].start = %x  length = %d\n",
			i, vbuffers[i].start, vbuffers[i].length);

	}

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


	cbuffers = calloc(creqbuf.count, sizeof(*cbuffers));
	/* mmap driver memory or allocate user memory, and queue each buffer */
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

	if (absMode == 1) {
		printf("Update Abs Layer Before Streamon\n");
		if (!strcasecmp(absGroup, "CCDC")) {
			ret = updateCcdc(cfd, absOption);
			printf("Still working...\n");
		} else if (!strcasecmp(absGroup, "PREVIEW"))
			ret = updatePreview(cfd, absOption);
		if (ret <= 0) {
			printf("Error updating abstraction layer values\n");
			goto exit1;
		}
	}

	/* turn on streaming */
	if (ioctl(cfd, VIDIOC_STREAMON, &creqbuf.type) < 0) {
		perror("cam VIDIOC_STREAMON");
		return -1;
	}

	if (absMode == 2) {
		printf("Update Abs Layer After Streamon\n");
		if (!strcasecmp(absGroup, "CCDC"))
			ret = updateCcdc(cfd, absOption);
		else if (!strcasecmp(absGroup, "PREVIEW"))
			ret = updatePreview(cfd, absOption);
		if (ret <= 0) {
			printf("Error updating abstraction layer values\n");
			goto exit2;
		}
	}

	/* caputure 1000 frames or when we hit the passed nmuber of frames */
	cfilledbuffer.type = creqbuf.type;
	vfilledbuffer.type = vreqbuf.type;
	i = 0;
	while (i < 1000) {
#ifdef DEBUG
		sleep(1);
#endif
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

		if (i == 3) {
			/* Turn on streaming for video */
			if (ioctl(vfd, VIDIOC_STREAMON, &vreqbuf.type)) {
				perror("dss VIDIOC_STREAMON");
				return -1;
			}
		}

		if (i >= 3) {
			/* De-queue the previous buffer from video driver */
			if (ioctl(vfd, VIDIOC_DQBUF, &vfilledbuffer)) {
				perror("dss VIDIOC_DQBUF");
				return;
			}
		}

		if (i == count) {
			printf("Cancelling the streaming capture...\n");
			creqbuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
			if (ioctl(cfd, VIDIOC_STREAMOFF, &creqbuf.type) < 0) {
				perror("cam VIDIOC_STREAMOFF");
				return -1;
			}
			if (ioctl(vfd, VIDIOC_STREAMOFF,
				  &vreqbuf.type) == -1) {
				perror("dss VIDIOC_STREAMOFF");
				return -1;
			}

			printf("Done\n");
			break;
		}

		if (i >= 3) {
			cfilledbuffer.index = vfilledbuffer.index;
			while (ioctl(cfd, VIDIOC_QBUF, &cfilledbuffer) < 0)
				perror("cam VIDIOC_QBUF");
		}
	}
	printf("Captured %d frames!\n", i);

	/* we didn't trun off streaming yet */
exit2:
	if (strcasecmp(absGroup, "CCDC") == 0)
		resetCcdc(cfd, absOption);

	if (strcasecmp(absGroup, "PREVIEW") == 0)
		resetPreview(cfd, absOption);

	if (count == -1) {
		creqbuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		if (ioctl(cfd, VIDIOC_STREAMOFF, &creqbuf.type) == -1) {
			perror("cam VIDIOC_STREAMOFF");
			return -1;
		}
		if (ioctl(vfd, VIDIOC_STREAMOFF, &vreqbuf.type) == -1) {
			perror("dss VIDIOC_STREAMOFF");
			return -1;
		}
	}

exit1:
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

	close(vfd);
	close(cfd);
}
