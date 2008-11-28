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

#define V4L2_BUF_TYPE_STILL_CAPTURE 	V4L2_BUF_TYPE_PRIVATE

static void usage(void)
{
	printf("ioctl2 <size> <format>\n");
	printf("\tTo set the still image size and format\n");
	printf("\tsize = QQCIF|QQVGA|QCIF|QVGA|CIF|VGA|D1PAL|\n");
	printf("\t       D1NTSC|SVGA|XGA|SXGA|QXGA|<width> <height>\n");
	printf("\tformat = YUYV|UYVY|RGB565|RGB555|RGB565X|RGB555X\n");
}

int main (int argc, char *argv[])
{
    	int fd;
	struct v4l2_format format;
	int ret = 0;
	int index = 0;

	if ((argc > 1) && (!strcmp(argv[1], "?"))) {
		usage();
		return 0;
	}

	if (argc < 3 || argc > 4) {
		usage();
		return 0;
	}
	if ((fd = open_cam_device(O_RDONLY)) <= 0) {
		printf ("Could not open the cam device\n");
		return -1;
	}

	/* get the current format of the still picture captured */
	format.type = V4L2_BUF_TYPE_STILL_CAPTURE;
	ret = ioctl(fd, VIDIOC_G_FMT, &format);
	if (ret < 0) {
		perror("VIDIOC_G_FMT use V4L2_BUF_TYPE_STILL_CAPTUR");
		format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		ret = ioctl(fd, VIDIOC_G_FMT, &format);
		if (ret < 0) {
			perror("VIDIOC_G_FMT");
			return -1;
		}
	}

	if (!strcmp (argv[++index], "QQCIF")) {
		format.fmt.pix.width = 88;
		format.fmt.pix.height = 72;
	}
	else if (!strcmp (argv[index], "QQVGA")) {
		format.fmt.pix.width = 160;
		format.fmt.pix.height = 120;
	}
	else if (!strcmp (argv[index], "QCIF")) {
		format.fmt.pix.width = 176;
		format.fmt.pix.height = 144;
	}
	else if (!strcmp (argv[index], "QVGA")) {
		format.fmt.pix.width = 320;
		format.fmt.pix.height = 240;
    	}
	else if (!strcmp (argv[index], "CIF")) {
		format.fmt.pix.width = 352;
		format.fmt.pix.height = 288;
	}
	else if (!strcmp (argv[index], "VGA")) {
		format.fmt.pix.width = 640;
		format.fmt.pix.height = 480;
	}
	else if (!strcmp (argv[index], "D1PAL")) {
		format.fmt.pix.width = 720;
		format.fmt.pix.height = 576;
	}
	else if (!strcmp (argv[index], "D1NTSC")) {
		format.fmt.pix.width = 720;
		format.fmt.pix.height = 486;
	}
	else if (!strcmp (argv[index], "SVGA")) {
		format.fmt.pix.width = 800;
		format.fmt.pix.height = 600;
	}
	else if (!strcmp (argv[index], "XGA")) {
		format.fmt.pix.width = 1024;
		format.fmt.pix.height = 768;
	}
	else if (!strcmp (argv[index], "SXGA")) {
		format.fmt.pix.width = 1280;
		format.fmt.pix.height = 960;
	}
	else if (!strcmp (argv[index], "UXGA")) {
		format.fmt.pix.width = 1600;
		format.fmt.pix.height = 1200;
	}
	else if (!strcmp (argv[index], "QXGA")) {
		format.fmt.pix.width = 2048;
		format.fmt.pix.height = 1536;
	}
	else if (argc > 3) {
		format.fmt.pix.width = atoi(argv[index]);
		format.fmt.pix.height = atoi(argv[++index]);
	}
	else {
		printf("unsupported size!\n");
		return -1;
	}
	
	if (!strcmp (argv[++index], "YUYV"))
		format.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
	else if (!strcmp (argv[index], "UYVY"))
		format.fmt.pix.pixelformat = V4L2_PIX_FMT_UYVY;
	else if (!strcmp (argv[index], "RGB565"))
		format.fmt.pix.pixelformat = V4L2_PIX_FMT_RGB565;
	else if (!strcmp (argv[index], "RGB555"))
		format.fmt.pix.pixelformat = V4L2_PIX_FMT_RGB555;
	else if (!strcmp (argv[index], "RGB565X"))
		format.fmt.pix.pixelformat = V4L2_PIX_FMT_RGB565X;
	else if (!strcmp (argv[index], "RGB555X"))
		format.fmt.pix.pixelformat = V4L2_PIX_FMT_RGB555X;
	else {
		printf("unsupported format!\n");
		return -1;
	}

	/* set size & format of the picture captured */
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
	printf ("New Still Image: ");
	print_image_size_format(&format);

	close(fd);
}
