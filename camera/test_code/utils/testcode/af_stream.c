/* =========================================================================
 *             Texas Instruments OMAP(TM) Platform Software
 *  (c) Copyright Texas Instruments, Incorporated.  All Rights Reserved.
 *
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied.
 * ========================================================================= */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <linux/ioctl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <linux/videodev2.h>
#include <linux/errno.h>
#include <errno.h>
#include "kbget.h"
#include "aftest.h"

#define VIDEO_DEVICE1 "/dev/v4l/video1"
#define VIDEO_DEVICE2 "/dev/v4l/video2"

#define V4L2_CID_PRIVATE_ISP_COLOR_FX     (V4L2_CID_PRIVATE_BASE + 0)
#define V4L2_CID_PRIVATE_ISP_CCDC_CFG     (V4L2_CID_PRIVATE_BASE + 1)
#define V4L2_CID_PRIVATE_ISP_PRV_CFG      (V4L2_CID_PRIVATE_BASE + 2)
#define V4L2_CID_PRIVATE_ISP_LSC_UPDATE   (V4L2_CID_PRIVATE_BASE + 3)
#define V4L2_CID_PRIVATE_ISP_AEWB_CFG     (V4L2_CID_PRIVATE_BASE + 4)
#define V4L2_CID_PRIVATE_ISP_AEWB_REQ     (V4L2_CID_PRIVATE_BASE + 5)
#define V4L2_CID_PRIVATE_ISP_AF_CFG       (V4L2_CID_PRIVATE_BASE + 6)
#define V4L2_CID_PRIVATE_ISP_AF_REQ       (V4L2_CID_PRIVATE_BASE + 7)
#define V4L2_CID_PRIVATE_ISP_HIST_CFG     (V4L2_CID_PRIVATE_BASE + 8)
#define V4L2_CID_PRIVATE_ISP_HIST_REQ     (V4L2_CID_PRIVATE_BASE + 9)

/* Flags for update field */
#define REQUEST_STATISTICS      (1 << 0)
#define LENS_DESIRED_POSITION   (1 << 1)
#define LENS_CURRENT_POSITION   (1 << 2)

#define DEFAULT_PIXEL_FMT "YUYV"
#define DEFAULT_VIDEO_SIZE "QVGA"

#ifndef u32
#define u32 unsigned long
#endif /* u32 */

#ifndef u16
#define u16 unsigned short
#endif /* u16 */

#ifndef u8
#define u8 unsigned char
#endif /* u8 */

int cfd, vfd;

