/* =========================================================================
*             Texas Instruments OMAP(TM) Platform Software
*  (c) Copyright Texas Instruments, Incorporated.  All Rights Reserved.
*
*  Use of this software is controlled by the terms and conditions found
*  in the license agreement under which this software has been supplied.
* ========================================================================== */

#include <stdlib.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <stdint.h>
#include <fcntl.h>
#include <errno.h>
#include <linux/videodev2.h>

#define V4L2_CID_FLASH_TORCH_INTENSITY		(V4L2_CID_CAMERA_CLASS_BASE+19)

int main(void)
{
	int fd, ret, device = 1;
	struct v4l2_control v_tourch, v_timeout, v_strobe;

	/* Open the video device */
	fd = open_cam_device(O_RDWR, device);
	if (fd <= 0) {
		printf("Could not open the cam device\n");
		return -1;
	}

	/* Set flash timeout in [us] */
	v_timeout.id = V4L2_CID_FLASH_TIMEOUT;
	/* timeout = 1s */
	v_timeout.value = 1000000;

	if (ioctl(fd, VIDIOC_S_CTRL, &v_timeout) == -1) {
		printf("ioctl timeout error: %s\n", strerror(errno));
		return -1;
	}
	printf("Set timeout OK!\n");

	/* Turn on the torch */
	v_tourch.id = V4L2_CID_FLASH_TORCH_INTENSITY;
	/* 0=off , 1 = on */
	v_tourch.value = 1;

	if (ioctl(fd, VIDIOC_S_CTRL, &v_tourch) == -1) {
		printf("ioctl torch error: %s\n", strerror(errno));
		return -1;
	}
	printf("Torch on!\n");
	/* Keep the torch on for 3 sec so that the intensity peak
		of the strobe can be observed */
	sleep(3);

	/* Turn on the strobe */
	v_strobe.id = V4L2_CID_FLASH_STROBE;
	/* It's a button the value is irrelevant */
	v_strobe.value = 0;

	if (ioctl(fd, VIDIOC_S_CTRL, &v_strobe) == -1) {
		printf("ioctl strobe error: %s\n", strerror(errno));
		return -1;
	}
	printf("Flash on!\n");

	/* Test the GET control to querry the timeout */
	if (ioctl(fd, VIDIOC_G_CTRL, &v_timeout) == -1) {
		printf("ioctl get timeout error: %s\n", strerror(errno));
		return -1;
	}
	printf("timeout is %d\n", v_timeout.value);

	/*Close the device */
	close(fd);
}
