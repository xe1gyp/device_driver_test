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

static int streaming_video(int od, int fd,
	int sleep_time, int buf_count)
{
	struct {
		void *start;
		size_t length;
	} *buffers;

	struct v4l2_capability capability;
	struct v4l2_format format;
	struct v4l2_buffer filledbuffer;
	struct v4l2_requestbuffers reqbuf;
	int i = 0, count, result, read_count = -1;
	int j , k, bpp, page_width, pix_height, pix_width, bpos;


	result = ioctl(od, VIDIOC_QUERYCAP, &capability);
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
	result = ioctl(od, VIDIOC_G_FMT, &format);

	if (result != 0) {
		perror("VIDIOC_G_FMT");
		return result;
	}

	reqbuf.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
	reqbuf.memory = V4L2_MEMORY_MMAP;
	reqbuf.count = buf_count;
	pix_width = format.fmt.pix.width;
	pix_height = format.fmt.pix.height;

	result = ioctl(od, VIDIOC_REQBUFS, &reqbuf);
	if (result != 0) {
		perror("VIDEO_REQBUFS");
		return 1;
	}

	printf("Driver allocated %d buffers when %d are requested\n",
		reqbuf.count, buf_count);

	buffers = calloc(reqbuf.count, sizeof(*buffers));
	for (i = 0; i < reqbuf.count ; ++i) {
		struct v4l2_buffer buffer;
		buffer.type = reqbuf.type;
		buffer.index = i;

		result = ioctl(od, VIDIOC_QUERYBUF, &buffer);
		if (result != 0) {
			perror("VIDIOC_QUERYBUF");
			return 1;
		}

		printf("%d: buffer.length=%d, buffer.m.offset=%d\n",
				i, buffer.length, buffer.m.offset);

		buffers[i].length = buffer.length;
		buffers[i].start = mmap(NULL, buffer.length, PROT_READ|
						PROT_WRITE, MAP_SHARED,
						od, buffer.m.offset);
		if (buffers[i].start == MAP_FAILED) {
			perror("mmap");
			return 1;
		}

		printf("Buffers[%d].start = %x  length = %d\n", i,
			buffers[i].start, buffers[i].length);
	}


	filledbuffer.type = reqbuf.type;
	filledbuffer.memory = V4L2_MEMORY_MMAP;

	printf("\npix attrs: width=%0x, height=%0x, imgsize=%0x, format=%0x\n",
			pix_width, pix_height,
			format.fmt.pix.sizeimage, format.fmt.pix.pixelformat);
	filledbuffer.flags = 0;

	bpp = getBpp(format.fmt.pix.pixelformat);

	if (bpp == -1) {
		perror("format not supported");
		return 1;
	}

	for (i = 0; i < buf_count; i++) {

		filledbuffer.index = i;
#ifndef OMAP_4430
		if (read(fd, buffers[i].start,
			format.fmt.pix.sizeimage) != format.fmt.pix.sizeimage) {
			perror("read");
			return 1;
		}
#else
		page_width = (pix_width * bpp +	4096 - 1) & ~(4096 - 1);
		printf("\nfirst bpp = %0x, page_width = %0x\n", bpp,
								page_width);
		/* page_width calculation is to account for 4k / 8k
		/* based on input size  for NV12, bpp is set to 1
		/* for the Y-buffer*/
		for (j = 0; j < pix_height; j++) {
			if (read(fd, buffers[i].start + (j*page_width),
				(pix_width * bpp)) != (pix_width * bpp)) {
				printf("current address = 0x%0x\n",
					buffers[i].start + (j*page_width));
				perror("first read");
				read_count = i;
				break;
			}
		}

		/* handle UV buffer filling also now */
		if (V4L2_PIX_FMT_NV12 == format.fmt.pix.pixelformat) {
			/* bpp for UV buffer is 2 */
			page_width = (pix_width * 2 + 4096 - 1) & ~(4096 - 1);
			for (j = pix_height; j <
					(pix_height * 3 / 2); j++) {
				if (read(fd, buffers[i].start +	(j*page_width),
					(pix_width)) /* * 2 */
					!= (pix_width)) { /* * 2 */
					/* printf("current address = 0x%0x\n",
					buffers[0].start +
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

		result = ioctl(od, VIDIOC_QBUF, &filledbuffer);
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


	result = ioctl(od, VIDIOC_STREAMON, &reqbuf.type);
	if (result != 0) {
		perror("VIDIOC_STREAMON");
		return 1;
	}

	while (count < 2000) {

#ifdef OMAP_4430
		for (k = 0; k <= read_count - 2; k++) {
#endif
			/* delay some for frame rate control */
			if (sleep_time)
				sleep(sleep_time);
			else {
				for (i = 0; i < 2000000; i++)
					;
			}

			result = ioctl(od, VIDIOC_DQBUF, &filledbuffer);
			if (result != 0) {
				perror("VIDIOC_DQBUF");
				return 1;
			}
#ifdef OMAP_4430
		}

		page_width = (pix_width * bpp + 4096 - 1) & ~(4096 - 1);

		printf("\ninside while: bpp = %0x, page_width = %0x\n",
							bpp, page_width);

		for (k = 0; k <= read_count - 2; k++) {

			filledbuffer.index = bpos = count % reqbuf.count;

			for (j = 0; j < pix_height; j++) {

				i = read(fd, buffers[bpos].start
					+ (j*page_width), pix_width * bpp);

				if (i < 0) {
					printf("current address = 0x%0x\n",
						buffers[bpos].start +
						(j*page_width));
					perror(" second read");
					return 1;
				}

				if (i != pix_width * bpp)
					goto exit;	/* we are done */
			}

			/* handle UV buffer filling also now */
			if (V4L2_PIX_FMT_NV12 == format.fmt.pix.pixelformat) {
				/* bpp for UV buffer is 2 */
				page_width = (pix_width * 2 + 4096 - 1) &
							~(4096 - 1);

				for (j = pix_height; j < (pix_height * 3 / 2);
								j++) {
					i = read(fd, buffers[bpos].start +
						(j*page_width), pix_width);
						/* *2 */

					if (i < 0) {
						printf("current address =\
								0x%0x\n",
						buffers[bpos].start +
						(j*page_width));
						perror("second UV read");
						return 1;
					}

					if (i != pix_width)
						goto exit; /* we are done */
				}
			}
			if (ioctl(od, VIDIOC_QBUF, &filledbuffer) != 0) {
				perror("VIDIOC_QBUF2");
				return 1;
			}

			count++;
		}
	}

#else
		bpos = count%reqbuf.count;
		i = read(fd, buffers[bpos].start, format.fmt.pix.sizeimage);

		if (i < 0) {
			perror("read");
			return 1;
		}
		if (i != format.fmt.pix.sizeimage)
			goto exit; /* we are done */

		filledbuffer.index = bpos;

		if (ioctl(od, VIDIOC_QBUF, &filledbuffer) != 0) {
			perror("VIDIOC_QBUF2");
			return 1;
		}

		count++;
	}
#endif

	if (ioctl(od, VIDIOC_DQBUF, &filledbuffer) != 0) {
		perror("VIDIOC_DQBUF2");
		return 1;
	}

exit:

	for (i = 0; i < reqbuf.count; i++) {
		if (buffers[i].start)
			munmap(buffers[i].start, buffers[i].length);
	}

	result = ioctl(od, VIDIOC_STREAMOFF, &reqbuf.type);
	if (result != 0) {
		perror("VIDIOC_STREAMOFF");
		return 1;
	}

	return 0;
}

static int usage(void)
{
	printf("Usage: streaming <video_device> <inputfile> \
			[<n_sleep>] [<n_bufcount>]\n");
	return 1;
}

int main(int argc, char *argv[])
{
	int video_device, file_descriptor, output_device, result = 0;
	int sleep_time = 0, buf_count = 2;

	if (argc < 3)
		return usage();

	video_device = atoi(argv[1]);
	if ((video_device != 1) && (video_device != 2) && (video_device != 3)) {
		printf("video_device has to be 1 or 2 or 3! video_device=%d, "
			"argv[1]=%s\n", video_device, argv[1]);
		return usage();
	}

	output_device =
		open((video_device == 1) ? VIDEO_DEVICE1 :
			((video_device == 2) ? VIDEO_DEVICE2 : VIDEO_DEVICE3),
		O_RDWR);
	if (output_device <= 0) {
		printf("Could not open %s\n",
			(video_device == 1) ? VIDEO_DEVICE1 :
			((video_device == 2) ? VIDEO_DEVICE2 : VIDEO_DEVICE3));
		return 1;
	} else
		printf("openned %s\n",
			(video_device == 1) ? VIDEO_DEVICE1 :
			((video_device == 2) ? VIDEO_DEVICE2 : VIDEO_DEVICE3));

	file_descriptor = open(argv[2], O_RDONLY);
	if (file_descriptor <= 0) {
		printf("Could not open input file %s\n", argv[2]);
		result = 1;
		goto err2;
	}

	if (argc >= 4)
		sleep_time = atoi(argv[3]);

	if (argc == 5) {

		buf_count = atoi(argv[4]);

		if (buf_count <= 1) {
			printf("Atleast 2 buffers are required\n");
			result = -EINVAL;
			goto err1;
		}
	}

	result = streaming_video(output_device, file_descriptor,
		sleep_time, buf_count);
err1:
	close(output_device);
err2:
	close(file_descriptor);

	return result;
}
