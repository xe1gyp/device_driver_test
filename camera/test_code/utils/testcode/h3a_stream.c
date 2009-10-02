/* ============================================================================
*             Texas Instruments OMAP(TM) Platform Software
*  (c) Copyright Texas Instruments, Incorporated.  All Rights Reserved.
*
*  Use of this software is controlled by the terms and conditions found
*  in the license agreement under which this software has been supplied.
* ========================================================================== */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <linux/videodev2.h>
#include <errno.h>
#include <mach/isp_user.h>
#include "kbget.h"

#define VIDEO_DEVICE1 "/dev/video1"
#define VIDEO_DEVICE2 "/dev/video2"
#define BYTES_PER_WINDOW	16
#define DSS_STREAM_START_FRAME	3

#define DIGITAL_GAIN_DEFAULT	0x100
#define DIGITAL_GAIN_MAX	0x500
#define BLUE_GAIN_DEFAULT	0x94
#define BLUE_GAIN_MAX		0x194
#define RED_GAIN_DEFAULT	0x68
#define RED_GAIN_MAX		0x168
#define GB_GAIN_DEFAULT		0x5C
#define GB_GAIN_MAX		0x15C
#define GR_GAIN_DEFAULT		0x5C
#define GR_GAIN_MAX		0x15C
#define ANALOG_GAIN_DEFAULT	0x40
#define ANALOG_GAIN_MIN		0x08
#define ANALOG_GAIN_MAX		0x7F
#define SHUTTER_SPEED_DEFAULT	20000
#define SHUTTER_SPEED_MIN	100
#define SHUTTER_SPEED_MAX	33000

int cfd, vfd;

static void usage(void)
{
	printf("h3a_stream [camDevice] [framerate] [vid] [gain]\n");
	printf("\tEnable 2A and capture  of 1000 frames using video driver for "
								"rendering\n");
	printf("\t[camDevice] Camera device to be open\n\t\t 1:Micron sensor "
					"2:OV sensor\n");
	printf("\t[framerate] is the framerate to be used, if no value"
				" is given \n\t           30 fps is default\n");
	printf("\t[vid] is the video pipeline to be used. Valid vid is 1"
							"(default) or 2\n");
	printf("\t[gain] \t1: White Balance Digital Gain\n"
		"\t\t2: White Balance Blue Gain\n "
		"\t\t3: White Balance Red Gain\n"
		"\t\t4: White Balance Green Blue Gain\n"
		"\t\t5: White Balance Green Red Gain\n"
		"\t\t6: Sensor Analog Gain\n"
		"\t\t7: Shutter Speed\n");
}

