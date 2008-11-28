/*
 * drivers/media/video/omap/isp/omap_previewer.h
 *
 * Include file for Preview module wrapper in TI's OMAP3430 ISP
 *
 * Copyright (C) 2007 Texas Instruments, Inc.
 *
 * This package is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * THIS PACKAGE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */

#ifndef OMAP_ISP_PREVIEW_WRAP_H
#define OMAP_ISP_PREVIEW_WRAP_H

#ifndef u32
#define u32 unsigned long
#endif /* u32 */

#ifndef u16
#define u16 unsigned short
#endif /* u16 */

#ifndef u8
#define u8 unsigned char
#endif /* u8 */

#ifndef s16
#define s16 signed short
#endif

#define PREV_IOC_BASE   	'P'
#define PREV_REQBUF     	_IOWR(PREV_IOC_BASE, 1, struct v4l2_requestbuffers)
#define PREV_QUERYBUF   	_IOWR(PREV_IOC_BASE, 2, struct v4l2_buffer)
#define PREV_SET_PARAM  	_IOW(PREV_IOC_BASE, 3, struct prev_params)
#define PREV_GET_PARAM  	_IOWR(PREV_IOC_BASE, 4, struct prev_params)
#define PREV_PREVIEW    	_IOR(PREV_IOC_BASE, 5, int)
#define PREV_GET_STATUS 	_IOR(PREV_IOC_BASE, 6, char)
#define PREV_GET_CROPSIZE 	_IOR(PREV_IOC_BASE, 7, struct prev_cropsize)
#define PREV_QUEUEBUF     	_IOWR(PREV_IOC_BASE, 8, struct v4l2_buffer)
#define PREV_FREEBUF     	_IOWR(PREV_IOC_BASE, 9, struct v4l2_buffer)
#define PREV_IOC_MAXNR  9


/* Feature lists */
#define PREV_INPUT_FORMATTER       0x1
#define PREV_INVERSE_ALAW          0x2
#define PREV_HORZ_MEDIAN_FILTER    0x4
#define PREV_NOISE_FILTER          0x8
#define PREV_CFA                   0x10
#define PREV_GAMMA                 0x20
#define PREV_LUMA_ENHANCE          0x40
#define PREV_CHROMA_SUPPRESS       0x80
#define PREV_DARK_FRAME_SUBTRACT   0x100
#define PREV_LENS_SHADING          0x200
#define PREV_DARK_FRAME_CAPTURE    0x400
#define PREV_DEFECT_COR  		(1 << 11)
/* -- */

#define NO_AVE   	0x0
#define AVE_2_PIX	0x1
#define AVE_4_PIX	0x2
#define AVE_8_PIX	0x3

#define LUMA_TABLE_SIZE            128
#define GAMMA_TABLE_SIZE           1024
#define CFA_COEFF_TABLE_SIZE       576
#define NOISE_FILTER_TABLE_SIZE    256

#define WB_GAIN_MAX     4
#define RGB_MAX         3

#define MAX_IMAGE_WIDTH   3300

#define PREV_INWIDTH_8BIT   0	/* pixel width of 8 bitS */
#define PREV_INWIDTH_10BIT  1	/* pixel width of 10 bits */

/* list of structures */

/*
 * Structure for size parameters
 */ 
struct prev_size_params {
	unsigned int hstart;	/* Starting pixel */
	unsigned int vstart;	/* Starting line */
	unsigned int hsize;	/* width of input image */
	unsigned int vsize;	/* height of input image */
	unsigned char pixsize;	/* pixel size of the image in 
				   terms of bits */
	unsigned short in_pitch;	/* line offset of input image */
	unsigned short out_pitch;	/* line offset of output image */
};
/*
 * Structure for White Balance
 */
struct ispprev_wbal {
	/*Digital gain (U10Q8) */
	u16 dgain;
	/*White balance gain - COEF 3 (U8Q5) */
	u8 coef3;
	/*White balance gain - COEF 2 (U8Q5) */
	u8 coef2;
	/*White balance gain - COEF 1 (U8Q5) */
	u8 coef1;
	/*White balance gain - COEF 0 (U8Q5) */
	u8 coef0;
};
/* structure for white balancing parameters */
struct prev_white_balance {
	u16 wb_dgain;	/* white balance common gain */
	u8 wb_gain[WB_GAIN_MAX];	/* individual color gains */
	u8 wb_coefmatrix[WB_GAIN_MAX][WB_GAIN_MAX];	/* 16 position
								   out of 4 
								   values */
};
/*
 * Structure for RGB to RGB Blending
 */
struct ispprev_rgbtorgb {
	/*
	 * Blending values(S12Q8 format)
	 *	[RR] [GR] [BR]
	 *	[RG] [GG] [BG]
	 *	[RB] [GB] [BB]
	 */
	u16 matrix[3][3];
	/*Blending offset value for R,G,B in 2's complement integer format*/
	u16 offset[3];
};

struct ispprev_hmed {
	/* Distance between consecutive pixels of same color in the odd line*/
	u8 odddist;
	/*Distance between consecutive pixels of same color in the even line*/
	u8 evendist;
	/* Horizontal median filter threshold */
	u8 thres;
};
/*
 * Structure for Noise Filter
 */
