/*
 * drivers/media/video/omap/isp/isp_af.h
 *
 * Include file for H3A module in TI's OMAP3430 Camera ISP
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

/* Device Constants */
#ifndef OMAP_ISP_AF_H
#define OMAP_ISP_AF_H

#define AF_MAJOR_NUMBER                 0
#define ISPAF_NAME                     "OMAPISP_AF"
#define AF_NR_DEVS                      1
#define AF_TIMEOUT                      ((300 * HZ) / 1000)


/* Range Constants */
#define AF_IIRSH_MIN                        0
#define AF_IIRSH_MAX                        4094
#define AF_PAXEL_HORIZONTAL_COUNT_MIN       0
#define AF_PAXEL_HORIZONTAL_COUNT_MAX       35
#define AF_PAXEL_VERTICAL_COUNT_MIN         0
#define AF_PAXEL_VERTICAL_COUNT_MAX         127
#define AF_PAXEL_INCREMENT_MIN              0
#define AF_PAXEL_INCREMENT_MAX              14
#define AF_PAXEL_HEIGHT_MIN                 0
#define AF_PAXEL_HEIGHT_MAX                 127
#define AF_PAXEL_WIDTH_MIN                  0
#define AF_PAXEL_WIDTH_MAX                  127
#define AF_PAXEL_HZSTART_MIN                2
#define AF_PAXEL_HZSTART_MAX                4094

#define AF_PAXEL_VTSTART_MIN                0
#define AF_PAXEL_VTSTART_MAX                4095
#define AF_THRESHOLD_MAX                    255
#define AF_COEF_MAX                         4095
#define AF_PAXEL_SIZE                       48

/* Print Macros */
/*list of error code */
#define AF_ERR_HZ_COUNT         800	/* Invalid Horizontal Count */
#define AF_ERR_VT_COUNT         801	/* Invalid Vertical Count */
#define AF_ERR_HEIGHT           802	/* Invalid Height */
#define AF_ERR_WIDTH            803	/* Invalid width */
#define AF_ERR_INCR             804	/* Invalid Increment */
#define AF_ERR_HZ_START         805	/* Invalid horizontal Start */
#define AF_ERR_VT_START         806	/* Invalud vertical Start */
#define AF_ERR_IIRSH            807	/* Invalid IIRSH value */
#define AF_ERR_IIR_COEF         808	/* Invalid Coefficient */
#define AF_ERR_SETUP            809	/* Setup not done */
#define AF_ERR_THRESHOLD        810	/* Invalid Threshold */
#define AF_ERR_ENGINE_BUSY      811	/* Engine is busy */
#define  AF_NUMBER_OF_COEF      11
/* list of ioctls */
#pragma pack(1)

#define  AF_MAGIC_NO        'a'
#define  AF_S_PARAM         _IOWR(AF_MAGIC_NO, 1, struct af_configuration *)
#define  AF_G_PARAM         _IOWR(AF_MAGIC_NO, 2, struct af_configuration *)
#define  AF_ENABLE          _IO(AF_MAGIC_NO, 3)
#define  AF_DISABLE         _IO(AF_MAGIC_NO, 4)
#define		AF_IOC_BASE			       'R'
#define		AF_IOC_MAXNR				2

/*Ioctl options which are to be passed while calling the ioctl*/
#define	AF_SET_POSITION		_IOWR(AF_IOC_BASE, 1, int)
#define	AF_GET_POSITION		_IOWR(AF_IOC_BASE, 2, int)
#pragma  pack()
#define AFPID               0x0	/*Peripheral Revision */
					/*and Class Information */

#define AFCOEF_OFFSET       0x00000004	/* COEFFICIENT BASE ADDRESS */

/* Register Fields */
/*
 *    PID fields
 */
/*#define AF_TID                          (0xFF<<16)
#define AF_CID                          (0xFF<<8)
#define AF_PREV                         0xFF*/

/*
 *    PCR fields
 */
#define AF_BUSYAF                       (1 << 15)
#define FVMODE                          (1 << 14)
#define RGBPOS                          (0x7 << 11)
#define MED_TH                          (0xFF << 3)
#define AF_MED_EN                       (1 << 2)
#define AF_ALAW_EN                      (1 << 1)
#define AF_EN                           (1 << 0)

/*
 * AFPAX1 fields
 */
#define PAXW                            (0x7F << 16)
#define PAXH                            0x7F

/*
 * AFPAX2 fields
 */
#define  AFINCV                         (0xF << 13)
#define  PAXVC                          (0x7F << 6)
#define  PAXHC                          0x3F

/*
 * AFPAXSTART fields
 */
#define  PAXSH                          (0xFFF << 16)
#define  PAXSV                          0xFFF

