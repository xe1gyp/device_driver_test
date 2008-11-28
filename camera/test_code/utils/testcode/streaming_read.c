/* =========================================================================
*             Texas Instruments OMAP(TM) Platform Software
*  (c) Copyright Texas Instruments, Incorporated.  All Rights Reserved.
*
*  Use of this software is controlled by the terms and conditions found 
*  in the license agreement under which this software has been supplied.
* ========================================================================== */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <linux/fb.h>
#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/videodev.h>

#define V4L2_BUF_TYPE_STILL_CAPTURE 	V4L2_BUF_TYPE_PRIVATE
#define VIDIOC_S_OMAP2_ROTATION         _IOW ('V', 3,  int)
#define VIDEO_DEVICE1 "/dev/video1"

int fd, levb = 4, levc = 4, i;

struct screen_info_struct {
	int fd;
	char *data;
	int width;
	int height;
	struct fb_fix_screeninfo fbfix;
	struct fb_var_screeninfo info;
} screen_info;

struct {
	void *start;
	size_t length;
} *vbuffers, *cbuffers;

struct v4l2_buffer cfilledbuffer, vfilledbuffer, rbuffer;
struct v4l2_requestbuffers creqbuf, vreqbuf;
struct v4l2_format cformat, vformat;
struct v4l2_capability capability;
struct v4l2_queryctrl queryctrl;
struct v4l2_control control;
struct v4l2_cropcap cropcap;
struct v4l2_crop crop;

int openDev()
{
	screen_info.fd = open(VIDEO_DEVICE1, O_RDWR);
	if (screen_info.fd <= 0) {
		printf("Could no open the device %s\n", VIDEO_DEVICE1);
	} else
		printf("openned %s for rendering\n", VIDEO_DEVICE1);

	if (ioctl(screen_info.fd, VIDIOC_QUERYCAP, &capability) == -1) {
		perror("video VIDIOC_QUERYCAP");
		exit(EXIT_FAILURE);
	}
	if (capability.capabilities & V4L2_CAP_STREAMING) {
		printf("The video driver is capable of Streaming!\n");
	} else {
		printf("The video driver is not capable of Streaming!\n");
		exit(EXIT_FAILURE);
	}

	if ((fd = open_cam_device(O_RDWR)) <= 0) {
		printf("Could not open the cam device\n");
		exit(EXIT_FAILURE);
	}
	return 0;
}

int setImgFormat(char *f)
{
	cformat.type = V4L2_BUF_TYPE_STILL_CAPTURE;
	if (ioctl(fd, VIDIOC_G_FMT, &cformat) < 0) {
		perror("VIDIOC_G_FMT use V4L2_BUF_TYPE_STILL_CAPTUR");
		cformat.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		if (ioctl(fd, VIDIOC_G_FMT, &cformat) < 0) {
			perror("VIDIOC_G_FMT");
			exit(EXIT_FAILURE);
		}
	}

	if (!strcmp(f, "YUYV"))
		cformat.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
	else if (!strcmp(f, "UYVY"))
		cformat.fmt.pix.pixelformat = V4L2_PIX_FMT_UYVY;
	else if (!strcmp(f, "RGB565"))
		cformat.fmt.pix.pixelformat = V4L2_PIX_FMT_RGB565;
	else if (!strcmp(f, "RGB555"))
		cformat.fmt.pix.pixelformat = V4L2_PIX_FMT_RGB555;
	else if (!strcmp(f, "RGB565X"))
		cformat.fmt.pix.pixelformat = V4L2_PIX_FMT_RGB565X;
	else if (!strcmp(f, "RGB555X"))
		cformat.fmt.pix.pixelformat = V4L2_PIX_FMT_RGB555X;
	else {
		printf("unsupported image cformat!\n");
		exit(EXIT_FAILURE);
	}

	if (ioctl(fd, VIDIOC_S_FMT, &cformat) < 0) {
		perror("VIDIOC_S_FMT");
		exit(EXIT_FAILURE);
	}

	vformat.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
	if (ioctl(screen_info.fd, VIDIOC_G_FMT, &vformat) < 0) {
		perror("video VIDIOC_G_FMT");
		return -1;
	}
	vformat.fmt.pix.width = cformat.fmt.pix.width;
	vformat.fmt.pix.height = cformat.fmt.pix.height;
	vformat.fmt.pix.sizeimage = cformat.fmt.pix.sizeimage;
	vformat.fmt.pix.pixelformat = cformat.fmt.pix.pixelformat;

	int degree = 270;
	if (ioctl(screen_info.fd, VIDIOC_S_OMAP2_ROTATION, &degree) < 0) {
		perror("VIDIOC_S_OMAP2_ROTATION");
	}
	printf("Rotation set to %d degree\n", degree);
	if (ioctl(screen_info.fd, VIDIOC_S_FMT, &vformat) < 0) {
		perror("video VIDIOC_S_FMT");
		exit(EXIT_FAILURE);
	}

	if ((cformat.fmt.pix.width != vformat.fmt.pix.width) ||
	    (cformat.fmt.pix.height != vformat.fmt.pix.height) ||
	    (cformat.fmt.pix.sizeimage != vformat.fmt.pix.sizeimage) ||
	    (cformat.fmt.pix.pixelformat != vformat.fmt.pix.pixelformat)) {
		printf("can't make camera and video image compatible!\n");
		return 0;
	}

	if (ioctl(fd, VIDIOC_G_FMT, &cformat) < 0) {
		perror("VIDIOC_G_FMT");
		exit(EXIT_FAILURE);
	}
	printf("New Still Image: ");
	print_image_size_format(&cformat);

	return 0;
}

