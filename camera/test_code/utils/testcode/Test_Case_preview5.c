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

#define NUM_FRAMES 50

#define NUM_BUF_USE 4

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

#define V4L2_CID_PRIVATE_ISP_COLOR_FX (V4L2_CID_PRIVATE_BASE + 0)
#define V4L2_CID_PRIVATE_ISP_PRV_CFG  (V4L2_CID_PRIVATE_BASE + 2)

/* Abstraction layer preview configurations */
#define ISP_ABS_PREV_LUMAENH		(1 << 1)
#define ISP_ABS_PREV_INVALAW		(1 << 2)
#define ISP_ABS_PREV_HRZ_MED		(1 << 5)
#define ISP_ABS_PREV_CFA		(1 << 6)
#define ISP_ABS_PREV_CHROMA_SUPP	(1 << 7)
#define ISP_ABS_PREV_WB			(1 << 8)
#define ISP_ABS_PREV_BLKADJ		(1 << 9)
#define ISP_ABS_PREV_RGB2RGB		(1 << 10)
#define ISP_ABS_PREV_COLOR_CONV		(1 << 11)
#define ISP_ABS_PREV_YC_LIMIT		(1 << 12)
#define ISP_ABS_PREV_DEFECT_COR		(1 << 13)
#define ISP_ABS_PREV_GAMMABYPASS	(1 << 14)

/* Abstraction layer Table Update Flags */
#define ISP_ABS_TBL_NF 			(1 << 1)
#define ISP_ABS_TBL_REDGAMMA		(1 << 2)
#define ISP_ABS_TBL_GREENGAMMA		(1 << 3)
#define ISP_ABS_TBL_BLUEGAMMA		(1 << 4)

