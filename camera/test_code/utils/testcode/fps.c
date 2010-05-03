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
#include <time.h>
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

void calc_fps(struct v4l2_buffer *vb, int wrap_count)
{
	static unsigned int frame;
	float fps = 0, sec, usec;
	static struct timeval stv, etv;

	if (frame == 0)
		memcpy(&stv, &vb->timestamp, sizeof(struct timeval));

	if (frame == wrap_count) {
		memcpy(&etv, &vb->timestamp, sizeof(struct timeval));
		sec = etv.tv_sec - stv.tv_sec;
		if (etv.tv_usec > stv.tv_usec)
			usec = etv.tv_usec - stv.tv_usec;
		else
			usec = stv.tv_usec - etv.tv_usec;

		fps = (frame / (sec+(usec/1000000)));
		printf(" fps=%f\n", fps);
		frame = 0;
		memcpy(&stv, &vb->timestamp, sizeof(struct timeval));
	} else {
		frame++;
	}
}

enum {
	TIMER_ENUM_START,
	TIMER_OPEN,
	TIMER_CLOSE,
	TIMER_QUERYCAP,
	TIMER_G_FMT,
	TIMER_S_FMT,
	TIMER_REQBUFS,
	TIMER_QUERYBUF,
	TIMER_QBUF,
	TIMER_DQBUF,
	TIMER_STREAMON,
	TIMER_STREAMOFF,
	TIMER_ENUM_END,
};

/**
 * struct instrument_timer - Structure for storing timing measurements
 * @start: System time when the last time sample started.
 * @min: Shortest time measurement (in us) of all samples.
 * @max: Longest time measurement (in us) of all samples.
 * @total: Average time measurement (in us) of all samples.
 * @count: Number of time samples recollected so far.
 * @name: 32 byte string describing instrument name.
 **/
struct instrument_timer {
	struct timeval start;
	unsigned long min;
	unsigned long max;
	unsigned long long total;
	unsigned long count;
	char name[32];
} timer[] = {
	{	/* TIMER_ENUM_START */
		.name  = "UNUSED",
	},
	{	/* TIMER_OPEN */
		.name  = "open",
	},
	{	/* TIMER_CLOSE */
		.name  = "close",
	},
	{	/* TIMER_QUERYCAP */
		.name  = "VIDIOC_QUERYCAP",
	},
	{	/* TIMER_G_FMT */
		.name  = "VIDIOC_G_FMT",
	},
	{	/* TIMER_S_FMT */
		.name  = "VIDIOC_S_FMT",
	},
	{	/* TIMER_REQBUFS */
		.name  = "VIDIOC_REQBUFS",
	},
	{	/* TIMER_QUERYBUF */
		.name  = "VIDIOC_QUERYBUF",
	},
	{	/* TIMER_QBUF */
		.name  = "VIDIOC_QBUF",
	},
	{	/* TIMER_DQBUF */
		.name  = "VIDIOC_DQBUF",
	},
	{	/* TIMER_STREAMON */
		.name  = "VIDIOC_STREAMON",
	},
	{	/* TIMER_STREAMOFF */
		.name  = "VIDIOC_STREAMOFF",
	},
	{	/* TIMER_SENSOR_INFO */
		.name  = "VIDIOC_SENSOR_INFO",
	},
	{	/* TIMER_ENUM_END */
		.name  = "UNUSED",
	},
};

/**
 * INSTRUMENT_INIT - Initialize the timer
 * @it: pointer to instrument_timer structure.
 * Must be called before and call to INSTRUMENT_START().
 **/
void INSTRUMENT_INIT(struct instrument_timer *it)
{
	it->min   = 0xffffffff;
	it->max   = 0;
	it->total = 0;
	it->count = 0;
}

/**
 * INSTRUMENT_START - Start the timer
 * @it: pointer to instrument_timer structure.
 * Must be called in a pair with INSTRUMENT_STOP().
 **/
inline void INSTRUMENT_START(struct instrument_timer *it)
{
	gettimeofday(&it->start, NULL);
}

/**
 * INSTRUMENT_STOP - Stop the timer
 * @it: pointer to instrument_timer structure.
 * Must be called in a pair with INSTRUMENT_STOP().
 * Adds the current timer value to a running total.
 * The avg must be calculated seperately using:
 *   avg in usec = it->total / it->count
 * or just call INSTRUMENT_DISPLAY().
 **/
inline void INSTRUMENT_STOP(struct instrument_timer *it)
{
	unsigned long diff;
	struct timeval end;

	gettimeofday(&end, NULL);

	/* Calculate time difference */
	diff = (end.tv_sec * 1000000 + end.tv_usec) -
		   (it->start.tv_sec * 1000000 + it->start.tv_usec);

	/* Check for min and max */
	if (diff < it->min)
		it->min = diff;

	if (diff > it->max)
		it->max = diff;

	it->total += diff;
	it->count++;
}

