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

#define BUFFERS_IN_QUEUE 2

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
	struct v4l2_requestbuffers reqbuf;
	int i, count, result;
	struct v4l2_requestbuffers reqbuf_user;

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
	sleep(1);

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

	reqbuf.reserved[0] = cache_flush_able;
	reqbuf.reserved[1] = cache_flush_able;

	result = ioctl(vdevice, VIDIOC_REQBUFS, &reqbuf);
	if (result != 0) {
		perror("VIDEO_REQBUFS");
		return 1;
	}

	printf("Driver allocated %d buffers when %d are requested\n",
		buff_num, reqbuf.count);

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

	for (i = 0; i < BUFFERS_IN_QUEUE; i++) {

		fbuffer.index = i;
		if (read(input_file, buffers[i].start,
			format.fmt.pix.sizeimage) != format.fmt.pix.sizeimage) {
			perror("read");
			return 1;
		}

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

	count = BUFFERS_IN_QUEUE;
	while (count < 2000) {
		/* delay some for frame rate control */
		if (sleep_time)
			sleep(sleep_time);
		else {
			for (i = 0; i < 2000000; i++)
				;
		}

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
		i = read(input_file, buffers[count%reqbuf.count].start,
			format.fmt.pix.sizeimage);

		if (i < 0) {
			perror("read");
			return 1;
		}
		if (i != format.fmt.pix.sizeimage)
			goto exit; /* we are done */

		fbuffer.index = count % reqbuf.count;
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
	}

	if (ioctl(vdevice, VIDIOC_DQBUF, &fbuffer) != 0) {
		perror("VIDIOC_DQBUF2");
		return 1;
	}

	if (ioctl(vdevice_linked, VIDIOC_DQBUF,
			&fbuffer_linked) != 0) {
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
		perror("VIDIOC_STREAMOFF_LINKED");
		return 1;
	}

	return 0;
}

static int usage(void)
{
	printf("Usage: streaming_usrptr <video_device> "
		"[<buff_num: 4 by default>] <inputfile> [<n>]\n");
	return 1;
}

int main(int argc, char *argv[])
{
	int video_device, video_device_linked, file_descriptor,
		fd, fd_linked, input_file, result, i;
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