int setVidFormat(char *f)
{
	int degree = 0;

	cformat.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	if (ioctl(fd, VIDIOC_G_FMT, &cformat) < 0) {
		perror("VIDIOC_G_FMT");
		exit(EXIT_FAILURE);
	}

	if (!strcmp(f, "YUYV"))
		cformat.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
	else if (!strcmp(f, "UYVY"))
		cformat.fmt.pix.pixelformat = V4L2_PIX_FMT_UYVY;
	else if (!strcmp(f, "RGB565"))
		cformat.fmt.pix.pixelformat = V4L2_PIX_FMT_RGB565;
	else if (!strcmp(f, "RGB555"))
		cformat.fmt.pix.pixelformat = V4L2_PIX_FMT_RGB555;
	else if (!strcmp(f, "RGB565X"))
		cformat.fmt.pix.pixelformat = V4L2_PIX_FMT_RGB565X;
	else if (!strcmp(f, "RGB555X"))
		cformat.fmt.pix.pixelformat = V4L2_PIX_FMT_RGB555X;
	else {
		printf("unsupported video cformat!\n");
		exit(EXIT_FAILURE);
	}

	vformat.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
	if (ioctl(screen_info.fd, VIDIOC_G_FMT, &vformat) < 0) {
		perror("video VIDIOC_G_FMT");
		exit(EXIT_FAILURE);
	}
	vformat.fmt.pix.width = cformat.fmt.pix.width;
	vformat.fmt.pix.height = cformat.fmt.pix.height;
	vformat.fmt.pix.sizeimage = cformat.fmt.pix.sizeimage;
	vformat.fmt.pix.pixelformat = cformat.fmt.pix.pixelformat;
	if (ioctl(screen_info.fd, VIDIOC_S_FMT, &vformat) < 0) {
		perror("video VIDIOC_S_FMT");
		exit(EXIT_FAILURE);
	}

	if (ioctl(screen_info.fd, VIDIOC_S_OMAP2_ROTATION, &degree) < 0) {
		perror("VIDIOC_S_OMAP2_ROTATION");
	}
	printf("Rotation set to %d degree\n", degree);
	if (ioctl(screen_info.fd, VIDIOC_S_FMT, &vformat) < 0) {
		perror("video VIDIOC_S_FMT");
		exit(EXIT_FAILURE);
	}

	if ((cformat.fmt.pix.width != vformat.fmt.pix.width) ||
	    (cformat.fmt.pix.height != vformat.fmt.pix.height) ||
	    (cformat.fmt.pix.sizeimage != vformat.fmt.pix.sizeimage) ||
	    (cformat.fmt.pix.pixelformat != vformat.fmt.pix.pixelformat)) {
		printf("can't make camera and video image compatible!\n");
		return 0;
	}
	if (ioctl(fd, VIDIOC_S_FMT, &cformat) < 0) {
		perror("VIDIOC_S_FMT");
		exit(EXIT_FAILURE);
	}
	printf("New Video Image: ");
	print_image_size_format(&cformat);
}

