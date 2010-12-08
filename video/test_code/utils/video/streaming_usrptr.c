#include <stdio.h>
#include <fcntl.h>
#include <linux/ioctl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <linux/videodev2.h>
#include <linux/errno.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include "lib.h"

static int getBpp(unsigned long pixelformat)
{
	int bpp = 0;
	switch (pixelformat) {
	case V4L2_PIX_FMT_YUYV:
	case V4L2_PIX_FMT_UYVY:
		bpp = 2;
		break;
	case V4L2_PIX_FMT_RGB565:
	case V4L2_PIX_FMT_RGB565X:
		bpp = 2;
		break;
	case V4L2_PIX_FMT_RGB24:
		bpp = 3;
		break;
	case V4L2_PIX_FMT_RGB32:
	case V4L2_PIX_FMT_BGR32:
		bpp = 4;
		break;
	case V4L2_PIX_FMT_NV12:
		bpp = 1;
		break;
	default:
		bpp = -1;
	}
	return bpp;
}

static int streaming_video(int vdevice, int vdevice_linked,
	int input_file, int buff_num, int sleep_time, int cache_flush_able)
{

	struct {
		void *start;
		size_t length;
	} *buffers;

	struct v4l2_capability capability;
	struct v4l2_format format;
	struct v4l2_buffer fbuffer, fbuffer_linked;
	struct v4l2_requestbuffers reqbuf, reqbuf_user;
	int i, count, result, read_count = -1;
	int j , k, bpp, page_width;

	memset(&reqbuf_user, 0, sizeof(reqbuf_user));
	reqbuf_user.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
	reqbuf_user.memory = V4L2_MEMORY_USERPTR;
	reqbuf_user.count = buff_num;

	sleep(1);
	result = ioctl(vdevice_linked, VIDIOC_REQBUFS, &reqbuf_user);

	if (result != 0) {
		if (errno == EINVAL)
			printf("Video capturing or user pointer\
						streaming is not supported\n");
		else
			perror("VIDIOC_REQBUFS");
	}


	result = ioctl(vdevice, VIDIOC_QUERYCAP, &capability);
	if (result != 0) {
		perror("VIDIOC_QUERYCAP");
		return 1;
	}

	if (capability.capabilities & V4L2_CAP_STREAMING == 0) {
		printf("VIDIOC_QUERYCAP indicated that driver is not "
			"capable of Streaming \n");
		return 1;
	}

	format.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
	result = ioctl(vdevice, VIDIOC_G_FMT, &format);

	if (result != 0) {
		perror("VIDIOC_G_FMT");
		return result;
	}

	reqbuf.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
	reqbuf.memory = V4L2_MEMORY_MMAP;
	reqbuf.count = buff_num;

	result = ioctl(vdevice, VIDIOC_REQBUFS, &reqbuf);
	if (result != 0) {
		perror("VIDEO_REQBUFS");
		return 1;
	}

	printf("Driver allocated %d buffers when %d are requested\n",
		reqbuf.count, buff_num);

	buffers = calloc(reqbuf.count, sizeof(*buffers));
	for (i = 0; i < reqbuf.count ; ++i) {
		struct v4l2_buffer buffer;
		buffer.type = reqbuf.type;
		buffer.index = i;

		result = ioctl(vdevice, VIDIOC_QUERYBUF, &buffer);
		if (result != 0) {
			perror("VIDIOC_QUERYBUF");
			return 1;
		}
		printf("%d: buffer.length=%d, buffer.m.offset=%d\n",
				i, buffer.length, buffer.m.offset);
		buffers[i].length = buffer.length;
		buffers[i].start = mmap(NULL, buffer.length, PROT_READ|
						PROT_WRITE, MAP_SHARED,
						vdevice, buffer.m.offset);
		if (buffers[i].start == MAP_FAILED) {
			perror("mmap");
			return 1;
		}

		printf("Buffers[%d].start = %x  length = %d\n", i,
			buffers[i].start, buffers[i].length);
	}

	sleep(2);
	fbuffer.type = reqbuf.type;
	fbuffer.memory = V4L2_MEMORY_MMAP;
	fbuffer.flags = 0;

	fbuffer_linked.type = reqbuf.type;
	fbuffer_linked.memory = V4L2_MEMORY_USERPTR;
	fbuffer_linked.flags = 0;

	printf("\n pix attribs: width=%0x, height=%0x, imagesize=%0x, format=%0x\n",
			format.fmt.pix.width, format.fmt.pix.height,
			format.fmt.pix.sizeimage, format.fmt.pix.pixelformat);

	bpp = getBpp(format.fmt.pix.pixelformat);

	if (-1 == bpp) {
		perror("format not supported");
		return 1;
	}


	for (i = 0; i <= buff_num - 1; i++) {
		fbuffer.index = i;
#ifndef OMAP_4430
		if (read(input_file, buffers[i].start,
			format.fmt.pix.sizeimage) != format.fmt.pix.sizeimage) {
			perror("read");
			return 1;
		}
#else
		page_width = (format.fmt.pix.width * bpp +
				4096 - 1) & ~(4096 - 1);
		printf("\nfirst bpp = %0x, page_width = %0x\n", bpp, page_width);
		/* page_width calculation is to account for 4k / 8k based on input size */
		/* for NV12, bpp is set to 1 for the Y-buffer*/
		for (j = 0; j < format.fmt.pix.height; j++) {
			if (read(input_file, buffers[i].start + (j*page_width),
				(format.fmt.pix.width * bpp)) != (format.fmt.pix.width * bpp)) {
				printf("current address = 0x%0x\n", buffers[i].start + (j*page_width));
				perror("first read");
				read_count = i;
				break;
			}
		}

		/* handle UV buffer filling also now */
		if (V4L2_PIX_FMT_NV12 == format.fmt.pix.pixelformat) {
			/* bpp for UV buffer is 2 */
			page_width = (format.fmt.pix.width * 2 +
				4096 - 1) & ~(4096 - 1);
			for (j = format.fmt.pix.height; j < (format.fmt.pix.height * 3 / 2); j++) {
				if (read(input_file, buffers[i].start +
					(j*page_width),
					(format.fmt.pix.width)) /* * 2 */
					!= (format.fmt.pix.width)) { /* * 2 */
					/* printf("current address = 0x%0x\n", buffers[0].start +
					(j*page_width)); */
					perror("first UV read");
					read_count = i;
					break;
				}
			}
		}

		if (read_count != -1)
			break;
#endif

		result = ioctl(vdevice, VIDIOC_QBUF, &fbuffer);
		if (result != 0) {
			perror("VIDIOC_QBUF");
			return 1;
		}

		fbuffer_linked.index = i;
		fbuffer_linked.m.userptr = (unsigned long)buffers[i].start;
		fbuffer_linked.length = buffers[i].length;

		result = ioctl(vdevice_linked, VIDIOC_QBUF, &fbuffer_linked);
		if (result != 0) {
			perror("VIDIOC_QBUF");
			return 1;
		}



	}

	if (read_count != -1 && read_count <= 2) {
		printf("There are less than 2 buffers in the file\n");
		return 1;
	}

	if (read_count == -1)
		count = read_count = reqbuf.count;
	else
		count = read_count;

	result = ioctl(vdevice, VIDIOC_STREAMON, &reqbuf.type);
	if (result != 0) {
		perror("VIDIOC_STREAMON");
		return 1;
	}

	result = ioctl(vdevice_linked, VIDIOC_STREAMON, &reqbuf.type);
	if (result != 0) {
		perror("VIDIOC_STREAMON");
		return 1;
	}

	while (count < 2000) {
		/* delay some for frame rate control */
#ifdef OMAP_4430
		for (k = 0; k <= read_count - 2; k++) {
#endif
			if (sleep_time)
				sleep(sleep_time);
			else
				for (i = 0; i < 20; i++)
					;

			result = ioctl(vdevice, VIDIOC_DQBUF, &fbuffer);
			if (result != 0) {
				perror("VIDIOC_DQBUF");
				return 1;
			}


			result = ioctl(vdevice_linked, VIDIOC_DQBUF, &fbuffer_linked);
			if (result != 0) {
				perror("VIDIOC_DQBUF");
				return 1;
			}
#ifdef OMAP_4430
		}

		page_width = (format.fmt.pix.width * bpp +
				4096 - 1) & ~(4096 - 1);

		printf("\ninside while: bpp = %0x, page_width = %0x\n", bpp, page_width);

		for (k = 0; k <= read_count - 2; k++) {

			fbuffer.index = count % reqbuf.count;

			for (j = 0; j < format.fmt.pix.height; j++) {

				i = read(input_file, buffers[count%reqbuf.count].start +
					(j*page_width), format.fmt.pix.width * bpp);

				if (i < 0) {
					printf("current address = 0x%0x\n",
						buffers[count%reqbuf.count].start + (j*page_width));
					perror(" second read");
					return 1;
				}

				if (i != format.fmt.pix.width * bpp)
					goto exit;	/* we are done */
			}

			/* handle UV buffer filling also now */
			if (V4L2_PIX_FMT_NV12 == format.fmt.pix.pixelformat) {
				/* bpp for UV buffer is 2 */
				page_width = (format.fmt.pix.width * 2 +
					4096 - 1) & ~(4096 - 1);

				for (j = format.fmt.pix.height; j < (format.fmt.pix.height * 3 / 2);
					j++) {
					i = read(input_file, buffers[count%reqbuf.count].start +
						(j*page_width), format.fmt.pix.width); /* *2 */

					if (i < 0) {
						printf("current address = 0x%0x\n",
							buffers[count%reqbuf.count].start + (j*page_width));
						perror("second UV read");
						return 1;
					}

					if (i != format.fmt.pix.width)
						goto exit; /* we are done */
				}
			}
#else
			i = read(input_file, buffers[count%reqbuf.count].start,
			format.fmt.pix.sizeimage);

			if (i < 0) {
				perror("read");
				return 1;
			}
			if (i != format.fmt.pix.sizeimage)
			goto exit; /* we are done */

			fbuffer.index = count % reqbuf.count;
#endif

			if (ioctl(vdevice, VIDIOC_QBUF, &fbuffer) != 0) {
				perror("VIDIOC_QBUF2");
				return 1;
			}

			fbuffer_linked.index = fbuffer.index;
			fbuffer_linked.m.userptr = (unsigned long)
						buffers[fbuffer_linked.index].start;
			fbuffer_linked.length = buffers[fbuffer_linked.index].length;
			result = ioctl(vdevice_linked, VIDIOC_QBUF, &fbuffer_linked);
			if (result != 0) {
				perror("VIDIOC_QBUF2_LINKED");
				return 1;
			}

			count++;
#ifdef OMAP_4430
		}
#endif
	}

	if (ioctl(vdevice, VIDIOC_DQBUF, &fbuffer) != 0) {
		perror("VIDIOC_DQBUF2");
		return 1;
	}

	if (ioctl(vdevice_linked, VIDIOC_DQBUF, &fbuffer_linked) != 0) {
		perror("VIDIOC_DQBUF2_LINKED");
		return 1;
	}

exit:

	result = ioctl(vdevice_linked, VIDIOC_STREAMOFF, &reqbuf_user.type);

	if (result != 0) {
		perror("VIDIOC_STREAMOFF");
		return 1;
	}

	for (i = 0; i < reqbuf.count; i++) {
		if (buffers[i].start)
			munmap(buffers[i].start, buffers[i].length);
	}

	result = ioctl(vdevice, VIDIOC_STREAMOFF, &reqbuf.type);
	if (result != 0) {
		perror("VIDIOC_STREAMOFF");
		return 1;
	}

	return 0;
}

