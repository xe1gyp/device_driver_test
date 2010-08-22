/* ========================================================================
*			 Texas Instruments OMAP(TM) Platform Software
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

/* #define DBG */

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

static int wait_for_h3a_event(int cfd, fd_set *excfds,
			struct v4l2_event *cam_ev)
{
	int ret, timeout;

	timeout = 0;

#if DBG
	printf("wait_for_h3a_event ");
	fflush(stdout);
#endif

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

		timeout++;

		if (timeout == 20) {
			printf("%s: timeout \n", __func__);
			return -1;
		}

	} while (cam_ev->type != V4L2_EVENT_OMAP3ISP_AEWB);

#if DBG
	printf(" ... Done \n");
#endif

	return 0;
}

static int wait_for_af_event(int cfd, fd_set *excfds,
			struct v4l2_event *cam_ev)
{
	int ret, timeout;

	timeout = 0;
#if DBG
	printf("wait_for_af_event ");
	fflush(stdout);
#endif

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

		timeout++;

	   if (timeout == 20) {
			printf("%s: timeout \n", __func__);
			return -1;
		}

	} while (cam_ev->type != V4L2_EVENT_OMAP3ISP_AF);

#if DBG
	printf(" ... Done \n");
#endif

	return 0;
}



int request_awb_stat(int cfd, struct isph3a_aewb_data *aewb_data_user)
{
	int ret;
#if DBG
	printf("request_awb_stat ");
	fflush(stdout);
#endif

	aewb_data_user->update = 0;
	aewb_data_user->frame_number = 0;
	aewb_data_user->curr_frame = 0;

	aewb_data_user->ts.tv_sec = 0;
	aewb_data_user->ts.tv_usec = 0;


	ret = ioctl(cfd, VIDIOC_PRIVATE_ISP_AEWB_REQ, aewb_data_user);

	if (ret) {
		perror("ioctl: VIDIOC_PRIVATE_ISP_AEWB_REQ \n");
	} else {
		printf("H3A curr_frame %d ",
			aewb_data_user->curr_frame);
	}
#if DBG
	printf(" ... done \n");
#endif

}

int request_af_stat(int cfd, struct isp_af_data *af_data_user)
{
	int ret;
#if DBG
	printf("request_af_stat ");
	fflush(stdout);
#endif

	af_data_user->update = 0;
	af_data_user->frame_number = 0;
	af_data_user->curr_frame = 0;

	af_data_user->xtrastats.ts.tv_sec = 0;
	af_data_user->xtrastats.ts.tv_usec = 0;

	ret = ioctl(cfd, VIDIOC_PRIVATE_ISP_AF_REQ, af_data_user);

/*	if (old_val == af_data_user->curr_frame)
		printf(" =================== SAME ");
	else
		old_val = af_data_user->curr_frame;   */

	if (ret < 0) {
		perror("ioctl: VIDIOC_PRIVATE_ISP_AF_REQ");
	} else {
		printf("AF frame_number %d \n",
			af_data_user->curr_frame);
	}

#if DBG
	printf(" ... done \n");
#endif
}