static void usage(void)
{
	printf("./Test_Case_preview [vid]  Preview_updates ");
	printf("\n\n---- [vid] is the video pipeline to be used: 1 "
							"(default) or 2");
	printf(" \n---- Options for Preview_Updates: \n "
		" le - Luma Enhancement,\n");
	printf(" ialaw- Inverse A law,\n");
	printf(" hm - Horizontal Median filter,\n"
		" cfa - CFA interpolation,\n cs - Chroma Suppression,\n");
	printf(" wb - White balance,\n ba - Black adjustment,\n r - "
							"RGB 2 RGB,\n");
	printf(" cc - Color conversion,\n yc - YC,\n dc - Defect "
							"Correction,\n");
	printf(" all - all\n");

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
	int cfd, vfd;
	int vid = 1, set_video_img = 0, i, ret;
	struct v4l2_queryctrl queryctrl2;
	struct v4l2_control control2, control5;

	struct ispprev_hmed {
		u8 odddist;
		u8 evendist;
		u8 thres;
	} hmed_p;

	hmed_p.odddist = 0x2; /* For Bayer sensor */
	hmed_p.evendist = 0x2;
	hmed_p.thres = 0x10;

	enum cfa_fmt {
		CFAFMT_BAYER, CFAFMT_SONYVGA, CFAFMT_RGBFOVEON,
		CFAFMT_DNSPL, CFAFMT_HONEYCOMB, CFAFMT_RRGGBBFOVEON
	};

	static u32 cfa_coef_table[] = {
		#include "cfa_coef_table.h"
	};

	static u32 luma_enhance_table[] = {
		#include "luma_enhance_table.h"
	};

	struct ispprev_cfa {
		enum cfa_fmt cfafmt;
		u8 cfa_gradthrs_vert;
		u8 cfa_gradthrs_horz;
		u32 *cfa_table;
	} cfa_p;

	cfa_p.cfafmt = CFAFMT_BAYER;
	cfa_p.cfa_gradthrs_vert = 0x28; /* Default values */
	cfa_p.cfa_gradthrs_horz = 0x28; /* Default values */
	cfa_p.cfa_table = cfa_coef_table;

	struct ispprev_csup {
		u8 gain;
		u8 thres;
		u8 hypf_en;
	}csup_p;

	csup_p.gain = 0x0D;
	csup_p.thres = 0xEB;
	csup_p.hypf_en = 0;

	/* Structure for White Balance */
	struct ispprev_wbal {
		u16 dgain;
		u8 coef3;
		u8 coef2;
		u8 coef1;
		u8 coef0;
	} wbal_p;

	wbal_p.dgain = 0x100;
	wbal_p.coef3 = 0x94;//0x68;
	wbal_p.coef2 = 0x5C;
	wbal_p.coef1 = 0x5C;
	wbal_p.coef0 = 0x68;//0x94;

	struct ispprev_blkadj {
		u8 red;
		u8 green;
		u8 blue;
	} blkadj_p;

	blkadj_p.red = 0x0;
	blkadj_p.green = 0x0;
	blkadj_p.blue = 0x0;

	struct ispprev_rgbtorgb {
		u16 matrix[3][3];
		u16 offset[3];
	} rgb2rgb_p;

	rgb2rgb_p.matrix[0][0] = 0x01E2 ;
	rgb2rgb_p.matrix[0][1] = 0x0F30 ;
	rgb2rgb_p.matrix[0][2] =   0x0FEE;
	rgb2rgb_p.matrix[1][0] =  0x0F9B;
	rgb2rgb_p.matrix[1][1] =  0x01AC;
	rgb2rgb_p.matrix[1][2] = 0x0FB9;
	rgb2rgb_p.matrix[2][0] =  0x0FE0;
	rgb2rgb_p.matrix[2][1] = 0x0EC0;
	rgb2rgb_p.matrix[2][2] = 0x0260;

	rgb2rgb_p.offset[0]= 0x0000;
	rgb2rgb_p.offset[1]= 0x0000;
	rgb2rgb_p.offset[2]= 0x0000;





struct ispprev_csc {
	u16 matrix[3][3];
	s16 offset[3];
} csc_p =
	{
	{
	{ 66, 129, 25},
	{ -38, -75, 112},
	{ 112, -94 , -18}
	},
	{0x0, 0x0, 0x0}
	};

	struct ispprev_yclimit{
		u8 minC;
		u8 maxC;
		u8 minY;
		u8 maxY;
	}yclimit_p;

	yclimit_p.minC = 0x00;	/* Default values */
	yclimit_p.maxC = 0xFF;
	yclimit_p.minY = 0x00;
	yclimit_p.maxY = 0xFF;

	struct ispprev_dcor {
		u8 couplet_mode_en;
		u32 detect_correct[4];
	}ispprev_dcor_t;

	ispprev_dcor_t.couplet_mode_en = 1;
	ispprev_dcor_t.detect_correct[0] = 0xE;	/* Default values */
	ispprev_dcor_t.detect_correct[1] = 0xE;
	ispprev_dcor_t.detect_correct[2] = 0xE;
	ispprev_dcor_t.detect_correct[3] = 0xE;

	struct ispprv_update_config {
		u16 update;
		u16 algo_enable;
		void *yen;
		u32   shading_shift;
		struct ispprev_hmed *prev_hmed;
		struct ispprev_cfa *prev_cfa;
		struct ispprev_csup *csup;
		struct ispprev_wbal *prev_wbal;
		struct ispprev_blkadj *prev_blkadj;
		struct ispprev_rgbtorgb *rgb2rgb;
		struct ispprev_csc *prev_csc;
		struct ispprev_yclimit *yclimit;
		struct ispprev_dcor *prev_dcor;
	}preview_struct;

	int all;


#define PREV_AVERAGER			(1 << 0)
#define PREV_INVERSE_ALAW 		(1 << 1)
#define PREV_HORZ_MEDIAN_FILTER		(1 << 2)
#define PREV_NOISE_FILTER 		(1 << 3)
#define PREV_CFA         		(1 << 4)
#define PREV_GAMMA_BYPASS      		(1 << 5)
#define PREV_LUMA_ENHANCE     		(1 << 6)
#define PREV_CHROMA_SUPPRESS  		(1 << 7)
#define PREV_DARK_FRAME_SUBTRACT	(1 << 8)
#define PREV_LENS_SHADING    		(1 << 9)
#define PREV_DARK_FRAME_CAPTURE  	(1 << 10)
#define PREV_DEFECT_COR  		(1 << 11)


	preview_struct.update = 0x1;
	preview_struct.algo_enable = ISP_ABS_PREV_LUMAENH |
		ISP_ABS_PREV_CHROMA_SUPP | ISP_ABS_PREV_CFA |
		ISP_ABS_PREV_DEFECT_COR;

	preview_struct.algo_enable &= ~(PREV_AVERAGER | PREV_INVERSE_ALAW |
						PREV_HORZ_MEDIAN_FILTER |
						PREV_GAMMA_BYPASS |
						PREV_DARK_FRAME_SUBTRACT |
						PREV_LENS_SHADING |
						PREV_DARK_FRAME_CAPTURE);

	preview_struct.yen = luma_enhance_table;
	preview_struct.shading_shift = 0;
	preview_struct.prev_hmed = &hmed_p;
	preview_struct.prev_cfa = &cfa_p;
	preview_struct.csup = &csup_p;
	preview_struct.prev_wbal = &wbal_p;
	preview_struct.prev_blkadj = &blkadj_p;
	preview_struct.rgb2rgb = &rgb2rgb_p;
	preview_struct.prev_csc = &csc_p;
	preview_struct.yclimit = &yclimit_p;
	preview_struct.prev_dcor = &ispprev_dcor_t;

	if (argc == 1) {
		usage();
		return 0;
	}

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
		if((!strcasecmp(argv[2], "le"))) {
			preview_struct.update = ISP_ABS_PREV_LUMAENH;
			preview_struct.algo_enable = ISP_ABS_PREV_LUMAENH;
		} else if((!strcasecmp(argv[2], "ialaw"))) {
			preview_struct.update=1<<2;
			preview_struct.algo_enable=1<<2;
			printf("\n TC 1<<2 sent\n");
		} else if((!strcasecmp(argv[2], "hm"))) {
			preview_struct.update=1<<5;
			preview_struct.algo_enable=1<<5;
		} else if((!strcasecmp(argv[2], "cfa"))) {
			preview_struct.update=1<<6;
			preview_struct.algo_enable=1<<6;
		} else if((!strcasecmp(argv[2], "cs"))) {
			preview_struct.update=1<<7;
			preview_struct.algo_enable=1<<7;
		 } else if((!strcasecmp(argv[2], "wb"))) {
			preview_struct.update=1<<8;
			preview_struct.algo_enable=1<<8;
		} else if((!strcasecmp(argv[2], "ba"))) {
			preview_struct.update=1<<9;
			preview_struct.algo_enable=1<<9;
		} else if((!strcasecmp(argv[2], "r"))) {
			preview_struct.update=1<<10;
			preview_struct.algo_enable=1<<10;
		} else if((!strcasecmp(argv[2], "cc"))) {
			preview_struct.update=1<<11;
			preview_struct.algo_enable=1<<11;
		} else if((!strcasecmp(argv[2], "yc"))) {
			preview_struct.update=1<<12;
			preview_struct.algo_enable=1<<12;
		} else if((!strcasecmp(argv[2], "dc"))) {
			preview_struct.update=1<<13;
			preview_struct.algo_enable=1<<13;
		} else if((!strcasecmp(argv[2], "gb"))) {
			preview_struct.algo_enable=1<<14;
		} else if((!strcasecmp(argv[2], "all"))) {
			preview_struct.update=(1<<14) - 1;
			preview_struct.algo_enable=(1<<14) - 1;
		} else {
			printf(" Wrong option please enter correct"
					" option argv[1]=%s\n", argv[2]);
			usage();
			return 0;
		}
	}


	if ((cfd = open_cam_device(O_RDWR)) <= 0) {
		printf ("Could not open the cam device\n");
		return -1;
	}


	memset(&queryctrl2, 0, sizeof(queryctrl2));
	memset(&control2, 0, sizeof(control2));


	control5.id = V4L2_CID_PRIVATE_ISP_PRV_CFG;
	control5.value = (int)&preview_struct;
	if (ioctl(cfd, VIDIOC_S_CTRL, &control5) == -1) {
		printf("\nerror\n");
	}
	else
		printf("VIDIOC_S_CTRL successful\n");

	close(cfd);
}
