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

#include <sys/time.h>

#define TIME_STRUCT                    struct timeval
#define TIME_GET(target)               gettimeofday(&target, NULL)
#define TIME_COPY(target, source) \
((target).tv_sec = (source).tv_sec), ((target).tv_usec = (source).tv_usec)
#define TIME_INCREASE(time, microsecs) \
((time).tv_sec += ((microsecs) / 1000000) +                             \
	       ((time).tv_usec + (microsecs) % 1000000) / 1000000), \
((time).tv_usec = ((time).tv_usec + (microsecs) % 1000000) % 1000000)
#define TIME_DELTA(time, base)                             \
    (((time).tv_sec - (base).tv_sec) * 1000000 + (time).tv_usec - (base).tv_usec)

typedef struct RATE
{
    TIME_STRUCT last_timestamp, last_reporting, first_time;
    long   n, tn, tn0, an;
    unsigned long x, tx;
    double xx, txx, ax, axx;
} RATE;

#define RATE_SQRT(a) do { \
    double b = (a + 1) / 2; \
    b = (b + a/b) / 2; \
    b = (b + a/b) / 2; \
    b = (b + a/b) / 2; \
    b = (b + a/b) / 2; \
    b = (b + a/b) / 2; \
    b = (b + a/b) / 2; \
    a = (b + a/b) / 2; \
} while(0)

#define RATE_INIT(r, time) do { \
	r.xx  = r.x  = r.n  = 0; \
	r.txx = r.tx = r.tn = r.tn0 = 0; \
	r.axx = r.ax = r.an = 0; \
	TIME_COPY(r.first_time, time); \
	TIME_COPY(r.last_timestamp, time); \
	TIME_COPY(r.last_reporting, time); \
} while(0)

#define RATE_REPORT_T(r) do { \
	double x = (r.tn ? (r.tx ? ((1e6 * r.tn) / r.tx) : 1e6) : 0.); \
	fprintf(stderr, "[%ld] %.3g fps (s=%.3g)\n", \
		TIME_DELTA(r.last_reporting, r.first_time)/1000000, \
		x, r.txx ? (r.tx * (double) r.tx / r.txx / r.tn) : 1); \
	r.axx += x * x; \
	r.ax  += x; \
	r.an  ++; \
	r.txx = r.tx = r.tn = 0; \
} while(0)

#define RATE_ADD(r, time, granularity) do { \
	int steps = TIME_DELTA(time, r.last_reporting) / granularity; \
	unsigned long delta = TIME_DELTA(time, r.last_timestamp); \
	while (steps) { \
		RATE_REPORT_T(r); \
		TIME_INCREASE(r.last_reporting, granularity); \
		steps--; \
	} \
 \
	r.x   += delta; \
	r.tx  += delta; \
	r.xx  += delta * (double) delta; \
	r.txx += delta * (double) delta; \
	r.n   ++; \
	r.tn  ++; \
	r.tn0 ++; \
	TIME_COPY(r.last_timestamp, time); \
} while(0)

#define RATE_DONE(r) do { \
	if (r.n > 0) { \
		double x = r.x * 1e-6 / r.n; \
		double s = (r.xx ? (r.x * (double) r.x / r.xx / r.n) : 1); \
		if (x > 0) { \
			if (r.an) { \
				double x2 = r.ax / r.an; \
				double xx = r.axx / r.an - x2 * x2; \
				RATE_SQRT(xx); \
				fprintf(stderr, "[ALL] %.3g +- %.3g fps (s=%.3g)\n(temporal difference is: %.3g)\n", \
					1/x, xx, s, x2-1/x); \
			} else fprintf(stderr, "[ALL] %.3g fps (s=%.3g)\n", 1/x, s); \
		} else fprintf(stderr, "[ALL] FAILED TO CALCULATE\n"); \
	} else fprintf(stderr, "[ALL] NOT ENOUGH DATA\n"); \
} while (0)

TIME_STRUCT time;
RATE rate;
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

