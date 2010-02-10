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
#include <errno.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <string.h>
#include <linux/ioctl.h>
#include <linux/fb.h>
#include <linux/videodev2.h>
#include <linux/errno.h>

#define VIDIOC_S_OMAP2_ROTATION		_IOW('V', 3, int)
#define VIDEO_DEVICE1 "/dev/video1"
#define VIDEO_DEVICE2 "/dev/video2"

#define DEFAULT_PIXEL_FMT "YUYV"
#define DEFAULT_VIDEO_SIZE "QCIF"


static void usage(void)
{
	printf("FPS Test Case\n");
	printf("Usage: fps [camDevice] [pixelFormat] [<sizeW> <sizeH>]"
			" [<numberOfFrames>] [framerate]\n");
	printf("   [camDevice] Camera device to be open\n\t 1:Micron sensor "
					"2:OV sensor 3:IMX046\n");
	printf("   [pixelFormat] set the pixelFormat to use. \n\tSupported: "
		"YUYV, UYVY, RGB565, RGB555, RGB565X, RGB555X, SGRBG10,"
		" SRGGB10, SBGGR10, SGBRG10 \n");
	printf("   [sizeW] Set the video width\n");
	printf("   [sizeH] Set the video heigth\n");
	printf("\tOptionally size can be specified using standard name sizes"
							"(VGA,PAL,etc)\n");
	printf("\tIf size is NOT specified QCIF used as default\n");
	printf("   [numberOfFrames] Number of Frames to be captured\n");
	printf("   [framerate] is the framerate to be used, if no value"
			" is given \n\t      15 fps is default\n");
}

void calc_fps(int wrap_count)
{
	static unsigned int frame;
	float fps = 0, sec, usec;
	static struct timeval stv, etv;

	if (frame == 0)
		gettimeofday(&stv, NULL);

	if (frame == wrap_count) {
		gettimeofday(&etv, NULL);
		sec = etv.tv_sec - stv.tv_sec;
		if (etv.tv_usec > stv.tv_usec)
			usec = etv.tv_usec - stv.tv_usec;
		else
			usec = stv.tv_usec - etv.tv_usec;

		fps = (frame / (sec+(usec/1000000)));
		printf(" fps=%f\n", fps);
		frame = 0;
		gettimeofday(&stv, NULL);
	} else {
		frame++;
	}
}


struct {
	void *start;
	size_t length;
} *cbuffers;

int main(int argc, char *argv[])
{
	struct screen_info_struct {
		int fd;
		char *data;
		int width;
		int height;
		struct fb_fix_screeninfo fbfix;
		struct fb_var_screeninfo info;
	} screen_info;

	void *src_start;
	struct v4l2_capability capability;
	struct v4l2_format cformat;
	struct v4l2_requestbuffers creqbuf;
	struct v4l2_buffer cfilledbuffer;
	int fd, i, ret, count = 1, memtype = V4L2_MEMORY_USERPTR;
	int index = 1, vid = 1, set_video_img = 0;
	int degree;
	int j;
	int device = 1;
	char *pixelFmt;
	int framerate = 15, wrap_count = 30;


	if ((argc > 1) && (!strcmp(argv[1], "?"))) {
		usage();
		return 0;
	}

	/**********************************************************************/

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
				if (argc > index) {
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


	if (argc > index) {
		count = atoi(argv[index]);
		index++;
	}
	printf("Frames: %d\n", count);


	if (argc > index) {
		framerate = atoi(argv[index]);
		if (framerate == 0) {
			printf("Invalid framerate value, Using Default "
							"framerate = 15\n");
			framerate = 15;
		}
		ret = setFramerate(fd, framerate);
		if (ret < 0) {
			printf("Error setting framerate = %d\n", framerate);
			return -1;
		}

		/* Set the number of frames at which frame rate should
		 * be calulated. For rates:
		 *   rate >= 30         : Calculate every 'rate'
		 *   rate>10 && rate<30 : Calculate every 'rate x 2'
		 *   rate<=10           : Calculate every 30
		 * This helps make the calculation more acurate and also
		 * makes the output prettier.
		 */
		if (framerate >= 30)
			wrap_count = framerate;
		else
			wrap_count = framerate*2;
		if (framerate <= 10)
			wrap_count = 30;

		index++;
	}

	/**********************************************************************/


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
	creqbuf.count = 2;

	printf("Requesting %d buffers of type %s\n", creqbuf.count,
		 (memtype ==
		  V4L2_MEMORY_USERPTR) ? "V4L2_MEMORY_USERPTR" :
		 "V4L2_MEMORY_MMAP");
	if (ioctl(fd, VIDIOC_REQBUFS, &creqbuf) < 0) {
		perror("VIDEO_REQBUFS");
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
		if (ioctl(fd, VIDIOC_QUERYBUF, &buffer) < 0) {
			perror("VIDIOC_QUERYBUF");
			return -1;
		}
		if (memtype == V4L2_MEMORY_USERPTR) {
			cbuffers[i].length = buffer.length;
			posix_memalign(&cbuffers[i].start, 0x1000,
				       cbuffers[i].length);
			buffer.m.userptr =
			    (unsigned int)cbuffers[i].start;
			printf("User Buffer [%d].start = 0x%08X  length = %d\n",
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

	/* turn on streaming */
	if (ioctl(fd, VIDIOC_STREAMON, &creqbuf.type) < 0) {
		perror("VIDIOC_STREAMON");
		return -1;
	}

	/* capture 1000 frames or when we hit the passed number of frames */
	cfilledbuffer.type = creqbuf.type;
	cfilledbuffer.memory = memtype;

	i = 0;

	while (1) {
		/* De-queue the next avaliable buffer */
		while (ioctl(fd, VIDIOC_DQBUF, &cfilledbuffer) < 0) {
			perror("VIDIOC_DQBUF");
			printf(" ERROR HAS OCCURED\n");
		}

		printf("."); fflush(stdout);
		i++;
		calc_fps(wrap_count);

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
	printf("Frame Size: %u x %u\n",
			cformat.fmt.pix.width, cformat.fmt.pix.height);


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
}