void setB(int b)
{
	memset(&queryctrl, 0, sizeof(queryctrl));
	queryctrl.id = V4L2_CID_BRIGHTNESS;
	if (ioctl(fd, VIDIOC_QUERYCTRL, &queryctrl) == -1) {
		printf("Brightness is not supported!\n");
		exit(EXIT_FAILURE);
	}

	if (b < queryctrl.minimum || b > queryctrl.maximum) {
		printf("Brightness out of range!\n");
		exit(EXIT_FAILURE);
	}

	memset(&control, 0, sizeof(control));
	control.id = V4L2_CID_BRIGHTNESS;
	if (ioctl(fd, VIDIOC_G_CTRL, &control) == -1) {
		printf("VIDIOC_G_CTRL failed!\n");
		exit(EXIT_FAILURE);
	}
	control.value = b;
	printf("Setting brightness level to %d\n", b);
	if (ioctl(fd, VIDIOC_S_CTRL, &control) == -1) {
		printf("VIDIOC_S_CTRL failed!\n");
		exit(EXIT_FAILURE);
	}
}

void setC(int c)
{
	memset(&queryctrl, 0, sizeof(queryctrl));
	memset(&control, 0, sizeof(control));

	queryctrl.id = control.id = V4L2_CID_CONTRAST;
	if (ioctl(fd, VIDIOC_QUERYCTRL, &queryctrl) == -1) {
		printf("Contrast is not supported!\n");
		exit(EXIT_FAILURE);
	}

	if (ioctl(fd, VIDIOC_G_CTRL, &control) == -1) {
		printf("VIDIOC_G_CTRL failed!\n");
		exit(EXIT_FAILURE);
	}

	if (c < queryctrl.minimum || c > queryctrl.maximum) {
		printf("Contrast out of range!\n");
		exit(EXIT_FAILURE);
	}

	printf("Setting contrast level to %d\n", c);
	control.value = c;
	control.id = V4L2_CID_CONTRAST;
	if (ioctl(fd, VIDIOC_S_CTRL, &control) == -1) {
		printf("VIDIOC_S_CTRL failed!\n");
		exit(EXIT_FAILURE);
	}
}

void setCropImg(int l, int t, int w, int h)
{
	cropcap.type = V4L2_BUF_TYPE_STILL_CAPTURE;
	if (ioctl(fd, VIDIOC_CROPCAP, &cropcap) != 0) {
		perror("VIDIOC_CROPCAP V4L2_BUF_TYPE_STILL_CAPTURE"
			" not supported\n");
		cropcap.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		if (ioctl(fd, VIDIOC_CROPCAP, &cropcap) != 0) {
			perror("VIDIOC_CROPCAP");
			exit(EXIT_FAILURE);
		}
	}

	crop.type = cropcap.type;
	if (ioctl(fd, VIDIOC_G_CROP, &crop) != 0) {
		perror("VIDIOC_G_CROP");
		exit(EXIT_FAILURE);
	}

	crop.c.left = l;
	crop.c.top = t;
	crop.c.width = w;
	crop.c.height = h;

	if (ioctl(fd, VIDIOC_S_CROP, &crop) != 0) {
		perror("VIDIOC_S_CROP");
		exit(EXIT_FAILURE);
	}

	if (ioctl(fd, VIDIOC_G_CROP, &crop) != 0) {
		perror("VIDIOC_G_CROP");
		exit(EXIT_FAILURE);
	}

	printf("New Still Image Crop (%d, %d) (%d, %d)\n",
	       crop.c.left, crop.c.top, crop.c.width, crop.c.height);
}

