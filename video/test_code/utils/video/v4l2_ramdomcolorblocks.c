/*
 * v4l2_ramdomcolorblocks.c
 *
 * This is a V4L2 sample application to show the display functionality`
 * The app puts a moving horizontal bar on the display device in various
 * shades of colors.
 * This application is an extenson of saMmapDisplay application.
 *
 * Copyright (C) 2009 Texas Instruments Incorporated - http://www.ti.com/
 *
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the
 *    distribution.
 *
 *    Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
*/

/******************************************************************************
  Header File Inclusion
 ******************************************************************************/
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <errno.h>
#include <string.h>
#include <linux/videodev2.h>
#include "lib.h"
/******************************************************************************
 Macros
 MAX_BUFFER     : Changing the following will result different number of
		  instances of buf_info structure.
 MAXLOOPCOUNT	: Display loop count
 WIDTH		: Width of the output image
 HEIGHT		: Height of the output image
 ******************************************************************************/
#define MAX_BUFFER	3
#define MAXLOOPCOUNT    200

static int display_fd;
int max_x_downscale, max_y_downscale;
int rand_height;
int rand_width;
int rand_crop_width;
int rand_crop_height;
int rand_win_width;
int rand_win_height;

struct buf_info {
	int index;
	unsigned int length;
	char *start;
};

static struct buf_info display_buff_info[MAX_BUFFER];
static int numbuffers = MAX_BUFFER;
static int dispheight, dispwidth, sizeimage;
struct v4l2_buffer buf;

/******************************************************************************
			Function Definitions
 ******************************************************************************/
static int releaseDisplay();
static void startDisplay();
static void stopDisplay();

/*
	This routine unmaps all the buffers
	This is the final step.
*/
static int releaseDisplay()
{
	int i;
	for (i = 0; i < numbuffers; i++) {
		munmap(display_buff_info[i].start,
				display_buff_info[i].length);
		display_buff_info[i].start = NULL;
	}
	close(display_fd);
	display_fd = 0;
	return 0;
}
/*
	Starts Streaming
*/
static void startDisplay()
{
	int a = V4L2_BUF_TYPE_VIDEO_OUTPUT, ret;
	ret = ioctl(display_fd, VIDIOC_STREAMON, &a);
	if (ret < 0) {
		perror("VIDIOC_STREAMON\n");
		close(display_fd);
		exit(1);
	}
}

/*
 Stops Streaming
*/
static void stopDisplay()
{
	int ret, a = V4L2_BUF_TYPE_VIDEO_OUTPUT;
	ret = ioctl(display_fd, VIDIOC_STREAMOFF, &a);
	if (ret < 0) {
		perror("Error in stopping display\n");
		close(display_fd);
		exit(1);
	}
}

static short ycbcr[8] = {
	(0x1F << 11) | (0x3F << 5) | (0x1F),
	(0x00 << 11) | (0x00 << 5) | (0x00),
	(0x1F << 11) | (0x00 << 5) | (0x00),
	(0x00 << 11) | (0x3F << 5) | (0x00),
	(0x00 << 11) | (0x00 << 5) | (0x1F),
	(0x1F << 11) | (0x3F << 5) | (0x00),
	(0x1F << 11) | (0x00 << 5) | (0x1F),
	(0x00 << 11) | (0x3F << 5) | (0x1F),
};

void color_bar(char *addr, int width, int height, int order)
{
	unsigned short *ptr = (unsigned short *)addr + order * (width / 2);
	unsigned short *last_ptr;
	int i, j, k;

	/*int page_width = (width * 2 +
				4096 - 1) & ~(4096 - 1);*/
	int page_width = width;
	width = rand_width;
	last_ptr = ptr - (page_width / 2);

	for (i = 0 ; i < 8; i++) {
		for (j = 0 ; j < height / 8 ; j++) {
			ptr = last_ptr + (page_width / 2);
			last_ptr = ptr;
			if ((unsigned int)ptr > (unsigned int)addr +
					(page_width)*(height-1)) {
				last_ptr = (unsigned short *)addr -
					(page_width/2);
				ptr = (unsigned short *)addr;
			}
			for (k = 0 ; k < width ; k++, ptr++)
				*ptr = ycbcr[i];

		}
	}
}

