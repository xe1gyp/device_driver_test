/* ========================================================================
*             Texas Instruments OMAP(TM) Platform Software
*  (c) Copyright Texas Instruments, Incorporated.  All Rights Reserved.
*
*  Use of this software is controlled by the terms and conditions found
*  in the license agreement under which this software has been supplied.
* ========================================================================= */

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
#include <linux/videodev2.h>

int setFramerate(int fd, int framerate)
{
	struct v4l2_streamparm parm;
	int ret;

	parm.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	ret = ioctl(fd, VIDIOC_G_PARM, &parm);
	if (ret != 0) {
		perror("VIDIOC_G_PARM ");
		return -1;
	}
	printf("Old frame rate is %d/%d = %d fps\n",
		parm.parm.capture.timeperframe.denominator,
		parm.parm.capture.timeperframe.numerator,
		parm.parm.capture.timeperframe.denominator/
		parm.parm.capture.timeperframe.numerator);

	parm.parm.capture.timeperframe.numerator = 1;
	parm.parm.capture.timeperframe.denominator = framerate;
	ret = ioctl(fd, VIDIOC_S_PARM, &parm);
	if (ret != 0) {
		perror("VIDIOC_S_PARM ");
		return -1;
	}

	printf("New frame rate is %d/%d = %d fps\n",
		parm.parm.capture.timeperframe.denominator,
		parm.parm.capture.timeperframe.numerator,
		parm.parm.capture.timeperframe.denominator/
		parm.parm.capture.timeperframe.numerator);
	printf("Done\n");

	return 0;
}