void setCropVid(int l, int t, int w, int h)
{
	cropcap.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	if (ioctl(fd, VIDIOC_CROPCAP, &cropcap) != 0) {
		perror("VIDIOC_CROPCAP");
		exit(EXIT_FAILURE);
	}
	crop.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	if (ioctl(fd, VIDIOC_G_CROP, &crop) != 0) {
		perror("VIDIOC_G_CROP");
		exit(EXIT_FAILURE);
	}

	crop.c.left = l;
	crop.c.top = t;
	crop.c.width = w;
	crop.c.height = h;

	if (ioctl(fd, VIDIOC_S_CROP, &crop) != 0) {
		perror("VIDIOC_S_CROP");
		exit(EXIT_FAILURE);
	}

	if (ioctl(fd, VIDIOC_G_CROP, &crop) != 0) {
		perror("VIDIOC_G_CROP");
		exit(EXIT_FAILURE);
	}
	printf("New Video Crop (%d, %d) (%d, %d)\n",
	       crop.c.left, crop.c.top, crop.c.width, crop.c.height);
}

void reqVidBuf(int b)
{
	vreqbuf.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
	vreqbuf.memory = V4L2_MEMORY_MMAP;
	vreqbuf.count = b;

	if (ioctl(screen_info.fd, VIDIOC_REQBUFS, &vreqbuf) == -1) {
		perror("Video VIDIO_REQBUFS");
		exit(EXIT_FAILURE);
	}

	printf("Video Driver allocated %d buffers when %d are requested\n",
	       vreqbuf.count, b);

	vbuffers = calloc(vreqbuf.count, sizeof(*vbuffers));

	for (i = 0; i < vreqbuf.count; i++) {
		struct v4l2_buffer buffer;
		buffer.type = vreqbuf.type;
		buffer.index = i;
		if (ioctl(screen_info.fd, VIDIOC_QUERYBUF, &buffer) == -1) {
			perror("video VIDIOC_QUERYBUF");
			exit(EXIT_FAILURE);
		}
		vbuffers[i].length = buffer.length;
		vbuffers[i].start =
		    mmap(NULL, buffer.length, PROT_READ | PROT_WRITE,
			 MAP_SHARED, screen_info.fd, buffer.m.offset);

		if (vbuffers[i].start == MAP_FAILED) {
			perror("video map");
			exit(EXIT_FAILURE);
		}

		printf("Video Buffers[%d].start = %x length = %d\n", i,
		       vbuffers[i].start, vbuffers[i].length);
	}
}

void reqCamBufMMAP(int b)
{
	creqbuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	creqbuf.memory = V4L2_MEMORY_MMAP;
	creqbuf.count = b;
	printf("Requesting %d buffers of type V4L2_MEMORY_MMAP\n", b);

	if (ioctl(fd, VIDIOC_REQBUFS, &creqbuf) < 0) {
		perror("VIDEO_REQBUFS");
		exit(EXIT_FAILURE);
	}

	printf("Camera Driver allowed buffers reqbuf.count = %d\n",
	       creqbuf.count);

	cbuffers = calloc(creqbuf.count, sizeof(*cbuffers));

	for (i = 0; i < creqbuf.count; ++i) {
		struct v4l2_buffer buffer;
		memset(&buffer, 0, sizeof(buffer));
		buffer.type = creqbuf.type;
		buffer.memory = V4L2_MEMORY_MMAP;
		buffer.index = i;
		if (ioctl(fd, VIDIOC_QUERYBUF, &buffer) < 0) {
			perror("cam VIDIOC_QUERYBUF");
			exit(EXIT_FAILURE);
		}
		cbuffers[i].length = buffer.length;
		cbuffers[i].start = mmap(NULL, buffer.length, PROT_READ |
					 PROT_WRITE, MAP_SHARED, fd,
					 buffer.m.offset);
		if (cbuffers[i].start == MAP_FAILED) {
			perror("mmap");
			exit(EXIT_FAILURE);
		}
		printf("Mapped Buffers[%d].start = %x  length = %d\n",
		       i, cbuffers[i].start, cbuffers[i].length);

		if (ioctl(fd, VIDIOC_QBUF, &buffer) < 0) {
			perror("cam VIDIOC_QBUF");
			exit(EXIT_FAILURE);
		}
	}
}