void set_format(void)
{

	struct v4l2_format fmt;
	int ret = 0;

	/* Get the parameters before setting and
	 * set only required parameters */
	fmt.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
	ret = ioctl(display_fd, VIDIOC_G_FMT, &fmt);
	if (ret < 0) {
		perror("Get Format failed\n");
		exit(1);
	}

	fmt.fmt.pix.width = rand_width;
	fmt.fmt.pix.height = rand_height;
	fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_RGB565;

	ret = ioctl(display_fd, VIDIOC_S_FMT, &fmt);
	if (ret < 0) {
		perror("Set Format failed\n");
		exit(1);
	}
}

int set_win(int rand_win_left, int rand_win_top)
{

	struct v4l2_format fmt;
	int ret = 0;

	/* Get the parameters before setting and
	 * set only required parameters */
	fmt.type = V4L2_BUF_TYPE_VIDEO_OVERLAY;
	ret = ioctl(display_fd, VIDIOC_G_FMT, &fmt);
	if (ret < 0) {
		perror("Get Format failed\n");
		exit(1);
	}

	fmt.fmt.win.w.left = rand_win_left;
	fmt.fmt.win.w.top = rand_win_top;
	fmt.fmt.win.w.width = rand_win_width;
	fmt.fmt.win.w.height = rand_win_height;
	fmt.type = V4L2_BUF_TYPE_VIDEO_OVERLAY;

	ret = ioctl(display_fd, VIDIOC_S_FMT, &fmt);

	return ret;
}

int set_crop(int rand_crop_left, int rand_crop_top)
{

	struct v4l2_crop crop;
	int ret = 0;

	crop.c.left   = rand_crop_left;
	crop.c.top    = rand_crop_top;
	crop.c.width  = rand_crop_width;
	crop.c.height = rand_crop_height;
	crop.type     = V4L2_BUF_TYPE_VIDEO_OUTPUT;

	ret = ioctl(display_fd, VIDIOC_S_CROP, &crop);

	return ret;
}

void get_format(int *dispheight, int *dispwidth, int *sizeimage)
{
	struct v4l2_format fmt;
	int ret = 0;
	/* It is necessary for applications to know about the
	 * buffer chacteristics that are set by the driver for
	 * proper handling of buffers These are : width,height,
	 * pitch and image size
	 * */
	fmt.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
	ret = ioctl(display_fd, VIDIOC_G_FMT, &fmt);
	if (ret < 0) {
		perror("Get Format failed\n");
		close(display_fd);
		exit(1);
	}
	*dispheight = fmt.fmt.pix.height;
	*dispwidth = fmt.fmt.pix.bytesperline;
	*sizeimage = fmt.fmt.pix.sizeimage;

}