static void usage(void)
{
	printf("af_stream [vid] [lens_pos] [framerate]\n");
	printf("\tSteaming capture of 1000 frames using video driver for"
							" rendering\n");
	printf("\t[vid] is the video pipeline to be used. Valid vid is"
							" 1(default) or 2\n");
	printf("\t[lens_pos] is the initial lens position to be used.\n"
					"\t\t- 1: Macro position\n"
					"\t\t- 2: Medium position\n"
					"\t\t- 3: Infinite position\n");
	printf("\t[framerate] is the framerate to be used, if no value"
					"is given 30 fps is default\n");
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
	int vid = 1, set_video_img = 0, i, ret;

	/*************************************************************/
	unsigned int num_windows = 0, num_color_windows = 0;
	unsigned int buff_size = 0;
	struct af_configuration af_config_user;
	struct isp_af_data af_data_user;
	u16 *buff_preview = NULL;
	u16 *buff_char = NULL;
	u8 *stats_buff = NULL;
	unsigned int buff_prev_size = 0;
	int data8, data2, window, unsat_cnt, k;
	int input, fd, new_gain = 0;
	u16 wposn, rposn;
	int frame_number;
	int enable = 1, j = 0, index = 1;
	struct v4l2_control control_af_config, control_af_request;
	FILE *fp_out;
	int framerate = 30;
	wposn = 1;

	memset(&control_af_config, 0, sizeof(control_af_config));
	memset(&control_af_request, 0, sizeof(control_af_request));
	af_config_user.alaw_enable = H3A_AF_ENABLE;	/* Enable Alaw */
	af_config_user.hmf_config.enable = H3A_AF_DISABLE;
	af_config_user.iir_config.hz_start_pos = 0;
	af_config_user.paxel_config.height = 16;
	af_config_user.paxel_config.width = 16;
	af_config_user.paxel_config.line_incr = 0;
	af_config_user.paxel_config.vt_start = 0;
	af_config_user.paxel_config.hz_start = 2;
	af_config_user.paxel_config.hz_cnt = 8;
	af_config_user.paxel_config.vt_cnt = 8;
	af_config_user.af_config = H3A_AF_ENABLE;
	af_config_user.hmf_config.threshold = 0;
	/* Set Accumulator mode */
	af_config_user.mode = ACCUMULATOR_SUMMED;

	for (index = 0; index < 11; index++) {
		af_config_user.iir_config.coeff_set0[index] = 12;
		af_config_user.iir_config.coeff_set1[index] = 12;
	}
	/* ********************************************************* */
	fp_out = fopen("af_mid.st", "wb");
	if (fp_out == NULL) {
		printf("ERROR opening output file!\n");
		return -EACCES;
	}
	/* ********************************************************* */
	index = 1;
	if (argc < 2) {
		printf("ERROR: Missing parameters!\n");
		usage();
		return 0;
	}
	if ((argc > 1) && (!strcmp(argv[1], "?"))) {
		usage();
		return 0;
	}
	
	if (argc > index) {
		vid = atoi(argv[index]);
		if ((vid != 1) && (vid != 2)) {
			printf("vid has to be 1 or 2! vid=%d, argv[1]=%s\n",
								vid, argv[1]);
			usage();
			return 0;
		}	
	}
	index++;
	if (argc > index) {
		wposn = atoi(argv[index]);
		index++;
	}
	
	if (argc > index) {
		framerate = atoi(argv[index]);
		printf("Framerate = %d\n",framerate);
	} else
		printf("Using framerate = 30, default value\n");
	
	cfd = open_cam_device(O_RDWR, 1);
	if (cfd <= 0) {
		printf("Could not open the cam device\n");
		return -1;
	}

	vfd = open((vid == 1)?VIDEO_DEVICE1:VIDEO_DEVICE2, O_RDWR);
	if (vfd <= 0) {
		printf("Could not open %s\n",
				(vid == 1)?VIDEO_DEVICE1:VIDEO_DEVICE2);
		return -1;
	} else
		printf("openned %s for rendering\n",
				(vid == 1)?VIDEO_DEVICE1:VIDEO_DEVICE2);

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
	
	ret = setFramerate(cfd, framerate);
	if (ret < 0){
		printf("ERROR: VIDIOC_S_PARM ioctl cam\n");
		return -1;
	}
	
	ret = cam_ioctl(cfd, DEFAULT_PIXEL_FMT, DEFAULT_VIDEO_SIZE);
	if (ret < 0){
		printf("ERROR: VIDIOC_S_FMT ioctl cam\n");
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
						(cformat.fmt.pix.height !=
						vformat.fmt.pix.height) ||
						(cformat.fmt.pix.sizeimage !=
						vformat.fmt.pix.sizeimage)) {
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
						(cformat.fmt.pix.height !=
						vformat.fmt.pix.height) ||
						(cformat.fmt.pix.sizeimage !=
						vformat.fmt.pix.sizeimage) ||
						(cformat.fmt.pix.pixelformat !=
						vformat.fmt.pix.pixelformat)) {
			printf("can't make camera and video image"
							" compatible!\n");
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
		vbuffers[i].length = buffer.length;
		vbuffers[i].start = mmap(NULL, buffer.length, PROT_READ |
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

	/* turn on streaming on both drivers */
	if (ioctl(cfd, VIDIOC_STREAMON, &creqbuf.type) < 0) {
		perror("cam VIDIOC_STREAMON");
		return -1;
	}
	if (ioctl(vfd, VIDIOC_STREAMON, &vreqbuf.type) < 0) {
		perror("video VIDIOC_STREAMON");
		return -1;
	}

	/* caputure 1000 frames */
	cfilledbuffer.type = creqbuf.type;
	vfilledbuffer.type = vreqbuf.type;
	i = 0;
	vfilledbuffer.index = -1;
	sleep(5);
	/* ***************************************************************** */

	if (wposn == 1)
		wposn = 0xFF; /* MACRO */
	else if (wposn == 2)
		wposn = 0x7F;
	else if (wposn == 3)
		wposn = 0x00; /* Infinite */
	else {
		printf("Invalid Focus \n");
		return -1;
	}
	control_af_config.id = V4L2_CID_PRIVATE_ISP_AF_CFG;
	control_af_config.value = (int)&af_config_user;
	/* set h3a params */
	ret = ioctl(cfd, VIDIOC_S_CTRL, &control_af_config);
	if (ret < 0) {
		printf("Error: %d, ", ret);
		perror("ISP_AF_CFG 1");
		return ret;
	}

	buff_size = (af_config_user.paxel_config.hz_cnt + 1) *
			(af_config_user.paxel_config.vt_cnt + 1) *
			AF_PAXEL_SIZE;
			
	stats_buff = malloc(buff_size);

	buff_prev_size = (buff_size / 2);

	af_data_user.af_statistics_buf = NULL;
	af_data_user.desired_lens_direction = wposn;
	af_data_user.lens_current_position = 0 ;
	af_data_user.update = REQUEST_STATISTICS | LENS_DESIRED_POSITION;
	af_data_user.af_statistics_buf = stats_buff;
	af_data_user.frame_number = 8; /* dummy */

	printf("Setting first parameters \n");
	control_af_request.id = V4L2_CID_PRIVATE_ISP_AF_REQ;
	control_af_request.value = (int)&af_data_user;
	ret = ioctl(cfd, VIDIOC_S_CTRL, &control_af_request);
	if (ret < 0) {
		perror("ISP_AF_REQ 1");
		return ret;
	}

	printf("Frame No %d\n", af_data_user.frame_number);
	printf("Lens Crt %d\n", af_data_user.lens_current_position);
	printf("Lens Des %d\n", af_data_user.desired_lens_direction);
	printf("Frame Curr %d\n", af_data_user.curr_frame);
	af_data_user.frame_number = af_data_user.curr_frame + 10;

request:
	frame_number = af_data_user.frame_number;
	/* request stats */

	af_data_user.update = REQUEST_STATISTICS | LENS_DESIRED_POSITION;
	af_data_user.af_statistics_buf = stats_buff;
	printf("Requesting stats for frame %d, try %d\n", frame_number, j);
	ret = ioctl(cfd,  VIDIOC_S_CTRL, &control_af_request);
	if (ret < 0) {
		perror("ISP_AF_REQ 2");
		return ret;
	}
	printf("Frame No %d\n", af_data_user.frame_number);
	printf("Lens Crt %d\n", af_data_user.lens_current_position);
	printf("Lens Des %d\n", af_data_user.desired_lens_direction);
	printf("Frame Curr %d\n", af_data_user.curr_frame);
	printf("xs.ts %d:%d\n", af_data_user.xtrastats.ts.tv_sec,
					af_data_user.xtrastats.ts.tv_usec);
	printf("xs.field_count %d\n", af_data_user.xtrastats.field_count);
	printf("xs.lens_position %d\n", af_data_user.xtrastats.lens_position);

	if (af_data_user.af_statistics_buf == NULL) {
		printf("NULL buffer, current frame is  %d.\n",
			af_data_user.curr_frame);
		af_data_user.frame_number =
					af_data_user.curr_frame + 5;
		af_data_user.update = REQUEST_STATISTICS;
		af_data_user.af_statistics_buf = stats_buff;
		goto request;
	} else {
	/* Display stats */
		buff_preview = (u16 *)af_data_user.af_statistics_buf;
		printf("H3A AE/AWB: buffer to display = %d data pointer = %p\n",
			buff_prev_size, af_data_user.af_statistics_buf);
		for (k = 0; k < 1024; k++)
			fprintf(fp_out, "%6x\n", buff_preview[k]);
	}

	sleep(1);

	while (i < 1000) {
		/* De-queue the next filled buffer from camera */
		while (ioctl(cfd, VIDIOC_DQBUF, &cfilledbuffer) < 0) {
			perror("cam VIDIOC_DQBUF");
			while (ioctl(vfd, VIDIOC_QBUF, &cfilledbuffer) < 0)
				perror("VIDIOC_QBUF***");
		}
		i++;

		if (vfilledbuffer.index != -1) {
			/* De-queue the previous buffer from video driver */
			if (ioctl(vfd, VIDIOC_DQBUF, &vfilledbuffer) < 0) {
				perror("cam VIDIOC_DQBUF");
				return;
			}
		}
		vfilledbuffer.index = cfilledbuffer.index;
		/* Queue the new buffer to video driver for rendering */
		if (ioctl(vfd, VIDIOC_QBUF, &vfilledbuffer) == -1) {
			perror("video VIDIOC_QBUF");
			return;
		}

		/* queue the buffer back to camera */
		while (ioctl(cfd, VIDIOC_QBUF, &cfilledbuffer) < 0)
			perror("cam VIDIOC_QBUF");
		/* *************************** */

		if (kbhit()) {
			input = getch();
			if (input == '1') {
				af_data_user.update = 0;
				ret = ioctl(cfd,  VIDIOC_S_CTRL,
							&control_af_request);
				if (ret < 0) {
					perror("ISP_AF_REQ 3");
					return ret;
				}

				af_data_user.frame_number =
						af_data_user.curr_frame;
				af_data_user.update = REQUEST_STATISTICS;
				af_data_user.af_statistics_buf = stats_buff;
				ret = ioctl(cfd, VIDIOC_S_CTRL,
							&control_af_request);
				if (ret < 0) {
					perror("ISP_AF_REQ 4");
					return ret;
				}
				printf("Frame No %d\n",
						af_data_user.frame_number);
				printf("xs.ts %d:%d\n", af_data_user.xtrastats.
							ts.tv_sec,
							af_data_user.xtrastats.
							ts.tv_usec);
				printf("xs.field_count %d\n", af_data_user.
							xtrastats.field_count);
				printf("xs.lens_position %d\n",
							af_data_user.xtrastats.
							lens_position);

				buff_preview = (u16 *)af_data_user.
							af_statistics_buf;
				printf("H3A AE/AWB: buffer to display = %d"
							" data pointer = %p\n",
							buff_prev_size,
							af_data_user.
							af_statistics_buf);
			} else if (input == '2') {
				if (wposn > 0)
					wposn--;
				af_data_user.desired_lens_direction = wposn;
				af_data_user.lens_current_position = 0;
				af_data_user.update = LENS_DESIRED_POSITION;
				ret = ioctl(cfd, VIDIOC_S_CTRL,
							&control_af_request);
				if (ret < 0) {
					perror("ISP_AF_REQ 5");
					return ret;
				}
				printf("Lens position (-1): %d\n", wposn);
			} else if (input == '3') {
				if (wposn < 0xFF)
					wposn++;
				af_data_user.desired_lens_direction = wposn;
				af_data_user.lens_current_position = 0;
				af_data_user.update = LENS_DESIRED_POSITION;
				ret = ioctl(cfd, VIDIOC_S_CTRL,
							&control_af_request);
				if (ret < 0) {
					perror("ISP_AF_REQ 5");
					return ret;
				}
				printf("Lens position (+1): %d\n", wposn);
			} else if (input == '4') {
				wposn = 0xFF;
				af_data_user.desired_lens_direction = wposn;
				af_data_user.lens_current_position = 0;
				af_data_user.update = LENS_DESIRED_POSITION;
				ret = ioctl(cfd, VIDIOC_S_CTRL,
							&control_af_request);
				if (ret < 0) {
					perror("ISP_AF_REQ 6");
					return ret;
				}
				printf("Lens position (macro): %d\n", wposn);
			} else if (input == '5') {
				wposn = 0x7F;
				af_data_user.desired_lens_direction = wposn;
				af_data_user.lens_current_position = 0;
				af_data_user.update = LENS_DESIRED_POSITION;
				ret = ioctl(cfd, VIDIOC_S_CTRL,
							&control_af_request);
				if (ret < 0) {
					perror("ISP_AF_REQ 7");
					return ret;
				}
				printf("Lens position (intermediate): %d\n",
									wposn);
			} else if (input == '6') {
				wposn = 0x0;
				af_data_user.desired_lens_direction = wposn;
				af_data_user.lens_current_position = 0;
				af_data_user.update = LENS_DESIRED_POSITION;
				ret = ioctl(cfd, VIDIOC_S_CTRL,
							&control_af_request);
				if (ret < 0) {
					perror("ISP_AF_REQ 8");
					return ret;
				}
				printf("Lens position (infinite): %d\n", wposn);
			} else if (input == 'q')
				break;
		}
	/* ******************************* */
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
	sleep(5);

	for (i = 0; i < vreqbuf.count; i++) {
		if (vbuffers[i].start)
			munmap(vbuffers[i].start, vbuffers[i].length);
	}
	
	ret = setFramerate(cfd,30);
	if (ret < 0){
		printf("ERROR: VIDIOC_S_PARM ioctl cam\n");
		return -1;
	}

	free(vbuffers);
	free(stats_buff);

	close(cfd);
	close(vfd);
}
