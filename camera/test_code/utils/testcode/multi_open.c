/* ========================================================================
*             Texas Instruments OMAP(TM) Platform Software
*  (c) Copyright Texas Instruments, Incorporated.  All Rights Reserved.
*
*  Use of this software is controlled by the terms and conditions found
*  in the license agreement under which this software has been supplied.
* ========================================================================== */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <linux/videodev2.h>

#define VIDEO_DEVICE1 "/dev/video1"
#define VIDEO_DEVICE2 "/dev/video2"
#define NUM_BUF 4

static void usage(void)
{
	printf("Multi Opens Test\n");
	printf("multi_open [test case] [framerate] [vid] [dev]\n");
	printf("\tSteaming capture and render of 50 frames using one file"
		" handle,\n");
	printf("\tfollowed by an increase in brightness using a second unique"
		" file\n");
	printf("\thandle, followed by another 50 frames of capture and"
		" rendering.\n");
	printf("\t[test case] is a value in range of [1...6], causing a"
		" certain\n");
	printf("\tsequence of open/close handles\n");
	printf("\t[framerate] is the framerate to be used, if no value"
				" is given \n\t           30 fps is default\n");
	printf("\t[vid] is the video pipeline to be used. Valid vid is"
		" 1(default) or 2\n");
	printf("\t[dev] Camera device to be open\n\t 1:Micron sensor "
					"2:OV sensor 3:IMX046\n");
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
	struct v4l2_buffer cfilledbuffer, vfilledbuffer;
	int cfd, cfd_ctrl, vfd;
	int vid = 1, set_video_img = 0, test_case = 1, i, ret;
	struct v4l2_control control;
	int framerate = 30;
	int index = 1, device = 1;

	if ((argc > 1) && (!strcmp(argv[1], "?"))) {
		usage();
		return 0;
	}
	if (argc > index) {
		test_case = atoi(argv[index]);
		if ((test_case < 1) || (test_case > 6)) {
			printf("test_case has to be in rang [1..5]\n");
			usage();
			return 0;
		}
	}
	index++;

	if (argc > index) {
		framerate = atoi(argv[index]);
		printf("Framerate = %d\n", framerate);
	} else
		printf("Using framerate = 30, default value\n");

	index++;

	if (argc > index) {
		vid = atoi(argv[index]);
		if ((vid != 1) && (vid != 2)) {
			printf("vid has to be 1 or 2! vid=%d, argv[%d]=%s\n",
				vid, index, argv[2]);
			usage();
			return 0;
		}
	}

	index++;
	if (argc > index)
		device = atoi(argv[index]);

	printf("Running Test Case ==> %d\n", test_case);

	if (test_case == 1 || test_case == 3 || test_case == 4 ||
	    test_case == 6) {
		cfd = open_cam_device(O_RDWR, device);
		if (cfd <= 0) {
			printf("Could not open the cam device\n");
			return -1;
		} else {
			printf("Opened camera handle for Streaming => 0x%08X\n",
				cfd);
		}

		if (test_case == 1 || test_case == 3 || test_case == 6) {
			cfd_ctrl = open_cam_device(O_RDWR, device);
			if (cfd_ctrl <= 0) {
				printf("Could not open the cam control device"
					" handle\n");
				return -1;
			} else {
				printf("Opened camera handle for Control Only"
					" => 0x%08X\n", cfd_ctrl);
			}
		}
	} else if (test_case == 2 || test_case == 5) {
		cfd_ctrl = open_cam_device(O_RDWR, device);
		if (cfd_ctrl <= 0) {
			printf("Could not open the cam control device"
				" handle\n");
			return -1;
		} else {
			printf("Opened camera handle for Control Only =>"
				" 0x%08X\n", cfd_ctrl);
		}

		cfd = open_cam_device(O_RDWR, device);
		if (cfd <= 0) {
			printf("Could not open the cam device\n");
			return -1;
		} else {
			printf("Opened camera handle for Streaming => 0x%08X\n",
				cfd);
		}
	}

	ret = setFramerate(cfd, framerate);
	if (ret < 0) {
		printf("ERROR: VIDIOC_S_PARM ioctl cam\n");
		return -1;
	}

	vfd = open((vid == 1) ? VIDEO_DEVICE1 : VIDEO_DEVICE2, O_RDWR) ;
	if (vfd <= 0) {
		printf("Could not open %s\n", (vid == 1) ? VIDEO_DEVICE1 :
								VIDEO_DEVICE2);
		return -1;
	}
	printf("openned %s for rendering\n", (vid == 1) ? VIDEO_DEVICE1 :
								VIDEO_DEVICE2);

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

	cformat.fmt.pix.width = 320;
	cformat.fmt.pix.height = 240;
	cformat.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;

	cformat.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	ret = ioctl(cfd, VIDIOC_S_FMT, &cformat);
	if (ret < 0) {
		perror("VIDIOC_S_FMT");
		return -1;
	}

	cformat.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	ret = ioctl(cfd, VIDIOC_G_FMT, &cformat);
	if (ret < 0) {
		perror("cam VIDIOC_G_FMT");
		return -1;
	}

	printf("Camera Image width = %d, Image height = %d, size = %d\n",
			cformat.fmt.pix.width,
			cformat.fmt.pix.height,
			cformat.fmt.pix.sizeimage);

	vformat.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
	ret = ioctl(vfd, VIDIOC_G_FMT, &vformat);
	if (ret < 0) {
		perror("video VIDIOC_G_FMT");
		return -1;
	}
	printf("Video Image width = %d, Image height = %d, size = %d\n",
		vformat.fmt.pix.width,
		vformat.fmt.pix.height,
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
	memset(&control, 0, sizeof(control));

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
			printf("can't make camera and video image"
				" compatible!\n");
			return 0;
		}

	}

	vreqbuf.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
	vreqbuf.memory = V4L2_MEMORY_MMAP;
	vreqbuf.count = NUM_BUF;
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
		vbuffers[i].length = buffer.length;
		vbuffers[i].start = mmap(NULL, buffer.length,
						PROT_READ | PROT_WRITE,
						MAP_SHARED,
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
	creqbuf.count = NUM_BUF;
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
		buffer.m.userptr = (unsigned long)vbuffers[i].start;
		buffer.length = vbuffers[i].length;

		/* printf("Q %d\n",buffer.index); */
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

	/* caputure 1000 frames */
	cfilledbuffer.type = creqbuf.type;
	vfilledbuffer.type = vreqbuf.type;

	if (test_case == 1 || test_case == 2 || test_case == 3) {
		printf("Setting BRIGHTNESS to 1 (out of 15)\n");
		memset(&control, 0, sizeof(control));
		control.id = V4L2_CID_BRIGHTNESS;
		control.value = 1;
		if (ioctl(cfd_ctrl, VIDIOC_S_CTRL, &control) == -1) {
			printf("VIDIOC_S_CTRL BRIGHTNESS failed!\n");
			return 0;
		}
	}

	i = 0;
	while (i < 150) {
		/* De-queue the next avaliable buffer */
		while (ioctl(cfd, VIDIOC_DQBUF, &cfilledbuffer) < 0)
			perror("cam VIDIOC_DQBUF");

		vfilledbuffer.index = cfilledbuffer.index;
		vfilledbuffer.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
		vfilledbuffer.memory = V4L2_MEMORY_MMAP;
		vfilledbuffer.m.userptr = 
			(unsigned int)(vbuffers[cfilledbuffer.index].start);
		vfilledbuffer.length = cfilledbuffer.length;
		if (ioctl(vfd, VIDIOC_QBUF, &vfilledbuffer) < 0) {
			perror("dss VIDIOC_QBUF");
			return -1;
		}
		i++;

		if (i == 3) {
			/* Turn on streaming for video */
			if (ioctl(vfd, VIDIOC_STREAMON, &vreqbuf.type)) {
				perror("dss VIDIOC_STREAMON");
				return -1;
			}
		}

		if (i >= 3) {
			/* De-queue the previous buffer from video driver */
			if (ioctl(vfd, VIDIOC_DQBUF, &vfilledbuffer)) {
				perror("dss VIDIOC_DQBUF");
				return;
			}
		}

		if (i == 50) {
			if (test_case == 4) {
				cfd_ctrl = open_cam_device(O_RDWR, device);
				if (cfd_ctrl <= 0) {
					printf("Could not open the cam control"
						" device handle\n");
					return -1;
				}
				printf("Opened camera handle for"
						" Control Only => 0x%08X\n",
						cfd_ctrl);
			}

			/* Increase brightness with second file handle */
			printf("Setting BRIGHTNESS to 10 (out of 15)\n");
			memset(&control, 0, sizeof(control));
			control.id = V4L2_CID_BRIGHTNESS;
			control.value = 10;
			if (ioctl(cfd_ctrl, VIDIOC_S_CTRL, &control) == -1) {
				printf("VIDIOC_S_CTRL BRIGHTNESS failed!\n");
				return 0;
			}
		}

		if (i == 100) {
			/* Setting brigthness to the default value */
			printf("Setting BRIGHTNESS to 1 (default value)\n");
			memset(&control, 0, sizeof(control));
			control.id = V4L2_CID_BRIGHTNESS;
			control.value = 1;
			if (ioctl(cfd_ctrl, VIDIOC_S_CTRL, &control) == -1) {
				printf("VIDIOC_S_CTRL BRIGHTNESS failed!\n");
				return 0;
			}

			if (test_case == 3) {
				printf("Closing handle for Control Only =>"
					" 0x%08X\n", cfd_ctrl);
				close(cfd_ctrl);
			}
		}

		if (i == 125) {
			if (test_case == 5 || test_case == 6)
				return;
		}

		if (i >= 3) {
			cfilledbuffer.index = vfilledbuffer.index;
			while (ioctl(cfd, VIDIOC_QBUF, &cfilledbuffer) < 0)
				perror("cam VIDIOC_QBUF");
		}
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

	for (i = 0; i < vreqbuf.count; i++) {
		if (vbuffers[i].start)
			munmap(vbuffers[i].start, vbuffers[i].length);
	}

	free(vbuffers);

	printf("Closing handle for Streaming    => 0x%08X\n", cfd);
	close(cfd);
	if (test_case == 4) {
		printf("Setting BRIGHTNESS to 1 (out of 15)\n");
		memset(&control, 0, sizeof(control));
		control.id = V4L2_CID_BRIGHTNESS;
		control.value = 1;
		if (ioctl(cfd_ctrl, VIDIOC_S_CTRL, &control) == -1) {
			printf("VIDIOC_S_CTRL BRIGHTNESS failed!\n");
			return 0;
		}
	}
	if (test_case == 1 || test_case == 2 || test_case == 4) {
		printf("Closing handle for Control Only => 0x%08X\n", cfd_ctrl);
		close(cfd_ctrl);
	}
	close(vfd);
}