inline void INSTRUMENT_DISPLAY(struct instrument_timer *it)
{
	printf("[%s] avg=%lld us  (count=%lu  min=%lu  max=%lu)\n",
		it->name,
		it->total / it->count,
		it->count,
		it->min,
		it->max);
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

	/*****************************************************************/
	/* Initialize instrumentation */

	for (i = TIMER_ENUM_START; i < TIMER_ENUM_END; i++)
		INSTRUMENT_INIT(&timer[i]);

	/*****************************************************************/

	if (argc > index) {
		device = atoi(argv[index]);
		index++;
	}

	INSTRUMENT_START(&timer[TIMER_OPEN]);
	fd = open_cam_device(O_RDWR, device);
	INSTRUMENT_STOP(&timer[TIMER_OPEN]);
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
				INSTRUMENT_START(&timer[TIMER_S_FMT]);
				ret = cam_ioctl(fd, pixelFmt, argv[index]);
				INSTRUMENT_STOP(&timer[TIMER_S_FMT]);
				if (ret < 0) {
					usage();
					return -1;
				}
			} else {
				index++;
				if (argc > index) {
					INSTRUMENT_START(&timer[TIMER_S_FMT]);
					ret = cam_ioctl(fd, pixelFmt,
						argv[index-1], argv[index]);
					INSTRUMENT_STOP(&timer[TIMER_S_FMT]);
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
			INSTRUMENT_START(&timer[TIMER_S_FMT]);
			ret = cam_ioctl(fd, pixelFmt, DEFAULT_VIDEO_SIZE);
			INSTRUMENT_STOP(&timer[TIMER_S_FMT]);
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

	INSTRUMENT_START(&timer[TIMER_QUERYCAP]);
	ret = ioctl(fd, VIDIOC_QUERYCAP, &capability);
	INSTRUMENT_STOP(&timer[TIMER_QUERYCAP]);
	if (ret < 0) {
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
	INSTRUMENT_START(&timer[TIMER_G_FMT]);
	ret = ioctl(fd, VIDIOC_G_FMT, &cformat);
	INSTRUMENT_STOP(&timer[TIMER_G_FMT]);
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

	INSTRUMENT_START(&timer[TIMER_REQBUFS]);
	ret = ioctl(fd, VIDIOC_REQBUFS, &creqbuf);
	INSTRUMENT_STOP(&timer[TIMER_REQBUFS]);
	if (ret < 0) {
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
		INSTRUMENT_START(&timer[TIMER_QUERYBUF]);
		ret = ioctl(fd, VIDIOC_QUERYBUF, &buffer);
		INSTRUMENT_STOP(&timer[TIMER_QUERYBUF]);
		if (ret < 0) {
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

		INSTRUMENT_START(&timer[TIMER_QBUF]);
		ret = ioctl(fd, VIDIOC_QBUF, &buffer);
		INSTRUMENT_STOP(&timer[TIMER_QBUF]);
		if (ret < 0) {
			perror("CAMERA VIDIOC_QBUF");
			return -1;
		}
	}

	/* turn on streaming */
	INSTRUMENT_START(&timer[TIMER_STREAMON]);
	ret = ioctl(fd, VIDIOC_STREAMON, &creqbuf.type);
	INSTRUMENT_STOP(&timer[TIMER_STREAMON]);
	if (ret < 0) {
		perror("VIDIOC_STREAMON");
		return -1;
	}

	/* capture 1000 frames or when we hit the passed number of frames */
	cfilledbuffer.type = creqbuf.type;
	cfilledbuffer.memory = memtype;

	i = 0;

	while (1) {
		/* De-queue the next avaliable buffer */
		INSTRUMENT_START(&timer[TIMER_DQBUF]);
		ret = ioctl(fd, VIDIOC_DQBUF, &cfilledbuffer);
		INSTRUMENT_STOP(&timer[TIMER_DQBUF]);
		if (ret < 0) {
			perror("VIDIOC_DQBUF");
			return -1;
		}

		printf("."); fflush(stdout);
		i++;
		calc_fps(&cfilledbuffer, wrap_count);

		if (i == count) {
			printf("Cancelling the streaming capture...\n");
			creqbuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
			INSTRUMENT_START(&timer[TIMER_STREAMOFF]);
			ret = ioctl(fd, VIDIOC_STREAMOFF, &creqbuf.type);
			INSTRUMENT_STOP(&timer[TIMER_STREAMOFF]);
			if (ret < 0) {
				perror("VIDIOC_STREAMOFF");
				return -1;
			}
			printf("Done\n");
			break;
		}

		INSTRUMENT_START(&timer[TIMER_QBUF]);
		ret = ioctl(fd, VIDIOC_QBUF, &cfilledbuffer);
		INSTRUMENT_STOP(&timer[TIMER_QBUF]);
		if (ret < 0) {
			perror("CAM VIDIOC_QBUF");
			return -1;
		}
	}

	/* we didn't turn off streaming yet */
	if (count == -1) {
		creqbuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		INSTRUMENT_START(&timer[TIMER_STREAMOFF]);
		ret = ioctl(fd, VIDIOC_STREAMOFF, &creqbuf.type);
		INSTRUMENT_STOP(&timer[TIMER_STREAMOFF]);
		if (ret == -1) {
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
	INSTRUMENT_START(&timer[TIMER_CLOSE]);
	close(fd);
	INSTRUMENT_STOP(&timer[TIMER_CLOSE]);

	/* Display Instrumentation info */
	printf("\n********* Instrumentation Stats *********\n");
	INSTRUMENT_DISPLAY(&timer[TIMER_OPEN]);
	INSTRUMENT_DISPLAY(&timer[TIMER_CLOSE]);
	INSTRUMENT_DISPLAY(&timer[TIMER_QUERYCAP]);
	INSTRUMENT_DISPLAY(&timer[TIMER_G_FMT]);
	INSTRUMENT_DISPLAY(&timer[TIMER_S_FMT]);
	INSTRUMENT_DISPLAY(&timer[TIMER_REQBUFS]);
	INSTRUMENT_DISPLAY(&timer[TIMER_QUERYBUF]);
	INSTRUMENT_DISPLAY(&timer[TIMER_QBUF]);
	INSTRUMENT_DISPLAY(&timer[TIMER_DQBUF]);
	INSTRUMENT_DISPLAY(&timer[TIMER_STREAMON]);
	INSTRUMENT_DISPLAY(&timer[TIMER_STREAMOFF]);
	printf("*****************************************\n");
}
