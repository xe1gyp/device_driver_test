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
#include <linux/ioctl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <linux/videodev2.h>
#include <linux/errno.h>
#include <errno.h>
#include "kbget.h"

#define VIDEO_DEVICE1 "/dev/v4l/video1"
#define VIDEO_DEVICE2 "/dev/v4l/video2"
#define VIDIOC_ISP_2ACFG        _IOWR ('O', 6,  struct isph3a_aewb_config)
#define VIDIOC_ISP_2AREQ        _IOWR ('O', 7,  struct isph3a_aewb_data)
#define BYTES_PER_WINDOW	16

#define V4L2_CID_PRIVATE_ISP_COLOR_FX     (V4L2_CID_PRIVATE_BASE + 0)
#define V4L2_CID_PRIVATE_ISP_CCDC_CFG     (V4L2_CID_PRIVATE_BASE + 1)
#define V4L2_CID_PRIVATE_ISP_PRV_CFG      (V4L2_CID_PRIVATE_BASE + 2)
#define V4L2_CID_PRIVATE_ISP_LSC_UPDATE   (V4L2_CID_PRIVATE_BASE + 3)
#define V4L2_CID_PRIVATE_ISP_AEWB_CFG     (V4L2_CID_PRIVATE_BASE + 4)
#define V4L2_CID_PRIVATE_ISP_AEWB_REQ     (V4L2_CID_PRIVATE_BASE + 5)
#define V4L2_CID_PRIVATE_ISP_HIST_CFG     (V4L2_CID_PRIVATE_BASE + 6)
#define V4L2_CID_PRIVATE_ISP_HIST_REQ     (V4L2_CID_PRIVATE_BASE + 7)
#define V4L2_CID_PRIVATE_ISP_AF_CFG       (V4L2_CID_PRIVATE_BASE + 8)
#define V4L2_CID_PRIVATE_ISP_AF_REQ       (V4L2_CID_PRIVATE_BASE + 9)

/* Flags for update field */
#define REQUEST_STATISTICS	(1 << 0)
#define SET_COLOR_GAINS		(1 << 1)
#define SET_DIGITAL_GAIN	(1 << 2)
#define SET_EXPOSURE		(1 << 3)
#define SET_ANALOG_GAIN		(1 << 4)

#ifndef u32
#define u32 unsigned long
#endif /* u32 */

#ifndef u16
#define u16 unsigned short
#endif /* u16 */

#ifndef u8
#define u8 unsigned char
#endif /* u8 */

/*#define CAP_UTILS*/
#ifdef CAP_UTILS
#include "CapUtils.h"
#else
struct isph3a_aewb_config{
	u16 saturation_limit;
	u16 win_height;		/* Range: 2 - 256*/
	u16 win_width;		/* Range: 6 - 256*/
	u16 ver_win_count;	/* vertical window count (Range: 1 - 128)*/
	u16 hor_win_count;	/* horizontal window count (Range: 1 - 36)*/
	u16 ver_win_start;	/* vertical window start position
					(Range: 0 -4095)*/
	u16 hor_win_start;	/* horizontal window start position
					(Range: 0 - 4095)*/
	u16 blk_ver_win_start;	/* black line vertical window start position
					(Range: 0 -4095)*/
	u16 blk_win_height;	/* black line height
					(Range: 2 - 256 even values only)*/
	u16 subsample_ver_inc;	/* vertical distance between subsamples
					(Range: 2 - 32 even values only)*/
	u16 subsample_hor_inc;	/* horizontal distance between subsamples
					(Range: 2 - 32 even values only)*/
	u8   alaw_enable;	/* enable AEW ALAW flag*/
	u8   aewb_enable;	/* AE AWB statistics generation enable flag*/
};

struct isph3a_aewb_data {
	void *h3a_aewb_statistics_buf;	/* Pointer to AE AWB statistics
						*buffer to be filled*/
	u32 shutter;			/* Shutter speed*/
	u16 gain;			/*Sensor Gain*/
	u32 shutter_cap;		/*Shutter speed for capture*/
	u16 gain_cap;			/* Sensor Gain for capture*/

	u16 dgain;			/* White balance digital gain for
						preview module*/
	u16 wb_gain_b;			/* White balance color gain blue*/
	u16 wb_gain_r;			/* White balance color gain red*/
	u16 wb_gain_gb;			/* White balance color gain green blue*/
	u16 wb_gain_gr;			/* White balance color gain green red*/

