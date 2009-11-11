/* =========================================================================
*             Texas Instruments OMAP(TM) Platform Software
*  (c) Copyright Texas Instruments, Incorporated.  All Rights Reserved.
*
*  Use of this software is controlled by the terms and conditions found
*  in the license agreement under which this software has been supplied.
* ========================================================================== */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <linux/videodev2.h>
#include <sys/mman.h>
#include <string.h>
#include "mach/isp_user.h"

#define VIDEO_DEVICE1 "/dev/video1"
#define VIDEO_DEVICE2 "/dev/video2"

#define DEFAULT_PIXEL_FMT "YUYV"
#define DEFAULT_VIDEO_SIZE "QCIF"
#define DEFAULT_FILE_NAME "output.yuv"
#define DEFAULT_FRAMERATE 15

/* have to align at 32 bytes */
static void usage(void)
{
	printf("Burst Mode Test Case\n");
	printf("Usage: burst_mode [camDevice] [pixelFormat] [<sizeW> <sizeH>]"
			" [<numberOfFrames>] [<file>] [<colorEffect>]\n");
	printf("   [camDevice] Camera device to be open\n\t 1:Micron sensor "
					"2:OV sensor\n");
	printf("   [pixelFormat] set the pixelFormat to use. \n\tSupported:"
					" YUYV, UYVY, RGB565, RGB555, RGB565X,"
							" RGB555X, RAW10 \n");
	printf("   [sizeW] Set the video width\n");
	printf("   [sizeH] Set the video heigth\n");
	printf("\tOptionally size can be specified using standard name sizes"
							"(VGA,PAL,etc)\n");
	printf("\tIf size is NOT specified QCIF used as default\n");
	printf("   [numberOfFrames] Number of Frames to be captured\n");
	printf("   [file] Optionally captured image can be saved to file "
								"<file>\n");
	printf("    If no file is specified output.yuv file is the default\n");
	printf("   [framerate] is the framerate to be used, if no value"
			" is given \n\t      30 fps is default\n");
	printf("   [colorEffect] COLOR The image captured with "
						"no color effect\n");

	printf("		 BW	 The image captured with "
						"Black & White effect\n");
	printf("                 SEPIA   The image captured with Sepia "
								"effect\n");
	printf("                 If not specified COLOR effect is the "
							"default Option\n");
}

struct {
	void *start;
	void *start_aligned;
	size_t length;
} *cbuffers;

