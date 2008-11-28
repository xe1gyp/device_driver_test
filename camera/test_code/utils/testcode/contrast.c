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
#include <errno.h>
#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/videodev.h>


#define VIDEO_DEVICE "/dev/video0"

int main (int argc, char *argv[])
{
	struct v4l2_queryctrl queryctrl;
	struct v4l2_control control;
	int fd;
	int level;

	if ((fd = open (VIDEO_DEVICE, O_RDONLY)) <= 0){
		printf("Could not open the device " VIDEO_DEVICE "\n") ;
		return -1;
	}

	memset(&queryctrl, 0, sizeof(queryctrl));
	memset(&control, 0, sizeof(control));

	queryctrl.id = V4L2_CID_CONTRAST;
	if (ioctl(fd, VIDIOC_QUERYCTRL, &queryctrl) == -1) {
		printf("Contrast is not supported!\n");
		return 0;
	}
	
	control.id = V4L2_CID_CONTRAST;
	if (ioctl(fd, VIDIOC_G_CTRL, &control) == -1) {
		printf("VIDIOC_G_CTRL failed!\n");
		return 0;
	}
	printf("Contrast is supported, min %d, max %d.\nOld contrast level is %d\n",
		queryctrl.minimum, queryctrl.maximum, control.value);

	if (argc == 1)
		return 0;

	level = atoi(argv[1]);
	if (level < queryctrl.minimum || level > queryctrl.maximum) {
		printf("Out of range!\n");
		return 0;
	}

	printf("Setting contrast level to %d\n", level);
	control.value = level;
	if (ioctl(fd, VIDIOC_S_CTRL, &control) == -1) {
		printf("VIDIOC_S_CTRL failed!\n");
		return 0;
	}
	printf("Done\n");

	close(fd) ;
}
