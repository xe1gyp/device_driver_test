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


int show_info(int file_descriptor)
{
 	struct v4l2_format format;
	struct v4l2_crop crop;
	int result;
	
	format.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
	result = ioctl(file_descriptor, VIDIOC_G_FMT, &format);
	if (result != 0) {
		perror("VIDIOC_G_FMT");
		return 1;
	}
	printf("new picture width = %d\n", format.fmt.pix.width);
	printf("new picture height = %d\n", format.fmt.pix.height);
	printf("new picture colorspace = %x\n", format.fmt.pix.colorspace);
	printf("new picture pixelformat = ");
	switch (format.fmt.pix.pixelformat) {
	case V4L2_PIX_FMT_YUYV:
		printf("YUYV\n");
		break;
	case V4L2_PIX_FMT_UYVY:
		printf("UYVY\n");
		break;
	case V4L2_PIX_FMT_RGB565:
		printf("RGB565\n");
		break;
	case V4L2_PIX_FMT_RGB565X:
		printf("RGB565X\n");
		break;
	case V4L2_PIX_FMT_RGB32:
		printf("RGB32\n");
		break;
	default:
		printf("not supported\n");
	}

	crop.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
	result = ioctl(file_descriptor, VIDIOC_G_CROP, &crop);
	if (result != 0) {
		perror("VIDIOC_G_CROP");
		return 1;
	}
	printf("new crop left = %d\n", crop.c.left);
	printf("new crop top = %d\n", crop.c.top);
	printf("new crop width = %d\n", crop.c.width);
	printf("new crop height = %d\n", crop.c.height);
 
	format.type = V4L2_BUF_TYPE_VIDEO_OVERLAY;
	result = ioctl(file_descriptor, VIDIOC_G_FMT, &format);
	if (result != 0) {
		perror("VIDIOC_G_FMT");
		return 1;
	}
	printf("new window left = %d\n", format.fmt.win.w.left);
	printf("new window top = %d\n", format.fmt.win.w.top);
	printf("new window width = %d\n", format.fmt.win.w.width);
	printf("new window height = %d\n", format.fmt.win.w.height);

	return 0;
}
