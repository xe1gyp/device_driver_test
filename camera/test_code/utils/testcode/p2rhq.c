/*
 * ISP Resizer device test
 *
 * Author:
 * 	Atanas Filipov <afilipov@mm-sol.com>
 *
 * This package is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * THIS PACKAGE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <assert.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/mman.h>

#include "types.h"
#include <mach/isp_user.h>

#include <linux/types.h>
#include <linux/videodev2.h>
#include <linux/omap_prev2resz.h>

/* Project includes */
#include "types.h"
#include "cmd_options.h"
#include "file_operations.h"

/*
 * device symbolic name
 */
#define DEV_SYMBOLIC "/dev/omap-prev2resz"

struct options cmd_line[] = {
	{ "-iw"    , e_WORD, (union default_u) 3280,
	" Image width (default = 3280)" },
	{ "-ih"    , e_WORD, (union default_u) 2464,
	" Image height (default = 2464)" },
	{ "-icl"   , e_WORD, (union default_u) 0,
	" Input image crop left" },
	{ "-icw"   , e_WORD, (union default_u) 0,
	" Input image crop width" },
	{ "-ict"   , e_WORD, (union default_u) 0,
	" Image image crop height" },
	{ "-ich"   , e_WORD, (union default_u) 0,
	" Image image crop top" },
	{ "-ow"    , e_WORD, (union default_u) 1280,
	" Output image width (default = 1280)" },
	{ "-oh"    , e_WORD, (union default_u) 960,
	" Output image height (default = 960)" },
	{ "-ifile" , e_CHAR, (union default_u) NULL,
	" Input image file name" },
	{ "-ofile" , e_CHAR, (union default_u) NULL,
	" Output image file name" },
	{ "-dev"	, e_CHAR, (union default_u) "/dev/omap-prev2resz",
	" Device (default = /dev/omap-prev2resz)" },
};

static __u32 cfa_coef_table[] = {
	#include "cfa_coef_table.h"
};


/* Default values in Office Flourescent Light for RGBtoRGB Blending */
static struct ispprev_rgbtorgb flr_rgb2rgb = {
	{	/* RGB-RGB Matrix */
		{ 0x01E2, 0x0F30, 0x0FEE },
		{ 0x0F9B, 0x01AC, 0x0FB9 },
		{ 0x0FE0, 0x0EC0, 0x0260 }
	},	/* RGB Offset */
		{0x0000, 0x0000, 0x0000}
};

static struct ispprev_csc flr_prev_csc_normal =	{
	{/* CSC Coef Matrix */
		{66, 129, 25},
		{-38, -75, 112},
		{112, -94, -18}
	},/* CSC Offset */
	{0x0, 0x0, 0x0}
};

/*
 * ISP memory page size
 */
#define PAGE_SIZE 4096
#define USED_BUFF 2
/*
 * Structure for V4L2 buffers
 */
struct user_buf {
	char			*maddr;	/* preserve malloc address */
	struct v4l2_buffer	v4lbuf;
} user_buf;

