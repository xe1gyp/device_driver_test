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
#include <linux/videodev2.h>
#include <sys/mman.h>

#define DEFAULT_PIXEL_FMT "YUYV"
#define DEFAULT_VIDEO_SIZE "QCIF"

static void usage(void)
{
	printf("Burst Zoom Test Case\n");
	printf("Usage: burst_zoom [camDevice] [pixelFormat] [<sizeW> <sizeH>] "
			"[<numberOfFrames>] [<file>] [<zoomFactor>]\n");
	printf("   [camDevice] Camera device to be open\n\t 1:Micron sensor "
					"2:OV sensor\n");
	printf("   [pixelFormat] set the pixelFormat to use. \n\tSupported: "
		"YUYV, UYVY, RGB565, RGB555, RGB565X, RGB555X, RAW10 \n");
	printf("   [sizeW] Set the video width\n");
	printf("   [sizeH] Set the video heigth\n");
	printf("\tOptionally size can be specified using standard name sizes"
							"(VGA,PAL,etc)\n");
	printf("   [numberOfFrames] Number of Frames to be captured\n");
	printf("   [file] Optionally captured image can be saved to file "
								"<file>\n");
	printf("   [zoomFactor] Optionally can specifiy the zoom "
					"factor.Supported values: 10-40\n");
	printf("\t10=1x, 20=2x, 30=3x, 40=4x, accept intermediate values"
							" e.g 35 = 3.5x\n");
}

struct {
	void *start;
	size_t length;
} *cbuffers;

static int crop(int cfd, int left, int top, int width, int height)
{
	struct v4l2_cropcap cropcap;
	struct v4l2_crop crop;
	int ret;

	cropcap.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	ret = ioctl(cfd, VIDIOC_CROPCAP, &cropcap);
	if (ret != 0) {
		perror("VIDIOC_CROPCAP");
		return -1;
	}
	printf("Video Crop bounds (%d, %d) (%d, %d), defrect (%d, %d) "
								"(%d, %d)\n",
		cropcap.bounds.left, cropcap.bounds.top,
		cropcap.bounds.width, cropcap.bounds.height,
		cropcap.defrect.left, cropcap.defrect.top,
		cropcap.defrect.width, cropcap.defrect.height);
	crop.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	ret = ioctl(cfd, VIDIOC_G_CROP, &crop);
	if (ret != 0) {
		perror("VIDIOC_G_CROP");
		return -1;
	}

	printf("Old Crop (%d, %d) (%d, %d)\n", crop.c.left, crop.c.top,
						crop.c.width, crop.c.height);
	printf("wanted Crop (%d, %d) (%d, %d)\n", left, top, width, height);

	crop.c.left = left;
	crop.c.top = top;
	crop.c.width = width;
	crop.c.height = height;

	ret = ioctl(cfd, VIDIOC_S_CROP, &crop);
	if (ret != 0) {
		perror("VIDIOC_S_CROP");
		return -1;
	}
	/* read back */
	ret = ioctl(cfd, VIDIOC_G_CROP, &crop);
	if (ret != 0) {
		perror("VIDIOC_G_CROP");
		return -1;
	}
	printf("New Video Crop (%d, %d) (%d, %d)\n\n", crop.c.left, crop.c.top,
						crop.c.width, crop.c.height);

	return 0;
}