static int usage(void)
{
	printf("Usage: streaming <video_device> <inputfile> [<n_sleep>] [<n_bufcount>]\n");
	return 1;
}

int main(int argc, char *argv[])
{
	int video_device, video_device_linked, input_file,
		fd, fd_linked, result, i;
	int buff_num = 4;
	int sleep_time = 0;
	int cache_flush_able = 0;

	if (argc < 3)
		return usage();

	video_device = atoi(argv[1]);

	if ((video_device != 1) && (video_device != 2)) {
		printf("video_device has to be 1 or 2! video_device=%d, "
			"argv[1]=%s\n", video_device, argv[1]);
		return usage();
	}

	video_device_linked = (video_device == 1) ? 2 : 1;

	fd = open((video_device == 1) ? VIDEO_DEVICE1 : VIDEO_DEVICE2,
				O_RDWR);
	if (fd <= 0) {
		printf("Could not open %s\n",
			(video_device == 1) ? VIDEO_DEVICE1 : VIDEO_DEVICE2);
		return 1;
	} else {
		printf("openned %s\n",
			(video_device == 1) ? VIDEO_DEVICE1 : VIDEO_DEVICE2);
	}

	fd_linked = open((video_device_linked == 1) ?
			VIDEO_DEVICE1 :	VIDEO_DEVICE2, O_RDWR);

	if (fd_linked <= 0) {
		printf("Could not open %s\n",
			(video_device_linked == 1) ? VIDEO_DEVICE1 :
				VIDEO_DEVICE2);
		return 1;
	} else {
		printf("openned %s\n",
			(video_device_linked == 1) ?
				VIDEO_DEVICE1 :
				VIDEO_DEVICE2);
	}


	if (atoi(argv[2]) == 0) {
		i = 0;
	} else {
		i = 1;
		buff_num = atoi(argv[2]);
		if (buff_num < 4)
			buff_num = 4;
	}

	input_file = open(argv[2+i], O_RDONLY);
	if (input_file <= 0) {
		printf("Could not open input file %s\n", argv[2]);
		return 1;
	}

	if (argc == 4+i)
		sleep_time = atoi(argv[3+i]);

	if (argc == 5+i)
		cache_flush_able = atoi(argv[4+i]);

	result = streaming_video(fd, fd_linked, input_file,
				buff_num, sleep_time, cache_flush_able);

	close(input_file);
	close(fd);
	close(fd_linked);

	return result;
}
