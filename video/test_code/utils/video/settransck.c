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

#include <linux/videodev2.h>
#define V4L2_FBUF_FLAG_SRC_CHROMAKEY 0x0040

#include "lib.h"

static int usage(void)
{
	printf("Usage: settransck < o/p device [1:LCD 2:TV]> "
		"<key type [0:Disabled 1:GFX DEST 2:VID SRC]> \
		<RGB key value>\n");
	printf("RGB Key value for 0xF801 -> 63489\n");
	return 1;
}

int main(int argc, char *argv[])
{
	int output_device, file_descriptor, result, cktype = 0;
	struct omap24xxvout_colorkey colorkey = {0};
	struct v4l2_framebuffer v4l2_fb;
	struct v4l2_format v4l2_fmt;
	int color_key = 0;

	if (argc < 4)
		return usage();

	output_device = atoi(argv[1]) + 3;

	if ((output_device != OMAP24XX_OUTPUT_LCD) &&
		(output_device != OMAP24XX_OUTPUT_TV)) {
		printf("Output device should be 1 for LCD or 2 for TV \n");
		return usage();
	}

	cktype	= atoi(argv[2]);

	if ((cktype != OMAP_GFX_DESTINATION) &&
		(cktype != OMAP_VIDEO_SOURCE) &&
		(cktype != 0)) {
		printf("Color Key type should be 0 for Disabled,\
			1 for GFX destination and 2 for VID src\n");
		return usage();
	}

	file_descriptor = open(VIDEO_DEVICE1, O_RDONLY);
	if (file_descriptor <= 0) {
		printf("Could not open %s\n", VIDEO_DEVICE1);
		return 1;
	} else {
		printf("openned %s\n", VIDEO_DEVICE1);
	}

	result = ioctl(file_descriptor, VIDIOC_G_FBUF, &v4l2_fb);
	if (result != 0) {
		perror("VIDIOC_G_FBUF");
		return 1;
	}

	v4l2_fmt.type = V4L2_BUF_TYPE_VIDEO_OVERLAY;
	result = ioctl(file_descriptor, VIDIOC_G_FMT, &v4l2_fmt);
	if (result != 0) {
		perror("VIDIOC_G_FMT");
		return 1;
	}

	color_key = atoi(argv[3]);
	if (v4l2_fmt.fmt.win.chromakey != color_key)
		v4l2_fmt.fmt.win.chromakey = color_key;

	result = ioctl(file_descriptor, VIDIOC_S_FMT, &v4l2_fmt);
	if (result != 0) {
		perror("VIDIOC_S_FMT");
		return 1;
	}

	switch (cktype) {
	case OMAP_GFX_DESTINATION:
		v4l2_fb.flags = V4L2_FBUF_FLAG_CHROMAKEY;
		break;
	case OMAP_VIDEO_SOURCE:
		v4l2_fb.flags = V4L2_FBUF_FLAG_SRC_CHROMAKEY;
		break;
	default:
		v4l2_fb.flags = 0;
	}

	result = ioctl(file_descriptor, VIDIOC_S_FBUF, &v4l2_fb);
	if (result != 0) {
		perror("VIDIOC_S_FBUF");
		return 1;
	}
	close(file_descriptor);
	return 0;
}
