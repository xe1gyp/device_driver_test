#include <stdio.h>
#include <fcntl.h>
#include <linux/ioctl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <linux/videodev2.h>
#include <linux/errno.h>
#include <errno.h>

#include "lib.h"
#include "omap24xxvout.h"

#define	VIDEO_DEVICE3	"/dev/video3"


static void streaming_video(int ofd, int ifd, int sleep_time)
{
	struct {
		void *start;
		size_t length;
	} *buffers;

	struct v4l2_capability capability;
	struct v4l2_format format;
	struct v4l2_buffer filledbuffer;
	struct v4l2_requestbuffers reqbuf;
	int i, count, ret;

#if 1
	if (ioctl(ofd, VIDIOC_QUERYCAP, &capability) == -1) {
		perror("VIDIOC_QUERYCAP");
		return;
	}

	if (capability.capabilities & V4L2_CAP_STREAMING == 0) {
		printf("VIDIOC_QUERYCAP indicated that:");
		printf(" driver is not capable of Streaming \n");
		return;
	}


	format.fmt.pix.pixelformat = V4L2_PIX_FMT_RGB444;
	ret = ioctl(ofd, VIDIOC_S_FMT, &format);
	if (ret < 0) {
	    fprintf(stdout, "pixel format = 0x%x \n", V4L2_PIX_FMT_RGB444);
	    perror("VIDIOC_S_FMT");
	}

	format.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
	ret = ioctl(ofd, VIDIOC_G_FMT, &format);
	if (ret < 0) {
	    perror("VIDIOC_G_FMT");
	    return;
	}
	fprintf(stdout, "Get pixel format= 0x%x\n", format.fmt.pix.pixelformat);
#endif

	reqbuf.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
	reqbuf.memory = V4L2_MEMORY_MMAP;
	reqbuf.count = 4;
	if (ioctl(ofd, VIDIOC_REQBUFS, &reqbuf) == -1) {
		perror("VIDEO_REQBUFS");
		return;
	}
	printf("Driver allocated %d buffers ", reqbuf.count);
	printf("when 4 are requested\n");

	buffers = calloc(reqbuf.count, sizeof(*buffers));
	for (i = 0; i < reqbuf.count ; ++i) {
		struct v4l2_buffer buffer;
		buffer.type = reqbuf.type;
		buffer.index = i;
		if (ioctl(ofd, VIDIOC_QUERYBUF, &buffer) == -1) {
			perror("VIDIOC_QUERYBUF");
			return;
		}
#if 1
		printf("%d: buffer.length=%d, buffer.m.offset=%d\n",
				i, buffer.length, buffer.m.offset);
#endif
		buffers[i].length = buffer.length;
		buffers[i].start = mmap(NULL, buffer.length, PROT_READ|
						PROT_WRITE, MAP_SHARED,
						ofd, buffer.m.offset);
		if (buffers[i].start == MAP_FAILED) {
			perror("mmap");
			return;
		}
		printf("Buffers[%d].start = %x  length = %d\n", i);
		printf(" =  %x ", buffers[i].start);
		printf(" length = %d\n", buffers[i].length);
	}

	sleep(20);

	if (ioctl(ofd, VIDIOC_STREAMON, &reqbuf.type) == -1) {
		perror("VIDIOC_STREAMON");
		return;
	}

	if (read(ifd, buffers[0].start, 49*68*3) <= 0) {
		perror("read");
		return;
	}
	filledbuffer.type = reqbuf.type;
	filledbuffer.index = 0;
	if (ioctl(ofd, VIDIOC_QBUF, &filledbuffer) == -1) {
		perror("VIDIOC_QBUF");
		return;
	}

	count = 1;
	while (count < 2000) {
		/* delay some for frame rate control */
		if (sleep_time)
			sleep(sleep_time);
		else
			for (i = 0; i < 2000000; i++);

		if (ioctl(ofd, VIDIOC_DQBUF, &filledbuffer) == -1) {
			perror("VIDIOC_DQBUF");
			return;
		}

		i = read(ifd, buffers[count%reqbuf.count].start, 49*68*3);
		if (i <= 0) {
			perror("read");
			return;
		}
#if 0
		if (i != format.fmt.pix.sizeimage)
			break; /* we are done */
#endif
		filledbuffer.index = count % reqbuf.count;
		if (ioctl(ofd, VIDIOC_QBUF, &filledbuffer) == -1) {
			perror("VIDIOC_QBUF2");
			return;
		}

		count++;
	}

	if (ioctl(ofd, VIDIOC_DQBUF, &filledbuffer) == -1) {
		perror("VIDIOC_DQBUF2");
		return;
	}

	for (i = 0; i < reqbuf.count; i++) {
		if (buffers[i].start)
			munmap(buffers[i].start, buffers[i].length);
	}

	if (ioctl(ofd, VIDIOC_STREAMOFF, &reqbuf.type) == -1) {
		perror("VIDIOC_STREAMOFF");
		return;
	}

}


static int usage(void)
{
	printf("Usage: streaming <inputfile> [<n>]\n");
	return 0;
}

int main(int argc, char *argv[])
{
	int vid, fd, fd2, ret;
	int sleep_time = 0;

	if (argc < 2)
		return usage();

	fd = open(VIDEO_DEVICE3, O_RDWR) ;
	if (fd <= 0) {
		printf("Could not open %s\n", VIDEO_DEVICE3);
		return -1;
	} else {
		printf("openned %s\n", VIDEO_DEVICE3);
	}

	fd2 = open(argv[1], O_RDONLY) ;
	if (fd2 <= 0) {
		printf("Could not open input file %s\n", argv[1]);
		return -1;
	}

	if (argc == 3)
		sleep_time = atoi(argv[2]);
	streaming_video(fd, fd2, sleep_time);

	close(fd2);
	close(fd);
}