void reqCamBufUSER(int b)
{
	creqbuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	creqbuf.memory = V4L2_MEMORY_USERPTR;
	creqbuf.count = b;
	printf("Requesting %d buffers of type V4L2_MEMORY_USERPTR\n", b);
	if (ioctl(fd, VIDIOC_REQBUFS, &creqbuf) < 0) {
		perror("cam VIDEO_REQBUFS");
		exit(EXIT_FAILURE);
	}
	printf("Camera Driver allowed %d buffers\n", creqbuf.count);
	cbuffers = calloc(creqbuf.count, sizeof(*cbuffers));
	for (i = 0; i < creqbuf.count; ++i) {
		struct v4l2_buffer buffer;
		buffer.type = creqbuf.type;
		buffer.memory = creqbuf.memory;
		buffer.index = i;
		if (ioctl(fd, VIDIOC_QUERYBUF, &buffer) < 0) {
			perror("cam VIDIOC_QUERYBUF");
			exit(EXIT_FAILURE);
		}

		buffer.length = cbuffers[i].length = vbuffers[i].length;
		buffer.flags = 0;

		if (cbuffers[i].length & 0xfff) {
			cbuffers[i].length =
			    (cbuffers[i].length & 0xfffff000) + 0x1000;
		}
		buffer.length = cbuffers[i].length;
		cbuffers[i].start = malloc(cbuffers[i].length);
		buffer.m.userptr =
		    ((unsigned int)cbuffers[i].start & 0xffffffe0) + 0x20;
		memset(cbuffers[i].start, 0, cbuffers[i].length);

		printf("Camera Buf[%d].start = 0x%x  len = %d\n", i,
		       cbuffers[i].start, cbuffers[i].length);

		if (ioctl(fd, VIDIOC_QBUF, &buffer) < 0) {
			perror("cam VIDIOC_QBUF");
			exit(EXIT_FAILURE);
		}
	}
}

void stream()
{
	if (ioctl(fd, VIDIOC_STREAMON, &creqbuf.type) < 0) {
		perror("cam VIDIOC_STREAMON");
		exit(EXIT_FAILURE);
	}

	if (ioctl(screen_info.fd, VIDIOC_STREAMON, &vreqbuf.type) < 0) {
		perror("video VIDIOC_STREAMON");
		exit(EXIT_FAILURE);
	}

	setB(levb);
	setC(levc);

	cfilledbuffer.type = creqbuf.type;
	vfilledbuffer.type = vreqbuf.type;

	i = 0;

	while (i < 100) {

		while (ioctl(fd, VIDIOC_DQBUF, &cfilledbuffer) < 0) {
			perror("cam VIDIOC_DQBUF");
			while (ioctl
			       (screen_info.fd, VIDIOC_QBUF,
				&cfilledbuffer) < 0) {
				perror("VIDIOC_QBUF***");
			}
		}

		i++;

		if (creqbuf.memory == V4L2_MEMORY_MMAP) {
			memcpy(vbuffers[cfilledbuffer.index].start,
			       (cbuffers[cfilledbuffer.index].start),
			       cfilledbuffer.length);
		} else if (creqbuf.memory == V4L2_MEMORY_USERPTR) {
			memcpy(vbuffers[cfilledbuffer.index].start,
			       ((unsigned int)cbuffers[cfilledbuffer.index].
				start & 0xffffffe0) + 0x20,
			       cfilledbuffer.length);
		}

		if (vfilledbuffer.index != -1) {
			if (ioctl(screen_info.fd, VIDIOC_DQBUF, &vfilledbuffer)
			    < 0) {
				perror("cam VIDIOC_DQBUF");
				exit(EXIT_FAILURE);
			}
		}
		vfilledbuffer.index = cfilledbuffer.index;
		if (ioctl(screen_info.fd, VIDIOC_QBUF, &vfilledbuffer) == -1) {
			perror("video VIDIOC_QBUF");
			exit(EXIT_FAILURE);
		}

		while (ioctl(fd, VIDIOC_QBUF, &cfilledbuffer) < 0) {
			perror("CAM VIDIOC_QBUF");
		}
	}

	if (ioctl(fd, VIDIOC_STREAMOFF, &creqbuf.type) == -1) {
		perror("cam VIDIOC_STREAMOFF");
		exit(EXIT_FAILURE);
	}

	if (ioctl(screen_info.fd, VIDIOC_STREAMOFF, &vreqbuf.type) == -1) {
		perror("video VIDIOC_STREAMOFF");
		exit(EXIT_FAILURE);
	}

	for (i = 0; i < vreqbuf.count; i++) {
		if (vbuffers[i].start) {
			munmap(vbuffers[i].start, vbuffers[i].length);
		}
	}

	free(vbuffers);

	for (i = 0; i < creqbuf.count; i++) {
		if (cbuffers[i].start) {
			if (creqbuf.memory == V4L2_MEMORY_USERPTR)
				free(cbuffers[i].start);
			else
				munmap(cbuffers[i].start, cbuffers[i].length);
		}
	}

	free(cbuffers);
}

