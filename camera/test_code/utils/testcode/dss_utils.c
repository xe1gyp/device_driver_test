#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <linux/videodev2.h>

#define VIDEO_DEVICE1 "/dev/video1"
#define VIDEO_DEVICE2 "/dev/video2"

#define WIDTH_720P			1280
#define HEIGHT_720P			720
#define DISPLAY_WIDTH		352
#define DISPLAY_HEIGHT		288


static int dss_set_rotation(int vfd, int degree)
{
	struct v4l2_control control;
	int err;

	/* Set up rotation */
	memset(&control, 0 , sizeof(control));
	control.id = V4L2_CID_ROTATE;
	control.value = degree;

	err = ioctl(vfd, VIDIOC_S_CTRL, &control);
	if (err != 0) {
		perror("VIDIOC_S_CTRL");
		return -1;
	}

	printf("DSS: Rotation set to %d degree\n", degree);
	return 0;
}

static int dss_set_img(int vfd, int fmt, int w, int h)
{
	struct v4l2_format format;
	int err;

	memset(&format, 0 , sizeof(format));
	format.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
	err = ioctl(vfd, VIDIOC_G_FMT, &format);
	if (err != 0) {
		perror("VIDIOC_G_FMT");
		return 1;
	}

	/* set format of the picture captured */
	format.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
	format.fmt.pix.width  = w;
	format.fmt.pix.height = h;

	err = ioctl(vfd, VIDIOC_S_FMT, &format);
	if (err != 0) {
		perror("VIDIOC_S_FMT");
		return -1;
	}

	printf("DSS: Set image window width=%d height=%d\n", w, h);
	return 0;
}

static int dss_set_crop(int vfd, int left, int top, int width, int height)
{
	struct v4l2_crop crop;
	int err;

	crop.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
	err = ioctl(vfd, VIDIOC_G_CROP, &crop);
	if (err != 0) {
		perror("VIDIOC_S_CROP");
		return 1;
	}

	memset(&crop, 0 , sizeof(crop));
	crop.c.left   = left;
	crop.c.top    = top;
	crop.c.width  = width;
	crop.c.height = height;
	crop.type     = V4L2_BUF_TYPE_VIDEO_OUTPUT;

	err = ioctl(vfd, VIDIOC_S_CROP, &crop);
	if (err != 0) {
		perror("VIDIOC_S_CROP");
		return 1;
	}

	printf("DSS: Set crop window left=%d top=%d width=%d "
		"height=%d\n", left, top, width, height);
	return 0;
}

static int dss_set_win(int vfd, int left, int top, int width, int height)
{
	struct v4l2_format format;
	int err;

	format.type = V4L2_BUF_TYPE_VIDEO_OVERLAY;
	err = ioctl(vfd, VIDIOC_G_FMT, &format);
	if (err != 0) {
		perror("VIDIOC_G_FMT");
		return -1;
	}

	format.fmt.win.w.left = left;
	format.fmt.win.w.top = top;
	format.fmt.win.w.width = width;
	format.fmt.win.w.height = height;

	err = ioctl(vfd, VIDIOC_S_FMT, &format);
	if (err != 0) {
		perror("VIDIOC_S_FMT");
		return -1;
	}

	printf("DSS: Set display window left=%d top=%d "
		"width=%d heigth=%d\n", left, top, width, height);
	return 0;
}

int dss_setup_720p(int vid)
{
	int vfd;

	if ((vid != 1) && (vid != 2)) {
		printf("video_device has to be 1 or 2!\n");
		return -1;
	}

	vfd = open((vid == 1) ? VIDEO_DEVICE1 : VIDEO_DEVICE2, O_RDONLY);
	if (vfd <= 0) {
		printf("Could not open %s\n",
			(vid == 1) ? VIDEO_DEVICE1 : VIDEO_DEVICE2);
		return -1;
	}

	/* 90 degree rotation */
	dss_set_rotation(vfd, 0);

	/* set format YUYV 1280x720 */
	dss_set_img(vfd, V4L2_PIX_FMT_YUYV, WIDTH_720P, HEIGHT_720P);

	/* set crop 1280x720 */
	dss_set_crop(vfd, 0, 0, WIDTH_720P, HEIGHT_720P);

	/* set display window */
	dss_set_win(vfd, 0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT);

	close(vfd);
	return 0;
}

int dss_reset(int vid)
{
	int vfd;

	if ((vid != 1) && (vid != 2)) {
		printf("video_device has to be 1 or 2!\n");
		return -1;
	}

	vfd = open((vid == 1) ? VIDEO_DEVICE1 : VIDEO_DEVICE2, O_RDONLY);
	if (vfd <= 0) {
		printf("Could not open %s\n",
			(vid == 1) ? VIDEO_DEVICE1 : VIDEO_DEVICE2);
		return -1;
	}

	dss_set_win(vfd, 0, 40, 480, 720);
	dss_set_crop(vfd, 0, 0, 480, 720);
	dss_set_img(vfd, V4L2_PIX_FMT_YUYV, 160, 120);
	dss_set_rotation(vfd, 0);

	close(vfd);
	return 0;
}
