/* =========================================================================
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
#include <linux/ioctl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <linux/videodev.h>
#include <linux/errno.h>
#include <errno.h>

#ifndef u32
#define u32 unsigned int
#endif /* u32 */

#ifndef u16
#define u16 unsigned short 
#endif /* u16 */

#ifndef u8
#define u8 unsigned char
#endif /* u8 */

#ifndef s16
#define s16 signed short
#endif /* s16 */

static u8 ispccdc_lsc_tbl[] = {
	#include "ispccdc_lsc5.dat"
};

#define V4L2_CID_PRIVATE_ISP_COLOR_FX	(V4L2_CID_PRIVATE_BASE + 0)
#define V4L2_CID_PRIVATE_ISP_CCDC_CFG	(V4L2_CID_PRIVATE_BASE + 1)
#define V4L2_CID_PRIVATE_ISP_PRV_CFG	(V4L2_CID_PRIVATE_BASE + 2)
#define V4L2_CID_PRIVATE_ISP_LSC_UPDATE	(V4L2_CID_PRIVATE_BASE + 3)

/*Abstraction layer ccdc configurations*/
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
struct ispccdc_blcomp{
	u8 b_mg;
	u8 gb_g;
	u8 gr_cy;
	u8 r_ye;
} blcomp_t;
struct ispccdc_fpc{
	u16 fpnum;
	u32 fpcaddr;
}fpc_t;
struct ispccdc_culling{
	u8 v_pattern;
	u16 h_odd;
	u16 h_even;
}culling_t;
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

static void printCcdc(void)
{
	printf("CCDC_Update\n");
	printf("\n---- Options for CCDC_Update:\n alc - Alaw compression   ");
	printf(" LPF- Low pass filter,    bcl - Black Clamp ,\t  bcomp -"
		"Black compensation ,    FPC -Faulty pixel correction,    "
		"cull - CCDC Culling,    col - Colptn \n");
}