static int streaming_flip(int output_device, char fills[2][4], int n)
{
	struct {
		void *start;
		size_t length;
	} *buffers;

	if (n == 0) n = 100000;

	struct v4l2_capability capability;
	struct v4l2_format format;
	struct v4l2_buffer filledbuffer;
	struct v4l2_requestbuffers reqbuf;
	int i, count, result;
	int j, k, bpp, stride, width;
	char nv_fill[4], *the_fill;

	result = ioctl(output_device, VIDIOC_QUERYCAP, &capability);
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
	result = ioctl(output_device, VIDIOC_G_FMT, &format);

	if (result != 0) {
		perror("VIDIOC_G_FMT");
		return result;
	}

	bpp = getBpp(format.fmt.pix.pixelformat);
	if (-1 == bpp) {
		perror("format not supported");
		return 1;
	}
	width = format.fmt.pix.width * bpp;
	stride = (width + 4095) &~ 4095;

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
		printf("%d: buffer.length=%d, buffer.m.offset=%d\n",
				i, buffer.length, buffer.m.offset);
		buffers[i].length = buffer.length;
		buffers[i].start = mmap(NULL, buffer.length, PROT_READ|
						PROT_WRITE, MAP_SHARED,
						output_device, buffer.m.offset);
		if (buffers[i].start == MAP_FAILED) {
			perror("mmap");
			return 1;
		}

		printf("Buffers[%d].start = %x  length = %d\n", i,
			buffers[i].start, buffers[i].length);

		/* fill buffer */
		the_fill = fills[i % 2];

		/* handle NV12 */
		if (V4L2_PIX_FMT_NV12 == format.fmt.pix.pixelformat) {
			/* fill UV */
			nv_fill[0] = nv_fill[2] = the_fill[1];
			nv_fill[1] = nv_fill[3] = the_fill[3];
			for (j = format.fmt.pix.height * 3 / 2 - 1;
			     j >= format.fmt.pix.height; j--) {
				for (k = 0; k + 3 < width; k += 4)
					memcpy(buffers[i].start + j * stride + k, nv_fill, 4);
			}

			/* fill Y as base buffer */
			nv_fill[0] = nv_fill[2] = the_fill[0];
			nv_fill[1] = nv_fill[3] = the_fill[2];
			the_fill = nv_fill;
		}

		/* fill base buffer */
		for (j = 0; j < format.fmt.pix.height; j++)
			for (k = 0; k + 3 < width; k += 4)
				memcpy(buffers[i].start + j * stride + k, the_fill, 4);
	}

	filledbuffer.type = reqbuf.type;
	filledbuffer.memory = V4L2_MEMORY_MMAP;

	printf("\n pix attribs: width=%0x, height=%0x, imagesize=%0x, format=%0x\n",
			format.fmt.pix.width, format.fmt.pix.height,
			format.fmt.pix.sizeimage, format.fmt.pix.pixelformat);
	filledbuffer.flags = 0;

	for (i = 0; i <= 3; i++) {
		filledbuffer.index = i;
		result = ioctl(output_device, VIDIOC_QBUF, &filledbuffer);
		if (result != 0) {
			perror("VIDIOC_QBUF");
			return 1;
		}
	}

	result = ioctl(output_device, VIDIOC_STREAMON, &reqbuf.type);
	if (result != 0) {
		perror("VIDIOC_STREAMON");
		return 1;
	}

	count = 4;
	TIME_GET(time);
	RATE_INIT(rate, time);
	while (count != n) {
		result = ioctl(output_device, VIDIOC_DQBUF, &filledbuffer);
		if (result != 0) {
			perror("VIDIOC_DQBUF");
			return 1;
		}

		TIME_GET(time);
		RATE_ADD(rate, time, 1000000);
		filledbuffer.index = count % reqbuf.count;
		if (ioctl(output_device, VIDIOC_QBUF, &filledbuffer) != 0) {
			perror("VIDIOC_QBUF2");
			return 1;
		}

		count++;
	}
	RATE_DONE(rate);

	if (ioctl(output_device, VIDIOC_DQBUF, &filledbuffer) != 0) {
		perror("VIDIOC_DQBUF2");
		return 1;
	}

exit:

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
	printf("Usage: streaming </dev/video_device> <pattern1> <pattern2> [<n>]\n");
	return 1;
}

int read_fill_pattern(char *s, char pattern[4])
{
	/* support 1, 2 and 4 byte patterns */
	int len = strlen(s), i, b;

	if (len != 2 && len != 4 && len != 8)
		goto error;

	/* parse each byte */
	for (i = 0; i < 4; i ++) {
		if (sscanf(s + (2 * i) % len, "%02x", &b) != 1)
			goto error;
		pattern[3 - i] = b;
	}
	return 0;

error:
	fprintf(stderr, "invalid fill pattern: %s\n", s);
	return -EINVAL;
}

int main(int argc, char *argv[])
{
	int file_descriptor, result;
	int n = 0;
	char *video_device, fill[2][4];

	if (argc < 4)
		return usage();

	video_device = argv[1];
	result = read_fill_pattern(argv[2], fill[0]);
	result = result ? : read_fill_pattern(argv[3], fill[1]);

	/* read fill patterns */
	file_descriptor = open(video_device, O_RDWR);
	if (file_descriptor <= 0) {
		printf("Could not open %s\n", video_device);
		return 1;
	} else
		printf("openned %s\n", video_device);

	if (argc == 5)
		n = atoi(argv[4]);
	result = streaming_flip(file_descriptor, fill, n);

	close(file_descriptor);

	return result;
}
