/* ===========================================================================.
*             Texas Instruments OMAP(TM) Platform Software
*  (c) Copyright Texas Instruments, Incorporated.  All Rights Reserved.
*
*  Use of this software is controlled by the terms and conditions found
*  in the license agreement under which this software has been supplied.
* ==========================================================================.*/

#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <string.h>
#include <linux/videodev2.h>
#include <errno.h>

int cam_ioctl(int fd, char *pixFormat, char *size, char *sizeH)
{
	struct v4l2_format format;

	int ret = 0;
	int index = 0;

	/* get the current format of the video capture */
	format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	ret = ioctl(fd, VIDIOC_G_FMT, &format);
	if (ret < 0) {
		perror("VIDIOC_G_FMT");
		return ret;
	}

	if (!strcmp(size, "QQCIF")) {
		format.fmt.pix.width = 88;
		format.fmt.pix.height = 72;
	} else if (!strcmp(size, "SQCIF")) {
		format.fmt.pix.width = 128;
		format.fmt.pix.height = 96;
	} else if (!strcmp(size, "QQVGA")) {
		format.fmt.pix.width = 160;
		format.fmt.pix.height = 120;
	} else if (!strcmp(size, "QCIF")) {
		format.fmt.pix.width = 176;
		format.fmt.pix.height = 144;
	} else if (!strcmp(size, "QVGA")) {
		format.fmt.pix.width = 320;
		format.fmt.pix.height = 240;
	} else if (!strcmp(size, "CIF")) {
		format.fmt.pix.width = 352;
		format.fmt.pix.height = 288;
	} else if (!strcmp(size, "VGA")) {
		format.fmt.pix.width = 640;
		format.fmt.pix.height = 480;
	} else if (!strcmp(size, "D1PAL")) {
		format.fmt.pix.width = 720;
		format.fmt.pix.height = 576;
	} else if (!strcmp(size, "D1NTSC")) {
		format.fmt.pix.width = 720;
		format.fmt.pix.height = 486;
	} else if (!strcmp(size, "SVGA")) {
		format.fmt.pix.width = 800;
		format.fmt.pix.height = 600;
	} else if (!strcmp(size, "XGA")) {
		format.fmt.pix.width = 1024;
		format.fmt.pix.height = 768;
	} else if (!strcmp(size, "SXGA")) {
		format.fmt.pix.width = 1280;
		format.fmt.pix.height = 960;
	} else if (!strcmp(size, "UXGA")) {
		format.fmt.pix.width = 1600;
		format.fmt.pix.height = 1200;
	} else if (!strcmp(size, "QXGA")) {
		format.fmt.pix.width = 2048;
		format.fmt.pix.height = 1536;
	} else if (!strcmp(size, "5MP")) {
		format.fmt.pix.width = 2592;
		format.fmt.pix.height = 1944;
	} else if (strcmp(sizeH, "")) {
		format.fmt.pix.width = atoi(size);
		format.fmt.pix.height = atoi(sizeH);
	} else {
		printf("Unsupported size!\n");
		return -EINVAL;
	}

	if (!strcmp(pixFormat, "YUYV"))
		format.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
	else if (!strcmp(pixFormat, "UYVY"))
		format.fmt.pix.pixelformat = V4L2_PIX_FMT_UYVY;
	else if (!strcmp(pixFormat, "RGB565"))
		format.fmt.pix.pixelformat = V4L2_PIX_FMT_RGB565;
	else if (!strcmp(pixFormat, "RGB555"))
		format.fmt.pix.pixelformat = V4L2_PIX_FMT_RGB555;
	else if (!strcmp(pixFormat, "RGB565X"))
		format.fmt.pix.pixelformat = V4L2_PIX_FMT_RGB565X;
	else if (!strcmp(pixFormat, "RGB555X"))
		format.fmt.pix.pixelformat = V4L2_PIX_FMT_RGB555X;
	else if (!strcmp(pixFormat, "SGRBG10"))
		format.fmt.pix.pixelformat = V4L2_PIX_FMT_SGRBG10;
	else if (!strcmp(pixFormat, "SRGGB10"))
		format.fmt.pix.pixelformat = V4L2_PIX_FMT_SRGGB10;
	else if (!strcmp(pixFormat, "SBGGR10"))
		format.fmt.pix.pixelformat = V4L2_PIX_FMT_SBGGR10;
	else if (!strcmp(pixFormat, "SGBRG10"))
		format.fmt.pix.pixelformat = V4L2_PIX_FMT_SGBRG10;
	else {
		printf("unsupported pixel format!\n");
		return -EINVAL;
	}

	/* set size & format of the video image */
	ret = ioctl(fd, VIDIOC_S_FMT, &format);
	if (ret < 0) {
		perror("VIDIOC_S_FMT");
		return ret;
	}

	/* read back */
	format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	ret = ioctl(fd, VIDIOC_G_FMT, &format);
	if (ret < 0) {
		perror("VIDIOC_G_FMT");
		return ret;
	}
	printf("New video image: ");
	print_image_size_format(&format);
	print_preview_window(&format);

	return 0;
}

int validateSize(char *size)
{
	if (!strcmp(size, "QQCIF"))
		return 0;
	else if (!strcmp(size, "SQCIF"))
		return 0;
	else if (!strcmp(size, "QQVGA"))
		return 0;
	else if (!strcmp(size, "QCIF"))
		return 0;
	else if (!strcmp(size, "QVGA"))
		return 0;
	else if (!strcmp(size, "CIF"))
		return 0;
	else if (!strcmp(size, "VGA"))
		return 0;
	else if (!strcmp(size, "D1PAL"))
		return 0;
	else if (!strcmp(size, "D1NTSC"))
		return 0;
	else if (!strcmp(size, "SVGA"))
		return 0;
	else if (!strcmp(size, "XGA"))
		return 0;
	else if (!strcmp(size, "SXGA"))
		return 0;
	else if (!strcmp(size, "UXGA"))
		return 0;
	else if (!strcmp(size, "QXGA"))
		return 0;
	else if (!strcmp(size, "5MP"))
		return 0;
	else
		return -EINVAL;
}