/*
 * COEFFICIENT MASK
 */

#define COEF_MASK0                      0xFFF
#define COEF_MASK1                      (0xFFF << 16)

/* CCDC BITS */
#define AF_VPEN_MASK                    0x8000
#define AF_FMTCG_VPEN                   15

/* BIT SHIFTS */
#define AF_RGBPOS_SHIFT                 11
#define AF_MED_TH_SHIFT                 3
#define AF_PAXW_SHIFT                   16
#define AF_LINE_INCR_SHIFT              13
#define AF_VT_COUNT_SHIFT               6
#define AF_HZ_START_SHIFT               16
#define AF_COEF_SHIFT                   16

/* Flags for update field */
#define REQUEST_STATISTICS      (1 << 0)
#define LENS_DESIRED_POSITION   (1 << 1)

#ifndef u32
#define u32 unsigned long
#endif /* u32 */

#ifndef u16
#define u16 unsigned short
#endif /* u16 */

#ifndef u8
#define u8 unsigned char
#endif /* u8 */

struct isp_af_xtrastats_fields {
	struct timeval ts;
	unsigned long field_count;
	u16 lens_position;
};

struct isp_af_data {
	void *af_statistics_buf;	/* Pointer to pass to user */
	u16 lens_current_position;	/* Read value of lens absolute
					 * position
					 */
	u16 desired_lens_direction;	/* Lens desired location */
	u16 update;			/* Bitwise flags to update
					 * parameters */
	u16 frame_number;		/* data for which frame is
					 * desired/given
					 */
	u16 curr_frame;
	struct isp_af_xtrastats_fields xtrastats;
};

/* enum used for status of specific feature */
typedef enum {
	H3A_AF_DISABLE = 0,
	H3A_AF_ENABLE = 1
} af_alaw_enable, af_hmf_enable, af_config_flag;

typedef enum {
	ACCUMULATOR_SUMMED = 0,
	ACCUMULATOR_PEAK = 1
} af_mode;

/* Red, Green, and blue pixel location in the AF windows */
typedef enum {
	GR_GB_BAYER = 0,	/* GR and GB as Bayer pattern */
	RG_GB_BAYER = 1,	/* RG and GB as Bayer pattern */
	GR_BG_BAYER = 2,	/* GR and BG as Bayer pattern */
	RG_BG_BAYER = 3,	/* RG and BG as Bayer pattern */
	GG_RB_CUSTOM = 4,	/* GG and RB as custom pattern */
	RB_GG_CUSTOM = 5	/* RB and GG as custom pattern */
} rgbpos;

/* Contains the information regarding the Horizontal Median Filter */
struct af_hmf {
	af_hmf_enable enable;	/* Status of Horizontal Median Filter */
	unsigned int threshold;	/* Threshhold Value for */
	/*Horizontal Median Filter */
};

/* Contains the information regarding the IIR Filters */
struct af_iir {
	unsigned int hz_start_pos;	/* IIR Start Register Value */
	int coeff_set0[AF_NUMBER_OF_COEF];/* IIR Filter Coefficient for Set 0 */
	int coeff_set1[AF_NUMBER_OF_COEF];/* IIR Filter Coefficient for Set 1 */
};

/* Contains the information regarding the Paxels Structure in AF Engine */
struct af_paxel {
	unsigned int width;	/* Width of the Paxel */
	unsigned int height;	/* Height of the Paxel */
	unsigned int hz_start;	/* Horizontal Start Position */
	unsigned int vt_start;	/* Vertical Start Position */
	unsigned int hz_cnt;	/* Horizontal Count */
	unsigned int vt_cnt;	/* vertical Count */
	unsigned int line_incr;	/* Line Increment */
};

/*Contains the parameters required for hardware set up of AF Engine */
struct af_configuration {
	af_alaw_enable alaw_enable;	/*ALWAW status */
	struct af_hmf hmf_config;	/*HMF configurations */
	rgbpos rgb_pos;		/*RGB Positions */
	struct af_iir iir_config;	/*IIR filter configurations */
	struct af_paxel paxel_config;	/*Paxel parameters */
	af_mode mode;		/*Accumulator mode */
	af_config_flag af_config;	/*Flag indicates Engine is configured */
};

/* Structure for device of AF Engine */
struct af_device {
	struct af_configuration *config;	/*Device configuration struct*/
	int size_paxel;				/*Paxel size in bytes */
};

int isp_af_check_paxel(void);
int isp_af_check_iir(void);
int isp_af_register_setup(struct af_device *);
int isp_af_enable(int);
void isp_af_set_address(unsigned long);

#endif   /* OMAP_ISP_AF_H */