struct ispprev_nf {
	/* Flag to enable or disable the Defect Correction in NF*/
	u8 defect_corr_en;
	/* Strength to be used in Noise Filter*/
	u8 strgth;
	/* Spread value to be used in Noise Filter*/
	u8 spread;
	/*Pointer to the Noise Filter table */
	u32 *table;
};

/*
 * Structure for Defect correction
 */
struct ispprev_dcor {
	/* Flag to enable or disable the couplet dc Correction in NF*/
	u8 couplet_mode_en;
	/* Thresholds for correction bit 0:10 detect 16:25 correct*/
	u32 detect_correct[4];
};

/*
 * Enumeration for CFA Formats supported by preview
 */
enum cfa_fmt {
	CFAFMT_BAYER, CFAFMT_SONYVGA, CFAFMT_RGBFOVEON,
	CFAFMT_DNSPL, CFAFMT_HONEYCOMB, CFAFMT_RRGGBBFOVEON
};
/*
 * Structure for CFA Inpterpolation
 */
struct ispprev_cfa {
	/* CFA Format Enum value supported by preview.*/
	enum cfa_fmt cfafmt;
	/* CFA Gradient Threshold - Vertical */
	u8 cfa_gradthrs_vert;
	/* CFA Gradient Threshold - Horizontal */
	u8 cfa_gradthrs_horz;
	/* Pointer to the CFA table */
	u32 *cfa_table;
};
/*
 * Structure for Gamma Correction
 */
struct ispprev_gtable {
	/* Pointer to the red gamma table */
	u32 *redtable;
	/* Pointer to the green gamma table */
	u32 *greentable;
	/* Pointer to the blue gamma table */
	u32 *bluetable;
};
/*
 * Structure for Chrominance Suppression
 */
struct ispprev_csup {
	/* Gain */
	u8 gain;
	/* Threshold */
	u8 thres;
	/* Flag to enable/disable the High Pass Filter */
	u8 hypf_en;
};
/*
 * Structure for Black Adjustment
 */
struct ispprev_blkadj {
	/*Black level offset adjustment for Red in 2's complement format */
	u8 red;
	/*Black level offset adjustment for Green in 2's complement format */
	u8 green;
	/* Black level offset adjustment for Blue in 2's complement format */
	u8 blue;
};
/*
 * Structure for Color Space Conversion from RGB-YCbYCr
 */
struct ispprev_csc {
	/*
	 *Color space conversion coefficients(S10Q8)
	 *	[CSCRY]    [CSCGY]   [CSCBY]
	 *	[CSCRCB] [CSCGCB] [CSCBCB]
	 *	[CSCRCR] [CSCGCR] [CSCBCR]
	 */
	u16 matrix[RGB_MAX][RGB_MAX];
	/*
	 *CSC offset values for Y offset, CB offset and CR offset respectively
	 */
	s16 offset[RGB_MAX];
};
/* Structure for Dark frame suppression */
struct prev_darkfrm_params {
	u32 addr;	/* memory start address */
	u32 offset;	/* line offset */
};

/*structure for Chroma Suppression */
struct prev_chroma_spr {
	unsigned char hpfy;	/* whether to use high passed 
				   version of Y or normal Y */
	char threshold;		/* threshold for chroma suppress */
	unsigned char gain;	/* chroma suppression gain */
};

/*
 * Configure byte layout of YUV image
 */
enum preview_ycpos_mode {
	YCPOS_YCrYCb = 0,
	YCPOS_YCbYCr = 1,
	YCPOS_CbYCrY = 2,
	YCPOS_CrYCbY = 3
};
/* -- */
/* structure for all configuration */
struct prev_params {
	u16 features;	/* Set of features enabled */
	enum preview_ycpos_mode pix_fmt; /* output pixel format */
	struct ispprev_cfa cfa; /* CFA coefficients */
	struct ispprev_csup csup;  /* chroma suppression coefficients */
	u32 *ytable;	/* luma enhancement coeffs */
	struct ispprev_nf nf; /* noise filter coefficients */
	struct ispprev_dcor dcor; /* noise filter coefficients */
	struct ispprev_gtable gtable;	/* gamma coefficients */
	struct ispprev_wbal wbal;
//	struct prev_white_balance prev_wbal;
	struct ispprev_blkadj blk_adj;	/* black adjustment parameters */
	struct ispprev_rgbtorgb rgb2rgb;  /* rgb blending parameters */
	struct ispprev_csc rgb2ycbcr;  /* rgb to ycbcr parameters */
	struct ispprev_hmed hmf_params;	/* horizontal median filter */
	struct prev_size_params size_params;	/* size parameters */
	struct prev_darkfrm_params drkf_params;
	u8 lens_shading_shift;
	u8 average;	/* down sampling rate for averager */
	u8 contrast;		/* contrast */
	u8 brightness;		/* brightness */
};

/* structure to know status of the hardware */
struct prev_status {
	char hw_busy;
};
/* structure to knwo crop size */
struct prev_cropsize {
	int hcrop;
	int vcrop;
};

#endif