void isp_correct_reg_vals(struct af_configuration *cfg)
{

	/* Paxel width.
	  The paxel width is set by 2 x (PAXW+1). The paxel-width
	  range varies from 2 to 256.
	  The paxel width must be set to a minimum value of 16
	  pixels ???????? */
	cfg->paxel_config.width = (cfg->paxel_config.width / 2) - 1;

	/* Paxel height.
	  The paxel height is set by 2 x (PAXH+1). The
	  paxel-height range varies from 2 to 256.
	  cfg->paxel_config.height = (cfg->paxel_config.height / 2) - 1; */

	/*  AF line increments.
	  The number of lines to skip in a paxel is set by 2 x
	  (AFINCV+1).*/
	{
		int line_incr = (cfg->paxel_config.line_incr / 2) - 1;
		if (line_incr < 0)
			line_incr = 0;
		cfg->paxel_config.line_incr = line_incr;
	}

	/*  Paxel count in the vertical direction.
	  The number of paxels in the vertical direction is set by
	  PAXVC+1. It is illegal to have more than 128 paxels in
	  the vertical direction. We have: 0<= PAXVC <= 127.
	  cfg->paxel_config.vt_cnt = cfg->paxel_config.vt_cnt - 1; */

	/*  Paxel count in the horizontal direction.
	  The number of paxels in the horizontal direction is set by
	  PAXHC+1. It is illegal to have more than 36 paxels in the
	  horizontal direction. We have: 0<= PAXHC <= 35.
	  cfg->paxel_config.hz_cnt = cfg->paxel_config.hz_cnt - 1; */

	/* AF paxel horizontal start position.
	  Sets the horizontal position for the first pixel. The range is
	  1 to 4095. PAXSH must be equal to or greater than
	  (H3A_AFIIRSH.AFIIRSH + 1). */
	  cfg->paxel_config.hz_start = cfg->paxel_config.hz_start - 0;

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

	struct v4l2_format format;

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

	struct af_configuration af_config_user;
	struct isp_af_data af_data_user;

	struct isph3a_aewb_config aewb_config_user;
	struct isph3a_aewb_data aewb_data_user;

	int gainType = 1;
	struct v4l2_control control_exp, control_an_gain;
	struct v4l2_queryctrl qc_exp, qc_an_gain;
	unsigned int num_windows = 0;
	unsigned int awb_buff_size = 0;
	unsigned int af_buff_size = 0;
	__u8 *af_stats_buff = NULL;
	__u8 *awb_stats_buff = NULL;
	unsigned int buff_prev_size = 0;
	int frame_number;
	int done_flag = 0, skip_aewb_req_flag = 0;
	int bytes;

	struct v4l2_crop crop;
	int is_cam_streaming = 0;
	int is_dss_streaming = 0;

	/* V4L2 Video Event handling */
	struct v4l2_event_subscription cam_sub;
	struct v4l2_event cam_ev;
	fd_set excfds;

	ret = 0;

	/* H3A params */
	aewb_config_user.saturation_limit = 0x1FF;
	aewb_config_user.win_height = 18;
	aewb_config_user.win_width = 78;
	aewb_config_user.hor_win_count = 12;
	aewb_config_user.ver_win_count = 32;
	aewb_config_user.hor_win_start = 180;
	aewb_config_user.ver_win_start = 76;
	aewb_config_user.subsample_ver_inc = 6;
	aewb_config_user.subsample_hor_inc = 6;
	aewb_config_user.blk_ver_win_start = 636;
	aewb_config_user.blk_win_height = 2;
	aewb_config_user.saturation_limit = 958;
	aewb_config_user.alaw_enable = 0;
	aewb_config_user.aewb_enable = 1;

	af_config_user.mode = 1;
	af_config_user.paxel_config.width = 40;
	af_config_user.paxel_config.height = 60;
	af_config_user.paxel_config.hz_cnt = 24;
	af_config_user.paxel_config.vt_cnt = 12;
	af_config_user.paxel_config.hz_start = 168;
	af_config_user.iir_config.hz_start_pos = 0;
	af_config_user.paxel_config.vt_start = 4;
	af_config_user.paxel_config.line_incr = 1;
	af_config_user.rgb_pos = 0;
	af_config_user.af_config = H3A_AF_CFG_ENABLE;

	af_config_user.iir_config.coeff_set0[0] = 13;
	af_config_user.iir_config.coeff_set1[0] = 13;

	af_config_user.iir_config.coeff_set0[1] = 4045;
	af_config_user.iir_config.coeff_set1[1] = 4045;

	af_config_user.iir_config.coeff_set0[2] = 4045;
	af_config_user.iir_config.coeff_set1[2] = 4045;

	af_config_user.iir_config.coeff_set0[3] = 19;
	af_config_user.iir_config.coeff_set1[3] = 19;

	af_config_user.iir_config.coeff_set0[4] = 37;
	af_config_user.iir_config.coeff_set1[4] = 37;

	af_config_user.iir_config.coeff_set0[5] = 19;
	af_config_user.iir_config.coeff_set1[5] = 19;

	af_config_user.iir_config.coeff_set0[6] = 4032;
	af_config_user.iir_config.coeff_set1[6] = 4032;

	af_config_user.iir_config.coeff_set0[7] = 4044;
	af_config_user.iir_config.coeff_set1[7] = 4044;

	af_config_user.iir_config.coeff_set0[8] = 23;
	af_config_user.iir_config.coeff_set1[8] = 23;

	af_config_user.iir_config.coeff_set0[9] = 4051;
	af_config_user.iir_config.coeff_set1[9] = 4051;

	af_config_user.iir_config.coeff_set0[10] = 23;
	af_config_user.iir_config.coeff_set1[10] = 23;

	/* Turn the value into register value */
	isp_correct_reg_vals(&af_config_user);

	cfd = open_cam_device(O_RDWR, 5);
	if (cfd <= 0) {
		printf("Could not open the cam device\n");
		ret = -1;
		goto exit;
	}

	crop.c.left = 160;
	crop.c.top = 0;
	crop.c.width = 960;
	crop.c.height = 720;

	ret = ioctl(cfd, VIDIOC_S_CROP, &crop);

	if (ret < 0) {
		perror("VIDIOC_S_CROP");
		goto exit;
	}


	format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	format.fmt.pix.width = 640;
	format.fmt.pix.height = 480;
	format.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;

	/* set size & format of the video image */
	ret = ioctl(cfd, VIDIOC_S_FMT, &format);
	if (ret < 0) {
		perror("VIDIOC_S_FMT");
		ret = -1;
		goto exit;
	}

	ret = setFramerate(cfd, 30);
	if (ret < 0) {
		perror("Error setting framerate");
		ret = -1;
		goto exit;
	}

	vfd = open(VIDEO_DEVICE1, O_RDWR);

	if (vfd <= 0) {
		printf("Could no open the device %s\n", VIDEO_DEVICE1);
		vid = 0;
	}

	cformat.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	ret = ioctl(cfd, VIDIOC_G_FMT, &cformat);
	if (ret < 0) {
		perror("cam VIDIOC_G_FMT");
		ret = -1;
		goto exit;
	}
	printf("Camera Image width = %d, Image height = %d, size = %d\n",
			cformat.fmt.pix.width, cformat.fmt.pix.height,
					cformat.fmt.pix.sizeimage);

	vformat.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
	ret = ioctl(vfd, VIDIOC_G_FMT, &vformat);
	if (ret < 0) {
		perror("dss VIDIOC_G_FMT");
		ret = -1;
		goto exit;
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
			ret = -1;
			goto exit;
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
			ret = -1;
			goto exit;
		}
	}

	vreqbuf.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
	vreqbuf.memory = V4L2_MEMORY_MMAP;
	vreqbuf.count = 4;
	if (ioctl(vfd, VIDIOC_REQBUFS, &vreqbuf) == -1) {
		perror("dss VIDEO_REQBUFS");
		ret = -1;
		goto exit;
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
			ret = -1;
			goto exit;
		}
		vbuffers[i].length = buffer.length;
		vbuffers[i].start = mmap(NULL, buffer.length,
					 PROT_READ | PROT_WRITE,
					 MAP_SHARED,
					 vfd,
					 buffer.m.offset);
		if (vbuffers[i].start == MAP_FAILED) {
			perror("dss mmap");
			ret = -1;
			goto exit;
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
		ret = -1;
		goto exit;
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
			ret = -1;
			goto exit;
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
			ret = -1;
			goto exit;
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

	cam_sub.type = V4L2_EVENT_OMAP3ISP_AEWB;

	ret = ioctl(cfd, VIDIOC_SUBSCRIBE_EVENT, &cam_sub);
	if (ret < 0)
		perror("subscribe()");

	printf("Subscribed for SCM AEWB_DONE event.\n");

	/* Init file descriptor list to check with select call */
	FD_ZERO(&excfds);
	FD_SET(cfd, &excfds);


	/* Set AF params */
	ret = ioctl(cfd, VIDIOC_PRIVATE_ISP_AF_CFG, &af_config_user);
	if (ret < 0) {
		printf("Error: %d, ", ret);
		perror("ISP_AF_CFG 1");
		return ret;
	}

	/* Subscribe to internal SCM AF_DONE event */
	cam_sub.type = V4L2_EVENT_OMAP3ISP_AF;

	ret = ioctl(cfd, VIDIOC_SUBSCRIBE_EVENT, &cam_sub);
	if (ret < 0)
		perror("subscribe()");

	/* Init file descriptor list to check with select call */
	FD_ZERO(&excfds);
	FD_SET(cfd, &excfds);

	/************************************************************/
	/* Turn on streaming */

	is_cam_streaming = 1;
	if (ioctl(cfd, VIDIOC_STREAMON, &creqbuf.type) < 0) {
		perror("cam VIDIOC_STREAMON");
		ret = -1;
		goto exit;
	}

	af_buff_size = (af_config_user.paxel_config.hz_cnt + 1) *
			(af_config_user.paxel_config.vt_cnt + 1) *
			AF_PAXEL_SIZE;

	af_stats_buff = malloc(af_buff_size);
	af_data_user.af_statistics_buf = af_stats_buff;

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
			ret = -1;
			goto exit;
		}
		i++;

		if (i == 3) {
			/* Turn on streaming for video */
			is_dss_streaming = 1;
			if (ioctl(vfd, VIDIOC_STREAMON, &vreqbuf.type)) {
				perror("dss VIDIOC_STREAMON");
				ret = -1;
				goto exit;
			}
		}

		if (i >= 3) {
			/* De-queue the previous buffer from video driver */
			if (ioctl(vfd, VIDIOC_DQBUF, &vfilledbuffer)) {
				perror("dss VIDIOC_DQBUF");
				ret = -1;
				goto exit;
			}
		}

		/* capture 50 frames*/
		if (i == 50) {
			printf("Cancelling the streaming capture...\n");
			creqbuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

			is_cam_streaming = 0;
			if (ioctl(cfd, VIDIOC_STREAMOFF, &creqbuf.type) < 0) {
				perror("cam VIDIOC_STREAMOFF");
				ret = -1;
				goto exit;
			}

			is_dss_streaming = 0;
			if (ioctl(vfd, VIDIOC_STREAMOFF,
				  &vreqbuf.type) == -1) {
				perror("dss VIDIOC_STREAMOFF");
				ret = -1;
				goto exit;
			}

			printf("Done\n");
			break;
		}

		if (i >= 3) {
			cfilledbuffer.index = vfilledbuffer.index;
			while (ioctl(cfd, VIDIOC_QBUF, &cfilledbuffer) < 0)
				perror("cam VIDIOC_QBUF");
		}

		if (wait_for_h3a_event(cfd, &excfds, &cam_ev)) {
			ret = -1;
			goto exit;
		}

		request_awb_stat(cfd, &aewb_data_user);

		if (wait_for_af_event(cfd, &excfds, &cam_ev)) {
			ret = -1;
			goto exit;
		}

		request_af_stat(cfd, &af_data_user);

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

	if (ioctl(cfd, VIDIOC_PRIVATE_ISP_AEWB_CFG, &aewb_config_user) < 0) {
		ret = -1;
		perror("VIDIOC_ISP_2ACFG disabling");
		return ret;
	}

	/* we didn't turn off streaming yet */
	if (count == -1) {
		creqbuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		if (is_cam_streaming)
			if (ioctl(cfd, VIDIOC_STREAMOFF, &creqbuf.type) == -1) {
				perror("cam VIDIOC_STREAMOFF");
				ret = -1;
				goto exit;
			}
		if (is_dss_streaming)
			if (ioctl(vfd, VIDIOC_STREAMOFF, &vreqbuf.type) == -1) {
				perror("dss VIDIOC_STREAMOFF");
				ret = -1;
				goto exit;
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
	free(awb_stats_buff);
	free(af_stats_buff);

	close(vfd);
	close(cfd);
	close(kfd);

	if (ret < 0)
		return 255;
	else
	   return 0;
}
