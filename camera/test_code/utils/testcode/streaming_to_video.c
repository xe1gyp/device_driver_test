/* ========================================================================
*             Texas Instruments OMAP(TM) Platform Software
*  (c) Copyright Texas Instruments, Incorporated.  All Rights Reserved.
*
*  Use of this software is controlled by the terms and conditions found
*  in the license agreement under which this software has been supplied.
* ========================================================================= */

#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <linux/ioctl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <linux/videodev2.h>
#include <linux/errno.h>
#include <errno.h>

#define VIDIOC_S_OMAP2_ROTATION         _IOW('V', 3,  int)
#define V4L2_BUF_TYPE_STILL_CAPTURE 	V4L2_BUF_TYPE_PRIVATE
#define VIDEO_DEVICE1 "/dev/video1"
#define VIDEO_DEVICE2 "/dev/video2"

static void usage(void)
{
	printf("streaming_to_video [vid]\n");
	printf("\tSteaming capture of 1000 frames using video driver for"
								" rendering\n");
	printf("\t[vid] is the video pipeline to be used. Valid vid is 1"
							"(default) or 2\n");
}

int main(int argc, char *argv[])
{
	struct {
		void *start;
		size_t length;
	} *vbuffers;
	struct v4l2_capability capability;
	struct v4l2_format cformat, vformat;
	struct v4l2_requestbuffers creqbuf, vreqbuf;
	struct v4l2_buffer cfilledbuffer, vfilledbuffer, buffer;
	int cfd, vfd;
	int vid = 1, set_video_img = 0, i, ret, degree;
	void *data_start, *buf;
	if ((argc > 1) && (!strcmp(argv[1], "?"))) {
		usage();
		return 0;
	}
	if (argc > 1) {
		vid = atoi(argv[1]);
		if ((vid != 1) && (vid != 2)) {
			printf("vid has to be 1 or 2! vid=%d, argv[1]=%s\n",
								vid, argv[1]);
			usage();
			return 0;
		}
	}
	cfd = open_cam_device(O_RDWR);
	if (cfd <= 0) {
		printf("Could not open the cam device\n");
		return -1;
	}

	vfd = open((vid == 1) ? VIDEO_DEVICE1 : VIDEO_DEVICE2, O_RDWR);
	if (vfd <= 0) {
		printf("Could not open %s\n",
			(vid == 1) ? VIDEO_DEVICE1 : VIDEO_DEVICE2);
		return -1;
	}
	printf("openned %s for rendering\n",
		(vid == 1) ? VIDEO_DEVICE1 : VIDEO_DEVICE2);

	if (ioctl(vfd, VIDIOC_QUERYCAP, &capability) == -1) {
		perror("video VIDIOC_QUERYCAP");
		return -1;
	}
	if (capability.capabilities & V4L2_CAP_STREAMING)
		printf("The video driver is capable of Streaming!\n");
	else {
		printf("The video driver is not capable of Streaming!\n");
		return -1;
	}

	if (ioctl(cfd, VIDIOC_QUERYCAP, &capability) < 0) {
		perror("VIDIOC_QUERYCAP");
		return -1;
	}
	if (capability.capabilities & V4L2_CAP_STREAMING)
		printf("The camera driver is capable of Streaming!\n");
	else {
		printf("The camera driver is not capable of Streaming!\n");
		return -1;
	}

	cformat.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	ret = ioctl(cfd, VIDIOC_G_FMT, &cformat);
	if (ret < 0) {
		perror("cam VIDIOC_G_FMT");
		return -1;
	}
	printf("Camera Image width = %d, Image height = %d, size = %d\n",
			cformat.fmt.pix.width, cformat.fmt.pix.height,
			cformat.fmt.pix.sizeimage);

	vformat.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
	ret = ioctl(vfd, VIDIOC_G_FMT, &vformat);
	if (ret < 0) {
		perror("video VIDIOC_G_FMT");
		return -1;
	}
	printf("Video Image width = %d, Image height = %d, size = %d\n",
			vformat.fmt.pix.width, vformat.fmt.pix.height,
			vformat.fmt.pix.sizeimage);

	if ((cformat.fmt.pix.width != vformat.fmt.pix.width) ||
	    (cformat.fmt.pix.height != vformat.fmt.pix.height)) {
		printf("image sizes don't match!\n");
		set_video_img = 1;
	}
	if (cformat.fmt.pix.pixelformat != vformat.fmt.pix.pixelformat) {
		printf("pixel formats don't match!\n");
		set_video_img = 1;
	}

	if (set_video_img) {
		printf("set video image the same as camera image ...\n");
		vformat.fmt.pix.width = cformat.fmt.pix.width;
		vformat.fmt.pix.height = cformat.fmt.pix.height;
		vformat.fmt.pix.sizeimage = cformat.fmt.pix.sizeimage;
		vformat.fmt.pix.pixelformat = cformat.fmt.pix.pixelformat;
		ret = ioctl(vfd, VIDIOC_S_FMT, &vformat);
		if (ret < 0) {
			perror("video VIDIOC_S_FMT");
			return -1;
		}
		if ((cformat.fmt.pix.width != vformat.fmt.pix.width) ||
		    (cformat.fmt.pix.height != vformat.fmt.pix.height) ||
		    (cformat.fmt.pix.pixelformat !=
		     vformat.fmt.pix.pixelformat)) {
			printf("can't make camera and video image "
							"compatible!\n");
			return 0;
		}

	}

	vreqbuf.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
	vreqbuf.memory = V4L2_MEMORY_MMAP;
	vreqbuf.count = 4;
	if (ioctl(vfd, VIDIOC_REQBUFS, &vreqbuf) == -1) {
		perror("video VIDEO_REQBUFS");
		return;
	}
	printf("Video Driver allocated %d buffers when 4 are requested\n",
								vreqbuf.count);

	vbuffers = calloc(vreqbuf.count, sizeof(*vbuffers));
	for (i = 0; i < vreqbuf.count ; ++i) {
		struct v4l2_buffer buffer;
		buffer.type = vreqbuf.type;
		buffer.index = i;
		if (ioctl(vfd, VIDIOC_QUERYBUF, &buffer) == -1) {
			perror("video VIDIOC_QUERYBUF");
			return;
		}
/*
		printf("video %d: buffer.length=%d, buffer.m.offset=%d\n",
				i, buffer.length, buffer.m.offset);
*/
		vbuffers[i].length = buffer.length;
		vbuffers[i].start = mmap(NULL, buffer.length, PROT_READ|
						PROT_WRITE, MAP_SHARED,
						vfd, buffer.m.offset);
		if (vbuffers[i].start == MAP_FAILED) {
			perror("video mmap");
			return;
		}
		printf("Video Buffers[%d].start = %x  length = %d\n", i,
					vbuffers[i].start, vbuffers[i].length);
	}

	creqbuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	creqbuf.memory = V4L2_MEMORY_USERPTR;
	creqbuf.count = 4;
	printf("Requesting %d buffers of type V4L2_MEMORY_USERPTR\n",
								creqbuf.count);
	if (ioctl(cfd, VIDIOC_REQBUFS, &creqbuf) < 0) {
		perror("cam VIDEO_REQBUFS");
		return -1;
	}
	printf("Camera Driver allowed %d buffers\n", creqbuf.count);

	for (i = 0; i < creqbuf.count; ++i) {
		struct v4l2_buffer buffer;
		buffer.type = creqbuf.type;
		buffer.memory = creqbuf.memory;
		buffer.index = i;
		if (ioctl(cfd, VIDIOC_QUERYBUF, &buffer) < 0) {
			perror("cam VIDIOC_QUERYBUF");
			return -1;
		}

		buffer.flags = 0;
		buffer.m.userptr = (unsigned long) vbuffers[i].start;
		buffer.length = vbuffers[i].length;
		if (ioctl(cfd, VIDIOC_QBUF, &buffer) < 0) {
			perror("cam VIDIOC_QBUF");
			return -1;
		}
	}

	/* turn on streaming on both drivers */
	if (ioctl(cfd, VIDIOC_STREAMON, &creqbuf.type) < 0) {
		perror("cam VIDIOC_STREAMON");
		return -1;
	}
	if (ioctl(vfd, VIDIOC_STREAMON, &vreqbuf.type) < 0) {
		perror("video VIDIOC_STREAMON");
		return -1;
	}

	/* caputure 1000 frames */
	cfilledbuffer.type = creqbuf.type;
	vfilledbuffer.type = vreqbuf.type;
	i = 0;
	vfilledbuffer.index = -1;
	while (i < 50) {
		/* De-queue the next filled buffer from camera */
		while (ioctl(cfd, VIDIOC_DQBUF, &cfilledbuffer) < 0) {
			perror("cam VIDIOC_DQBUF");
			while (ioctl(vfd, VIDIOC_QBUF, &cfilledbuffer) < 0)
				perror("VIDIOC_QBUF***");
		}
		i++;

		if (vfilledbuffer.index != -1) {
			/* De-queue the previous buffer from video driver */
			if (ioctl(vfd, VIDIOC_DQBUF, &vfilledbuffer) < 0) {
				perror("cam VIDIOC_DQBUF");
				return;
			}
		}



		vfilledbuffer.index = cfilledbuffer.index;
		/* Queue the new buffer to video driver for rendering */

		if (ioctl(vfd, VIDIOC_QBUF, &vfilledbuffer) == -1) {
			perror("video VIDIOC_QBUF");
			return;
		}

		/* queue the buffer back to camera */
		while (ioctl(cfd, VIDIOC_QBUF, &cfilledbuffer) < 0)
			perror("cam VIDIOC_QBUF");
	}
	printf("Captured and rendered %d frames!\n", i);

	if (ioctl(cfd, VIDIOC_STREAMOFF, &creqbuf.type) == -1) {
		perror("cam VIDIOC_STREAMOFF");
		return -1;
	}
	if (ioctl(vfd, VIDIOC_STREAMOFF, &vreqbuf.type) == -1) {
		perror("video VIDIOC_STREAMOFF");
		return -1;
	}
	/*  Read */

	buf = malloc(cformat.fmt.pix.sizeimage + 0x20);
	data_start = buf;
	if (buf == NULL) {
		printf("can't allocate memory!\n");
		return -1;
	}

	while ((((int)data_start) & 0x1f) != 0)
		data_start++;
	printf("User buffer start = 0x%lx\n", data_start);

	ret = read(cfd, data_start, cformat.fmt.pix.sizeimage);
	if (ret <= 0) {
		perror("READ");
		return -1;
	}
	printf("Read Done!\n");

	munmap(data_start, buffer.length);

	/* End of Read */

	for (i = 0; i < vreqbuf.count; i++) {
		if (vbuffers[i].start)
			munmap(vbuffers[i].start, vbuffers[i].length);
	}

	free(vbuffers);
	close(cfd);
	close(vfd);
}