int main(int argc, char *argv[])
{
	unsigned i;

	int in_hsize, in_vsize;
	int out_hsize, out_vsize;

	/* user parameters */
	static struct isp_node upipe;

	struct ispprv_update_config ispprv_update;

	/* command line argument index */
	int opt_idx;
	int rz_fd;

	if (argc == 1 || (argc == 2 && (!strcmp(argv[1], "?") ||
		!strcmp(argv[1], "-h") || !strcmp(argv[1], "--help")))) {
		fprintf(stderr, "\nUSAGE: command <options> \n");
		for (opt_idx = 0; opt_idx < COUNT_OF(cmd_line); opt_idx++) {
			printf("\n \t %s %s ", cmd_line[opt_idx].o_symb, \
						cmd_line[opt_idx].o_help);
			switch (cmd_line[opt_idx].o_type) {
			case e_BYTE:
				printf("[Default = %d]\n", \
					cmd_line[opt_idx].o_dflt.v_byte);
				break;
			case e_WORD:
				printf("[Default = %d]\n", \
					cmd_line[opt_idx].o_dflt.v_word);
				break;
			case e_LONG:
				printf("[Default = %d]\n", \
					cmd_line[opt_idx].o_dflt.v_long);
				break;
			case e_CHAR:
				printf("[Default = %s]\n", \
					cmd_line[opt_idx].o_dflt.v_char);
				break;
			}
		}

		/* Print additional help */
		printf("\nExamples:\n");

		printf("\nResize image: \n ");
		printf("p2rhq -iw [in width] -ih [in height] "\
			"-ifile [input file] "\
			"-ow [out width] -oh [out height] "\
			"-ofile [output file]\n");

		return -1;
	}


	if (parse_prepare(argc, argv, cmd_line, COUNT_OF(cmd_line)))
		return -1;

	/* Read input image parameters */
	opt_idx = get_option_index("-iw", cmd_line, COUNT_OF(cmd_line));
	in_hsize = cmd_line[opt_idx].o_dflt.v_word;
	printf(" optindex %d hsize %d \n", opt_idx, in_hsize);

	opt_idx = get_option_index("-ih", cmd_line, COUNT_OF(cmd_line));
	in_vsize = cmd_line[opt_idx].o_dflt.v_word;
	printf(" optindex %d vsize %d \n", opt_idx, in_vsize);

	if (in_hsize <= 0 || in_vsize <= 0) {
		error(-1, ECANCELED, "Input size out of range.");
		return -1;
	}

	/* Read output image parameters */
	opt_idx = get_option_index("-ow", cmd_line, COUNT_OF(cmd_line));
	out_hsize = cmd_line[opt_idx].o_dflt.v_word;

	opt_idx = get_option_index("-oh", cmd_line, COUNT_OF(cmd_line));
	out_vsize = cmd_line[opt_idx].o_dflt.v_word;

	if (in_hsize <= 0 || in_vsize <= 0) {
		error(-1, ECANCELED, "Output size out of range.");
		return -1;
	}


	/* index of device name */
	opt_idx = get_option_index("-dev", cmd_line, COUNT_OF(cmd_line));
	rz_fd = open(cmd_line[opt_idx].o_dflt.v_char, O_RDWR);

	if (rz_fd < 0) {
		error(-1, ECANCELED, "Can't open device.");
		return -1;
	}

	/* user parameters */
	struct isp_node user_pipe;
	/* clear parameters */
	memset(&user_pipe, 0, sizeof(user_pipe));
	/* determining 16 bit data */
	user_pipe.in.image.pixelformat	= V4L2_PIX_FMT_SGRBG10;
	/* input frame horizontal/vertical size */
	user_pipe.in.image.width	= in_hsize;
	user_pipe.in.image.height	= in_vsize;
	/* input crop horizontal/vertical size */
	user_pipe.in.crop.width		= user_pipe.in.image.width;
	user_pipe.in.crop.height	= user_pipe.in.image.height;
	/* determining 16 bit data */
	user_pipe.out.image.pixelformat	= V4L2_PIX_FMT_UYVY;
	/* output frame horizontal/verticals size */
	user_pipe.out.image.width	= out_hsize;
	user_pipe.out.image.height	= out_vsize;
	/* output frame horizontal/verticals crop */
	user_pipe.out.crop.width	= user_pipe.out.image.width;
	user_pipe.out.crop.height	= user_pipe.out.image.height;

	fprintf(stderr, "\nTRY: IW:%4d IH:%4d icL:%4d icW:%4d icT:%4d "
	      "icH:%4d OW:%4d OH:%4d ocL:%4d ocW:%4d ocT:%4d ocH:%4d\n",
		user_pipe.in.image.width, user_pipe.in.image.height,
		user_pipe.in.crop.left, user_pipe.in.crop.width,
		user_pipe.in.crop.top, user_pipe.in.crop.height,
		user_pipe.out.image.width, user_pipe.out.image.height,
		user_pipe.out.crop.left, user_pipe.out.crop.width,
		user_pipe.out.crop.top, user_pipe.out.crop.height);

	if (ioctl(rz_fd, PREV2RESZ_SET_CONFIG, &user_pipe) == -1) {
		fprintf(stderr, "\nInvalid config parameters");
		return -1;
	}
	/* Get updated parameters */
	if (ioctl(rz_fd, PREV2RESZ_GET_CONFIG, &user_pipe) == -1) {
		fprintf(stderr, "\nDevice not configured!");
		return -1;
	}

	fprintf(stderr, "\nSET: IW:%4d IH:%4d icL:%4d icW:%4d icT:%4d "
	"icH:%4d OW:%4d OH:%4d ocL:%4d ocW:%4d ocT:%4d ocH:%4d\n",
		user_pipe.in.image.width, user_pipe.in.image.height,
		user_pipe.in.crop.left, user_pipe.in.crop.width,
		user_pipe.in.crop.top, user_pipe.in.crop.height,
		user_pipe.out.image.width, user_pipe.out.image.height,
		user_pipe.out.crop.left, user_pipe.out.crop.width,
		user_pipe.out.crop.top, user_pipe.out.crop.height);



	ispprv_update.prev_blkadj = (struct ispprev_blkadj *) \
				malloc(sizeof(struct ispprev_blkadj));
	ispprv_update.rgb2rgb = (struct ispprev_rgbtorgb *) \
				malloc(sizeof(struct ispprev_rgbtorgb));
	ispprv_update.prev_csc = (struct ispprev_csc *) \
				malloc(sizeof(struct ispprev_csc));
	ispprv_update.prev_wbal = (struct ispprev_wbal *) \
				malloc(sizeof(struct ispprev_wbal));
	ispprv_update.prev_cfa = (struct ispprev_cfa *) \
				malloc(sizeof(struct ispprev_cfa));
	ispprv_update.prev_nf = (struct ispprev_nf *) \
				malloc(sizeof(struct ispprev_nf));

	ispprv_update.flag = ISP_ABS_TBL_NF | ISP_ABS_PREV_CFA |
			ISP_ABS_PREV_WB	| ISP_ABS_PREV_BLKADJ;

	ispprv_update.update = ispprv_update.flag;

	ispprv_update.prev_blkadj->red = 0;
	ispprv_update.prev_blkadj->green = 0;
	ispprv_update.prev_blkadj->blue = 0;
	/* RGB to RGB Blending */
	ispprv_update.rgb2rgb = &flr_rgb2rgb;

	/* RGB to YCbCr Blending */
	ispprv_update.prev_csc = &flr_prev_csc_normal;

	ispprv_update.prev_wbal->dgain = 0x100;
	ispprv_update.prev_wbal->coef0 = 0x23;
	ispprv_update.prev_wbal->coef1 = 0x20;
	ispprv_update.prev_wbal->coef2 = 0x20;
	ispprv_update.prev_wbal->coef3 = 0x39;

	/* Clear other tables */
	ispprv_update.prev_cfa->cfa_table = cfa_coef_table;
	ispprv_update.prev_cfa->cfa_gradthrs_vert = 0x28;
	ispprv_update.prev_cfa->cfa_gradthrs_horz = 0x28;

	ispprv_update.prev_nf->spread = 0x3;

	if (ioctl(rz_fd, VIDIOC_PRIVATE_ISP_PRV_CFG, &ispprv_update) == -1) {
		fprintf(stderr, "\nDevice not configured!");
		return -1;
	}

	/* V4L2 Buffer */
	struct v4l2_requestbuffers v4l2_req;
	/* user buffer source pointer */
	struct v4l2_buffer src_ubuf;
	struct v4l2_buffer dst_ubuf;

	/* clear buffers */
	memset(&v4l2_req, 0, sizeof(v4l2_req));
	memset(&src_ubuf, 0, sizeof(src_ubuf));
	memset(&dst_ubuf, 0, sizeof(dst_ubuf));

	/* V4L2 Memory map type */
	v4l2_req.memory	= V4L2_MEMORY_USERPTR;
	/* V4L2 The number of buffers requested */
	v4l2_req.count	= 1;
	/* V4L2 Buffer type */
	v4l2_req.type	= V4L2_BUF_TYPE_VIDEO_OUTPUT;

	if (ioctl(rz_fd, PREV2RESZ_REQBUF, &v4l2_req)) {
		error(-1, ECANCELED, "input: REQBUF failed.");
		exit(errno);
	}

	/* setup input buffer parameters */
	src_ubuf.index	= 0;
	src_ubuf.type	= V4L2_BUF_TYPE_VIDEO_OUTPUT;
	src_ubuf.memory	= v4l2_req.memory;
	/* calculate input buffer length */
	src_ubuf.length	= ALIGN(upipe.in.image.bytesperline *
					upipe.in.image.height, PAGE_SIZE);

	/* V4L2 - Query input buffer */
	if (ioctl(rz_fd, PREV2RESZ_QUERYBUF, &src_ubuf)) {
		error(-1, ECANCELED, "RSZ_QUERYBUF failed.");
		exit(errno);
	}

	/* allocate input data buffer */
	posix_memalign((void **) &src_ubuf.m.userptr, PAGE_SIZE,
				   src_ubuf.length);
	if (!src_ubuf.m.userptr) {
		perror("input: Can't allocate memory");
		exit(errno);
	}


	/* V4L2 - Setup input buffer in queue */
	if (ioctl(rz_fd, PREV2RESZ_QUEUEBUF, &src_ubuf)) {
		error(-1, ECANCELED, "RSZ_QUEUEBUF failed.");
		exit(errno);
	}

	/* index of options for input file name */
	opt_idx = get_option_index("-ifile", cmd_line, COUNT_OF(cmd_line));
	/* load data buffer */
	if (Load_Image(cmd_line[opt_idx].o_dflt.v_char,
			(char *)src_ubuf.m.userptr, src_ubuf.length)) {
		error(-1, ECANCELED, "File load error.");
		exit(errno);
	}

	/* V4L2 Memory map type */
	v4l2_req.memory	= V4L2_MEMORY_USERPTR;
	/* V4L2 The number of buffers requested */
	v4l2_req.count	= 1;
	/* V4L2 Buffer type */
	v4l2_req.type	= V4L2_BUF_TYPE_VIDEO_CAPTURE;

	if (ioctl(rz_fd, PREV2RESZ_REQBUF, &v4l2_req)) {
		error(-1, ECANCELED, "RSZ_REQBUF failed.");
		exit(errno);
	}
	/* setup output buffer parameters */
	dst_ubuf.index	= 0;
	dst_ubuf.type	= V4L2_BUF_TYPE_VIDEO_CAPTURE;
	dst_ubuf.memory	= v4l2_req.memory;
	/* calculate buffer/buffers length */
	dst_ubuf.length	= ALIGN(upipe.out.image.bytesperline *
					upipe.out.image.height, PAGE_SIZE);

	/* V4L2 - Query output buffer */
	if (ioctl(rz_fd, PREV2RESZ_QUERYBUF, &dst_ubuf)) {
		error(-1, ECANCELED, "RSZ_QUERYBUF failed.");
		exit(errno);
	}
#if 0
	/* allocate output buffer */
	dst_ubuf.maddr = malloc(dst_ubuf.length);
	if (dst_ubuf.maddr == 0) {
		error(-1, ECANCELED, "Buffer allocation failed.");
		exit(errno);
	}
	/* align and setup ouput pointer */
	dst_ubuf.m.userptr = (unsigned long) (ALIGN(dst_ubuf.maddr, PAGE_SIZE));
#else
	/* allocate input data buffer */
	posix_memalign((void **) &dst_ubuf.m.userptr, PAGE_SIZE,
				   dst_ubuf.length);
	if (!dst_ubuf.m.userptr) {
		perror("input: Can't allocate memory");
		exit(errno);
	}
#endif
	/* V4L2 - Setup output buffer in queue */
	if (ioctl(rz_fd, PREV2RESZ_QUEUEBUF, &dst_ubuf)) {
		error(-1, ECANCELED, "RSZ_QUEUEBUF failed.");
		exit(errno);
	}

#if !defined(NDEBUG)
	/* initialize start time */
	struct timeval beg_tv; gettimeofday(&beg_tv, NULL);
#endif

	/*
	 * Execute operation
	 */
	if (ioctl(rz_fd, PREV2RESZ_RUN_ENGINE, &user_pipe) == -1) {
		printf("PREV2RESZ_RUN_ENGINE error\n");
		return -1;
	}

#if !defined(NDEBUG)
	/* initialize end time */
	struct timeval end_tv; gettimeofday(&end_tv, NULL);
	fprintf(stderr, "\nOperation finished: %ld (usec)\n",
			(end_tv.tv_sec - beg_tv.tv_sec) * 1000000 +
			(end_tv.tv_usec - beg_tv.tv_usec)
	);
#endif

	/* index of options for output file name */
	opt_idx = get_option_index("-ofile", cmd_line, COUNT_OF(cmd_line));
	/* save image */
	if (Save_Image(cmd_line[opt_idx].o_dflt.v_char,
		(char *)dst_ubuf.m.userptr,
		user_pipe.out.image.bytesperline * \
				user_pipe.out.image.height)) {
		error(-1, ECANCELED, "File save error");
		exit(errno);
	}

	/* release allocated buffers */
	if (src_ubuf.m.userptr)
		free((void *) src_ubuf.m.userptr);
	if (dst_ubuf.m.userptr)
		free((void *) dst_ubuf.m.userptr);

	if (rz_fd)
		close(rz_fd);

	parse_release(cmd_line, COUNT_OF(cmd_line));

	return 0;
}