void capture()
{
	void *data_start;
	data_start = malloc(cformat.fmt.pix.sizeimage + 0x20);
	if (data_start == NULL) {
		printf("can't allocate memory!\n");
		exit(EXIT_FAILURE);
	}
	while ((((int)data_start) & 0x1f) != 0)
		data_start++;
	printf("User buffer start = 0x%lx\n", data_start);

	if (read(fd, data_start, cformat.fmt.pix.sizeimage) <= 0) {
		perror("READ");
		exit(EXIT_FAILURE);
	}

	vformat.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
	if (ioctl(screen_info.fd, VIDIOC_G_FMT, &vformat) < 0) {
		perror("video VIDIOC_G_FMT");
		exit(EXIT_FAILURE);
	}

	vreqbuf.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
	vreqbuf.memory = V4L2_MEMORY_MMAP;
	vreqbuf.count = 1;
	if (ioctl(screen_info.fd, VIDIOC_REQBUFS, &vreqbuf) == -1) {
		perror("video ss VIDEO_REQBUFS");
		exit(EXIT_FAILURE);
	}
	printf("Video Driver allocated 1 buffer\n");

	rbuffer.type = vreqbuf.type;
	rbuffer.index = 0;
	if (ioctl(screen_info.fd, VIDIOC_QUERYBUF, &rbuffer) == -1) {
		perror("video VIDIOC_QUERYBUF");
		exit(EXIT_FAILURE);
	}
	data_start = mmap(NULL, rbuffer.length, PROT_READ |
			  PROT_WRITE, MAP_SHARED,
			  screen_info.fd, rbuffer.m.offset);
	if (data_start == MAP_FAILED) {
		perror("video mmap");
		exit(EXIT_FAILURE);
	}

	if (ioctl(screen_info.fd, VIDIOC_STREAMON, &vreqbuf.type) < 0) {
		perror("video VIDIOC_STREAMON");
		exit(EXIT_FAILURE);
	}

	if (read(fd, data_start, cformat.fmt.pix.sizeimage) < 0) {
		perror("READ");
		exit(EXIT_FAILURE);
	} else {
		printf("Read Done\n");
	}

	if (ioctl(screen_info.fd, VIDIOC_QBUF, &rbuffer) == -1) {
		perror("video VIDIOC_QBUF");
		exit(EXIT_FAILURE);
	}

	sleep(5);

	if (ioctl(screen_info.fd, VIDIOC_DQBUF, &rbuffer) < 0) {
		perror("cam VIDIOC_DQBUF");
		exit(EXIT_FAILURE);
	}

	if (ioctl(screen_info.fd, VIDIOC_STREAMOFF, &vreqbuf.type) == -1) {
		perror("video VIDIOC_STREAMOFF");
		exit(EXIT_FAILURE);
	}

	munmap(data_start, rbuffer.length);

	vreqbuf.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
	vreqbuf.memory = V4L2_MEMORY_MMAP;
	vreqbuf.count = 0;
	if (ioctl(screen_info.fd, VIDIOC_REQBUFS, &vreqbuf) == -1) {
		perror("video ss VIDEO_REQBUFS");
		exit(EXIT_FAILURE);
	}
	printf("Video Driver request 0 buffers\n");
	munmap(screen_info.data, screen_info.width * screen_info.height * 2);

}

