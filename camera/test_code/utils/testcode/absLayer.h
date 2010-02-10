/* =========================================================================
*             Texas Instruments OMAP(TM) Platform Software
*  (c) Copyright Texas Instruments, Incorporated.  All Rights Reserved.
*
*  Use of this software is controlled by the terms and conditions found
*  in the license agreement under which this software has been supplied.
* ========================================================================== */

static __u8 ispccdc_lsc_tbl[] = {
	#include "ispccdc_lsc5.dat"
};

/*Abstraction layer ccdc configurations*/
__u32 fpc_tbl[5];

struct ispccdc_bclamp bclamp_t;
struct ispccdc_blcomp blcomp_t;
struct ispccdc_fpc fpc_t;
struct ispccdc_culling culling_t;
struct ispccdc_lsc_config ispccdc_lsc_config_t;
/* Structure for CCDC configuration*/
struct ispccdc_update_config arg_ccdc_t;

static void printCcdc(void)
{
	printf("CCDC_Update\n");
	printf("\n---- Options for CCDC_Update:\n alc - Alaw compression   ");
	printf(" LPF- Low pass filter,    bcl - Black Clamp ,\t  bcomp -"
		"Black compensation ,    FPC -Faulty pixel correction,    "
		"cull - CCDC Culling,    col - Colptn,    lc - LSC table \n");
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
	printf("\n---- Options for Tables_Updates: \n");
	printf("nf - Noise Filter Table\n");
	printf("bg - Blue Gamma Table Update,   gg - Green Gamma Table "
							"Update,   ");
	printf("rg - Red Gamma Table Update, \n");
	printf(" all - all\n");
}

int absLayerValidateGroup(char *absGroup)
{
	if (!strcasecmp(absGroup, "CCDC"))
		return 1;
	else if (!strcasecmp(absGroup, "PREVIEW"))
		return 1;
	return 0;
}

