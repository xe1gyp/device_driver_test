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

#define PREV_IOC_BASE   	'P'
#define PREV_REQBUF     	_IOWR(PREV_IOC_BASE, 1, \
					struct v4l2_requestbuffers)
#define PREV_QUERYBUF   	_IOWR(PREV_IOC_BASE, 2, struct v4l2_buffer)
#define PREV_SET_PARAM  	_IOW(PREV_IOC_BASE, 3, struct prev_params)
#define PREV_GET_PARAM  	_IOWR(PREV_IOC_BASE, 4, struct prev_params)
#define PREV_PREVIEW    	_IOR(PREV_IOC_BASE, 5, int)
#define PREV_GET_STATUS 	_IOR(PREV_IOC_BASE, 6, char)
#define PREV_GET_CROPSIZE 	_IOR(PREV_IOC_BASE, 7, struct prev_cropsize)
#define PREV_QUEUEBUF     	_IOWR(PREV_IOC_BASE, 8, struct v4l2_buffer)
#define PREV_FREEBUF     	_IOWR(PREV_IOC_BASE, 9, struct v4l2_buffer)
#define PREV_IOC_MAXNR		9


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

/**
 * struct prev_white_balance - Structure for White Balance 2.
 * @wb_dgain: White balance common gain.
 * @wb_gain: Individual color gains.
 * @wb_coefmatrix: Coefficient matrix
 */
struct prev_white_balance {
	__u16 wb_dgain;
	__u8 wb_gain[WB_GAIN_MAX];
	__u8 wb_coefmatrix[WB_GAIN_MAX][WB_GAIN_MAX];
};

/* Structure for Chroma Suppression */
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
/**
 * struct ispprev_gtable - Structure for Gamma Correction.
 * @redtable: Pointer to the red gamma table.
 * @greentable: Pointer to the green gamma table.
 * @bluetable: Pointer to the blue gamma table.
 */
struct ispprev_gtable {
	__u32 *redtable;
	__u32 *greentable;
	__u32 *bluetable;
};

/**
 * struct prev_darkfrm_params - Structure for Dark frame suppression.
 * @addr: Memory start address.
 * @offset: Line offset.
 */
struct prev_darkfrm_params {
	__u32 addr;
	__u32 offset;
};

/* -- */
/* structure for all configuration */
struct prev_params {
	__u16 features;	/* Set of features enabled */

	enum preview_ycpos_mode pix_fmt; /* output pixel format */

	struct ispprev_cfa cfa; /* CFA coefficients */

	struct ispprev_csup csup;  /* chroma suppression coefficients */

	__u32 *ytable;	/* luma enhancement coeffs */

	struct ispprev_nf nf; /* noise filter coefficients */

	struct ispprev_dcor dcor; /* noise filter coefficients */

	struct ispprev_gtable gtable;	/* gamma coefficients */

	struct ispprev_wbal wbal;

	struct ispprev_blkadj blk_adj;	/* black adjustment parameters */

	struct ispprev_rgbtorgb rgb2rgb;  /* rgb blending parameters */

	struct ispprev_csc rgb2ycbcr;  /* rgb to ycbcr parameters */

	struct ispprev_hmed hmf_params;	/* horizontal median filter */

	struct prev_size_params size_params;	/* size parameters */
	struct prev_darkfrm_params drkf_params;
	__u8 lens_shading_shift;
	__u8 average;	/* down sampling rate for averager */

	__u8 contrast;		/* contrast */
	__u8 brightness;		/* brightness */
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