	u16 frame_number;		/* Requested frame*/
	u16 curr_frame;			/* Current frame being processed*/
	u8 update;			/* Flags to update parameters*/
};
#endif

#ifdef CAP_UTILS
#define PIX_PER_WINDOW 25
h3a_aewb_paxel_data_t h3a_stats[H3A_AEWB_MAX_WIN_NUM];
h3a_aewb_paxel_data_t h3a_avg[1];
#endif
	int cfd, vfd;

static void usage(void)
{
	printf("h3a_stream [camDevice] [vid] [framerate]\n");
	printf("\tEnable 2A and capture  of 1000 frames using video driver for "
								"rendering\n");
	printf("\t[camDevice] Camera device to be open\n\t\t 1:Micron sensor "
					"2:OV sensor\n");
	printf("\t[vid] is the video pipeline to be used. Valid vid is 1"
							"(default) or 2\n");
	printf("\t[framerate] is the framerate to be used, if no value"
				" is given \n\t           30 fps is default\n");
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
	struct isph3a_aewb_config aewb_config_user;
	struct isph3a_aewb_data aewb_data_user;
	u16 *buff_preview = NULL;
	u16 *buff_char = NULL;
	u8 *stats_buff = NULL;
	unsigned int buff_prev_size = 0;
	int data8, data2, window, unsat_cnt;
	int input, new_gain = 0;
	int frame_number;
	int enable = 1, j = 0;
	int device = 1;
	int index = 1;
	int framerate = 30;
	struct v4l2_control control_h3a_config, control_h3a_request;
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
/***************************************************************/

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
		printf("Framerate = %d\n", framerate);
	} else
		printf("Using framerate = 30, default value\n");

	cfd = open_cam_device(O_RDWR, device);
	if (cfd <= 0) {
		printf("Could not open the cam device\n");
		return -1;
	}
	
	if (argc > 1) {
		vid = atoi(argv[1]);
		if ((vid != 1) && (vid != 2)) {
			printf("vid has to be 1 or 2! vid=%d, argv[1]=%s\n",
								vid, argv[1]);
			usage();
			return 0;
		}
	}
	ret = cam_ioctl(cfd, "YUYV", "QVGA");
	ret = setFramerate(cfd, framerate);
	vfd = open((vid == 1)?VIDEO_DEVICE1:VIDEO_DEVICE2, O_RDWR);
	if (vfd <= 0) {
		printf("Could not open %s\n", (vid == 1)?VIDEO_DEVICE1:
								VIDEO_DEVICE2);
		return -1;
	} else
		printf("openned %s for rendering\n", (vid == 1)?VIDEO_DEVICE1:
								VIDEO_DEVICE2);

	if (ioctl(vfd, VIDIOC_QUERYCAP, &capability) == -1) {
		perror("video VIDIOC_QUERYCAP");
		return -1;
	}
	if ( capability.capabilities & V4L2_CAP_STREAMING)
		printf("The video driver is capable of Streaming!\n");
	else {
		printf("The video driver is not capable of Streaming!\n");
		return -1;
	}	

	if (ioctl(cfd, VIDIOC_QUERYCAP, &capability) < 0) {
		perror("VIDIOC_QUERYCAP");
		return -1;
	}
	if ( capability.capabilities & V4L2_CAP_STREAMING)
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
	
	if ((cformat.fmt.pix.width!=vformat.fmt.pix.width) ||
		(cformat.fmt.pix.height!=vformat.fmt.pix.height) ||
		(cformat.fmt.pix.sizeimage!=vformat.fmt.pix.sizeimage)) {
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
		ret = ioctl (vfd, VIDIOC_S_FMT, &vformat);
		if (ret < 0) {
			perror ("video VIDIOC_S_FMT");
			return -1;
		}
		if ((cformat.fmt.pix.width!=vformat.fmt.pix.width) ||
			(cformat.fmt.pix.height!=vformat.fmt.pix.height) ||
			(cformat.fmt.pix.sizeimage!=vformat.fmt.pix.sizeimage) 
			|| (cformat.fmt.pix.pixelformat != 
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
		perror ("video VIDEO_REQBUFS");
		return;
	}
	printf("Video Driver allocated %d buffers when 4 are requested\n", 
								vreqbuf.count);

	vbuffers = calloc(vreqbuf.count, sizeof(*vbuffers));
	for (i = 0; i < vreqbuf.count ; ++i) {
		struct v4l2_buffer buffer;
		buffer.type = vreqbuf.type;
		buffer.index = i;
		if (ioctl(vfd, VIDIOC_QUERYBUF, &buffer) == -1){
			perror("video VIDIOC_QUERYBUF");
			return;
		}
#if 0
		printf("video %d: buffer.length=%d, buffer.m.offset=%d\n",
				i, buffer.length, buffer.m.offset);
#endif
		vbuffers[i].length = buffer.length;
		vbuffers[i].start = mmap(NULL, buffer.length, PROT_READ|
						PROT_WRITE, MAP_SHARED,
						vfd, buffer.m.offset);
		if (vbuffers[i].start == MAP_FAILED ){
			perror ("video mmap");
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
		perror ("cam VIDEO_REQBUFS");
		return -1;
	}
	printf("Camera Driver allowed %d buffers\n", creqbuf.count);

	for (i = 0; i < creqbuf.count; ++i) {
		struct v4l2_buffer buffer;
		buffer.type = creqbuf.type;
		buffer.memory = creqbuf.memory;
		buffer.index = i;
		if(ioctl(cfd, VIDIOC_QUERYBUF, &buffer) < 0){
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
	if (ioctl(cfd, VIDIOC_STREAMON, &creqbuf.type) < 0 ) {
		perror("cam VIDIOC_STREAMON");
		return -1;
	}
	if (ioctl(vfd, VIDIOC_STREAMON, &vreqbuf.type) < 0 ) {
		perror("video VIDIOC_STREAMON");
		return -1;
	}

	/* caputure 1000 frames */
	cfilledbuffer.type = creqbuf.type;
	vfilledbuffer.type = vreqbuf.type;
	i = 0;
	vfilledbuffer.index = -1;
	sleep(1);
	///////***************///////////////
	
		control_h3a_config.id = V4L2_CID_PRIVATE_ISP_AEWB_CFG;
		control_h3a_config.value = (int)&aewb_config_user;
		/* set h3a params */
		ret = ioctl(cfd, VIDIOC_S_CTRL, &control_h3a_config);
		if (ret < 0) {
			printf("Error: %d, ", ret);
			perror ("ISP_AEWB_CFG 1");
			return ret;
		}
		
		num_windows = ((aewb_config_user.ver_win_count
				* aewb_config_user.hor_win_count)
				+ aewb_config_user.hor_win_count);
		num_color_windows = (aewb_config_user.ver_win_count
				* aewb_config_user.hor_win_count);
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
		
		/* Shutter & gain for preview */
		/* Exposure time between 26 and 65000 microseconds */
		aewb_data_user.shutter = 20000;
		/* Gain between 0x08 and 0x7F */
		aewb_data_user.gain = 0x40;

		aewb_data_user.update = (SET_COLOR_GAINS | SET_DIGITAL_GAIN);
		aewb_data_user.frame_number = 8; //dummy

		printf("Setting first parameters \n");
		control_h3a_request.id = V4L2_CID_PRIVATE_ISP_AEWB_REQ;
		control_h3a_request.value = (int)&aewb_data_user;
		ret = ioctl(cfd, VIDIOC_S_CTRL, &control_h3a_request);
		if (ret < 0) {
			perror("ISP_AEWB_REQ 1");
			return ret;
		}
		aewb_data_user.frame_number = aewb_data_user.curr_frame + 3;
request:
		frame_number = aewb_data_user.frame_number;
		// request stats 
		printf("Requesting stats for frame %d, try %d\n", 
							frame_number, j);
		aewb_data_user.update = REQUEST_STATISTICS;
		aewb_data_user.h3a_aewb_statistics_buf = stats_buff;
		ret = ioctl(cfd,  VIDIOC_S_CTRL, &control_h3a_request);
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
		} else {
		// Display stats
			buff_preview = 
				(u16 *)aewb_data_user.h3a_aewb_statistics_buf;
			printf("H3A AE/AWB: buffer to display = %d data "
				"pointer = %p\n",buff_prev_size, 
				aewb_data_user.h3a_aewb_statistics_buf);
			printf("num_windows = %d\n", num_windows);
			printf("ver_windows = %d\n", 
				aewb_config_user.ver_win_count);
			printf("hor_windows = %d\n", 
				aewb_config_user.hor_win_count);
			printf("plus one row of black windows\n");
			
			unsat_cnt = 0;
			for (i = 0; i < (buff_prev_size); i++) {
				data8 = (i + 1 ) % 8;
				data2 = (i + 1 ) % 2;
				window = (i + 1) / 8;
				printf("%05d ", buff_preview[i]);
				if (0 == data8) {
					if (((window > 1) && 
						(0 == (window % 9)))
						|| (window ==
						((num_windows + 
						(num_windows / 8) + 
						((num_windows % 8) ? 1 : 
						0))))) {
						printf("   Unsaturated block "
								"count\n");
						unsat_cnt++;
					}
					else {
						printf("    Window %5d\n",
							(window - 1) - 
							unsat_cnt);
					}
				}
				if (0 == data2)
					printf("\n");
			}
		}

		sleep(1);

		j++;
		if (j < 2) {
			aewb_data_user.frame_number += 100;
			aewb_data_user.update = REQUEST_STATISTICS;
			aewb_data_user.h3a_aewb_statistics_buf = stats_buff;
			goto request;
		}

	//////****************///////////////

	while (i < 1000) {
	
		/* De-queue the next filled buffer from camera */
		while (ioctl(cfd, VIDIOC_DQBUF, &cfilledbuffer) < 0) {
			perror ("cam VIDIOC_DQBUF");
			while (ioctl(vfd, VIDIOC_QBUF, &cfilledbuffer) < 0) {
				perror ("VIDIOC_QBUF***");
			}
		}
//		printf("%d - ", cfilledbuffer.sequence);
		i++;

		if (vfilledbuffer.index != -1) {
			/* De-queue the previous buffer from video driver */
			if (ioctl(vfd, VIDIOC_DQBUF, &vfilledbuffer) < 0) {
				perror ("cam VIDIOC_DQBUF");
				return;
			}
		}
		vfilledbuffer.index = cfilledbuffer.index;
		/* Queue the new buffer to video driver for rendering */
		if (ioctl(vfd, VIDIOC_QBUF, &vfilledbuffer) == -1){
			perror ("video VIDIOC_QBUF");
			return;
		}

		/* queue the buffer back to camera */
		while (ioctl(cfd, VIDIOC_QBUF, &cfilledbuffer) < 0) {
			perror ("cam VIDIOC_QBUF");
		}
				
		if ((input=kbhit())!=0) {
			input = getch();
			if (input == 'y') {
				aewb_data_user.dgain = aewb_data_user.dgain + 
									0x10;
				printf("Set new dgain: %u\n", 
							aewb_data_user.dgain);
				new_gain = 1;
				aewb_data_user.update = (SET_DIGITAL_GAIN);
			}
			else if (input == 'h') {
				aewb_data_user.dgain = aewb_data_user.dgain - 
									0x10;
				printf("Set new dgain: %u\n", 
							aewb_data_user.dgain);
				new_gain = 1;
				aewb_data_user.update = (SET_DIGITAL_GAIN);
			}
			else if (input == 'u') {
				aewb_data_user.wb_gain_b = 
					aewb_data_user.wb_gain_b + 0x10;
				printf("Set new bgain: %u\n", 
					aewb_data_user.wb_gain_b);
				new_gain = 1;
				aewb_data_user.update = (SET_COLOR_GAINS);
			}
			else if (input == 'j') {
				aewb_data_user.wb_gain_b = 
						aewb_data_user.wb_gain_b - 0x10;
				printf("Set new bgain: %u\n", 
						aewb_data_user.wb_gain_b);
				new_gain = 1;
				aewb_data_user.update = (SET_COLOR_GAINS);
			}
			else if (input == 'i') {
				aewb_data_user.wb_gain_r = 
					aewb_data_user.wb_gain_r + 0x10;
				printf("Set new rgain: %u\n", 
					aewb_data_user.wb_gain_r);
				new_gain = 1;
				aewb_data_user.update = (SET_COLOR_GAINS);
			}
			else if (input == 'k') {
				aewb_data_user.wb_gain_r = 
					aewb_data_user.wb_gain_r - 0x10;
				printf("Set new rgain: %u\n", 
					aewb_data_user.wb_gain_r);
				new_gain = 1;
				aewb_data_user.update = (SET_COLOR_GAINS);
			}
			else if (input == 'o') {
				aewb_data_user.wb_gain_gb = 
					aewb_data_user.wb_gain_gb + 0x10;
				printf("Set new gb gain: %u\n", 
					aewb_data_user.wb_gain_gb);
				new_gain = 1;
				aewb_data_user.update = (SET_COLOR_GAINS);
			}
			else if (input == 'l') {
				aewb_data_user.wb_gain_gb = 
					aewb_data_user.wb_gain_gb - 0x10;
				printf("Set new gb gain: %u\n", 
					aewb_data_user.wb_gain_gb);
				new_gain = 1;
				aewb_data_user.update = (SET_COLOR_GAINS);
			}
			else if (input == 'p') {
				aewb_data_user.wb_gain_gr = 
					aewb_data_user.wb_gain_gr + 0x10;
				printf("Set new gr gain: %u\n", 
					aewb_data_user.wb_gain_gr);
				new_gain = 1;
				aewb_data_user.update = (SET_COLOR_GAINS);
			}
			else if (input == ';') {
				aewb_data_user.wb_gain_gr = 
					aewb_data_user.wb_gain_gr - 0x10;
				printf("Set new gr gain: %u\n", 
					aewb_data_user.wb_gain_gr);
				new_gain = 1;
				aewb_data_user.update = (SET_COLOR_GAINS);
			}
			else if (input == 't') {
				aewb_data_user.gain = 
					aewb_data_user.gain + 0x04;
				printf("Set new analog gain: %u\n", 
					aewb_data_user.gain);
				new_gain = 1;
				aewb_data_user.update = (SET_ANALOG_GAIN);
			}
			else if (input == 'g') {
				aewb_data_user.gain = 
					aewb_data_user.gain - 0x04;
				printf("Set new analog gain: %u\n", 
					aewb_data_user.gain);
				new_gain = 1;
				aewb_data_user.update = (SET_ANALOG_GAIN);
			}
			else if (input == 'r') {
				aewb_data_user.shutter = 
					aewb_data_user.shutter + 2000;
				printf("Set new shutter: %u\n", 
					aewb_data_user.shutter);
				new_gain = 1;
				aewb_data_user.update = (SET_EXPOSURE);
			}
			else if (input == 'f') {
				aewb_data_user.shutter = 
					aewb_data_user.shutter - 2000;
				printf("Set new shutter: %u\n", 
					aewb_data_user.shutter);
				new_gain = 1;
				aewb_data_user.update = (SET_EXPOSURE);
			}
			else if (input == 'z') {
				new_gain = 1;
				printf ("STRM OFF\n");

				if (ioctl(cfd, VIDIOC_STREAMOFF, 
						&creqbuf.type) == -1) {
					printf("cam VIDIOC_STREAMOFF");
					return -1;
				}
				aewb_data_user.update = 0;
				ret = ioctl(cfd,  VIDIOC_S_CTRL, 
						&control_h3a_request);
				if (ret < 0) {
					printf("ISP_AEWB_REQ 4");
					return ret;
				}
				break;
			}
			else if (input == '1') {
				aewb_data_user.update = 0;
				ret = ioctl(cfd,  VIDIOC_S_CTRL, 
							&control_h3a_request);
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
				ret = ioctl(cfd,  VIDIOC_S_CTRL, 
							&control_h3a_request);
				if (ret < 0) {
					perror("ISP_AEWB_REQ 6");
					return ret;
				}

				// Display stats 
				buff_preview = 
				(u16 *)aewb_data_user.h3a_aewb_statistics_buf;

				unsat_cnt = 0;
				for (i = 0; i < (buff_prev_size); i++) {
					data8 = (i + 1 ) % 8;
					data2 = (i + 1 ) % 2;
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
			else if (input == '9') {
				aewb_config_user.win_height = 14;
				aewb_config_user.win_width = 14;
				/* set h3a params */
				ret = ioctl(cfd, VIDIOC_ISP_2ACFG, 
							&aewb_config_user);
				if (ret < 0) {
					printf("Error: %d, ", ret);
					perror ("ISP_2ACFG 2");
					return ret;
				}
			}
			else if (input == 'q') goto exit;//break;
			
			if (new_gain == 1) {
				ret = ioctl(cfd,  VIDIOC_S_CTRL, 
							&control_h3a_request);
				if (ret < 0) {
					perror("ISP_AEWB_REQ 7");
					goto exit;
					//return ret;
				}
				new_gain = 0;
			}
		}
	}
	
	/******************/
	// disable 
exit:
		aewb_config_user.aewb_enable = 0;
/***************************************************************/   

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
/***************************************************************/   

	ret = ioctl(cfd, VIDIOC_S_CTRL, &control_h3a_config);
	if (ret < 0) {
		printf(" Error: %d, ", ret);
		perror ("VIDIOC_ISP_2ACFG disabling");
		return ret;
	}	
/*******************/
	
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
		if (vbuffers[i].start) {
			munmap(vbuffers[i].start, vbuffers[i].length);
		}
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
