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

#define FILE_SAVE 1

#define BRT_TEST 1
#define CONT_TEST 2
#define COLOR_TEST 3

#if FILE_SAVE
#define NUM_FRAMES 40
#else
#define NUM_FRAMES 40
#endif

#define NUM_BUF_USE 4

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

#define RGB_MAX         3


#ifndef s16
#define s16 signed short
#endif /* s16 */

	static u8 ispccdc_lsc_tbl[] = {
		#include "ispccdc_lsc5.dat"
	};

static void usage(void)
{
	printf("./Test_case_ccdc [vid] CCDC_Update\n");
	printf("---- [vid] is the video pipeline to be used. "
					"Valid vid is 1(default) or 2\n");
	printf("\n\n---- Options for CCDC_Update:\n alc - Alaw compression   ");
	printf(" LPF- Low pass filter,    bcl - Black Clamp ,\t  bcomp -"
		"Black compensation ,    FPC -Faulty pixel correction,    "
		"cull - CCDC Culling,    col - Colptn \n");

}


#define V4L2_CID_PRIVATE_ISP_COLOR_FX   (V4L2_CID_PRIVATE_BASE + 0)
#define V4L2_CID_PRIVATE_ISP_CCDC_CFG (V4L2_CID_PRIVATE_BASE + 1)

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
	int cfd, vfd, test,all1;
	int vid = 1, set_video_img = 0, i, ret;
	struct v4l2_queryctrl queryctrl, queryctrl2;
	struct v4l2_control control, control2,control5;
	u32 fpc_tbl[5];

	enum alaw_ipwidth{
		ALAW_BIT12_3 = 0x3,
		ALAW_BIT11_2 = 0x4,
		ALAW_BIT10_1 = 0x5,
		ALAW_BIT9_0 = 0x6
		};

	struct ispccdc_bclamp{
		u8 obgain;
		u8 obstpixel;
		u8 oblines;
		u8 oblen;
		u16 dcsubval;
		}bclamp_t;

		bclamp_t.obgain = 10;
		bclamp_t.obstpixel = 20;
		bclamp_t.oblines = 30;
		bclamp_t.oblen = 40;
		bclamp_t.dcsubval = 42;

	struct ispccdc_blcomp{
		u8 b_mg;
		u8 gb_g;
		u8 gr_cy;
		u8 r_ye;
		} blcomp_t;

	blcomp_t.b_mg = 0;//OJV
	blcomp_t.gb_g = 0;//OJV
	blcomp_t.gr_cy = 0;//OJV
	blcomp_t.r_ye = 0;//OJV


	/*for(all1=0; all1<10; all1++)
	{
		fpc_tbl[all1]=2;//10;//0x6401901;
	}*/
	fpc_tbl[0] = ((10 << 19) || (10 << 5) || (1 << 0));
	fpc_tbl[1] = ((10 << 19) || (20 << 5) || (1 << 0));
	fpc_tbl[2] = ((10 << 19) || (30 << 5) || (1 << 0));
	fpc_tbl[3] = ((10 << 19) || (40 << 5) || (1 << 0));
	fpc_tbl[4] = ((10 << 19) || (50 << 5) || (1 << 0));


	struct ispccdc_fpc{
		u16 fpnum;
		u32 fpcaddr;
		}fpc_t;

		fpc_t.fpnum = 5;//1200;//25;//1200;//0xa;
		fpc_t.fpcaddr = (u32)fpc_tbl;//0xc0;

	struct ispccdc_culling{
		u8 v_pattern;
		u16 h_odd;
		u16 h_even;
		}culling_t;

		culling_t.v_pattern=12;
		culling_t.h_odd=13;
		culling_t.h_even=14;
	

	struct ispccdc_lsc_config {
		u8 offset;
		u8 gain_mode_n;
		u8 gain_mode_m;
		u8 gain_format;
		u16 fmtsph;
		u16 fmtlnh;
		u16 fmtslv;
		u16 fmtlnv;
		u8 initial_x;
		u8 initial_y;
		u32 size;
	}ispccdc_lsc_config_t ;

	ispccdc_lsc_config_t.offset = 0x60;
	ispccdc_lsc_config_t.gain_mode_n = 6;
	ispccdc_lsc_config_t.gain_mode_m = 6;
	ispccdc_lsc_config_t.gain_format = 4;
	ispccdc_lsc_config_t.fmtsph = 0;
	ispccdc_lsc_config_t.fmtlnh = 0;
	ispccdc_lsc_config_t.fmtslv = 0;
	ispccdc_lsc_config_t.fmtlnv = 0;
	ispccdc_lsc_config_t.size = sizeof(ispccdc_lsc_tbl);

	/* Structure for CCDC configuration*/
	struct ispccdc_update_config {
		u16 update;
		u16 flag;
		enum alaw_ipwidth alawip;
		struct ispccdc_bclamp *bclamp;
		struct ispccdc_blcomp *blcomp;
		struct ispccdc_fpc *fpc;
		struct ispccdc_lsc_config *lsc_cfg;
		struct ispccdc_culling *cull;
		u32 colptn;
	} arg_ccdc_t;

	arg_ccdc_t.update = 0;
	arg_ccdc_t.flag = 0;
	arg_ccdc_t.alawip = ALAW_BIT10_1;  
	arg_ccdc_t.bclamp = &bclamp_t;
	arg_ccdc_t.blcomp = &blcomp_t;
	arg_ccdc_t.fpc = &fpc_t;
	arg_ccdc_t.cull = &culling_t;
	arg_ccdc_t.colptn = 30;
	arg_ccdc_t.lsc_cfg = &ispccdc_lsc_config_t;

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

	if (argc > 2) {
		if((!strcasecmp(argv[2], "alc"))) {
			arg_ccdc_t.update= 1 << 0;
			arg_ccdc_t.flag= 1 << 0;
		} else if((!strcasecmp(argv[2], "lpf"))) {
			arg_ccdc_t.update=1<<1;
			arg_ccdc_t.flag= 1 << 1;
		} else if((!strcasecmp(argv[2], "bcl"))) {
			arg_ccdc_t.update=1<<2;
			arg_ccdc_t.flag= 1 << 2;

		} else if((!strcasecmp(argv[2], "bcomp"))) {
			arg_ccdc_t.update=1<<3;
			arg_ccdc_t.flag= 1 << 3;

		} else if((!strcasecmp(argv[2], "fpc"))) {
			 arg_ccdc_t.update=1<<4;
			arg_ccdc_t.flag=1 << 4;

		} else if((!strcasecmp(argv[2], "cull"))) {
			arg_ccdc_t.update=1<<5;
			arg_ccdc_t.flag= 1 << 5;


		} else if((!strcasecmp(argv[2], "col"))) {
			arg_ccdc_t.update=1<<6;
			arg_ccdc_t.flag = 1 << 6;

		} else if((!strcasecmp(argv[2], "lc"))) {
			arg_ccdc_t.update = 1 << 7;
			arg_ccdc_t.flag = 1 << 7;

		} else if((!strcasecmp(argv[2], "all"))) {
			arg_ccdc_t.update = (1 << 8) - 1;
			arg_ccdc_t.flag = (1 << 8) - 1;

		} else if((!strcasecmp(argv[2], "none"))) {
			arg_ccdc_t.update = 0;
			arg_ccdc_t.flag = 0;

		} else {
			printf(" Wrong option please enter correct option "
						"argv[1]=%s\n", argv[2]);
			usage();
			return 0;
		}
	}


	if ((cfd = open_cam_device(O_RDWR)) <= 0) {
		printf ("Could not open the cam device\n");
		return -1;
	}

	memset(&queryctrl, 0, sizeof(queryctrl));
	memset(&control, 0, sizeof(control));

	control5.id = V4L2_CID_PRIVATE_ISP_CCDC_CFG;
	control5.value = (int)&arg_ccdc_t;
	if (ioctl(cfd, VIDIOC_S_CTRL, &control5) == -1)
		printf("\nerror\n");
	else
		printf("VIDIOC_S_CTRL successful\n");

	close(cfd);
}
