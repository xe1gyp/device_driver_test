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
#include <error.h>
#include <errno.h>
#include <time.h>
#include <assert.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/param.h>
#include <sys/mman.h>

#include <linux/types.h>
#include <linux/omap_resizer.h>
#include <mach/isp_user.h>

/* Project includes */
#include "types.h"
#include "cmd_options.h"
#include "file_operations.h"

/*
 * device symbolic name
 */
#define DEV_SYMBOLIC "/dev/omap-resizer"

#define RESZ_USED_BUFF 2

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
	{ "-dev"	, e_CHAR, (union default_u) "/dev/omap-resizer",
	" Device (default = /dev/omap-resizer)" },
};

static int setup_user_parameters(struct isp_node *upipe, struct options *cl,
							unsigned cl_items_count)
{
	/* Read input image parameters */
	int opt_idx = get_option_index("-iw", cl, cl_items_count);
	int in_hsize = cl[opt_idx].o_dflt.v_word;
	printf(" optindex %d hsize %d \n", opt_idx, in_hsize);

	opt_idx = get_option_index("-ih", cl, cl_items_count);
	int in_vsize = cl[opt_idx].o_dflt.v_word;
	printf(" optindex %d vsize %d \n", opt_idx, in_vsize);

	if (in_hsize <= 0 || in_vsize <= 0) {
		error(-1, ECANCELED, "Input size out of range.");
		return -1;
	}

	/* Read output image parameters */
	opt_idx = get_option_index("-ow", cl, cl_items_count);
	int out_hsize = cl[opt_idx].o_dflt.v_word;

	opt_idx = get_option_index("-oh", cl, cl_items_count);
	int out_vsize = cl[opt_idx].o_dflt.v_word;

	if (in_hsize <= 0 || in_vsize <= 0) {
		error(-1, ECANCELED, "Output size out of range.");
		return -1;
	}

	/* clear parameters */
	memset(upipe, 0, sizeof(struct isp_node));
	/* determining 16 bit or 8 bit data */
	upipe->in.image.pixelformat	= V4L2_PIX_FMT_YUYV;
	/* input frame horizontal/vertical size */
	upipe->in.image.width		= in_hsize;
	upipe->in.image.height		= in_vsize;

	/* input crop horizontal size */
	opt_idx = get_option_index("-icw", cl, cl_items_count);
	if (cl[opt_idx].o_dflt.v_word)
		upipe->in.crop.width 	= cl[opt_idx].o_dflt.v_word;
	else
		upipe->in.crop.width 	= upipe->in.image.width;

	/* input crop horizontal offset */
	opt_idx = get_option_index("-icl", cl, cl_items_count);
	if (cl[opt_idx].o_dflt.v_word)
		upipe->in.crop.left		= cl[opt_idx].o_dflt.v_word;
	else
		upipe->in.crop.left		= 0;

	/* input crop vertical size */
	opt_idx = get_option_index("-ich", cl, cl_items_count);
	if (cl[opt_idx].o_dflt.v_word)
		upipe->in.crop.height	= cl[opt_idx].o_dflt.v_word;
	else
		upipe->in.crop.height	= upipe->in.image.height;

	/* input crop vertical offset */
	opt_idx = get_option_index("-ict", cl, cl_items_count);
	if (cl[opt_idx].o_dflt.v_word)
		upipe->in.crop.top		= cl[opt_idx].o_dflt.v_word;
	else
		upipe->in.crop.top		= 0;

	/* output frame horizontal/verticals size */
	upipe->out.image.width		= out_hsize;
	upipe->out.image.height		= out_vsize;

	/* output crop horizontal size */
	opt_idx = get_option_index("-ocw", cl, cl_items_count);
	if (cl[opt_idx].o_dflt.v_word)
		upipe->out.crop.width 	= cl[opt_idx].o_dflt.v_word;
	else
		upipe->out.crop.width	= upipe->out.image.width;

	/* output crop horizontal offset */
	opt_idx = get_option_index("-ocl", cl, cl_items_count);
	if (cl[opt_idx].o_dflt.v_word)
		upipe->out.crop.left		= cl[opt_idx].o_dflt.v_word;
	else
		upipe->out.crop.left		= 0;

	/* output crop vertical size */
	opt_idx = get_option_index("-och", cl, cl_items_count);
	if (cl[opt_idx].o_dflt.v_word)
		upipe->out.crop.height	= cl[opt_idx].o_dflt.v_word;
	else
		upipe->out.crop.height	= upipe->out.image.height;

	/* output crop vertical offset */
	opt_idx = get_option_index("-oct", cl, cl_items_count);
	if (cl[opt_idx].o_dflt.v_word)
		upipe->out.crop.top		= cl[opt_idx].o_dflt.v_word;
	else
		upipe->out.crop.top		= 0;

#if defined(DEBUG)
	print_options(cl, cl_items_count);
#endif
	return 0;
}

