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
	printf("crop [<left> <top> [<width> <height>]]\n");
	printf("\tTo set crop rectangle for video image.\n");
}

int main (int argc, char *argv[])
{
	struct v4l2_cropcap cropcap;
	struct v4l2_crop crop;
	int left, top, width, height;
	int ret, fd, set = 0;

	if ((argc > 1) && (!strcmp(argv[1], "?"))) {
		usage();
		return 0;
	}

	if (argc >= 3) {
		left = atoi(argv[1]);
		top = atoi(argv[2]);
		set++;
		if (argc >= 5) {
			width = atoi(argv[3]);
			height = atoi(argv[4]);
			set++;
		}
	}

	if ((fd = open_cam_device(O_RDWR)) <= 0) {
		printf ("Could not open the cam device\n");
		return -1;
	}
	
	cropcap.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	ret = ioctl (fd, VIDIOC_CROPCAP, &cropcap);
	if (ret != 0) {
		perror("VIDIOC_CROPCAP");
		return -1;
	}
	printf("Video Crop bounds (%d, %d) (%d, %d), defrect (%d, %d) (%d, %d)\n",
		cropcap.bounds.left, cropcap.bounds.top,
		cropcap.bounds.width, cropcap.bounds.height,
		cropcap.defrect.left, cropcap.defrect.top,
		cropcap.defrect.width, cropcap.defrect.height);

	crop.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	ret = ioctl (fd, VIDIOC_G_CROP, &crop);
	if (ret != 0) {
		perror("VIDIOC_G_CROP");
		return -1;
	}
	printf("Old Crop (%d, %d) (%d, %d)\n",
		crop.c.left, crop.c.top, crop.c.width, crop.c.height);

	if (set) { 
		crop.c.left = left;
		crop.c.top = top;
		if (set == 2) {
			crop.c.width = width;
			crop.c.height = height;
		}
		ret = ioctl (fd, VIDIOC_S_CROP, &crop);
		if (ret != 0) {
			perror("VIDIOC_S_CROP");
			return -1;
		}
		/* read back */
		ret = ioctl (fd, VIDIOC_G_CROP, &crop);
		if (ret != 0) {
			perror("VIDIOC_G_CROP");
			return -1;
		}
		printf("New Video Crop (%d, %d) (%d, %d)\n",
			crop.c.left, crop.c.top, crop.c.width, crop.c.height);
	}

	close(fd);
}




