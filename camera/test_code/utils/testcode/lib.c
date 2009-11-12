/* ========================================================================
*             Texas Instruments OMAP(TM) Platform Software
*  (c) Copyright Texas Instruments, Incorporated.  All Rights Reserved.
*
*  Use of this software is controlled by the terms and conditions found
*  in the license agreement under which this software has been supplied.
* ========================================================================== */

#include <stdio.h>

#include <linux/videodev2.h>

#define VIDEO_DEVICE_0 "/dev/video0"
#define VIDEO_DEVICE_4 "/dev/video4"
#define VIDEO_DEVICE_5 "/dev/video5"

int open_cam_device(int flag, int device)
{
	if (device == 1)
		return open(VIDEO_DEVICE_0, flag);
	else if (device == 2)
		return open(VIDEO_DEVICE_4, flag);
	else if (device == 3)
		return open(VIDEO_DEVICE_5, flag);

	printf("Wrong Cam device, specify a correct device (1, 2 or 3)\n");
	return -1;
}

void print_image_size_format(struct v4l2_format *format)
{
	printf("width = %d ", format->fmt.pix.width);
	printf("height = %d ", format->fmt.pix.height);
	printf("colorspace = %x ", format->fmt.pix.colorspace);
	printf("pixelformat = ");
	switch (format->fmt.pix.pixelformat) {
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
	case V4L2_PIX_FMT_RGB555:
		printf("RGB555\n");
		break;
	case V4L2_PIX_FMT_RGB555X:
		printf("RGB555X\n");
		break;
	case V4L2_PIX_FMT_SGRBG10:
		printf("RAW10\n");
		break;
	default:
		printf("unkown\n");
	}
}
void print_preview_crop(struct v4l2_crop *crop)
{
	printf("Preview Crop (%d, %d) (%d, %d)\n", crop->c.left,
		crop->c.top, crop->c.width, crop->c.height);
}

void print_preview_window(struct v4l2_format *format)
{
	printf("Preview Window (%d, %d) (%d, %d)\n",
		format->fmt.win.w.left, format->fmt.win.w.top,
		format->fmt.win.w.width, format->fmt.win.w.height);
}

/* counter-clock wise rotate 90 degree of the image at src_buf to frame buffer
   and crop the image to the passed screen size if it is too big */
void rotate_image(char *src_buf, int image_width, int image_height,
		   char *fb, int screen_width, int screen_height)
{
	int i, j, k, index;
	int row_size, total_size;

	/* j now points to the fb location that the top-left pix should go */
	row_size = image_width * 2;
	j = (image_width - 1) * screen_width * 2;
	if (screen_height < image_width) {
		/* chop the right part of the image */
		row_size = screen_height * 2;
		j = (screen_height - 1) * screen_width * 2;
	}

	total_size = image_width * image_height * 2;
	if (screen_width < image_height) {
		/* chop the bottom part of the image */
		total_size = screen_width * image_width * 2;
	}

	/* each iteration copies one pix row in the original image */
	for (i = 0; i < total_size; i += image_width * 2) {
		k = j;
		for (index = 0; index < row_size;) {
				/* copy one line */
				fb[k] =  src_buf[i + index];
				index++;
				fb[k+1] = src_buf[i + index];
				index++;
				k = k - screen_width * 2;
			}
		/* advance to next column in FB */
		j += 2;
	}
}