int main(int argc, char *argv[])
{
	/* user parameters */
	static struct isp_node upipe;
	/* command line argument index */
	int opt_idx;

	if (argc == 1 || (argc == 2 && (!strcmp(argv[1], "?") ||
	!strcmp(argv[1], "-h") || !strcmp(argv[1], "--help")))) {
		fprintf(stderr, "\nUSAGE: command <options>");
		opt_idx = get_option_index("-ifile",
			cmd_line, COUNT_OF(cmd_line));
		fprintf(stderr, " %s<input file>",
			cmd_line[opt_idx].o_symb);
		opt_idx = get_option_index("-ofile",
			cmd_line, COUNT_OF(cmd_line));
		fprintf(stderr, " %s<output file>\n",
			cmd_line[opt_idx].o_symb);
		print_options(cmd_line, COUNT_OF(cmd_line));

		/* Print additional help */
		printf("\nExamples:\n");

		printf("\nResize image: \n ");
		printf("reszhq -iw [in width] -ih [in height] "\
			"-ifile [input file] "\
			"-ow [out width] -oh [out height] "\
			"-ofile [output file]\n");

		exit(EXIT_FAILURE);
	}

	if (parse_prepare(argc, argv, cmd_line, COUNT_OF(cmd_line)))
		exit(errno);

	if (setup_user_parameters(&upipe, cmd_line, COUNT_OF(cmd_line)))
		exit(errno);

	/* index of input file name argument */
	opt_idx = get_option_index("-ifile", cmd_line, COUNT_OF(cmd_line));
	if (!cmd_line[opt_idx].o_dflt.v_char) {
		error(-1, ECANCELED, "Input file parameter missing.");
		exit(errno);
	}

	/* index of output file name argument */

	opt_idx = get_option_index("-ofile", cmd_line, COUNT_OF(cmd_line));
	if (!cmd_line[opt_idx].o_dflt.v_char) {
		error(-1, ECANCELED, "Output file parameter missing.");
		exit(errno);
	}

	int rz_fd;
	/* index of device name */
	opt_idx = get_option_index("-dev", cmd_line, COUNT_OF(cmd_line));
	rz_fd = open(cmd_line[opt_idx].o_dflt.v_char, O_RDWR);

	if (rz_fd < 0) {
		error(-1, ECANCELED, "Can't open device.");
		exit(errno);
	}
	printf(" Before  RSZ_S_PARAM : ");
	printf("%dx%d->", upipe.in.image.width, upipe.in.image.height);
	printf("%dx%d \n ", upipe.out.image.width, upipe.out.image.height);

	if (ioctl(rz_fd, RSZ_S_PARAM, &upipe) == -1) {
		error(-1, ECANCELED, "Invalid config parameters.");
		exit(errno);
	}

	printf(" After  RSZ_S_PARAM : ");
	printf("%dx%d->", upipe.in.image.width, upipe.in.image.height);
	printf("%dx%d \n ", upipe.out.image.width, upipe.out.image.height);

	/* V4L2 Buffer */
	struct v4l2_requestbuffers v4l2_req;
	/* user buffer source pointer */
	struct v4l2_buffer src_ubuf;
	struct v4l2_buffer dst_ubuf;

	/* clear buffers */
	memset(&v4l2_req, 0, sizeof(v4l2_req));
	memset(&src_ubuf, 0, sizeof(src_ubuf));
	memset(&dst_ubuf, 0, sizeof(dst_ubuf));
	void * inBuf;
	void * outBuf;

	/* V4L2 Memory map type */
	v4l2_req.memory	= V4L2_MEMORY_USERPTR;
	/* V4L2 The number of buffers requested */
	v4l2_req.count	= 1;
	/* V4L2 Buffer type */
	v4l2_req.type	= V4L2_BUF_TYPE_VIDEO_OUTPUT;

	if (ioctl(rz_fd, RSZ_REQBUF, &v4l2_req)) {
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
	if (ioctl(rz_fd, RSZ_QUERYBUF, &src_ubuf)) {
		error(-1, ECANCELED, "RSZ_QUERYBUF failed.");
		exit(errno);
	}

	/* allocate input data buffer */
	inBuf =  malloc(src_ubuf.length + PAGE_SIZE);
	if (inBuf == NULL ) {
		perror("input: Can't allocate memory");
		exit(errno);
	}

	src_ubuf.m.userptr = (unsigned long)(ALIGN(inBuf, PAGE_SIZE));

	if (!src_ubuf.m.userptr) {
		perror("input: Can't allocate memory");
		exit(errno);
	}


	/* V4L2 - Setup input buffer in queue */
	if (ioctl(rz_fd, RSZ_QUEUEBUF, &src_ubuf)) {
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

	if (ioctl(rz_fd, RSZ_REQBUF, &v4l2_req)) {
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
	if (ioctl(rz_fd, RSZ_QUERYBUF, &dst_ubuf)) {
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

	outBuf =  malloc(dst_ubuf.length + PAGE_SIZE);
	if (outBuf == NULL ) {
		perror("input: Can't allocate memory");
		exit(errno);
	}

	dst_ubuf.m.userptr = (unsigned long)(ALIGN(outBuf, PAGE_SIZE));

	if (!dst_ubuf.m.userptr) {
		perror("input: Can't allocate memory");
		exit(errno);
	}
#endif
	/* V4L2 - Setup output buffer in queue */
	if (ioctl(rz_fd, RSZ_QUEUEBUF, &dst_ubuf)) {
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
	if (ioctl(rz_fd, RSZ_RESIZE, &upipe) == -1) {
		error(-1, ECANCELED, "RSZ_RESIZE error\n");
		exit(errno);
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
		upipe.out.image.bytesperline * upipe.out.image.height)) {
		error(-1, ECANCELED, "File save error");
		exit(errno);
	}

	/* release allocated buffers */
	if (src_ubuf.m.userptr)
		free(inBuf);

	if (dst_ubuf.m.userptr)
		free(outBuf);

	if (rz_fd)
		close(rz_fd);

	parse_release(cmd_line, COUNT_OF(cmd_line));

	return 0;
}
