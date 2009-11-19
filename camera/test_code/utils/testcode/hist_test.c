/* ========================================================================
*             Texas Instruments OMAP(TM) Platform Software
*  (c) Copyright Texas Instruments, Incorporated.  All Rights Reserved.
*
*  Use of this software is controlled by the terms and conditions found
*  in the license agreement under which this software has been supplied.
* ========================================================================= */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <linux/videodev2.h>
#include <string.h>
#include <mach/isp_user.h>
#include "kbget.h"
#include <errno.h>

#define VIDIOC_S_OMAP2_ROTATION		_IOW('V', 3, int)
#define FBDEVICE "/dev/fb0"
#define VIDEO_DEVICE1 "/dev/video1"
#define VIDEO_DEVICE2 "/dev/video2"

#define DEFAULT_PIXEL_FMT "YUYV"
#define DEFAULT_VIDEO_SIZE "QVGA"

#define MODE_MANUAL		1
#define MODE_AUTO		2

#define MAX_FRAME_AUTO_MODE		1000

#define BYTES_PER_WINDOW	16
#define DIGITAL_GAIN_DEFAULT	0x100
#define DIGITAL_GAIN_MAX	0x3FF
#define BLUE_GAIN_DEFAULT	0x94
#define BLUE_GAIN_MAX		0xFF
#define RED_GAIN_DEFAULT	0x68
#define RED_GAIN_MAX		0xFF
#define GB_GAIN_DEFAULT		0x5C
#define GB_GAIN_MAX		0xFF
#define GR_GAIN_DEFAULT		0x5C
#define GR_GAIN_MAX		0xFF

static void usage(void)
{
	printf("hist_test [camDevice] [framerate] [vid] [mode]\n");
	printf("\tEnable 2A and capture  of 1000 frames using video driver for "
								"rendering\n");
	printf("\t[camDevice] Camera device to be open\n\t\t 1:Micron sensor "
					"2:OV sensor\n");
	printf("\t[framerate] is the framerate to be used, if no value"
				" is given \n\t           30 fps is default\n");
	printf("\t[vid] is the video pipeline to be used. Valid vid is 1"
							"(default) or 2\n");
	printf("\t[mode] is the tesing mode.\n"
			"\t\tAUTO   - Automatic (default)\n"
			"\t\tMANUAL - Manual key press mode\n");
}

static void display_keys(void)
{
	printf("\nControl Keys:\n");
	printf("  h - Request histogram\n");
	printf("  q - Quit\n\n");
}

static int get_histogram(int cfd, __u32 *stats_buff, int *frame)
{
	int ret;
	struct isp_hist_data hist_data_user;

	hist_data_user.hist_statistics_buf = stats_buff;
	hist_data_user.update = 0;
	ret = ioctl(cfd,  VIDIOC_PRIVATE_ISP_HIST_REQ,
				&hist_data_user);
	if (ret < 0) {
		if (frame) *frame = -1;
		return ret;
	}

	hist_data_user.frame_number =
			hist_data_user.curr_frame - 1;
	if (frame)
		*frame = hist_data_user.frame_number;
	hist_data_user.update = REQUEST_STATISTICS;
	hist_data_user.hist_statistics_buf = stats_buff;
	ret = ioctl(cfd,  VIDIOC_PRIVATE_ISP_HIST_REQ,
				&hist_data_user);

	return ret;
}


