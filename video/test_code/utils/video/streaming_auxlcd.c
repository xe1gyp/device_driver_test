#include <stdio.h>
#include <fcntl.h>
#include <linux/ioctl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <linux/videodev2.h>
#include <linux/errno.h>
#include <errno.h>
#include <stdlib.h>

#include "lib.h"
#include "omap24xxvout.h"

#define	VIDEO_DEVICE3	"/dev/video3"


static int streaming_video(int output_device, int file_descriptor,
	int sleep_time, int lcd_off)
{
	struct {
		void *start;
		size_t length;
	} *buffers;

	struct v4l2_capability capability;
	struct v4l2_format format;
	struct v4l2_buffer filledbuffer;
	struct v4l2_requestbuffers reqbuf;
	int i, count, result;

#if 1
	result = ioctl(output_device, VIDIOC_QUERYCAP, &capability);
	if (result != 0) {
		perror("VIDIOC_QUERYCAP");
		return 1;
	}

	if (capability.capabilities & V4L2_CAP_STREAMING == 0) {
		printf("VIDIOC_QUERYCAP indicated that driver");
		printf(" is not capable of Streaming \n");
		return 1;
	}

	format.fmt.pix.pixelformat = V4L2_PIX_FMT_RGB444;
	result = ioctl(output_device, VIDIOC_S_FMT, &format);

	if (result != 0) {
		fprintf(stdout, "pixel format = 0x%x \n", V4L2_PIX_FMT_RGB444);
		perror("VIDIOC_S_FMT");
		return 1;
	}

	format.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
	result = ioctl(output_device, VIDIOC_G_FMT, &format);
	if (result != 0) {
		perror("VIDIOC_G_FMT");
		return 1;
	}

	fprintf(stdout, "Get pixel format = 0x%x \n",
		format.fmt.pix.pixelformat);
#endif

	reqbuf.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
	reqbuf.memory = V4L2_MEMORY_MMAP;
	reqbuf.count = 4;

	result = ioctl(output_device, VIDIOC_REQBUFS, &reqbuf);
	if (result != 0) {
		perror("VIDEO_REQBUFS");
		return 1;
	}

	printf("Driver allocated %d buffers when 4 are requested\n",
		reqbuf.count);

	buffers = calloc(reqbuf.count, sizeof(*buffers));
	for (i = 0; i < reqbuf.count ; ++i) {
		struct v4l2_buffer buffer;
		buffer.type = reqbuf.type;
		buffer.index = i;

		result = ioctl(output_device, VIDIOC_QUERYBUF, &buffer);
		if (result != 0) {
			perror("VIDIOC_QUERYBUF");
			return 1;
		}
#if 1
		printf("%d: buffer.length=%d, buffer.m.offset=%d\n",
				i, buffer.length, buffer.m.offset);
#endif
		buffers[i].length = buffer.length;
		buffers[i].start = mmap(NULL, buffer.length, PROT_READ|
						PROT_WRITE, MAP_SHARED,
						output_device,
						buffer.m.offset);

		if (buffers[i].start == MAP_FAILED) {
			perror("mmap");
			return 1;
		}

		printf("Buffers[%d].start = %x  length = %d\n", i,
				buffers[i].start, buffers[i].length);
	}

	if (lcd_off) {
		printf("Entering sleep 20\n");
		sleep(20);
	}

	result = ioctl(output_device, VIDIOC_STREAMON, &reqbuf.type);
	if (result != 0) {
		perror("VIDIOC_STREAMON");
		return 1;
	}

	result = read(file_descriptor, buffers[0].start, 49 * 68 * 3);
	if (result <= 0) {
		perror("read");
		return 1;
	}

	filledbuffer.type = reqbuf.type;
	filledbuffer.index = 0;

	result = ioctl(output_device, VIDIOC_QBUF, &filledbuffer);
	if (result != 0) {
		perror("VIDIOC_QBUF");
		return 1;
	}

	count = 1;
	while (count < 2000) {
		if (sleep_time) {
			sleep(sleep_time);
		} else {
			for (i = 0; i < 2000000; i++)
				;
		}

		result = ioctl(output_device, VIDIOC_DQBUF, &filledbuffer);
		if (result != 0) {
			perror("VIDIOC_DQBUF");
			return 1;
		}

		i = read(file_descriptor, buffers[count%reqbuf.count].start,
						49 * 68 * 3);
		if (i <= 0)
			return 0; /*There is not more job to do.*/

#if 0
		if (i != format.fmt.pix.sizeimage)
			break;
#endif
		filledbuffer.index = count % reqbuf.count;
		result = ioctl(output_device, VIDIOC_QBUF, &filledbuffer);
		if (result != 0) {
			perror("VIDIOC_QBUF2");
			return 1;
		}
		count++;
	}

	result = ioctl(output_device, VIDIOC_DQBUF, &filledbuffer);
	if (result != 0) {
		perror("VIDIOC_DQBUF2");
		return 1;
	}

	for (i = 0; i < reqbuf.count; i++) {
		if (buffers[i].start)
			munmap(buffers[i].start, buffers[i].length);
	}

	result = ioctl(output_device, VIDIOC_STREAMOFF, &reqbuf.type);
	if (result != 0) {
		perror("VIDIOC_STREAMOFF");
		return 1;
	}

	return 0;
}

static int usage(void)
{
	printf("Usage: streaming <inputfile> [<n>]\n");
	return 1;
}

int main(int argc, char *argv[])
{
	int video_device, output_device, file_descriptor, result;
	int sleep_time = 0;
	int lcd_off = 0;

	if (argc < 2)
		return usage();

	output_device = open(VIDEO_DEVICE3, O_RDWR);

	if (output_device <= 0) {
		printf("Could not open %s\n", VIDEO_DEVICE3);
		return 1;
	} else {
		printf("openned %s\n", VIDEO_DEVICE3);
	}

	file_descriptor = open(argv[1], O_RDONLY);
	if (file_descriptor <= 0) {
		printf("Could not open input file %s\n", argv[1]);
		return 1;
	}

	if (argc == 3)
		lcd_off = atoi(argv[2]);

	if (argc == 4)
		sleep_time = atoi(argv[3]);

	result = streaming_video(output_device, file_descriptor,
				sleep_time, lcd_off);
	close(file_descriptor);
	close(output_device);

	return result;
}