void setup_buffers()
{
	struct v4l2_requestbuffers reqbuf;
	int ret = 0, i;
	/* Now for the buffers.Request the number of buffers needed
	 * and the kind of buffers(User buffers or kernel buffers
	 * for memory mapping). Please note that the return value
	 * in the reqbuf.count might be lesser than numbuffers under
	 * some low memory circumstances */
	reqbuf.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
	reqbuf.count = numbuffers;
	reqbuf.memory = V4L2_MEMORY_MMAP;

	ret = ioctl(display_fd, VIDIOC_REQBUFS, &reqbuf);
	if (ret < 0) {
		perror("Could not allocate the buffers\n");
		close(display_fd);
		exit(1);
	}

	numbuffers = reqbuf.count;
	for (i = 0 ; i < reqbuf.count ; i++) {
		/* query */
		fflush(stdout);
		buf.index = i;
		buf.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
		buf.memory = V4L2_MEMORY_MMAP;
		ret = ioctl(display_fd, VIDIOC_QUERYBUF, &buf);
		if (ret < 0) {
			perror("quering for buffer info failed\n");
			close(display_fd);
			exit(1);
		}
		fflush(stdout);
		/* mmap */
		display_buff_info[i].length = buf.length;
		display_buff_info[i].index = i;
		fflush(stdout);
		display_buff_info[i].start =
			mmap(NULL, buf.length, PROT_READ | PROT_WRITE,
					MAP_SHARED, display_fd, buf.m.offset);
	fflush(stdout);

		if ((unsigned int) display_buff_info[i].
				start == MAP_SHARED) {
			printf("Cannot mmap = %d buffer\n", i);
			close(display_fd);
			exit(1);
		}
		/*
		   After mapping each buffer, it is a good
		   idea to first "zero" them out.
		   Here it is being set to a mid grey-scale
		   Y=0x80, Cb=0x80, Cr=0x80
		 */
	fflush(stdout);

		/* Fill up the buffers with the values.*/
		color_bar(display_buff_info[i].start, dispwidth, dispheight,
				0);
	fflush(stdout);
	}

	for (i = 0 ; i < reqbuf.count ; i++) {
		buf.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
		buf.memory = V4L2_MEMORY_MMAP;
		buf.index = i;
		ret = ioctl(display_fd, VIDIOC_QBUF, &buf);
		if (ret < 0) {
			perror("VIDIOC_QBUF\n");
			close(display_fd);
			exit(1);
		}
	}
}

void open_display(const char *str)
{
	struct v4l2_capability capability;
	int mode = O_RDWR;
	int video_device = atoi(str);

	display_fd = open((video_device == 1) ? VIDEO_DEVICE1 :
		((video_device == 2) ? VIDEO_DEVICE2 : VIDEO_DEVICE3), mode);

	if (display_fd == -1) {
		perror("failed to open display device\n");
		exit(1);
	}
	if (ioctl(display_fd, VIDIOC_QUERYCAP, &capability) < 0) {
		perror("VIDIOC_QUERYCAP");
		exit(1);
	}

}

