/* ================================================================================
*             Texas Instruments OMAP(TM) Platform Software
*  (c) Copyright Texas Instruments, Incorporated.  All Rights Reserved.
*
*  Use of this software is controlled by the terms and conditions found 
*  in the license agreement under which this software has been supplied.
* ================================================================================ */

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <string.h>

#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/videodev.h>

static void usage(void)
{
	printf("zoomdemo [<delay>]\n");
	printf("\tDigitalZoom & Viewfinder demo using crop API. Delay <delay> seconds between steps.\n");
}

#define VIEW_WIN_WIDTH		240
#define VIEW_WIN_HEIGHT	180
#define X_STEP			40
#define Y_STEP			30
#define CROP_WIDTH_MIN		(VIEW_WIN_WIDTH*2)
#define PHASE_ZOOM_IN		0
#define PHASE_ZOOM_OUT2	1
#define PHASE_ZOOM_IN2		2
#define PHASE_ZOOM_OUT3	3
#define PHASE_ZOOM_IN3		4
#define PHASE_MOVE_RIGHT	5
#define PHASE_MOVE_LEFT	6
#define PHASE_MOVE_RIGHT2	7
#define PHASE_MOVE_UP		8
#define PHASE_MOVE_DOWN	9
#define PHASE_MOVE_UP2		10
#define PHASE_ZOOM_OUT		11
#define TOTAL_PHASES		(PHASE_ZOOM_OUT+1)		


int main (int argc, char *argv[])
{
	struct v4l2_format format;
	struct v4l2_cropcap cropcap;
	struct v4l2_crop crop;
	int ret, fd, enable, phase, n, i, saved_left, saved_top, delay=0;

	if ((argc > 1) && (!strcmp(argv[1], "?"))) {
		usage();
		return 0;
	}
	if (argc > 1)
		delay = atoi(argv[1]);

	if ((fd = open_cam_device(O_RDWR)) <= 0) {
		printf ("Could not open the cam device\n");
		return -1;
	}

	format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	ret = ioctl(fd, VIDIOC_G_FMT, &format);
	if (ret < 0) {
		perror("VIDIOC_G_FMT");
		return -1;
	}
	format.fmt.pix.width = VIEW_WIN_WIDTH;
	format.fmt.pix.height = VIEW_WIN_HEIGHT;
	//format.fmt.pix.pixelformat = V4L2_PIX_FMT_RGB565;
 
	/* set size & format of the video image */
	ret = ioctl(fd, VIDIOC_S_FMT, &format);
	if (ret < 0) {
		perror ("VIDIOC_S_FMT");
		return -1;
	}

	/* read back */
	ret = ioctl(fd, VIDIOC_G_FMT, &format);
	if (ret < 0) {
		perror("VIDIOC_G_FMT");
		return -1;
	}
	printf ("New video image: ");
	print_image_size_format(&format);

 	cropcap.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	ret = ioctl (fd, VIDIOC_CROPCAP, &cropcap);
	if (ret != 0) {
		perror("VIDIOC_CROPCAP");
		return -1;
	}
	printf("Crop bounds (%d, %d) (%d, %d), defrect (%d, %d) (%d, %d)\n",
		cropcap.bounds.left, cropcap.bounds.top,
		cropcap.bounds.width, cropcap.bounds.height,
		cropcap.defrect.left, cropcap.defrect.top,
		cropcap.defrect.width, cropcap.defrect.height);

	crop.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
#if 0
	ret = ioctl (fd, VIDIOC_G_CROP, &crop);
	if (ret != 0) {
		perror("VIDIOC_G_CROP");
		return -1;
	}
	printf("Old Crop (%d, %d) (%d, %d)\n",
		crop.c.left, crop.c.top, crop.c.width, crop.c.height);
#endif
	/* start with the max */
	crop.c = cropcap.defrect;  
	ret = ioctl (fd, VIDIOC_S_CROP, &crop);
	if (ret != 0) {
		perror("VIDIOC_S_CROP");
		return -1;
	}

	enable = 1;
	ret = ioctl (fd, VIDIOC_OVERLAY, &enable);
	if (ret != 0)	{
		perror("VIDIOC_OVERLAY");
		return -1;
	}

	n = 1; /* number of iterations */
	phase = PHASE_ZOOM_IN; /* first phase */
	while (n) {
		if (delay) sleep(delay);
		switch (phase) {
		case PHASE_ZOOM_IN:
		case PHASE_ZOOM_IN2:
		case PHASE_ZOOM_IN3:
			crop.c.left += X_STEP;
			crop.c.width -= X_STEP*2;
			crop.c.top += Y_STEP;
			crop.c.height -= Y_STEP*2;
			if (crop.c.width == CROP_WIDTH_MIN) {
				phase++;
				saved_left = crop.c.left; 	
				saved_top = crop.c.top; 	
				sleep(1);
			}
			break;
		case PHASE_MOVE_RIGHT:
			crop.c.left += X_STEP;
			if ((crop.c.left + crop.c.width)  == 
			    (cropcap.bounds.left + cropcap.bounds.width))
				phase++;
			break;
		case PHASE_MOVE_LEFT:
			crop.c.left -= X_STEP;
			if (crop.c.left == cropcap.bounds.left)
				phase++;
			break;
		case PHASE_MOVE_RIGHT2:
			crop.c.left += X_STEP;
			if (crop.c.left == saved_left) 
 				phase++;
			break;
		case PHASE_MOVE_UP:
			crop.c.top -= Y_STEP;
			if (crop.c.top == cropcap.bounds.top)
				phase++;
			break;
		case PHASE_MOVE_DOWN:
			crop.c.top += Y_STEP;
			if ((crop.c.top + crop.c.height)  == 
			    (cropcap.bounds.top + cropcap.bounds.height))
				phase++;
			break;
		case PHASE_MOVE_UP2:
			crop.c.top -= Y_STEP;
			if (crop.c.top == saved_top) 
 				phase++;
			break;
		case PHASE_ZOOM_OUT:  
		case PHASE_ZOOM_OUT2:  
		case PHASE_ZOOM_OUT3:  
			crop.c.left -= X_STEP;
			crop.c.width += X_STEP*2;
			crop.c.top -= Y_STEP;
			crop.c.height += Y_STEP*2;	
			if (crop.c.width == cropcap.bounds.width) {
				if (phase == PHASE_ZOOM_OUT) { 
					n--;   /* last phase */
					sleep(1);
				}
				phase = (phase +1 )%TOTAL_PHASES;	
				sleep(1);
			}
			break;
		} /*switch */

		ret = ioctl (fd, VIDIOC_S_CROP, &crop);
		if (ret != 0) {
			perror("VIDIOC_S_CROP");
			return -1;
		}
	} /*while */

	enable = 0;
	ret = ioctl (fd, VIDIOC_OVERLAY, &enable);
	if (ret != 0)	{
		perror("VIDIOC_OVERLAY2");
		return -1;
	}
	close(fd);
}