static void usage(void)
{
	printf("streaming_read <img_format> <vid_format>"
		" <brightness> <contrast>\n              "
		" <img_left> <img_top> <img_width> <img_height>\n"
		"               <vid_left> <vid_top> <vid_width> "
		"<vid_height> [<user>]\n");
	printf("To capture an one-shot image while streaming"
		" a video the user must\n");
	printf("indicate image format, video format, brightness level, "
		"contrast level\n");
	printf("crop for image and video and if MMAP or user buffer is used\n");
	printf("\timg_format = YUYV|UYVY|RGB565|RGB555|RGB565X|RGB555X\n");
	printf("\tvid_format = YUYV|UYVY|RGB565|RGB555|RGB565X|RGB555X\n");
	printf("\tbringness = A integer number between 1 and 15\n");
	printf("\tcontrast = A integer number between 1 and 15\n");
	printf("\timg_left, img_top, img_width and img_height set crop "
		"rectangle for still image. \n");
	printf("\tvid_left, vid_top, vid_width and vid_height set crop "
		"rectangle for video image. \n");
	printf("\t[<user>] for user alloacted buffers, otherwise driver "
		"alloacted buffers\n");
}

int main(int argc, char *argv[])
{

	if (((argc > 1) && (!strcmp(argv[1], "?")))
		|| (argc < 13 && argc != 1)) {
		usage();
		return 0;
	}

	if (argc == 1) {
		openDev();
		setVidFormat("YUYV");
		reqVidBuf(4);
		reqCamBufMMAP(4);
		stream();
		setImgFormat("YUYV");
		capture();
		setVidFormat("YUYV");
		reqVidBuf(4);
		reqCamBufMMAP(4);
		stream();
		close(fd);
		close(screen_info.fd);
		return 0;
	}

	if (argc == 14 && !strcmp(argv[13], "user")) {
		levb = atoi(argv[3]);
		levc = atoi(argv[4]);
		openDev();
		setVidFormat(argv[2]);
		reqVidBuf(4);
		reqCamBufUSER(4);
		setCropVid(atoi(argv[9]), atoi(argv[10]), atoi(argv[11]),
			   atoi(argv[12]));
		stream();
		setImgFormat(argv[1]);
		setCropImg(atoi(argv[5]), atoi(argv[6]), atoi(argv[7]),
			   atoi(argv[8]));
		capture();
		setVidFormat(argv[2]);
		reqVidBuf(4);
		reqCamBufUSER(4);
		setCropVid(atoi(argv[9]), atoi(argv[10]), atoi(argv[11]),
			   atoi(argv[12]));
		stream();
		close(fd);
		close(screen_info.fd);
		return 0;
	}

	if (argc == 13) {
		levb = atoi(argv[3]);
		levc = atoi(argv[4]);
		openDev();
		setVidFormat(argv[2]);
		reqVidBuf(4);
		reqCamBufMMAP(4);
		setCropVid(atoi(argv[9]), atoi(argv[10]), atoi(argv[11]),
			   atoi(argv[12]));
		stream();
		setImgFormat(argv[1]);
		setCropImg(atoi(argv[5]), atoi(argv[6]), atoi(argv[7]),
			   atoi(argv[8]));
		capture();
		setVidFormat(argv[2]);
		reqVidBuf(4);
		reqCamBufMMAP(4);
		setCropVid(atoi(argv[9]), atoi(argv[10]), atoi(argv[11]),
			   atoi(argv[12]));
		stream();
		close(fd);
		close(screen_info.fd);
		return 0;
	}
}