int updateCcdc(int fd, char *ccdcOption)
{
	struct v4l2_queryctrl queryctrl, queryctrl2;
	struct v4l2_control control;
	int ret;
	static __u8 ispccdc_lsc_tbl[] = {
#include "ispccdc_lsc5.dat"
	};

	bclamp_t.obgain = 10;
	bclamp_t.obstpixel = 20;
	bclamp_t.oblines = 30;
	bclamp_t.oblen = 40;
	bclamp_t.dcsubval = 150;

	blcomp_t.b_mg = 0;
	blcomp_t.gb_g = 0;
	blcomp_t.gr_cy = 0;
	blcomp_t.r_ye = 0;

	fpc_tbl[0] = ((10 << 19) || (10 << 5) || (1 << 0));
	fpc_tbl[1] = ((10 << 19) || (20 << 5) || (1 << 0));
	fpc_tbl[2] = ((10 << 19) || (30 << 5) || (1 << 0));
	fpc_tbl[3] = ((10 << 19) || (40 << 5) || (1 << 0));
	fpc_tbl[4] = ((10 << 19) || (50 << 5) || (1 << 0));

	fpc_t.fpnum = 5;
	fpc_t.fpcaddr = (__u32)fpc_tbl;

	culling_t.v_pattern = 12;
	culling_t.h_odd = 13;
	culling_t.h_even = 14;

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
	arg_ccdc_t.lsc = (void *)ispccdc_lsc_tbl;

	if ((!strcasecmp(ccdcOption, "alc"))) {
		arg_ccdc_t.update = ISP_ABS_CCDC_ALAW;
		arg_ccdc_t.flag = ISP_ABS_CCDC_ALAW;
	} else if ((!strcasecmp(ccdcOption, "lpf"))) {
		arg_ccdc_t.update = ISP_ABS_CCDC_LPF;
		arg_ccdc_t.flag = ISP_ABS_CCDC_LPF;
	} else if ((!strcasecmp(ccdcOption, "bcl"))) {
		arg_ccdc_t.update = ISP_ABS_CCDC_BLCLAMP;
		arg_ccdc_t.flag = ISP_ABS_CCDC_BLCLAMP;
	} else if ((!strcasecmp(ccdcOption, "bcomp"))) {
		arg_ccdc_t.update = ISP_ABS_CCDC_BCOMP;
		arg_ccdc_t.flag = ISP_ABS_CCDC_BCOMP;
	} else if ((!strcasecmp(ccdcOption, "fpc"))) {
		arg_ccdc_t.update = ISP_ABS_CCDC_FPC;
		arg_ccdc_t.flag = ISP_ABS_CCDC_FPC;
	} else if ((!strcasecmp(ccdcOption, "cull"))) {
		arg_ccdc_t.update = ISP_ABS_CCDC_CULL;
		arg_ccdc_t.flag = ISP_ABS_CCDC_CULL;
	} else if ((!strcasecmp(ccdcOption, "col"))) {
		arg_ccdc_t.update = ISP_ABS_CCDC_COLPTN;
		arg_ccdc_t.flag = ISP_ABS_CCDC_COLPTN;
	} else if ((!strcasecmp(ccdcOption, "lc"))) {
		arg_ccdc_t.update = ISP_ABS_CCDC_CONFIG_LSC | ISP_ABS_TBL_LSC;
		arg_ccdc_t.flag = ISP_ABS_CCDC_CONFIG_LSC;
	} else if ((!strcasecmp(ccdcOption, "all"))) {
		arg_ccdc_t.update = ~0;
		arg_ccdc_t.flag = ~0;
	} else if ((!strcasecmp(ccdcOption, "none"))) {
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

	ret = ioctl(fd, VIDIOC_PRIVATE_ISP_CCDC_CFG, &arg_ccdc_t);
	if (ret == -1) {
		printf("\nerror\n");
		return 0;
	}
	printf("abs Layer VIDIOC_S_CTRL successful\n");

	return 1;
}

/**
 * resetCcdc - Reset Abstraction layer to default values.
 * @fd: File descriptor
 * @ccdcOption: Text string indicating option to reset
 **/
int resetCcdc(int fd, char *ccdcOption)
{
	int ret;
	static __u8 ispccdc_lsc_tbl[] = {
#include "ispccdc_lsc5.dat"
	};

	/* ok - reset default */
	bclamp_t.obgain = 0x10;
	bclamp_t.obstpixel = 0;
	bclamp_t.oblines = 0;
	bclamp_t.oblen = 0;
	bclamp_t.dcsubval = 0;

	/* ok - reset default */
	blcomp_t.b_mg = 0;
	blcomp_t.gb_g = 0;
	blcomp_t.gr_cy = 0;
	blcomp_t.r_ye = 0;

	fpc_tbl[0] = ((10 << 19) || (10 << 5) || (1 << 0));
	fpc_tbl[1] = ((10 << 19) || (20 << 5) || (1 << 0));
	fpc_tbl[2] = ((10 << 19) || (30 << 5) || (1 << 0));
	fpc_tbl[3] = ((10 << 19) || (40 << 5) || (1 << 0));
	fpc_tbl[4] = ((10 << 19) || (50 << 5) || (1 << 0));
	fpc_t.fpnum = 0;
	fpc_t.fpcaddr = (__u32)fpc_tbl;

	/* ok - reset default */
	culling_t.v_pattern = 0xFF;
	culling_t.h_odd = 0xFF;
	culling_t.h_even = 0xFF;

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
	arg_ccdc_t.alawip = 0;
	arg_ccdc_t.bclamp = &bclamp_t;
	arg_ccdc_t.blcomp = &blcomp_t;
	arg_ccdc_t.fpc = &fpc_t;
	arg_ccdc_t.cull = &culling_t;

	arg_ccdc_t.lsc_cfg = &ispccdc_lsc_config_t;
	arg_ccdc_t.lsc = (void *)ispccdc_lsc_tbl;


	if ((!strcasecmp(ccdcOption, "alc"))) {
		arg_ccdc_t.update = ISP_ABS_CCDC_ALAW;

	} else if ((!strcasecmp(ccdcOption, "lpf"))) {
		arg_ccdc_t.flag = 0;

	} else if ((!strcasecmp(ccdcOption, "bcl"))) {
		arg_ccdc_t.update = ISP_ABS_CCDC_BLCLAMP;
		arg_ccdc_t.flag = ISP_ABS_CCDC_BLCLAMP;

	} else if ((!strcasecmp(ccdcOption, "bcomp"))) {
		/* No way to disable. Needs default values */
		arg_ccdc_t.update = ISP_ABS_CCDC_BCOMP;

	} else if ((!strcasecmp(ccdcOption, "fpc"))) {
		arg_ccdc_t.update = ISP_ABS_CCDC_FPC;

	} else if ((!strcasecmp(ccdcOption, "cull"))) {
		/* No way to disable. Needs default values */
		arg_ccdc_t.update = ISP_ABS_CCDC_CULL;

	} else if ((!strcasecmp(ccdcOption, "col"))) {
		arg_ccdc_t.colptn = 0;
		arg_ccdc_t.update = ISP_ABS_CCDC_COLPTN;

	} else if ((!strcasecmp(ccdcOption, "lc"))) {
		arg_ccdc_t.update = 0;
		arg_ccdc_t.flag = 0;
	} else if ((!strcasecmp(ccdcOption, "all"))) {
		arg_ccdc_t.update = ~0;
		arg_ccdc_t.flag = ~0;
	} else if ((!strcasecmp(ccdcOption, "none"))) {
		arg_ccdc_t.update = 0;
		arg_ccdc_t.flag = 0;
	} else {
		return 0;
	}

	ret = ioctl(fd, VIDIOC_PRIVATE_ISP_CCDC_CFG, &arg_ccdc_t);
	if (ret == -1) {
		printf("\nerror\n");
		return 0;
	}

	/* Special case for 'bcl'.
	 * First we reset to default values then call ioclt again
	 * with only 'update' field set, which has the effect of
	 * turning it off.
	 */
	if ((!strcasecmp(ccdcOption, "bcl"))) {
		arg_ccdc_t.update = ISP_ABS_CCDC_BLCLAMP;
	} else {
		return 1;
	}

	ret = ioctl(fd, VIDIOC_PRIVATE_ISP_CCDC_CFG, &arg_ccdc_t);
	if (ret == -1) {
		printf("\nerror\n");
		return 0;
	}

	return 1;
}


int updatePreview(int fd, char *previewOption)
{
	int ret;

	struct ispprev_hmed hmed_p;

	hmed_p.odddist = 0x2; /* For Bayer sensor */
	hmed_p.evendist = 0x2;
	hmed_p.thres = 0x10;

	static __u32 cfa_coef_table[] = {
		#include "cfa_coef_table.h"
	};

	static __u32 luma_enhance_table[] = {
		#include "luma_enhance_table.h"
	};

	struct ispprev_cfa cfa_p;

	cfa_p.cfafmt = CFAFMT_BAYER;
	cfa_p.cfa_gradthrs_vert = 0x28; /* Default values */
	cfa_p.cfa_gradthrs_horz = 0x28; /* Default values */
	cfa_p.cfa_table = cfa_coef_table;

	struct ispprev_csup csup_p;

	csup_p.gain = 0x0D;
	csup_p.thres = 0xEB;
	csup_p.hypf_en = 0;

	/* Structure for White Balance */
	struct ispprev_wbal wbal_p;

	wbal_p.dgain = 0x100;
	wbal_p.coef3 = 0x94;
	wbal_p.coef2 = 0x5C;
	wbal_p.coef1 = 0x5C;
	wbal_p.coef0 = 0x68;

	struct ispprev_blkadj blkadj_p;

	blkadj_p.red = 0x0;
	blkadj_p.green = 0x0;
	blkadj_p.blue = 0x0;

	struct ispprev_rgbtorgb rgb2rgb_p;

	rgb2rgb_p.matrix[0][0] = 0x01E2;
	rgb2rgb_p.matrix[0][1] = 0x0F30;
	rgb2rgb_p.matrix[0][2] = 0x0FEE;
	rgb2rgb_p.matrix[1][0] = 0x0F9B;
	rgb2rgb_p.matrix[1][1] = 0x01AC;
	rgb2rgb_p.matrix[1][2] = 0x0FB9;
	rgb2rgb_p.matrix[2][0] = 0x0FE0;
	rgb2rgb_p.matrix[2][1] = 0x0EC0;
	rgb2rgb_p.matrix[2][2] = 0x0260;

	rgb2rgb_p.offset[0] = 0x0000;
	rgb2rgb_p.offset[1] = 0x0000;
	rgb2rgb_p.offset[2] = 0x0000;

	struct ispprev_csc csc_p = {
		/*CSC Coef Matrix Sepia*/
		{
			{66, 129, 25},
			{0, 0, 0},
			{0, 0, 0}
		},
		/* CSC Offset */
		{0x0, 0xE7, 0x14}
	};

	struct ispprev_yclimit yclimit_p;

	yclimit_p.minC = 0x00;	/* Default values */
	yclimit_p.maxC = 0xFF;
	yclimit_p.minY = 0x00;
	yclimit_p.maxY = 0xFF;

	struct ispprev_dcor ispprev_dcor_t;

	ispprev_dcor_t.couplet_mode_en = 1;
	ispprev_dcor_t.detect_correct[0] = 0xE;	/* Default values */
	ispprev_dcor_t.detect_correct[1] = 0xE;
	ispprev_dcor_t.detect_correct[2] = 0xE;
	ispprev_dcor_t.detect_correct[3] = 0xE;

	struct ispprv_update_config preview_struct;

	__u32 red_gamma_t[1024];
	__u32 green_gamma_t[1024];
	__u32 blue_gamma_t[1024];
	__u32 table_t[64];
	int all;
	struct ispprev_nf ispprev_nf_t;

	for (all = 0; all < 32; all++)
		table_t[all] = 40; /*16 Default*/

	for (all = 32; all < 64; all++)
		table_t[all] = 20; /*31 Default*/

	ispprev_nf_t.spread = 3;
	memcpy(ispprev_nf_t.table, table_t, sizeof(ispprev_nf_t.table));
	for (all = 0; all < 1024; all++)
		red_gamma_t[all] = 0;

	preview_struct.red_gamma = (void *)red_gamma_t;
	preview_struct.green_gamma = (void *)green_gamma_t;
	preview_struct.blue_gamma = (void *)blue_gamma_t;
	preview_struct.prev_nf = &ispprev_nf_t;

	preview_struct.update = 0x1;
	preview_struct.flag = ISP_ABS_PREV_LUMAENH |
		ISP_ABS_PREV_CHROMA_SUPP | ISP_ABS_PREV_CFA |
		ISP_ABS_PREV_DEFECT_COR;

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

	if ((!strcasecmp(previewOption, "le"))) {
		preview_struct.update = ISP_ABS_PREV_LUMAENH;
		preview_struct.flag = ISP_ABS_PREV_LUMAENH;
	} else if ((!strcasecmp(previewOption, "ialaw"))) {
		preview_struct.update = ISP_ABS_PREV_INVALAW;
		preview_struct.flag = ISP_ABS_PREV_INVALAW;
		printf("\n TC 1<<2 sent\n");
	} else if ((!strcasecmp(previewOption, "hm"))) {
		preview_struct.update = ISP_ABS_PREV_HRZ_MED;
		preview_struct.flag = ISP_ABS_PREV_HRZ_MED;
	} else if ((!strcasecmp(previewOption, "cfa"))) {
		preview_struct.update = ISP_ABS_PREV_CFA;
		preview_struct.flag = ISP_ABS_PREV_CFA;
	} else if ((!strcasecmp(previewOption, "cs"))) {
		preview_struct.update = ISP_ABS_PREV_CHROMA_SUPP;
		preview_struct.flag = ISP_ABS_PREV_CHROMA_SUPP;
	} else if ((!strcasecmp(previewOption, "wb"))) {
		preview_struct.update = ISP_ABS_PREV_WB;
		preview_struct.flag = ISP_ABS_PREV_WB;
	} else if ((!strcasecmp(previewOption, "ba"))) {
		preview_struct.update = ISP_ABS_PREV_BLKADJ;
		preview_struct.flag = ISP_ABS_PREV_BLKADJ;
	} else if ((!strcasecmp(previewOption, "r"))) {
		preview_struct.update = ISP_ABS_PREV_RGB2RGB;
		preview_struct.flag = ISP_ABS_PREV_RGB2RGB;
	} else if ((!strcasecmp(previewOption, "cc"))) {
		preview_struct.update = ISP_ABS_PREV_COLOR_CONV;
		preview_struct.flag = ISP_ABS_PREV_COLOR_CONV;
	} else if ((!strcasecmp(previewOption, "yc"))) {
		preview_struct.update = ISP_ABS_PREV_YC_LIMIT;
		preview_struct.flag = ISP_ABS_PREV_YC_LIMIT;
	} else if ((!strcasecmp(previewOption, "dc"))) {
		preview_struct.update = ISP_ABS_PREV_DEFECT_COR;
		preview_struct.flag = ISP_ABS_PREV_DEFECT_COR;
	} else if ((!strcasecmp(previewOption, "gb"))) {
		preview_struct.flag = ISP_ABS_PREV_GAMMABYPASS;
	} else if ((!strcasecmp(previewOption, "nf"))) {
		preview_struct.update = ISP_ABS_TBL_NF;
		preview_struct.flag = ISP_ABS_TBL_NF;
	} else if ((!strcasecmp(previewOption, "rg"))) {
		preview_struct.update = ISP_ABS_TBL_REDGAMMA;
		preview_struct.flag = ISP_ABS_TBL_GREENGAMMA;
	} else if ((!strcasecmp(previewOption, "gg"))) {
		preview_struct.update = ISP_ABS_TBL_GREENGAMMA;
		preview_struct.flag = ISP_ABS_TBL_GREENGAMMA;
	} else if ((!strcasecmp(previewOption, "bg"))) {
		preview_struct.update = ISP_ABS_TBL_BLUEGAMMA;
		preview_struct.flag = ISP_ABS_TBL_BLUEGAMMA;
	} else if ((!strcasecmp(previewOption, "all"))) {
		preview_struct.update = ~0;
		preview_struct.flag = ~0;
	} else if ((!strcasecmp(previewOption, "none"))) {
		return 1;
	} else {
		printf("Wrong option %s, please enter correct"
				" option\n", previewOption);
		printPreview();
		return 0;
	}

	ret = ioctl(fd, VIDIOC_PRIVATE_ISP_PRV_CFG, &preview_struct);

	if (ret == -1) {
		printf("\nerror\n");
		return 0;
	} else
		printf("abs Layer VIDIOC_S_CTRL successful\n");

	return 1;
}

/**
 * resetPreview - Reset Abstraction layer to default values.
 * @fd: File descriptor
 * @previewOption: Text string indicating option to reset
 **/
int resetPreview(int fd, char *previewOption)
{
	int ret;
	struct ispprev_hmed hmed_p;

	hmed_p.odddist = 0x2; /* For Bayer sensor */
	hmed_p.evendist = 0x2;
	hmed_p.thres = 0x10;

	static __u32 redgamma_table[] = {
	#include "redgamma_table.h"
	};

	static __u32 greengamma_table[] = {
	#include "greengamma_table.h"
	};

	static __u32 bluegamma_table[] = {
	#include "bluegamma_table.h"
	};

	static __u32 cfa_coef_table[] = {
		#include "cfa_coef_table.h"
	};

	static __u32 luma_enhance_table[] = {
		#include "luma_enhance_table.h"
	};

	struct ispprev_cfa cfa_p;

	cfa_p.cfafmt = CFAFMT_BAYER;
	cfa_p.cfa_gradthrs_vert = 0x28; /* Default values */
	cfa_p.cfa_gradthrs_horz = 0x28; /* Default values */
	cfa_p.cfa_table = cfa_coef_table;

	struct ispprev_csup csup_p;

	csup_p.gain = 0x0D;
	csup_p.thres = 0xEB;
	csup_p.hypf_en = 0;

	/* Structure for White Balance */
	struct ispprev_wbal wbal_p;

	wbal_p.dgain = 0x100;
	wbal_p.coef3 = 0x23;
	wbal_p.coef2 = 0x20;
	wbal_p.coef1 = 0x20;
	wbal_p.coef0 = 0x39;

	struct ispprev_blkadj blkadj_p;

	blkadj_p.red = 0x0;
	blkadj_p.green = 0x0;
	blkadj_p.blue = 0x0;

	struct ispprev_rgbtorgb rgb2rgb_p;

	rgb2rgb_p.matrix[0][0] = 0x01E2;
	rgb2rgb_p.matrix[0][1] = 0x0F30;
	rgb2rgb_p.matrix[0][2] = 0x0FEE;
	rgb2rgb_p.matrix[1][0] = 0x0F9B;
	rgb2rgb_p.matrix[1][1] = 0x01AC;
	rgb2rgb_p.matrix[1][2] = 0x0FB9;
	rgb2rgb_p.matrix[2][0] = 0x0FE0;
	rgb2rgb_p.matrix[2][1] = 0x0EC0;
	rgb2rgb_p.matrix[2][2] = 0x0260;

	rgb2rgb_p.offset[0] = 0x0000;
	rgb2rgb_p.offset[1] = 0x0000;
	rgb2rgb_p.offset[2] = 0x0000;

	struct ispprev_csc csc_p = {
		/*CSC Coef Matrix No Effect*/
		{
			{66, 129, 25},
			{-38, -75, 112},
			{112, -94, -18}
		},
		/* CSC Offset */
		{0x0, 0x0, 0x0}
	};

	struct ispprev_yclimit yclimit_p;

	yclimit_p.minC = 0x00;	/* Default values */
	yclimit_p.maxC = 0xFF;
	yclimit_p.minY = 0x00;
	yclimit_p.maxY = 0xFF;

	struct ispprev_dcor ispprev_dcor_t;

	ispprev_dcor_t.couplet_mode_en = 1;
	ispprev_dcor_t.detect_correct[0] = 0xE;	/* Default values */
	ispprev_dcor_t.detect_correct[1] = 0xE;
	ispprev_dcor_t.detect_correct[2] = 0xE;
	ispprev_dcor_t.detect_correct[3] = 0xE;

	struct ispprv_update_config preview_struct;

	__u32 table_t[64];
	int all;
	struct ispprev_nf ispprev_nf_t;

	for (all = 0; all < 32; all++)
		table_t[all] = 40; /*16 Default*/

	for (all = 32; all < 64; all++)
		table_t[all] = 20; /*31 Default*/

	ispprev_nf_t.spread = 3;
	memcpy(ispprev_nf_t.table, table_t, sizeof(ispprev_nf_t.table));

	preview_struct.red_gamma = (void *)redgamma_table;
	preview_struct.green_gamma = (void *)greengamma_table;
	preview_struct.blue_gamma = (void *)bluegamma_table;
	preview_struct.prev_nf = &ispprev_nf_t;


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

	if ((!strcasecmp(previewOption, "le"))) {
		preview_struct.update = ISP_ABS_PREV_LUMAENH;

	} else if ((!strcasecmp(previewOption, "ialaw"))) {
		preview_struct.update = ISP_ABS_PREV_INVALAW;

	} else if ((!strcasecmp(previewOption, "hm"))) {
		preview_struct.update = ISP_ABS_PREV_HRZ_MED;

	} else if ((!strcasecmp(previewOption, "cfa"))) {
		preview_struct.update = ISP_ABS_PREV_CFA;

	} else if ((!strcasecmp(previewOption, "cs"))) {
		preview_struct.update = ISP_ABS_PREV_CHROMA_SUPP;

	} else if ((!strcasecmp(previewOption, "wb"))) {
		preview_struct.update = ISP_ABS_PREV_WB;
		/* NOTE: Added default table */
	} else if ((!strcasecmp(previewOption, "ba"))) {
		preview_struct.update = ISP_ABS_PREV_BLKADJ;
		preview_struct.flag = ISP_ABS_PREV_BLKADJ;
		/* NOTE: Table is same as kernels default */
	} else if ((!strcasecmp(previewOption, "r"))) {
		preview_struct.update = ISP_ABS_PREV_RGB2RGB;
		preview_struct.flag = ISP_ABS_PREV_RGB2RGB;
		/* NOTE: Table is same as kernels default */
	} else if ((!strcasecmp(previewOption, "cc"))) {
		preview_struct.update = ISP_ABS_PREV_COLOR_CONV;
		preview_struct.flag = ISP_ABS_PREV_COLOR_CONV;
		/* NOTE: Added default table */
	} else if ((!strcasecmp(previewOption, "yc"))) {
		preview_struct.update = ISP_ABS_PREV_YC_LIMIT;
		preview_struct.flag = ISP_ABS_PREV_YC_LIMIT;
		/* NOTE: Table is same as kernels default */
	} else if ((!strcasecmp(previewOption, "dc"))) {
		preview_struct.update = ISP_ABS_PREV_DEFECT_COR;

	} else if ((!strcasecmp(previewOption, "gb"))) {

	} else if ((!strcasecmp(previewOption, "nf"))) {
		preview_struct.update = ISP_ABS_TBL_NF;

	} else if ((!strcasecmp(previewOption, "rg"))) {
		preview_struct.update = ISP_ABS_TBL_REDGAMMA;
		preview_struct.flag = ISP_ABS_TBL_GREENGAMMA;
		/* NOTE: Table is same as kernels default */
	} else if ((!strcasecmp(previewOption, "gg"))) {
		preview_struct.update = ISP_ABS_TBL_GREENGAMMA;
		preview_struct.flag = ISP_ABS_TBL_GREENGAMMA;
		/* NOTE: Table is same as kernels default */
	} else if ((!strcasecmp(previewOption, "bg"))) {
		preview_struct.update = ISP_ABS_TBL_BLUEGAMMA;
		preview_struct.flag = ISP_ABS_TBL_BLUEGAMMA;
		/* NOTE: Table is same as kernels default */
	} else if ((!strcasecmp(previewOption, "none"))) {
		return 1;
	} else {
		printf("Wrong option %s, please enter correct"
				" option\n", previewOption);
		return 0;
	}


	ret = ioctl(fd, VIDIOC_PRIVATE_ISP_PRV_CFG, &preview_struct);

	if (ret == -1) {
		printf("\nerror\n");
		return 0;
	} else
		printf("abs Layer VIDIOC_S_CTRL successful\n");

	return 1;
}
