/* ========================================================================
*             Texas Instruments OMAP(TM) Platform Software
*  (c) Copyright Texas Instruments, Incorporated.  All Rights Reserved.
*
*  Use of this software is controlled by the terms and conditions found
*  in the license agreement under which this software has been supplied.
* ========================================================================= */

#include <sys/select.h>
#include <sys/types.h>
#include <sys/stat.h>
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

static int display_h3a_stats(unsigned int num_windows,
			unsigned int buff_prev_size,
			struct isph3a_aewb_data *aewb_data_user,
			struct isph3a_aewb_config *aewb_config_user)
{
	int i, data8, data2, window, unsat_cnt;
	__u16 *buff_preview = NULL;

	printf("H3A AE/AWB: buffer to display = %d data pointer = %p\n",
		buff_prev_size, aewb_data_user->h3a_aewb_statistics_buf);
	printf("num_windows = %d\n", num_windows);
	printf("ver_windows = %d\n", aewb_config_user->ver_win_count);
	printf("hor_windows = %d\n", aewb_config_user->hor_win_count);
	printf("plus one row of black windows\n");

	unsat_cnt = 0;
	buff_preview = (__u16 *)aewb_data_user->h3a_aewb_statistics_buf;
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
	return 0;
}

static int wait_for_h3a_event(int cfd, fd_set *excfds,
			struct v4l2_event *cam_ev)
{
	int ret;

	do {
		ret = pselect(cfd + 1, NULL, NULL, excfds, NULL, NULL);
		if (ret < 0) {
			perror("cam select()");
			return -1;
		}

		ret = ioctl(cfd, VIDIOC_DQEVENT, cam_ev);
		if (ret < 0) {
			perror("cam DQEVENT");
			return -1;
		}
	} while (cam_ev->type != V4L2_EVENT_OMAP3ISP_AEWB);

	return 0;
}


