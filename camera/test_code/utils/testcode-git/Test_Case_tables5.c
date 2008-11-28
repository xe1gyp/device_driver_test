/* ================================================================================
*             Texas Instruments OMAP(TM) Platform Software
*  (c) Copyright Texas Instruments, Incorporated.  All Rights Reserved.
*
*  Use of this software is controlled by the terms and conditions found
*  in the license agreement under which this software has been supplied.
* ================================================================================ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <linux/ioctl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <linux/videodev.h>
#include <linux/errno.h>
#include <errno.h>

#define FILE_SAVE 0

#define BRT_TEST 1
#define CONT_TEST 2
#define COLOR_TEST 3

#if FILE_SAVE
#define NUM_FRAMES 150
#else
#define NUM_FRAMES 150
#endif

#define NUM_BUF_USE 1

#define NUM_CONT_LEVELS 15
#define NUM_BRT_LEVELS 15
#define NUM_COLOR_LEVELS 3

#define DEF_CONT_LEVEL 2
#define DEF_BRT_LEVEL 1
#define DEF_COLOR_LEVEL 0


#define VIDEO_DEVICE1 "/dev/v4l/video1"
#define VIDEO_DEVICE2 "/dev/v4l/video2"

#ifndef u32
#define u32 unsigned int
#endif /* u32 */

#ifndef u16
#define u16 unsigned short
#endif /* u16 */

#ifndef u8
#define u8 unsigned char
#endif /* u8 */

#define RGB_MAX		3


#ifndef s16
#define s16 signed short
#endif /* s16 */

#define V4L2_CID_PRIVATE_ISP_COLOR_FX   (V4L2_CID_PRIVATE_BASE + 0)
#define V4L2_CID_PRIVATE_ISP_LSC_UPDATE  (V4L2_CID_PRIVATE_BASE + 3)

extern int errno;
char * msg;

 void usage(void)
{
	printf("./Test_Case_tables [vid] Tables_Updates\n");
	printf("\n\n [vid] is the video pipeline to be used:1(default) or 2\n");
	printf("\n Options for Tables_Updates: \n");
	printf("lsc - Lens Shading Compensation,   nf - Noise Filter   ");
	printf("bg - Blue Gamma Table Update,   gg - Green Gamma Table "
							"Update,   ");
	printf("rg - Red Gamma Table Update, \n");
}



int main (int argc, char *argv[])
{
	struct {
		void *start;
		size_t length;
	} *vbuffers;
	struct v4l2_capability capability;
	struct v4l2_format cformat, vformat;
	struct v4l2_requestbuffers creqbuf, vreqbuf;
	struct v4l2_buffer cfilledbuffer, vfilledbuffer;
	FILE * fOut = NULL;
	int cfd, vfd, test;
	int vid = 1, set_video_img = 0, i, ret;
	struct v4l2_queryctrl queryctrl, queryctrl2;
	struct v4l2_control control, control2,control5;
	int vv=42;

	u32 red_gamma_t[1024];
	u32 green_gamma_t[1024];
	u32 blue_gamma_t[1024];
	u32 table_t[64];
	int all;
	static u8 ispccdc_lsc_tbl[] = {
		#include "ispccdc_lsc5.dat"
	};

	printf("\n ispccdc_lsc3_tbl: %d\n",sizeof(ispccdc_lsc_tbl));

	struct isptables_update {
		u16 update;
		u16 algo_enable;
		struct ispprev_nf *prev_nf;
		void *lsc;
		void *red_gamma;
		void *green_gamma;
		void *blue_gamma;
	}isptables_update_t;


	struct ispprev_nf {
		u8 spread;
		u32 *table;
	} ispprev_nf_t;

	for(all=0;all<64;all++) {
		table_t[all]=30;
	}


	ispprev_nf_t.spread=3;
	ispprev_nf_t.table=(u32 *)table_t;
	for(all=0;all<1024;all++)
	{
	red_gamma_t[all]=0;
	}

	isptables_update_t.red_gamma = (void *)red_gamma_t;
	isptables_update_t.green_gamma = (void *)green_gamma_t;
	isptables_update_t.blue_gamma = (void *)blue_gamma_t;
	isptables_update_t.prev_nf = &ispprev_nf_t;
	isptables_update_t.lsc = (void *)ispccdc_lsc_tbl;//(void *)lsc_t;

	printf("ispccdc_lsc_tbl = 0x%x\n", (int)ispccdc_lsc_tbl);
	printf("isptables_update_t.lsc = 0x%x\n", (int)isptables_update_t.lsc);

	if ((argc > 1) && (!strcmp(argv[1], "?"))) {
		usage();
		return 0;
	}
	if (argc > 1) {
		vid = atoi(argv[1]);
		if ((vid != 1) && (vid != 2)){
			printf("vid has to be 1 or 2! vid=%d, argv[1]=%s\n", 
							vid, argv[1]);
			usage();
			return 0;
		}
	}

	test = 0;


	if (argc > 2) {
		if((!strcasecmp(argv[2], "lsc"))) {
			isptables_update_t.update= 1<< 0;
			isptables_update_t.algo_enable= 1 << 0;
		} else if((!strcasecmp(argv[2], "lsc_dis"))) {
			isptables_update_t.update= 1<< 0;
			isptables_update_t.algo_enable= 0 << 0;
		} else if((!strcasecmp(argv[2], "nf"))) {
			isptables_update_t.update=1<<1;
			isptables_update_t.algo_enable=1<<1;
		} else if((!strcmp(argv[2], "rg"))) {
			isptables_update_t.update=1<<2;
			isptables_update_t.algo_enable=1<<2;
		} else if((!strcmp(argv[2], "gg"))){
			isptables_update_t.update=1<<3;
			isptables_update_t.algo_enable=1<<3;
		} else if((!strcmp(argv[2], "bg"))) {
			isptables_update_t.update=1<<4;
			isptables_update_t.algo_enable=1<<4;
		} else {
			printf(" Wrong option please enter correct option "
						"argv[1]=%s\n", argv[2]);
			usage();
			return 0;
		}
	}


	if ((cfd = open_cam_device(O_RDWR,1)) <= 0) {
		printf ("Could not open the cam device\n");
		return -1;
	}




	memset(&queryctrl, 0, sizeof(queryctrl));
	memset(&control, 0, sizeof(control));


	control.id = V4L2_CID_PRIVATE_ISP_LSC_UPDATE;//V4L2_CID_PRIVATE_BASE + 3;
	control.value = (int)&isptables_update_t;
	if (ioctl(cfd, VIDIOC_S_CTRL, &control) == -1)
		printf("\nerror\n");
	else
		printf("VIDIOC_S_CTRL successful\n");

	close(cfd);
}