int main(int argc, char *argv[])
{
	struct {
		void *start;
		size_t length;
	} *vbuffers, *cbuffers;

	struct v4l2_capability capability;
	struct v4l2_format cformat, vformat;
	struct v4l2_requestbuffers creqbuf, vreqbuf;
	struct v4l2_buffer cfilledbuffer, vfilledbuffer;
	int vfd, cfd;
	int i, ret, memtype = V4L2_MEMORY_USERPTR;
	int index = 1, vid = 1, set_video_img = 0;
	int device = 3;
	char *pixelFmt;
	int framerate = 30, mode = MODE_AUTO;

	struct isp_hist_config hist_config_user;
	struct isp_hist_data hist_data_user;
	struct v4l2_control control_exp, control_an_gain;
	struct v4l2_queryctrl qc_exp, qc_an_gain;
	__u32 *stats_buff = NULL;
	int frame_number, input, pass = 0;
	int try = 0, buff_size;
	int done_flag = 0;
char *dom;

	if ((argc > 1) && (!strcmp(argv[1], "?"))) {
		usage();
		return 0;
	}

	if (argc > index) {
		device = atoi(argv[index]);
		index++;
	}

	cfd = open_cam_device(O_RDWR, device);
	if (cfd <= 0) {
		printf("Could not open the cam device\n");
		return -1;
	}

	if (argc > index) {
		framerate = atoi(argv[index]);
		if (framerate == 0) {
			printf("Invalid framerate value, Using Default "
							"framerate = 15\n");
			framerate = 15;
		}
		index++;
	}

	if (argc > index) {
		vid = atoi(argv[index]);
		if ((vid != 1) && (vid != 2)) {
				printf("vid has to be 1 or 2!\n ");
				return 0;
		}
		index++;
	}

	if (argc > index) {
		if (strcmp(argv[index], "MANUAL") == 0)
			mode = MODE_MANUAL;
		else
			mode = MODE_AUTO;
		index++;
	}

	if (mode == MODE_AUTO)
		printf("Mode: Auto test\n");
	else
		printf("Mode: Manual test\n");


	printf("Setting pixel format and video size with default "
					"values\n");

	ret = cam_ioctl(cfd, DEFAULT_PIXEL_FMT, DEFAULT_VIDEO_SIZE);
	if (ret < 0)
		return -1;
	ret = setFramerate(cfd, framerate);
	if (ret < 0) {
		printf("Error setting framerate = %d\n", framerate);
		return -1;
	}
	vfd = open((vid == 1) ? VIDEO_DEVICE1 : VIDEO_DEVICE2, O_RDWR);
	if (vfd <= 0) {
		printf("Could no open the device %s\n",
			(vid == 1) ? VIDEO_DEVICE1 : VIDEO_DEVICE2);
		vid = 0;
	} else
		printf("openned %s for rendering\n",
			(vid == 1) ? VIDEO_DEVICE1 : VIDEO_DEVICE2);

	if (ioctl(vfd, VIDIOC_QUERYCAP, &capability) == -1) {
		perror("dss VIDIOC_QUERYCAP");
		return -1;
	}
	if (capability.capabilities & V4L2_CAP_STREAMING)
		printf("The video driver is capable of Streaming!\n");
	else {
		printf("The video driver is not capable of "
					"Streaming!\n");
		return -1;
	}

	if (ioctl(cfd, VIDIOC_QUERYCAP, &capability) < 0) {
		perror("cam VIDIOC_QUERYCAP");
		return -1;
	}
	if (capability.capabilities & V4L2_CAP_STREAMING)
		printf("The driver is capable of Streaming!\n");
	else {
		printf("The driver is not capable of Streaming!\n");
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
		perror("dss VIDIOC_G_FMT");
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
	if (cformat.fmt.pix.pixelformat !=
	    vformat.fmt.pix.pixelformat) {
		printf("pixel formats don't match!\n");
		set_video_img = 1;
	}

	if (set_video_img) {
		printf("set video image the same as camera image ..\n");
		vformat.fmt.pix.width = cformat.fmt.pix.width;
		vformat.fmt.pix.height = cformat.fmt.pix.height;
		vformat.fmt.pix.sizeimage = cformat.fmt.pix.sizeimage;
		vformat.fmt.pix.pixelformat =
					cformat.fmt.pix.pixelformat;
		ret = ioctl(vfd, VIDIOC_S_FMT, &vformat);
		if (ret < 0) {
			perror("dss VIDIOC_S_FMT");
			return -1;
		}
		printf("New Image & Video sizes, after "
			"equaling:\nCamera Image width = %d, "
			"Image height = %d, size = %d\n",
			cformat.fmt.pix.width, cformat.fmt.pix.height,
			cformat.fmt.pix.sizeimage);
		printf("Video Image width = %d, Image height "
			"= %d, size = %d\n",
			vformat.fmt.pix.width, vformat.fmt.pix.height,
			vformat.fmt.pix.sizeimage);

		if ((cformat.fmt.pix.width != vformat.fmt.pix.width) ||
		    (cformat.fmt.pix.height !=
		     vformat.fmt.pix.height) ||
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
		perror("dss VIDEO_REQBUFS");
		return;
	}
	printf("Video Driver allocated %d buffers when 4 are "
			"requested\n", vreqbuf.count);

	vbuffers = calloc(vreqbuf.count, sizeof(*vbuffers));
	for (i = 0; i < vreqbuf.count; ++i) {
		struct v4l2_buffer buffer;
		buffer.type = vreqbuf.type;
		buffer.index = i;
		if (ioctl(vfd, VIDIOC_QUERYBUF, &buffer) == -1) {
			perror("dss VIDIOC_QUERYBUF");
			return;
		}
		vbuffers[i].length = buffer.length;
		vbuffers[i].start = mmap(NULL, buffer.length,
					 PROT_READ | PROT_WRITE,
					 MAP_SHARED,
					 vfd,
					 buffer.m.offset);
		if (vbuffers[i].start == MAP_FAILED) {
			perror("dss mmap");
			return;
		}
		printf("Video Buffers[%d].start = %x  length = %d\n",
			i, vbuffers[i].start, vbuffers[i].length);

	}

	creqbuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	creqbuf.memory = memtype;
	creqbuf.count = 4;
	printf("Requesting %d buffers of type %s\n", creqbuf.count,
		(memtype == V4L2_MEMORY_USERPTR) ? "V4L2_MEMORY_USERPTR" :
						"V4L2_MEMORY_MMAP");
	if (ioctl(cfd, VIDIOC_REQBUFS, &creqbuf) < 0) {
		perror("cam VIDEO_REQBUFS");
		return -1;
	}
	printf("Camera Driver allowed buffers reqbuf.count = %d\n",
						creqbuf.count);


	cbuffers = calloc(creqbuf.count, sizeof(*cbuffers));
	/* mmap driver memory or allocate user memory, and queue each buffer */
	for (i = 0; i < creqbuf.count; ++i) {
		struct v4l2_buffer buffer;
		buffer.type = creqbuf.type;
		buffer.memory = creqbuf.memory;
		buffer.index = i;
		if (ioctl(cfd, VIDIOC_QUERYBUF, &buffer) < 0) {
			perror("cam VIDIOC_QUERYBUF");
			return -1;
		}
		if (memtype == V4L2_MEMORY_USERPTR) {
			buffer.flags = 0;
			buffer.m.userptr = (unsigned int)vbuffers[i].start;
			buffer.length = vbuffers[i].length;
		} else {
			cbuffers[i].length = buffer.length;
			cbuffers[i].start = vbuffers[i].start;
			printf("Mapped Buffers[%d].start = %x  length = %d\n",
				i, cbuffers[i].start, cbuffers[i].length);

			buffer.m.userptr = (unsigned int)cbuffers[i].start;
			buffer.length = cbuffers[i].length;
		}

		if (ioctl(cfd, VIDIOC_QBUF, &buffer) < 0) {
			perror("cam VIDIOC_QBUF");
			return -1;
		}
	}

	/*************************************************************/
	/* H3A params */

	hist_config_user.enable = 1;
	hist_config_user.source = HIST_SOURCE_CCDC;
	hist_config_user.input_bit_width = 10; /* 10-bit coming from CCDC */
	hist_config_user.num_acc_frames = 1;
	hist_config_user.hist_bins = HIST_BINS_256;
	hist_config_user.cfa = HIST_CFA_BAYER;

	/* Memory input specific registers need to be zero */
	hist_config_user.hist_h_v_info = 0;
	hist_config_user.hist_radd = 0;
	hist_config_user.hist_radd_off = 0;

	hist_config_user.wg[0] = 32; /* WB Field-to-Pattern Assignments */
	hist_config_user.wg[1] = 32; /* WB Field-to-Pattern Assignments */
	hist_config_user.wg[2] = 32; /* WB Field-to-Pattern Assignments */
	hist_config_user.wg[3] = 32; /* WB Field-to-Pattern Assignments */

	hist_config_user.num_regions = HIST_MIN_REGIONS;
	hist_config_user.reg_hor[0] = 320;	/* Region 0 size and position */
	hist_config_user.reg_ver[0] = 240;	/* Region 0 size and position */

	/* set h3a params */
	ret = ioctl(cfd, VIDIOC_PRIVATE_ISP_HIST_CFG, &hist_config_user);
	if (ret < 0) {
		printf("Error: %d, ", ret);
		perror("ISP_HIST_CFG 1");
		return ret;
	}

	/*************************************************************/
	/* Start Camera streaming */

	if (ioctl(cfd, VIDIOC_STREAMON, &creqbuf.type) < 0) {
		perror("cam VIDIOC_STREAMON");
		return -1;
	}


	/*************************************************************/
	/* Grab initial histogram data before queue/dequeue loop */

	sleep(1);

	buff_size = HIST_MEM_SIZE_BINS(256) *
					hist_config_user.num_regions;
	stats_buff = malloc(buff_size);
	if (stats_buff == NULL) {
		perror("Memory allocation failed\n");
		return -1;
	}

	hist_data_user.hist_statistics_buf = stats_buff;
	hist_data_user.update = 0;

	printf("Get the current histogram frame number...\n");
	ret = ioctl(cfd, VIDIOC_PRIVATE_ISP_HIST_REQ, &hist_data_user);
	if (ret < 0) {
		perror("ISP_HIST_REQ 1");
		return ret;
	}

	hist_data_user.frame_number = hist_data_user.curr_frame - 1;
request:
	/* request stats */
	printf("Requesting stats for frame %d, try %d\n",
				hist_data_user.frame_number, ++try);
	hist_data_user.update = REQUEST_STATISTICS;
	hist_data_user.hist_statistics_buf = stats_buff;
	ret = ioctl(cfd,  VIDIOC_PRIVATE_ISP_HIST_REQ, &hist_data_user);
	if (ret) {
		/* Stats not found, shall we retry? */
		printf("No stats, current frame is %d.\n",
			hist_data_user.curr_frame);
		hist_data_user.frame_number =
					hist_data_user.curr_frame;
		hist_data_user.update = REQUEST_STATISTICS;
		goto request;
	}

	/*************************************************************/

	if (mode == MODE_MANUAL)
		display_keys();

	/*************************************************************/
	/* Start main queue/dequeue loop */

	cfilledbuffer.type = creqbuf.type;
	vfilledbuffer.type = vreqbuf.type;
	i = 0;
	while (!done_flag) {
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

		if (i == MAX_FRAME_AUTO_MODE && mode == MODE_AUTO) {
			printf("Cancelling the streaming capture...\n");
			creqbuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
			if (ioctl(cfd, VIDIOC_STREAMOFF, &creqbuf.type) < 0) {
				perror("cam VIDIOC_STREAMOFF");
				return -1;
			}
			if (ioctl(vfd, VIDIOC_STREAMOFF,
				  &vreqbuf.type) == -1) {
				perror("dss VIDIOC_STREAMOFF");
				return -1;
			}

			printf("Done\n");
			break;
		}

		if (i >= 3) {
			cfilledbuffer.index = vfilledbuffer.index;
			while (ioctl(cfd, VIDIOC_QBUF, &cfilledbuffer) < 0)
				perror("cam VIDIOC_QBUF");
		}


		if (mode == MODE_AUTO) {
			ret = get_histogram(cfd, stats_buff, NULL);
			pass += (ret == 0);
			printf("Histogram collection results: "
				"Pass %i  Failed %i\r", pass, i-pass);
			fflush(stdout);
		}

		/* Manual mode */
		if (mode == MODE_MANUAL && kbhit()) {
			input = getch();
			if (input == 'h') {
				ret = get_histogram(cfd, stats_buff,
					&frame_number);
				printf("Histogram collection results"
					" for frame %i: %s\n", frame_number,
					(ret == 0) ? "RETRIEVED" : "FAIL");
			} else if (input == 'q') {
				done_flag = 1;
			}
		}
	}

exit:
	hist_config_user.enable = 0;

	ret = ioctl(cfd, VIDIOC_PRIVATE_ISP_HIST_CFG, &hist_config_user);
	if (ret < 0) {
		perror("ISP_HIST_CFG disabling");
		return ret;
	}

	printf("Captured %d frames!\n", i);

	/* we didn't turn off streaming yet */
	if (done_flag == 1) {
		creqbuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		if (ioctl(cfd, VIDIOC_STREAMOFF, &creqbuf.type) == -1) {
			perror("cam VIDIOC_STREAMOFF");
			return -1;
		}
		if (ioctl(vfd, VIDIOC_STREAMOFF, &vreqbuf.type) == -1) {
			perror("dss VIDIOC_STREAMOFF");
			return -1;
		}
	}

	for (i = 0; i < vreqbuf.count; i++) {
		if (vbuffers[i].start)
			munmap(vbuffers[i].start, vbuffers[i].length);
	}
	free(vbuffers);

	free(cbuffers);
	if (stats_buff)
		free(stats_buff);

	close(vfd);
	close(cfd);
}