int main(int argc, char *argv[])
{
	struct {
		void *start;
		size_t length;
	} *vbuffers, *cbuffers;

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
	int vfd, cfd, kfd;
	int i, ret, count = -1, memtype = V4L2_MEMORY_USERPTR;
	int index = 1, vid = 1, set_video_img = 0;
	int device = 3;
	char *pixelFmt;
	int framerate = 30;

	struct isph3a_aewb_config aewb_config_user;
	struct isph3a_aewb_data aewb_data_user;
	int gainType = 1;
	struct v4l2_control control_exp, control_an_gain;
	struct v4l2_queryctrl qc_exp, qc_an_gain;
	unsigned int num_windows = 0;
	unsigned int buff_size = 0;
	__u8 *stats_buff = NULL;
	unsigned int buff_prev_size = 0;
	int frame_number;
	int done_flag = 0, skip_aewb_req_flag = 0;
	int bytes;

	/* V4L2 Video Event handling */
	struct v4l2_event_subscription cam_sub;
	struct v4l2_event cam_ev;
	fd_set excfds;

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
	kfd = open("/dev/input/event0", O_RDONLY | O_NONBLOCK);

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

	/* set h3a params */
	ret = ioctl(cfd, VIDIOC_PRIVATE_ISP_AEWB_CFG, &aewb_config_user);
	if (ret < 0) {
		printf("Error: %d, ", ret);
		perror("ISP_AEWB_CFG 1");
		return ret;
	}

	/************************************************************/
	/* Subscribe to internal SCM AEWB_DONE event */

        memset(&cam_sub, 0x00, sizeof(cam_sub));
	cam_sub.type = V4L2_EVENT_OMAP3ISP_AEWB;

	ret = ioctl(cfd, VIDIOC_SUBSCRIBE_EVENT, &cam_sub);
	if (ret < 0)
		perror("subscribe()");

	printf("Subscribed for SCM AEWB_DONE event.\n");

	/* Init file descriptor list to check with select call */
	FD_ZERO(&excfds);
	FD_SET(cfd, &excfds);

	/************************************************************/
	/* Turn on streaming */
	if (ioctl(cfd, VIDIOC_STREAMON, &creqbuf.type) < 0) {
		perror("cam VIDIOC_STREAMON");
		return -1;
	}

	/************************************************************/
	/* Query Exposure limits */
	qc_exp.id = V4L2_CID_EXPOSURE;
	if (ioctl(cfd, VIDIOC_QUERYCTRL, &qc_exp) < 0) {
			perror("cam Query V4L2_CID_EXPOSURE");
			return -1;
	}

	/* Get current value */
	control_exp.id = V4L2_CID_EXPOSURE;
	if (ioctl(cfd, VIDIOC_G_CTRL, &control_exp) != 0) {
			perror("cam Get V4L2_CID_EXPOSURE");
			return -1;
	}

	/* Set initial value */
	control_exp.value = qc_exp.minimum + qc_exp.step;

	/************************************************************/
	/* Query Gain limits */
	qc_an_gain.id = V4L2_CID_GAIN;
	if (ioctl(cfd, VIDIOC_QUERYCTRL, &qc_an_gain) < 0) {
			perror("cam Query V4L2_CID_GAIN");
			return -1;
	}

	/* Get current value */
	control_an_gain.id = V4L2_CID_GAIN;
	if (ioctl(cfd, VIDIOC_G_CTRL, &control_an_gain) != 0) {
			perror("cam Get V4L2_CID_GAIN");
			return -1;
	}

	/* Set initial value */
	control_an_gain.value = qc_an_gain.minimum + qc_an_gain.step;

	/************************************************************/
	/* Wait for H3A event */

	printf("Syncup on frame number, before starting streaming\n");
	ret = wait_for_h3a_event(cfd, &excfds, &cam_ev);
	if (ret != 0) {
		perror("Failed wait_for_h3a_event()");
		return -1;
	}

	/************************************************************/

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

	/* Request stats */
	aewb_data_user.frame_number = aewb_data_user.curr_frame - 1;
	frame_number = aewb_data_user.frame_number;
	printf("Requesting stats for frame %d\n",
						frame_number);
	aewb_data_user.update = REQUEST_STATISTICS;
	aewb_data_user.h3a_aewb_statistics_buf = stats_buff;
	ret = ioctl(cfd,  VIDIOC_PRIVATE_ISP_AEWB_REQ, &aewb_data_user);

	/* Display stats */
	if (!ret)
		display_h3a_stats(num_windows,
					buff_prev_size,
					&aewb_data_user,
					&aewb_config_user);
	else
		printf("No stats, current frame is %d.\n",
			aewb_data_user.curr_frame);


	/************************************************************/
	/* Capture 1000 frames OR when we hit the passed number of frames */

	sleep(1);
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

		if (i == count) {
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

		/* Checks if key has been pressed */
		bytes = read(kfd, &keyinfo, sizeof(struct input_event));
		if ((bytes < 0) && (errno != EAGAIN)) {
			return 1;
		} else if (bytes > 0 && keyinfo.code == 35) {
			printf("\n");
			goto exit;
		} else if (bytes > 0 && keyinfo.code == 46) {
			wait_for_h3a_event(cfd, &excfds, &cam_ev);

			aewb_data_user.update = 0;
			ret = ioctl(cfd,  VIDIOC_PRIVATE_ISP_AEWB_REQ,
						&aewb_data_user);
			if (ret < 0) {
				perror("ISP_AEWB_REQ 5");
				return ret;
			}
			aewb_data_user.frame_number =
					aewb_data_user.curr_frame - 1;
			aewb_data_user.update = REQUEST_STATISTICS;
			aewb_data_user.h3a_aewb_statistics_buf =
							stats_buff;
			printf("Obtaining stats frame:%d\n",
					aewb_data_user.frame_number);
			ret = ioctl(cfd,  VIDIOC_PRIVATE_ISP_AEWB_REQ,
						&aewb_data_user);
			if (ret) {
				perror("ISP_AEWB_REQ 6");
			} else {
				/* Display stats */
				display_h3a_stats(num_windows,
						buff_prev_size,
						&aewb_data_user,
						&aewb_config_user);
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
			control_an_gain.value += qc_an_gain.step;
			printf("Analog gain: %d\r", control_an_gain.value);
			fflush(stdout);

			if (control_an_gain.value >= qc_an_gain.maximum) {
				control_an_gain.value = qc_an_gain.maximum;
				done_flag = 1;
			}

			control_an_gain.id = V4L2_CID_GAIN;
			ret = ioctl(cfd, VIDIOC_S_CTRL, &control_an_gain);
			if (ret != 0)
				printf("Failed to set V4L2_CID_GAIN to %d\n",
					control_an_gain.value);
			skip_aewb_req_flag = 1;
			break;

		case 7:
			control_exp.value += qc_exp.step;
			printf("Shutter speed: %d\r", control_exp.value);
			fflush(stdout);

			if (control_exp.value >= qc_exp.maximum) {
				control_exp.value = qc_exp.maximum;
				done_flag = 1;
			}

			control_exp.id = V4L2_CID_EXPOSURE;
			ret = ioctl(cfd, VIDIOC_S_CTRL, &control_exp);
			if (ret != 0)
				printf("Failed to set V4L2_CID_EXPOSURE"
					" to %d\n", control_exp.value);
			skip_aewb_req_flag = 1;
			break;

		case 8:
			aewb_data_user.dgain = DIGITAL_GAIN_DEFAULT;
			printf("Digital gain: %d\r", aewb_data_user.dgain);
			fflush(stdout);
			aewb_data_user.update = SET_DIGITAL_GAIN;
			done_flag = 1;
			break;

		case 9:
			aewb_data_user.wb_gain_b = BLUE_GAIN_DEFAULT;
			printf("Blue gain: %d\r", aewb_data_user.wb_gain_b);
			fflush(stdout);
			aewb_data_user.update = SET_DIGITAL_GAIN;
			done_flag = 1;
			break;

		case 10:
			aewb_data_user.wb_gain_r = GB_GAIN_DEFAULT;
			printf("Red gain: %d\r", aewb_data_user.wb_gain_r);
			fflush(stdout);
			aewb_data_user.update = SET_DIGITAL_GAIN;
			done_flag = 1;
			break;

		case 11:
			aewb_data_user.wb_gain_gb = GB_GAIN_DEFAULT;
			printf("Green/Blue gain: %d\r",
				aewb_data_user.wb_gain_gb);
			fflush(stdout);
			aewb_data_user.update = SET_DIGITAL_GAIN;
			done_flag = 1;
			break;

		case 12:
			aewb_data_user.wb_gain_gr = GR_GAIN_DEFAULT;
			printf("Green/Red gain: %d\r",
				aewb_data_user.wb_gain_gr);
			fflush(stdout);
			aewb_data_user.update = SET_DIGITAL_GAIN;
			done_flag = 1;
			break;

		default:
			printf("Test is in incorrect state");
			done_flag = 1;
		}

		if (!skip_aewb_req_flag) {
			wait_for_h3a_event(cfd, &excfds, &cam_ev);
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

	printf("Captured %d frames!\n", i);

	/* we didn't turn off streaming yet */
	if (count == -1) {
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

	for (i = 0; i < creqbuf.count; i++) {
		if (cbuffers[i].start) {
			if (memtype == V4L2_MEMORY_USERPTR)
				free(cbuffers[i].start);
			else
				munmap(cbuffers[i].start, cbuffers[i].length);
		}
	}

	free(cbuffers);
	free(stats_buff);

	close(vfd);
	close(cfd);
	close(kfd);
}
