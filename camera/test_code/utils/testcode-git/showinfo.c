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
#define VIDIOC_S_OVERLAY_ROT		_IOW ('O', 1,  int)
#define VIDIOC_G_OVERLAY_ROT		_IOR ('O', 2,  int)
 
static void usage(void)
{
	printf("showinfo\n");
	printf("\tshow the current camera image setting info\n");
}

int main (int argc, char *argv[])
{
	int fd;
	struct v4l2_format format;
	struct v4l2_cropcap cropcap;
	struct v4l2_crop crop;
	int preview_rotation;

	int ret = 0;	

	if ((argc > 1) && (!strcmp(argv[1], "?"))) {
		usage();
		return 0;
	}

	if ((fd = open_cam_device(O_RDONLY)) <= 0) {
		printf ("Could not open the cam device\n");
		return -1;
	}

	/* get the crop info of the still picture capture */
	cropcap.type = V4L2_BUF_TYPE_STILL_CAPTURE;
	ret = ioctl (fd, VIDIOC_CROPCAP, &cropcap);
	if (ret == 0)     
		printf("Still Image Crop bounds (%d, %d) (%d, %d), defrect (%d, %d) (%d, %d)\n",
		cropcap.bounds.left, cropcap.bounds.top,
		cropcap.bounds.width, cropcap.bounds.height,
		cropcap.defrect.left, cropcap.defrect.top,
		cropcap.defrect.width, cropcap.defrect.height);
	crop.type = V4L2_BUF_TYPE_STILL_CAPTURE;
	ret = ioctl (fd, VIDIOC_G_CROP, &crop);
	if (ret == 0)
		printf("Still Image Crop (%d, %d) (%d, %d)\n",
			crop.c.left, crop.c.top, crop.c.width, crop.c.height);

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
	printf ("Still Image: ");
	print_image_size_format(&format);

	/* get the crop info of the video picture capture */
	cropcap.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	ret = ioctl (fd, VIDIOC_CROPCAP, &cropcap);
	if (ret == 0)
		printf("Video Image Crop bounds (%d, %d) (%d, %d), defrect (%d, %d) (%d, %d)\n",
		cropcap.bounds.left, cropcap.bounds.top,
		cropcap.bounds.width, cropcap.bounds.height,
		cropcap.defrect.left, cropcap.defrect.top,
		cropcap.defrect.width, cropcap.defrect.height);
	
	crop.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	ret = ioctl (fd, VIDIOC_G_CROP, &crop);
	if (ret == 0)
		printf("Video Crop (%d, %d) (%d, %d)\n",
			crop.c.left, crop.c.top, crop.c.width, crop.c.height);

	/* get the current format of the video picture captured */
	format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	ret = ioctl(fd, VIDIOC_G_FMT, &format);
	if (ret < 0) {
		perror("VIDIOC_G_FMT");
		return -1;
	}
	printf ("Video Image: ");
	print_image_size_format(&format);

	crop.type = V4L2_BUF_TYPE_VIDEO_OVERLAY;
	ret = ioctl(fd, VIDIOC_G_CROP, &crop);
	if (ret < 0) {
		perror ("VIDIOC_G_CROP");
		return -1;
	}
	print_preview_crop(&crop);

	format.type = V4L2_BUF_TYPE_VIDEO_OVERLAY;
	ret = ioctl(fd, VIDIOC_G_FMT, &format);
	if (ret < 0) {
		perror ("VIDIOC_S_FMT");
		return -1;
	}
	print_preview_window(&format);

	ret = ioctl (fd, VIDIOC_G_OVERLAY_ROT, &preview_rotation);
	if (ret == 0) {
		if (preview_rotation)
			printf("Preview Rotation: %d\n",(preview_rotation-1)*90);
		else
			printf("Preview Rotation: No\n");
	}

	close(fd);
}