int main(int argc, char *argv[])
{
	struct v4l2_capability capability;
	struct v4l2_format cformat;
	struct v4l2_requestbuffers creqbuf;
	struct v4l2_buffer cfilledbuffer;
	struct v4l2_queryctrl queryctrl;
	struct v4l2_control control;
	struct omap34xxcam_sensor_info sens_info;
	int fd, i, ret, count = 1, memtype = V4L2_MEMORY_USERPTR;
	int fd_save = 0;
	int index = 1;
	int device = 1, framerate = 30;
	int colorLevel = V4L2_COLORFX_NONE;
	char *pixelFmt;
	char *fileName;

	if ((argc > 1) && (!strcmp(argv[1], "?"))) {
		usage();
		return 0;
	}

	if (argc > index) {
		device = atoi(argv[index]);
		index++;
	}

	fd = open_cam_device(O_RDWR, device);
	if (fd <= 0) {
		printf("Could not open the cam device\n");
		return -1;
	}

	if (argc > index) {
		pixelFmt = argv[index];
		index++;
		if (argc > index) {
			ret = validateSize(argv[index]);
			if (ret == 0) {
				ret = cam_ioctl(fd, pixelFmt, argv[index]);
				if (ret < 0) {
					usage();
					return -1;
				}
			} else {
				index++;
				if (argc > (index)) {
					ret = cam_ioctl(fd, pixelFmt,
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
			ret = cam_ioctl(fd, pixelFmt, DEFAULT_VIDEO_SIZE);
			if (ret < 0)
				return -1;
		}
	} else {
		printf("Setting pixel format and video size with default"
								" values\n");
		ret = cam_ioctl(fd, DEFAULT_PIXEL_FMT, DEFAULT_VIDEO_SIZE);
		if (ret < 0)
			return -1;
	}


	if (argc > index)
		count = atoi(argv[index]);

	printf("Frames: %d\n", count);
	index++;

	if (count >= 32 || count <= 0) {
		printf("Camera driver only support max 32 buffers, "
			"you request %d\n", count);
		return -1;
	}

	if (argc > index)
		fileName = argv[index];
	else
		fileName = DEFAULT_FILE_NAME;

	fd_save = creat(fileName, O_RDWR);
	if (fd_save <= 0) {
		printf("Can't create file %s\n", fileName);
		fd_save = 0;
	} else {
		printf("The captured frames will be saved into: %s\n",
		       fileName);
	}

	index++;
	if (argc > index) {
		framerate = atoi(argv[index]);
		if (framerate == 0) {
			printf("Invalid framerate value, Using Default "
							"framerate = 15\n");
			framerate = DEFAULT_FRAMERATE;
		}
		index++;
	}

	if (argc > index) {
		if (!strcmp(argv[index], "COLOR")) {
			colorLevel = V4L2_COLORFX_NONE;
			printf("Using default color level: %d\n", colorLevel);
		}
		if (!strcmp(argv[index], "BW")) {
			colorLevel = V4L2_COLORFX_BW;
			printf("Using black & white color level: %d\n",
			       colorLevel);
		} else {
			if (!strcmp(argv[index], "SEPIA")) {
				colorLevel = V4L2_COLORFX_SEPIA;
				printf("Using SEPIA color level: %d\n",
				       colorLevel);
			} else {
				if (!strcmp(argv[index], "SEPIA"))
					colorLevel = V4L2_COLORFX_SEPIA;
				else {
					printf("Invalid Color Effect: argv[%d]"
					       "=%s", index, argv[index]);
					usage();
					return 0;
				}
			}
		}
	}

	/*************************************************************/
	/* Set Frame Rate */

	ret = setFramerate(fd, framerate);
	if (ret < 0) {
		printf("Error setting framerate = %d\n", framerate);
		return -1;
	}


	if (ioctl(fd, VIDIOC_QUERYCAP, &capability) < 0) {
		perror("VIDIOC_QUERYCAP");
		return -1;
	}
	if (capability.capabilities & V4L2_CAP_STREAMING)
		printf("The driver is capable of Streaming!\n");
	else {
		printf("The driver is not capable of Streaming!\n");
		return -1;
	}
	cformat.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	ret = ioctl(fd, VIDIOC_G_FMT, &cformat);
	if (ret < 0) {
		perror("VIDIOC_G_FMT");
		return -1;
	}
	printf("Camera Image width = %d, Image height = %d, size = %d\n",
		 cformat.fmt.pix.width, cformat.fmt.pix.height,
		 cformat.fmt.pix.sizeimage);

	creqbuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	creqbuf.memory = memtype;
	creqbuf.count = count;

	printf("Requesting %d buffers of type %s\n", creqbuf.count,
		(memtype == V4L2_MEMORY_USERPTR) ? "V4L2_MEMORY_USERPTR" :
						"V4L2_MEMORY_MMAP");
	if (ioctl(fd, VIDIOC_REQBUFS, &creqbuf) < 0) {
		perror("VIDEO_REQBUFS");
		return -1;
	}
	printf("Camera Driver allowed buffers reqbuf.count = %d\n",
		creqbuf.count);
	if (creqbuf.count != count)
		count = creqbuf.count;

	cbuffers = calloc(creqbuf.count, sizeof(*cbuffers));

	/* mmap driver memory or allocate user memory, and queue each buffer */
	for (i = 0; i < creqbuf.count; ++i) {
		struct v4l2_buffer buffer;
		buffer.type = creqbuf.type;
		buffer.memory = creqbuf.memory;
		buffer.index = i;
		if (ioctl(fd, VIDIOC_QUERYBUF, &buffer) < 0) {
			perror("VIDIOC_QUERYBUF");
			return -1;
		}
		if (memtype == V4L2_MEMORY_USERPTR) {
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
			buffer.m.userptr =
			    (unsigned int)cbuffers[i].start_aligned;
			printf("User Buffer [%d].start = %x  length = %d\n",
				 i, cbuffers[i].start, cbuffers[i].length);
		} else {
			cbuffers[i].length = buffer.length;
			cbuffers[i].start =
			    mmap(NULL, buffer.length, PROT_READ | PROT_WRITE,
				 MAP_SHARED, fd, buffer.m.offset);
			if (cbuffers[i].start == MAP_FAILED) {
				perror("mmap");
				return -1;
			}
			printf("Mapped Buffers[%d].start = %x  length = %d\n",
				i, cbuffers[i].start, cbuffers[i].length);
			buffer.m.userptr = (unsigned int)cbuffers[i].start;
			buffer.length = cbuffers[i].length;
		}
		if (ioctl(fd, VIDIOC_QBUF, &buffer) < 0) {
			perror("CAMERA VIDIOC_QBUF");
			return -1;
		}
	}

	/* capture 1000 frames or when we hit the passed number of frames */
	cfilledbuffer.type = creqbuf.type;
	cfilledbuffer.memory = memtype;

	/* query color capability*/
	memset(&queryctrl, 0, sizeof(queryctrl));

	queryctrl.id = V4L2_CID_COLORFX;
	if (ioctl(fd, VIDIOC_QUERYCTRL, &queryctrl) == -1)
		printf("COLOR effect is not supported!\n");

	control.id = V4L2_CID_COLORFX;
	if (ioctl(fd, VIDIOC_G_CTRL, &control) == -1)
		printf("VIDIOC_G_CTRL failed!\n");

	printf("Color effect at the beginning of the test is supported, min %d,"
		"max %d.\nCurrent color is level is %d\n",
		queryctrl.minimum, queryctrl.maximum, control.value);

	control.value = colorLevel;
	if (ioctl(fd, VIDIOC_S_CTRL, &control) == -1)
		printf("VIDIOC_S_CTRL COLOR failed!\n");

	i = 0;

	control.id = V4L2_CID_COLORFX;
	if (ioctl(fd, VIDIOC_G_CTRL, &control) == -1)
		printf("VIDIOC_G_CTRL failed!\n");

	printf("Color effect values after setup is supported, min %d,"
		"max %d.\nCurrent color is level is %d\n",
		queryctrl.minimum, queryctrl.maximum, control.value);

	/* turn on streaming */
	if (ioctl(fd, VIDIOC_STREAMON, &creqbuf.type) < 0) {
		perror("VIDIOC_STREAMON");
		return -1;
	}

	/********************************************************/
	/* Get Sensor info using SENSOR_INFO ioctl */

	printf("Getting Sensor Info...\n");
	if (ioctl(fd, VIDIOC_PRIVATE_OMAP34XXCAM_SENSOR_INFO, &sens_info) < 0) {
		printf("VIDIOC_PRIVATE_OMAP34XXCAM_SENSOR_INFO not supported.\n");
	} else {
		printf("  Pixel clk:   %d Hz\n", sens_info.current_xclk);
		printf("  Full size:   %d x %d\n",
			sens_info.full_size.width,
			sens_info.full_size.height);
		printf("  Active size: %d x %d\n",
			sens_info.active_size.width,
			sens_info.active_size.height);
	}

	/********************************************************/

	while (i < 1000) {
		/* De-queue the next avaliable buffer */
		while (ioctl(fd, VIDIOC_DQBUF, &cfilledbuffer) < 0) {
			perror("VIDIOC_DQBUF");
			printf(" ERROR HAS OCCURED\n");
		}

		i++;

		if (i == count) {
			printf("Cancelling the streaming capture...\n");
			creqbuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
			if (ioctl(fd, VIDIOC_STREAMOFF, &creqbuf.type) < 0) {
				perror("VIDIOC_STREAMOFF");
				return -1;
			}
			printf("Done\n");
			break;
		}

		while (ioctl(fd, VIDIOC_QBUF, &cfilledbuffer) < 0)
			perror("CAM VIDIOC_QBUF");
	}
	/* we didn't turn off streaming yet */
	if (count == -1) {
		creqbuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		if (ioctl(fd, VIDIOC_STREAMOFF, &creqbuf.type) == -1) {
			perror("VIDIOC_STREAMOFF");
			return -1;
		}
	}

	printf("Captured %d frames!\n", i);
	printf("Output Frame Size: %u x %u\n",
		cformat.fmt.pix.width, cformat.fmt.pix.height);

	printf("Start writing to file\n");
	if (fd_save > 0) {
		for (i = 0; i < count; i++)
			write(fd_save, cbuffers[i].start_aligned,
			       cformat.fmt.pix.width * cformat.fmt.pix.height *
			       2);
	}
	printf("Completed writing to file: %s\n", fileName);
	for (i = 0; i < creqbuf.count; i++) {
		if (cbuffers[i].start) {
			if (memtype == V4L2_MEMORY_USERPTR)
				free(cbuffers[i].start);
			else
				munmap(cbuffers[i].start, cbuffers[i].length);
		}
	}

	free(cbuffers);
	close(fd);
	if (fd_save > 0)
		close(fd_save);
}