static void printPreview(void)
{
	printf("PREVIEW_Update\n");
	printf("\n---- Options for Preview_Updates: \n "
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

static void printTables(void)
{
	printf("TABLES_Update\n");
	printf("\n---- Options for Tables_Updates: \n");
	printf("lsc - Lens Shading Compensation,   nf - Noise Filter   ");
	printf("bg - Blue Gamma Table Update,   gg - Green Gamma Table "
							"Update,   ");
	printf("rg - Red Gamma Table Update, \n");
}

int absLayerValidateGroup(char * absGroup)
{
	if (!strcasecmp(absGroup,"CCDC"))
		return 1;
	else if (!strcasecmp(absGroup,"PREVIEW"))
		return 1;
	else if (!strcasecmp(absGroup,"TABLES"))
		return 1;
	else 
		return 0;
}

int updateCcdc(int fd, char * ccdcOption, int bclFlag)
{
	struct v4l2_queryctrl queryctrl, queryctrl2;
	struct v4l2_control control;
	
	/*bclamp_t.obgain = 10;
	bclamp_t.obstpixel = 20;
	bclamp_t.oblines = 30;
	bclamp_t.oblen = 40;
	bclamp_t.dcsubval = 42;*/
	
	bclamp_t.obgain = 10;
	bclamp_t.obstpixel = 20;
	bclamp_t.oblines = 30;
	bclamp_t.oblen = 40;
	bclamp_t.dcsubval = 150;
	
	blcomp_t.b_mg = 0;//OJV
	blcomp_t.gb_g = 0;//OJV
	blcomp_t.gr_cy = 0;//OJV
	blcomp_t.r_ye = 0;//OJV
	
	fpc_tbl[0] = ((10 << 19) || (10 << 5) || (1 << 0));
	fpc_tbl[1] = ((10 << 19) || (20 << 5) || (1 << 0));
	fpc_tbl[2] = ((10 << 19) || (30 << 5) || (1 << 0));
	fpc_tbl[3] = ((10 << 19) || (40 << 5) || (1 << 0));
	fpc_tbl[4] = ((10 << 19) || (50 << 5) || (1 << 0));
	
	fpc_t.fpnum = 5;//1200;//25;//1200;//0xa;
	fpc_t.fpcaddr = (u32)fpc_tbl;//0xc0;
	
	culling_t.v_pattern=12;
	culling_t.h_odd=13;
	culling_t.h_even=14;
	
	ispccdc_lsc_config_t.offset = 0x60;
	ispccdc_lsc_config_t.gain_mode_n = 6;
	ispccdc_lsc_config_t.gain_mode_m = 6;
	ispccdc_lsc_config_t.gain_format = 4;
	ispccdc_lsc_config_t.fmtsph = 0;
	ispccdc_lsc_config_t.fmtlnh = 0;
	ispccdc_lsc_config_t.fmtslv = 0;
	ispccdc_lsc_config_t.fmtlnv = 0;
	ispccdc_lsc_config_t.size = sizeof(ispccdc_lsc_tbl);
	
	arg_ccdc_t.update = 0;
	arg_ccdc_t.flag = 0;
	arg_ccdc_t.alawip = ALAW_BIT10_1;  
	arg_ccdc_t.bclamp = &bclamp_t;
	arg_ccdc_t.blcomp = &blcomp_t;
	arg_ccdc_t.fpc = &fpc_t;
	arg_ccdc_t.cull = &culling_t;
	arg_ccdc_t.colptn = 30;
	arg_ccdc_t.lsc_cfg = &ispccdc_lsc_config_t;
	
	if((!strcasecmp(ccdcOption, "alc"))) {
		arg_ccdc_t.update= 1 << 0;
		arg_ccdc_t.flag= 1 << 0;
	} else if((!strcasecmp(ccdcOption, "lpf"))) {
		arg_ccdc_t.update= 1 << 1;
		arg_ccdc_t.flag= 1 << 1;
	} else if((!strcasecmp(ccdcOption, "bcl"))) {
		if (bclFlag == 1)
			arg_ccdc_t.flag= 1 << 2;
		else if (bclFlag == 0)
			arg_ccdc_t.flag= 0 << 2;
		else {
			printf("Wrong bcl Flag value\n");
			return 0;
		}
		arg_ccdc_t.update= 1 << 2;
	} else if((!strcasecmp(ccdcOption, "bcomp"))) {
		arg_ccdc_t.update= 1 << 3;
		arg_ccdc_t.flag= 1 << 3;
	} else if((!strcasecmp(ccdcOption, "fpc"))) {
		 arg_ccdc_t.update=1<<4;
		arg_ccdc_t.flag=1 << 4;
	} else if((!strcasecmp(ccdcOption, "cull"))) {
		arg_ccdc_t.update=1<<5;
		arg_ccdc_t.flag= 1 << 5;
	} else if((!strcasecmp(ccdcOption, "col"))) {
		arg_ccdc_t.update=1<<6;
		arg_ccdc_t.flag = 1 << 6;
	} else if((!strcasecmp(ccdcOption, "lc"))) {
		arg_ccdc_t.update = 1 << 7;
		arg_ccdc_t.flag = 1 << 7;
	} else if((!strcasecmp(ccdcOption, "all"))) {
		arg_ccdc_t.update = (1 << 8) - 1;
		arg_ccdc_t.flag = (1 << 8) - 1;
	} else if((!strcasecmp(ccdcOption, "none"))) {
		arg_ccdc_t.update = 0;
		arg_ccdc_t.flag = 0;
	} else {
		printf(" Wrong option %s, please enter correct option\n",
					ccdcOption);
		printCcdc();
		return 0;
	}
	
	memset(&queryctrl, 0, sizeof(queryctrl));
	memset(&control, 0, sizeof(control));

	control.id = V4L2_CID_PRIVATE_ISP_CCDC_CFG;
	control.value = (int)&arg_ccdc_t;
	if (ioctl(fd, VIDIOC_S_CTRL, &control) == -1) {
		printf("\nerror\n");
		return 0;
		}
	else
		printf("abs Layer VIDIOC_S_CTRL successful\n");
	
	return 1;
}

int updatePreview(int fd, char * previewOption)
{
	struct v4l2_queryctrl queryctrl;
	struct v4l2_control control;

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
/*		{
		{
		{ 66, 129, 25},
		{ -38, -75, 112},
		{ 112, -94 , -18}
		},
		{0x0, 0x0, 0x0}
		};*/
		
		{ /*CSC Coef Matrix Sepia*/
		{
		{ 66, 129, 25},
		{ 0, 0, 0},
		{ 0, 0, 0}
		},          /* CSC Offset */
		{0x0, 0xE7, 0x14}
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
	
	if((!strcasecmp(previewOption, "le"))) {
		preview_struct.update = ISP_ABS_PREV_LUMAENH;
		preview_struct.algo_enable = ISP_ABS_PREV_LUMAENH;
	} else if((!strcasecmp(previewOption, "ialaw"))) {
		preview_struct.update=1<<2;
		preview_struct.algo_enable=1<<2;
		printf("\n TC 1<<2 sent\n");
	} else if((!strcasecmp(previewOption, "hm"))) {
		preview_struct.update=1<<5;
		preview_struct.algo_enable=1<<5;
	} else if((!strcasecmp(previewOption, "cfa"))) {
		preview_struct.update=1<<6;
		preview_struct.algo_enable=1<<6;
	} else if((!strcasecmp(previewOption, "cs"))) {
		preview_struct.update=1<<7;
		preview_struct.algo_enable=1<<7;
	 } else if((!strcasecmp(previewOption, "wb"))) {
		preview_struct.update=1<<8;
		preview_struct.algo_enable=1<<8;
	} else if((!strcasecmp(previewOption, "ba"))) {
		preview_struct.update=1<<9;
		preview_struct.algo_enable=1<<9;
	} else if((!strcasecmp(previewOption, "r"))) {
		preview_struct.update=1<<10;
		preview_struct.algo_enable=1<<10;
	} else if((!strcasecmp(previewOption, "cc"))) {
		preview_struct.update=1<<11;
		preview_struct.algo_enable=1<<11;
	} else if((!strcasecmp(previewOption, "yc"))) {
		preview_struct.update=1<<12;
		preview_struct.algo_enable=1<<12;
	} else if((!strcasecmp(previewOption, "dc"))) {
		preview_struct.update=1<<13;
		preview_struct.algo_enable=1<<13;
	} else if((!strcasecmp(previewOption, "gb"))) {
		preview_struct.algo_enable=1<<14;
	} else if((!strcasecmp(previewOption, "all"))) {
		preview_struct.update=(1<<14) - 1;
		preview_struct.algo_enable=(1<<14) - 1;
	} else {
		printf("Wrong option %s, please enter correct"
				" option\n", previewOption);
		printPreview();
		return 0;
	}
	
	memset(&queryctrl, 0, sizeof(queryctrl));
	memset(&control, 0, sizeof(control));

	control.id = V4L2_CID_PRIVATE_ISP_PRV_CFG;
	control.value = (int)&preview_struct;

	if (ioctl(fd, VIDIOC_S_CTRL, &control) == -1) {
		printf("\nerror\n");
		return 0;
		}
	else
		printf("abs Layer VIDIOC_S_CTRL successful\n");
	
	return 1;
}

int updateTables(int fd, char * tablesOption)
{
	struct v4l2_queryctrl queryctrl;
	struct v4l2_control control;

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

	for(all=0;all<32;all++) {
		table_t[all]=40; /*16 Default*/
	}
	
	for(all=32;all<64;all++) {
		table_t[all]=20; /*31 Default*/
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
	
	if((!strcasecmp(tablesOption, "lsc"))) {
		isptables_update_t.update= 1 << 0;
		isptables_update_t.algo_enable= 1 << 0;
	} else if((!strcasecmp(tablesOption, "lsc_dis"))) {
		isptables_update_t.update= 1 << 0;
		isptables_update_t.algo_enable= 0 << 0;
	} else if((!strcasecmp(tablesOption, "nf"))) {
		isptables_update_t.update= 1 << 1;
		isptables_update_t.algo_enable= 1 << 1;
	} else if((!strcmp(tablesOption, "rg"))) {
		isptables_update_t.update= 1 << 2;
		isptables_update_t.algo_enable= 1 << 2;
	} else if((!strcmp(tablesOption, "gg"))){
		isptables_update_t.update= 1 << 3;
		isptables_update_t.algo_enable= 1 << 3;
	} else if((!strcmp(tablesOption, "bg"))) {
		isptables_update_t.update= 1 << 4;
		isptables_update_t.algo_enable= 1 << 4;
	} else {
		printf(" Wrong option %s, please enter correct option \n"
					, tablesOption);
		printTables();
		return 0;
	}
	
	control.id = V4L2_CID_PRIVATE_ISP_LSC_UPDATE;
        control.value = (int)&isptables_update_t;

	if (ioctl(fd, VIDIOC_S_CTRL, &control) == -1) {
		printf("\nerror\n");
		return 0;
		}
	else
		printf("abs Layer VIDIOC_S_CTRL successful\n");
	
	return 1;
}