int main(int argc, char *argv[])
{
	struct {
		void *start;
		size_t length;
	} *vbuffers;

	/* Structure stores values for key strokes */
	struct input_event{
		struct timeval time;
		unsigned short type;
		unsigned short code;
		unsigned int value;
	} keyinfo;

	struct v4l2_capability capability;
	struct v4l2_format cformat, vformat;
	struct v4l2_requestbuffers creqbuf, vreqbuf;
	struct v4l2_buffer cfilledbuffer, vfilledbuffer;
	struct v4l2_control control;
	struct v4l2_queryctrl qc;

	int vid = 1, set_video_img = 0, i, ret;

	unsigned int num_windows = 0;
	unsigned int buff_size = 0;
	struct isph3a_aewb_config aewb_config_user;
	struct isph3a_aewb_data aewb_data_user;
	__u16 *buff_preview = NULL;
	__u8 *stats_buff = NULL;
	unsigned int buff_prev_size = 0;
	int data8, data2, window, unsat_cnt;
	int frame_number;
	int j = 0;
	int device = 1;
	int index = 1;
	int framerate = 30;
	int gainType = 1;
	int done_flag = 0, skip_aewb_req_flag = 0;
	int bytes;
	unsigned int exp_max, exp_min, exp_step, exp_cur;
	unsigned int gain_max, gain_min, gain_step, gain_cur;

	/* H3A params */
	aewb_config_user.saturation_limit = 0x1FF;
	aewb_config_user.win_height = 10;
	aewb_config_user.win_width = 10;
	aewb_config_user.ver_win_count = 2;
	aewb_config_user.hor_win_count = 3;
	aewb_config_user.ver_win_start = 3;
	aewb_config_user.hor_win_start = 10;
	aewb_config_user.blk_ver_win_start = 30;
	/* blk_win_height MUST NOT be the same height as win_height*/
	aewb_config_user.blk_win_height = 8;
	aewb_config_user.subsample_ver_inc = 2;
	aewb_config_user.subsample_hor_inc = 2;
	aewb_config_user.alaw_enable = 1;
	aewb_config_user.aewb_enable = 1;

	/* Open keypad input device */
	int kfd = open("/dev/input/event0", O_RDONLY | O_NONBLOCK);

	if ((argc > 1) && (!strcmp(argv[1], "?"))) {
		usage();
		return 0;
	}
	if (argc > index) {
		device = atoi(argv[index]);
		index++;
	}
	if (argc > index) {
		framerate = atoi(argv[index]);
		index++;
		printf("Framerate = %d\n", framerate);
	} else
		printf("Using framerate = 30, default value\n");

	cfd = open_cam_device(O_RDWR, device);
	if (cfd <= 0) {
		printf("Could not open the cam device\n");
		return -1;
	}

	if (argc > index) {
		vid = atoi(argv[index]);
		index++;
		if ((vid != 1) && (vid != 2)) {
			printf("vid has to be 1 or 2! vid=%d, argv[3]=%s\n",
							vid, argv[index - 1]);
			usage();
			return 0;
		}
	}

	/* Added gain parameter */
	if (argc > index) {
		gainType = atoi(argv[index]);
		if ((gainType < 1) || (gainType > 7)) {
			printf("gain has to be 1 - 7, argv[%d]=%s\n", index,
				argv[index]);
			usage();
			return 0;
		}
	}

	ret = cam_ioctl(cfd, "YUYV", "QVGA");
	ret = setFramerate(cfd, framerate);
	vfd = open((vid == 1) ? VIDEO_DEVICE1 : VIDEO_DEVICE2, O_RDWR);
	if (vfd <= 0) {
		printf("Could not open %s\n", (vid == 1) ? VIDEO_DEVICE1 :
								VIDEO_DEVICE2);
		return -1;
	} else {
		printf("openned %s for rendering\n",
			(vid == 1) ? VIDEO_DEVICE1 : VIDEO_DEVICE2);
	}

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


	/************************************************************/
	/* turn on streaming for camera */

	if (ioctl(cfd, VIDIOC_STREAMON, &creqbuf.type) < 0) {
		perror("cam VIDIOC_STREAMON");
		return -1;
	}


	/************************************************************/
	/* Query Exposure limits */
	qc.id = V4L2_CID_EXPOSURE;
	if (ioctl(cfd, VIDIOC_QUERYCTRL, &qc) < 0) {
			perror("cam Query V4L2_CID_EXPOSURE");
			return -1;
	}
	exp_max = qc.maximum;
	exp_min = qc.minimum;
	exp_step = qc.step;

	/* Set initial value */
	aewb_data_user.shutter = exp_min + exp_step;

	/* Get current value */
	control.id = V4L2_CID_EXPOSURE;
	if (ioctl(cfd, VIDIOC_G_CTRL, &control) != 0) {
			perror("cam Get V4L2_CID_EXPOSURE");
			return -1;
	}
	exp_cur = control.value;

	/************************************************************/
	/* Query Gain limits */
	qc.id = V4L2_CID_GAIN;
	if (ioctl(cfd, VIDIOC_QUERYCTRL, &qc) < 0) {
			perror("cam Query V4L2_CID_GAIN");
			return -1;
	}
	gain_max = qc.maximum;
	gain_min = qc.minimum;
	gain_step = qc.step;

	/* Set initial value */
	aewb_data_user.gain = gain_min + gain_step;

	/* Get current value */
	control.id = V4L2_CID_GAIN;
	if (ioctl(cfd, VIDIOC_G_CTRL, &control) != 0) {
			perror("cam Get V4L2_CID_GAIN");
			return -1;
	}
	gain_cur = control.value;

	/************************************************************/


	/* capture 1000 frames */
	cfilledbuffer.type = creqbuf.type;
	vfilledbuffer.type = vreqbuf.type;
	i = 0;
	sleep(1);
	/* set h3a params */
	ret = ioctl(cfd, VIDIOC_PRIVATE_ISP_AEWB_CFG, &aewb_config_user);
	if (ret < 0) {
		printf("Error: %d, ", ret);
		perror("ISP_AEWB_CFG 1");
		return ret;
	}

	num_windows = ((aewb_config_user.ver_win_count
			* aewb_config_user.hor_win_count)
			+ aewb_config_user.hor_win_count);
	buff_size = ((num_windows + (num_windows / 8) +
		((num_windows % 8) ? 1 : 0)) * BYTES_PER_WINDOW);
	buff_prev_size = (buff_size / 2);

	stats_buff = malloc(buff_size);

	aewb_data_user.h3a_aewb_statistics_buf = NULL;

	/* Digital gain in U10Q8 format */
	aewb_data_user.dgain = 0x100;	/* Gain = 1.000 */
	/* Particular gains in U8Q5 format */
	aewb_data_user.wb_gain_b = 0x94;
	aewb_data_user.wb_gain_r = 0x68;
	aewb_data_user.wb_gain_gb = 0x5C;
	aewb_data_user.wb_gain_gr = 0x5C;

	aewb_data_user.update = (SET_COLOR_GAINS | SET_DIGITAL_GAIN);
	aewb_data_user.frame_number = 8; /* dummy */

	printf("Setting first parameters \n");
	ret = ioctl(cfd, VIDIOC_PRIVATE_ISP_AEWB_REQ, &aewb_data_user);
	if (ret < 0) {
		perror("ISP_AEWB_REQ 1");
		return ret;
	}
	aewb_data_user.frame_number = aewb_data_user.curr_frame + 3;
request:
	frame_number = aewb_data_user.frame_number;
	/* request stats */
	printf("Requesting stats for frame %d, try %d\n",
						frame_number, j);
	aewb_data_user.update = REQUEST_STATISTICS;
	aewb_data_user.h3a_aewb_statistics_buf = stats_buff;
	ret = ioctl(cfd,  VIDIOC_PRIVATE_ISP_AEWB_REQ, &aewb_data_user);
	if (ret < 0) {
		perror("ISP_AEWB_REQ 2");
		return ret;
	}

	if (aewb_data_user.h3a_aewb_statistics_buf == NULL) {
		printf("NULL buffer, current frame is  %d.\n",
			aewb_data_user.curr_frame);
		aewb_data_user.frame_number =
					aewb_data_user.curr_frame + 10;
		aewb_data_user.update = REQUEST_STATISTICS;
		aewb_data_user.h3a_aewb_statistics_buf = stats_buff;
		goto request;
	}

	/* Display stats */
	buff_preview = (__u16 *)aewb_data_user.h3a_aewb_statistics_buf;
	printf("H3A AE/AWB: buffer to display = %d data pointer = %p\n",
		buff_prev_size, aewb_data_user.h3a_aewb_statistics_buf);
	printf("num_windows = %d\n", num_windows);
	printf("ver_windows = %d\n", aewb_config_user.ver_win_count);
	printf("hor_windows = %d\n", aewb_config_user.hor_win_count);
	printf("plus one row of black windows\n");

	unsat_cnt = 0;
	for (i = 0; i < (buff_prev_size); i++) {
		data8 = (i + 1) % 8;
		data2 = (i + 1) % 2;
		window = (i + 1) / 8;
		printf("%05d ", buff_preview[i]);
		if (0 == data8) {
			if (((window > 1) && (0 == (window % 9))) ||
			    (window == ((num_windows + (num_windows / 8) +
			     ((num_windows % 8) ? 1 : 0))))) {
				printf("   Unsaturated block count\n");
				unsat_cnt++;
			} else {
				printf("    Window %5d\n", (window - 1) -
					unsat_cnt);
			}
		}
		if (0 == data2)
			printf("\n");
	}

	sleep(1);

	j++;
	if (j < 2) {
		aewb_data_user.frame_number += 100;
		aewb_data_user.update = REQUEST_STATISTICS;
		aewb_data_user.h3a_aewb_statistics_buf = stats_buff;
		goto request;
	}

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

		if (i == DSS_STREAM_START_FRAME) {
			/* Turn on streaming for video */
			if (ioctl(vfd, VIDIOC_STREAMON, &vreqbuf.type)) {
				perror("dss VIDIOC_STREAMON");
				return -1;
			}
		}

		if (i >= DSS_STREAM_START_FRAME) {
			/* De-queue the previous buffer from video driver */
			if (ioctl(vfd, VIDIOC_DQBUF, &vfilledbuffer)) {
				perror("dss VIDIOC_DQBUF");
				return;
			}
		}

		if (i >= DSS_STREAM_START_FRAME) {
			cfilledbuffer.index = vfilledbuffer.index;
			while (ioctl(cfd, VIDIOC_QBUF, &cfilledbuffer) < 0)
				perror("cam VIDIOC_QBUF");
		}

		/* Checks if key has been pressed */
		bytes = read(kfd, &keyinfo, sizeof(struct input_event));
		if ((bytes < 0) && (errno != EAGAIN)) {
			return 1;
		} else if (bytes > 0 && keyinfo.code == 35) {
			printf("\n");
			goto exit;
		} else if (bytes > 0 && keyinfo.code == 46) {
			aewb_data_user.update = 0;
			ret = ioctl(cfd,  VIDIOC_PRIVATE_ISP_AEWB_REQ,
						&aewb_data_user);
			if (ret < 0) {
				perror("ISP_AEWB_REQ 5");
				return ret;
			}
			aewb_data_user.frame_number =
					aewb_data_user.curr_frame;
			aewb_data_user.update = REQUEST_STATISTICS;
			aewb_data_user.h3a_aewb_statistics_buf =
							stats_buff;
				printf("Obtaining stats frame:%d\n",
					aewb_data_user.frame_number);
			ret = ioctl(cfd,  VIDIOC_PRIVATE_ISP_AEWB_REQ,
						&aewb_data_user);
			if (ret < 0) {
				perror("ISP_AEWB_REQ 6");
				return ret;
			}
				/* Display stats */
			buff_preview =
			(__u16 *)aewb_data_user.h3a_aewb_statistics_buf;
				unsat_cnt = 0;
			for (i = 0; i < (buff_prev_size); i++) {
				data8 = (i + 1) % 8;
				data2 = (i + 1) % 2;
				window = (i + 1) / 8;
				printf("%05d ", buff_preview[i]);
				if (0 == data8) {
					if (((window > 1) &&
						(0 == (window % 9)))
						|| (window ==
						((num_windows +
						(num_windows / 8) +
						((num_windows % 8) ?
						1 : 0))))) {
						printf("   Unsaturated "
							"block "
							"count\n");
						unsat_cnt++;
					} else {
						printf("    Window %5d\n",
							(window - 1) -
							unsat_cnt);
					}
				}
				if (0 == data2)
					printf("\n");
			}
		}

		/*Switch emulates state machine to test different gains
		  and shutter speed */
		switch (gainType) {
			/* State 1 cycles digital gain */
		case 1:
			aewb_data_user.dgain = aewb_data_user.dgain + 0x01;
			printf("Digital gain: %d\r", aewb_data_user.dgain);
			fflush(stdout);
			aewb_data_user.update = SET_DIGITAL_GAIN;
			if (aewb_data_user.dgain == DIGITAL_GAIN_MAX)
				gainType = 8;
			break;

		case 2:
			aewb_data_user.wb_gain_b =
					aewb_data_user.wb_gain_b + 0x01;
			printf("Blue gain: %d\r", aewb_data_user.wb_gain_b);
			fflush(stdout);
			aewb_data_user.update = SET_COLOR_GAINS;
			if (aewb_data_user.wb_gain_b == BLUE_GAIN_MAX)
				gainType = 9;
			break;

		case 3:
			aewb_data_user.wb_gain_r =
					aewb_data_user.wb_gain_r + 0x01;
			printf("Red gain: %d\r", aewb_data_user.wb_gain_r);
			fflush(stdout);
			aewb_data_user.update = SET_COLOR_GAINS;
			if (aewb_data_user.wb_gain_r == RED_GAIN_MAX)
				gainType = 10;
			break;

		case 4:
			aewb_data_user.wb_gain_gb =
					aewb_data_user.wb_gain_gb + 0x01;
			printf("Green/Blue gain: %d\r",
				aewb_data_user.wb_gain_gb);
			fflush(stdout);
			aewb_data_user.update = SET_COLOR_GAINS;
			if (aewb_data_user.wb_gain_gb == GB_GAIN_MAX)
				gainType = 11;
			break;

		case 5:
			aewb_data_user.wb_gain_gr =
					aewb_data_user.wb_gain_gr + 0x01;
			printf("Green/Red gain: %d\r",
				aewb_data_user.wb_gain_gr);
			fflush(stdout);
			aewb_data_user.update = SET_COLOR_GAINS;
			if (aewb_data_user.wb_gain_gr == GR_GAIN_MAX)
				gainType = 12;
			break;

		case 6:
			aewb_data_user.gain =
					aewb_data_user.gain + gain_step;
			printf("Analog gain: %d\r", aewb_data_user.gain);
			fflush(stdout);

			if (aewb_data_user.gain >= gain_max) {
				aewb_data_user.gain = gain_cur;
				done_flag = 1;
			}

			control.id = V4L2_CID_GAIN;
			control.value = aewb_data_user.gain;
			ret = ioctl(cfd, VIDIOC_S_CTRL, &control);
			if (ret != 0)
				printf("Failed to set V4L2_CID_GAIN to %d\n",
					control.value);
			skip_aewb_req_flag = 1;
			break;

		case 7:
			aewb_data_user.shutter =
					aewb_data_user.shutter + exp_step;
			printf("Shutter speed: %d\r", aewb_data_user.shutter);
			fflush(stdout);

			if (aewb_data_user.shutter >= exp_max) {
				aewb_data_user.shutter = exp_cur;
				done_flag = 1;
			}

			control.id = V4L2_CID_EXPOSURE;
			control.value = aewb_data_user.shutter;
			ret = ioctl(cfd, VIDIOC_S_CTRL, &control);
			if (ret != 0)
				printf("Failed to set V4L2_CID_EXPOSURE"
					" to %d\n", control.value);
			skip_aewb_req_flag = 1;
			break;

		case 8:
			aewb_data_user.dgain = aewb_data_user.dgain + 0x100;
			printf("Digital gain: %d\r", aewb_data_user.dgain);
			fflush(stdout);
			aewb_data_user.update = SET_DIGITAL_GAIN;
			if (aewb_data_user.dgain == DIGITAL_GAIN_DEFAULT)
				done_flag = 1;
			break;

		case 9:
			aewb_data_user.wb_gain_b =
					aewb_data_user.wb_gain_b + 0x100;
			printf("Blue gain: %d\r", aewb_data_user.wb_gain_b);
			fflush(stdout);
			aewb_data_user.update = SET_DIGITAL_GAIN;
			if (aewb_data_user.wb_gain_b == BLUE_GAIN_DEFAULT)
				done_flag = 1;
			break;

		case 10:
			aewb_data_user.wb_gain_r =
					aewb_data_user.wb_gain_r + 0x100;
			printf("Red gain: %d\r", aewb_data_user.wb_gain_r);
			fflush(stdout);
			aewb_data_user.update = SET_DIGITAL_GAIN;
			if (aewb_data_user.wb_gain_r == RED_GAIN_DEFAULT)
				done_flag = 1;
			break;

		case 11:
			aewb_data_user.wb_gain_gb =
					aewb_data_user.wb_gain_gb + 0x100;
			printf("Green/Blue gain: %d\r",
				aewb_data_user.wb_gain_gb);
			fflush(stdout);
			aewb_data_user.update = SET_DIGITAL_GAIN;
			if (aewb_data_user.wb_gain_gb == GB_GAIN_DEFAULT)
				done_flag = 1;
			break;

		case 12:
			aewb_data_user.wb_gain_gr =
					aewb_data_user.wb_gain_gr + 0x100;
			printf("Green/Red gain: %d\r",
				aewb_data_user.wb_gain_gr);
			fflush(stdout);
			aewb_data_user.update = SET_DIGITAL_GAIN;
			if (aewb_data_user.wb_gain_gr == GR_GAIN_DEFAULT)
				done_flag = 1;
			break;

		default:
			printf("Test is in incorrect state");
			done_flag = 1;
		}

		if (!skip_aewb_req_flag) {
			ret = ioctl(cfd, VIDIOC_PRIVATE_ISP_AEWB_REQ,
							&aewb_data_user);
			if (ret < 0) {
				perror("ISP_AEWB_REQ 7");
				goto exit;
			}
		}

	}

exit:
	aewb_config_user.aewb_enable = 0;
	close(kfd);

	/* H3A params */
	aewb_config_user.saturation_limit = 0x1FF;
	aewb_config_user.win_height = 10;
	aewb_config_user.win_width = 10;
	aewb_config_user.ver_win_count = 2;
	aewb_config_user.hor_win_count = 3;
	aewb_config_user.ver_win_start = 3;
	aewb_config_user.hor_win_start = 10;
	aewb_config_user.blk_ver_win_start = 30;
	/* blk_win_height MUST NOT be the same height as win_height*/
	aewb_config_user.blk_win_height = 8;
	aewb_config_user.subsample_ver_inc = 2;
	aewb_config_user.subsample_hor_inc = 2;

	ret = ioctl(cfd, VIDIOC_PRIVATE_ISP_AEWB_CFG, &aewb_config_user);
	if (ret < 0) {
		printf(" Error: %d, ", ret);
		perror("VIDIOC_ISP_2ACFG disabling");
		return ret;
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
	free(stats_buff);

	close(cfd);
	close(vfd);
}