int main(int argc, char *argv[])
{
	struct v4l2_capability capability;
	struct v4l2_format cformat;
	struct v4l2_requestbuffers creqbuf;
	struct v4l2_buffer cfilledbuffer;
	int fd, i, ret, count = -1, memtype = V4L2_MEMORY_USERPTR;
	int fd_save = 0;
	int index = 1;
	int zoomFactor = 10;
	int device = 1;
	char *pixelFmt;

	if ((argc > 1) && (!strcmp(argv[1], "?"))) {
		usage();
		return 0;
	}

	if (argc > index) {
		device = atoi(argv[index]);
		index++;
	}

	fd = open_cam_device(O_RDWR, device);
	if (fd <= 0) {
		printf("Could not open the cam device\n");
		return -1;
	}

	if (argc > index) {
		pixelFmt = argv[index];
		index++;
		if (argc > index) {
			ret = validateSize(argv[index]);
			if (ret == 0) {
				ret = cam_ioctl(fd, pixelFmt, argv[index]);
				if (ret < 0) {
					usage();
					return -1;
				}
			} else {
				index++;
				if (argc > (index)) {
					ret = cam_ioctl(fd, pixelFmt,
						argv[index-1], argv[index]);
					if (ret < 0) {
						usage();
						return -1;
					}
				} else {
					printf("Invalid size\n");
					usage();
					return -1;
				}
			}
			index++;
		} else {
			printf("Setting QCIF as video size, default value\n");
			ret = cam_ioctl(fd, pixelFmt, DEFAULT_VIDEO_SIZE);
			if (ret < 0)
				return -1;
		}
	} else {
		printf("Setting pixel format and video size with default "
								"values\n");
		ret = cam_ioctl(fd, DEFAULT_PIXEL_FMT, DEFAULT_VIDEO_SIZE);
		if (ret < 0)
			return -1;
	}

	if (argc > index)
		count = atoi(argv[index]);

	printf("Frames: %d\n", count);
	index++;

	if (count >= 32 || count <= 0) {
		printf("Camera driver only support max 32 buffers, "
			"you request %d\n", count);
		return -1;
	}
	if (argc > index) {
		fd_save = creat(argv[index], O_RDWR);
		if (fd_save <= 0) {
			printf("Can't create file %s\n", argv[index]);
			fd_save = 0;
		}
	}

	index++;

	if (argc > index) {
		zoomFactor = atoi(argv[index]);
		if (zoomFactor < 10 || zoomFactor > 40) {
			printf("ERROR: Zoom Factor Not Supported Value must "
								"be between"
								" 10-40\n");
			printf("argv[%d]=%s\n", index, argv[index]);
			usage();
			return 0;
		}
	}

	index++;

	if (ioctl(fd, VIDIOC_QUERYCAP, &capability) < 0) {
		perror("VIDIOC_QUERYCAP");
		return -1;
	}
	if (capability.capabilities & V4L2_CAP_STREAMING)
		printf("The driver is capable of Streaming!\n");
	else {
		printf("The driver is not capable of Streaming!\n");
		return -1;
	}
	cformat.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	ret = ioctl(fd, VIDIOC_G_FMT, &cformat);
	if (ret < 0) {
		perror("VIDIOC_G_FMT");
		return -1;
	}
	printf("Camera Image width = %d, Image height = %d, size = %d\n",
		 cformat.fmt.pix.width, cformat.fmt.pix.height,
		 cformat.fmt.pix.sizeimage);

	ret = crop(fd, (cformat.fmt.pix.width -
			(cformat.fmt.pix.width * 10) / zoomFactor) / 2,
			(cformat.fmt.pix.height -
			(cformat.fmt.pix.height * 10) / zoomFactor) / 2,
			(cformat.fmt.pix.width * 10) / zoomFactor,
			(cformat.fmt.pix.height * 10) / zoomFactor);
	if (ret < 0)
		printf("CROP FAILURE\n");

	creqbuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	creqbuf.memory = memtype;
	creqbuf.count = count;

	printf("Requesting %d buffers of type %s\n", creqbuf.count,
		(memtype == V4L2_MEMORY_USERPTR) ? "V4L2_MEMORY_USERPTR" :
						   "V4L2_MEMORY_MMAP");
	if (ioctl(fd, VIDIOC_REQBUFS, &creqbuf) < 0) {
		perror("VIDEO_REQBUFS");
		return -1;
	}
	printf("Camera Driver allowed buffers reqbuf.count = %d\n",
		 creqbuf.count);
	if (creqbuf.count != count)
		count = creqbuf.count;

	cbuffers = calloc(creqbuf.count, sizeof(*cbuffers));

	/* mmap driver memory or allocate user memory, and queue each buffer */
	for (i = 0; i < creqbuf.count; ++i) {
		struct v4l2_buffer buffer;
		buffer.type = creqbuf.type;
		buffer.memory = creqbuf.memory;
		buffer.index = i;
		if (ioctl(fd, VIDIOC_QUERYBUF, &buffer) < 0) {
			perror("VIDIOC_QUERYBUF");
			return -1;
		}
		if (memtype == V4L2_MEMORY_USERPTR) {
			cbuffers[i].length = buffer.length;
			posix_memalign(&cbuffers[i].start, 0x1000,
				       cbuffers[i].length);
			buffer.m.userptr =
			    (unsigned int)cbuffers[i].start;
			printf("User Buffer [%d].start = %x  length = %d\n",
				 i, cbuffers[i].start, cbuffers[i].length);
		} else {
			cbuffers[i].length = buffer.length;
			cbuffers[i].start =
			    mmap(NULL, buffer.length, PROT_READ | PROT_WRITE,
				 MAP_SHARED, fd, buffer.m.offset);
			if (cbuffers[i].start == MAP_FAILED) {
				perror("mmap");
				return -1;
			}
			printf("Mapped Buffers[%d].start = %x  length = %d\n",
				i, cbuffers[i].start, cbuffers[i].length);
			buffer.m.userptr = (unsigned int)cbuffers[i].start;
			buffer.length = cbuffers[i].length;
		}
		if (ioctl(fd, VIDIOC_QBUF, &buffer) < 0) {
			perror("CAMERA VIDIOC_QBUF");
			return -1;
		}
	}

	/* turn on streaming */
	if (ioctl(fd, VIDIOC_STREAMON, &creqbuf.type) < 0) {
		perror("VIDIOC_STREAMON");
		return -1;
	}

	/* capture 1000 frames or when we hit the passed number of frames */
	cfilledbuffer.type = creqbuf.type;
	cfilledbuffer.memory = memtype;
	i = 0;

	while (i < 1000) {
		/* De-queue the next avaliable buffer */
		while (ioctl(fd, VIDIOC_DQBUF, &cfilledbuffer) < 0) {
			perror("VIDIOC_DQBUF");
			printf(" ERROR HAS OCCURED\n");
		}

		i++;

		if (i == count) {
			printf("Cancelling the streaming capture...\n");
			creqbuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
			if (ioctl(fd, VIDIOC_STREAMOFF, &creqbuf.type) < 0) {
				perror("VIDIOC_STREAMOFF");
				return -1;
			}
			printf("Done\n");
			break;
		}


		while (ioctl(fd, VIDIOC_QBUF, &cfilledbuffer) < 0)
			perror("CAM VIDIOC_QBUF");
	}
	/* we didn't turn off streaming yet */
	if (count == -1) {
		creqbuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		if (ioctl(fd, VIDIOC_STREAMOFF, &creqbuf.type) == -1) {
			perror("VIDIOC_STREAMOFF");
			return -1;
		}
	}
	printf("Captured %d frames!\n", i);
	printf("Start writing to file\n");
	if (fd_save > 0) {
		for (i = 0; i < count; i++)
			write(fd_save, cbuffers[i].start,
			       cformat.fmt.pix.width * cformat.fmt.pix.height *
			       2);
	}
	printf("Completed writing to file\n");
	for (i = 0; i < creqbuf.count; i++) {
		if (cbuffers[i].start) {
			if (memtype == V4L2_MEMORY_USERPTR)
				free(cbuffers[i].start);
			else
				munmap(cbuffers[i].start, cbuffers[i].length);
		}
	}

	zoomFactor = 10;
	ret = crop(fd, (cformat.fmt.pix.width -
			(cformat.fmt.pix.width * 10) / zoomFactor) / 2,
			(cformat.fmt.pix.height -
			(cformat.fmt.pix.height * 10) / zoomFactor) / 2,
			(cformat.fmt.pix.width * 10) / zoomFactor,
			(cformat.fmt.pix.height * 10) / zoomFactor);
	if (ret < 0)
		printf("CROP FAILURE\n");
	free(cbuffers);
	close(fd);
	if (fd_save > 0)
		close(fd_save);
}
