/*
 * drivers/media/video/omap/isp/omap_resizer.h
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

#ifndef OMAP_ISP_RESIZER_WRAP_H
#define OMAP_ISP_RESIZER_WRAP_H

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

#define		RSZ_IOC_BASE			       'R'
#define		RSZ_IOC_MAXNR				8


#define	RSZ_REQBUF		_IOWR(RSZ_IOC_BASE, 1, struct v4l2_requestbuffers)
#define	RSZ_QUERYBUF		_IOWR(RSZ_IOC_BASE, 2, struct v4l2_buffer)
#define	RSZ_S_PARAM		_IOWR(RSZ_IOC_BASE, 3, struct rsz_params)
#define	RSZ_G_PARAM		_IOWR(RSZ_IOC_BASE, 4, struct rsz_params)
#define	RSZ_RESIZE		_IOWR(RSZ_IOC_BASE, 5, int)
#define	RSZ_G_STATUS		_IOWR(RSZ_IOC_BASE, 6, struct rsz_status)
#define RSZ_QUEUEBUF     	_IOWR(RSZ_IOC_BASE, 7, struct v4l2_buffer)
#define	RSZ_GET_CROPSIZE	_IOWR(RSZ_IOC_BASE, 8, struct rsz_cropsize)

#define	RSZ_BUF_IN						0
#define	RSZ_BUF_OUT						1

#define RSZ_INTYPE_YCBCR422_16BIT               0
#define RSZ_INTYPE_PLANAR_8BIT                  1
#define	RSZ_PIX_FMT_UYVY			1	/*    cb:y:cr:y */
#define	RSZ_PIX_FMT_YUYV			0	/*    y:cb:y:cr */

/*Structure Definitions*/
enum config_done {
	STATE_CONFIGURED,	/* Resizer driver configured by application */
	STATE_NOT_CONFIGURED	/* Resizer driver not configured by
				   application */
};

/*Structure Definitions*/
/* To allocate the memory*/
struct rsz_reqbufs {
	int buf_type;		/* type of frame buffer */
	int size;		/* size of the frame bufferto be allocated */
	int count;		/* number of frame buffer to be allocated */
};

/* assed for quering the buffer to get physical address*/
struct rsz_buffer {
	int index;		/* buffer index number, 0 -> N-1 */
	int buf_type;		/* buffer type, input or output */
	unsigned long offset;	/* physical     address of the buffer,
				   used in the mmap() system call */
	int size;
};

/* resize structure passed during the resize IOCTL*/
struct rsz_resize {
	struct rsz_buffer in_buf;
	struct rsz_buffer out_buf;
};

/* Contains the status of hardware and channel*/
struct rsz_status {
	int chan_busy;		/* 1: channel is busy, 0: channel is not busy */
	int hw_busy;		/*1: hardware  is busy, 0:
				   hardware is not     busy */
	int src;		/* # defined, can be either
				   SD-RAM or CCDC/PREVIEWER */
};

/* used	to luma	enhancement options*/

struct rsz_yenh {
	int type;		/* represents luma enable or disable */
	unsigned char gain;	/*represents gain */
	unsigned char slop;	/*represents slop */
	unsigned char core;	/* Represents core value */
};

/* Conatins	all	the	parameters for resizing	. This structure
	is used	to configure resiser parameters*/
struct rsz_params {
	int in_hsize;		/* input frame horizontal size */
	int in_vsize;		/* input frame vertical size */
	int in_pitch;		/* offset between two rows of input frame */
	int inptyp;		/* for determining 16 bit or 8 bit data */
	int vert_starting_pixel;	/* for specifying vertical
					   starting pixel in input */
	int horz_starting_pixel;	/* for specyfing horizontal
					   starting pixel in input */
	int cbilin;		/* # defined, filter with luma or bi-linear
				   interpolation */
	int pix_fmt;		/* # defined, UYVY or YUYV */
	int out_hsize;		/* output frame horizontal size */
	int out_vsize;		/* output frame vertical size */
	int out_pitch;		/* offset between two rows of output frame */
	int hstph;		/* for specifying horizontal starting phase */
	int vstph;		/* for specifying vertical starting phase */
	u16 tap4filt_coeffs[32];	/* horizontal filter coefficients */
	u16 tap7filt_coeffs[32];	/* vertical filter coefficients */
	struct rsz_yenh yenh_params;
};


#endif