int app_main(char *argv[])
{

	void *displaybuffer;
	int counter = 0;
	int ret = 0;
	int i;
	int rand_crop_left = 0, rand_crop_top = 0;
	int rand_win_left = 0, rand_win_top = 0;

	for (i = 0; i < 50; i++) {
retry:
		rand_width = rand() % 864;
		rand_height = rand() % 480;

		if (rand_width % 2 == 1)	/*even rounding*/
			rand_width++;

		if (rand_height % 2 == 1)
			rand_height++;

		if (rand_width == 0 || rand_height == 0)
			goto retry;

		rand_crop_left = rand() % rand_width;
		rand_crop_top = rand() % rand_height;

		if ((rand_width - 2) == 0 || (rand_height - 2) == 0)
			goto retry;

		rand_crop_width = (rand() % (rand_width - 2)) + 2;
		rand_crop_height = (rand() % (rand_height - 2)) + 2;

		rand_win_left = rand() % 864;
		rand_win_top = rand() % 480;

		if ((864 - rand_win_left) == 0 || (480 - rand_win_top) == 0)
			return -EINVAL;

		rand_win_width = (rand() %
				((rand_crop_width * 8) - rand_win_left)) +
				(rand_crop_width / max_x_downscale);

		rand_win_height = (rand() %
				((rand_crop_height * 8) - rand_win_top)) +
				(rand_crop_height / max_y_downscale);

		if (rand_crop_width > rand_win_width) {
			if (rand_crop_width >=
				(max_x_downscale * rand_win_width))
				goto retry;
		} else {
			if (rand_win_width >= (8 * rand_crop_width))
				goto retry;
		}

		if (rand_crop_height > rand_win_height) {
			if (rand_crop_height >=
				(max_y_downscale * rand_win_height))
				goto retry;
		} else {
			if (rand_win_height >= (8 * rand_crop_height))
				goto retry;
		}

		/* open display channel */
		open_display(argv[1]);
		fflush(stdout);

		/* Set input format */
		set_format();
		fflush(stdout);

		/* Get the negotiatied format */
		get_format(&dispheight, &dispwidth, &sizeimage);

		/* Set crop */
		ret = set_crop(rand_crop_left, rand_crop_top);
		if (ret < 0) {
			perror("Set Crop failed\n");
			close(display_fd);
			goto retry;
		}
		/* Set output window */
		ret = set_win(rand_win_left, rand_win_top);
		if (ret < 0) {
			perror("Set Win failed\n");
			close(display_fd);
			goto retry;
		}
		if (rand_crop_width > rand_win_width) {
			if (rand_crop_width >=
				(max_x_downscale * rand_win_width)) {
				printf("WARNING from test app: This width"
				"downscale ratio may not be supported,"
				"retrying\n");
				close(display_fd);
				goto retry;
			}
		} else {
			if (rand_win_width >= (8 * rand_crop_width)) {
				printf("WARNING from test app: This width"
				"upscale ratio may not be supported\n");
				close(display_fd);
				goto retry;
			}
		}

		if (rand_crop_height > rand_win_height) {
			if (rand_crop_height >=
				(max_y_downscale * rand_win_height)) {
				printf("WARNING from test app: This height"
				"downscale ratio may not be supported,"
				"retrying\n");
				close(display_fd);
				goto retry;
			}
		} else {
			if (rand_win_height >= (8 * rand_crop_height)) {
				printf("WARNING from test app: This height"
				"upscale ratio may not be supported\n");
				close(display_fd);
				goto retry;
			}
		}

		printf("iteration %d\n", i);

		fflush(stdout);

		/*
		 * 1. Request the buffers
		 * 2. Query for the buffer info like the phys address
		 * 3. mmap the buffer to user space.
		 * This information about the buffers is currently
		 * stored in a user level data structure
		 */

		setup_buffers();
		fflush(stdout);

		/* Start Displaying */
		startDisplay();

		/*
		   This is a running loop where the buffer is
		   DEQUEUED  <-----|  PROCESSED	|  & QUEUED -------|
		 */
		while (counter < MAXLOOPCOUNT) {
			/* Get display buffer using DQBUF ioctl */
			ret = ioctl(display_fd, VIDIOC_DQBUF, &buf);
			if (ret < 0) {
				perror("VIDIOC_DQBUF\n");
				return -1;
			}

			displaybuffer = display_buff_info[buf.index].start;
			/* Process it
			   In this example, the "processing" is putting
			   a horizontally moving color bars with changing
			   starting line of display.
			 */
			color_bar(displaybuffer, dispwidth, dispheight,
					counter%(dispheight));

			/* Now queue it back to display it */
			buf.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
			buf.memory = V4L2_MEMORY_MMAP;
			buf.index = buf.index;
			ret = ioctl(display_fd, VIDIOC_QBUF, &buf);
			if (ret < 0) {
				perror("VIDIOC_QBUF\n");
				return -1;
			}

			counter += 1;
		}

		counter = 0;

		/* Once the streaming is done  stop the display  hardware  */
		stopDisplay();

		/* close  display channel */
		releaseDisplay();
	}
	return 0;

}



static int usage(void)
{
	printf("Usage: v4l2_ramdomcolorblocks <video_device>"
		" <max_x_downscale> <max_y_downscale>\n");
	return 1;
}

int main(int argc, char *argv[])
{
	int video_device = 0;
	if (argc != 4)
		return usage();

	video_device = atoi(argv[1]);

	if ((video_device != 1) && (video_device != 2) && (video_device != 3)) {
		printf("video_device has to be 1 or 2 or 3! video_device=%d, "
			"argv[1]=%s\n", video_device, argv[1]);
		return usage();
	}
	max_x_downscale = atoi(argv[2]);
	max_y_downscale = atoi(argv[3]);

	if (app_main(argv))
		releaseDisplay();
	return 0;
}
