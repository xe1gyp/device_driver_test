/* ==========================================================================
*             Texas Instruments OMAP(TM) Platform Software
*  (c) Copyright Texas Instruments, Incorporated.  All Rights Reserved.
*
*  Use of this software is controlled by the terms and conditions found
*  in the license agreement under which this software has been supplied.
* ========================================================================== */

#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <linux/types.h>
#include <linux/videodev2.h>
#include <mach/isp_user.h>
#include <linux/omap_previewer.h>

/* Project includes */
#include "types.h"
#include "cmd_options.h"
#include "file_operations.h"

#ifndef ALIGN
 #define ALIGN(x, mask) (((x)+(mask-1))&~(mask-1))
#endif

static __u32 cfa_coef_table[] = {
#include "cfa_coef_table.h"
};

static __u32 luma_enhance_table[] = {
	#include "luma_enhance_table.h"
};

/*
 * Gamma Correction Table - Red
 */
static __u32 redgamma_table[] = {
#include "redgamma_table.h"
};

/*
 * Gamma Correction Table - Green
 */
static __u32 greengamma_table[] = {
#include "greengamma_table.h"
};

/*
 * Gamma Correction Table - Blue
 */
static __u32 bluegamma_table[] = {
#include "bluegamma_table.h"
};

struct options cmd_line[] = {
	{ "-iw"    , e_WORD, (union default_u) 3280,
	" Image width (default = 3280)", 0 },
	{ "-ih"    , e_WORD, (union default_u) 2464,
	" Image height (default = 2464)", 0 },
	{ "-ow"    , e_WORD, (union default_u) 1280,
	" Output image width (default = 1280)", 0 },
	{ "-oh"    , e_WORD, (union default_u) 960,
	" Output image height (default = 960)", 0 },
	{ "-ifile" , e_CHAR, (union default_u) NULL,
	" Input image file name", 0 },
	{ "-ofile" , e_CHAR, (union default_u) NULL,
	" Output image file name", 0 },
	{ "-dev"	, e_CHAR, (union default_u) "/dev/omap-previewer",
	" Device (default = /dev/omap-previewer)", 0 },
	{ "-brightness"  , e_CHAR, (union default_u) "nothing",
	" Set brightness level. \n\t\t Possible parameters: \
	Minimum, maximum or default \n\t\t (or number from 0 to 255)", 0 },
	{ "-contrast"  , e_CHAR, (union default_u) "nothing",
	" Set contrast level. \n\t\t Possible parameters: Minimum, \
	maximum or default \n\t\t (or number from 0 to 255)", 0 },
	{ "-turnoff_cfa" , e_CHAR, (union default_u) NULL,
	" Turns off the use of cfa table.", 0 },
	{ "-turnoff_nf" , e_CHAR, (union default_u) NULL,
	" Turns off the use of Noise Filter.", 0 },
	{ "-wbal" , e_CHAR, (union default_u) NULL,
	"Set maximum red/green/blue White ballans coefficients.", 0 },

};


/*
 * ISP memory page size
 */

#define PAGE_SIZE 4096

int main(int argc, char *argv[])
{
	int ret_val, fd;
	int sizeimage, out_img_sz;
	struct prev_params params;
	FILE *in_data, *out_data;
	struct v4l2_requestbuffers creqbuf;
	struct v4l2_buffer vbuffer;
	struct v4l2_rect outsize;
	void *ibuffer;
	void *ibuffer_aligned;
	int ibuffer_length;
	int i;
	int in_width, in_height;
	int memtype = V4L2_MEMORY_USERPTR;

	struct v4l2_queryctrl queryctrl;
	struct v4l2_control control;

	/* command line argument index */
	int opt_idx;


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

		printf("\nProcess image and change brightness: \n ");
		printf("prvhq -iw [in width] -ih [in height] "\
			"-idev [in device] "\
			"-ifile [input file] "\
			"-ofile [output file] "\
			"-brightness [number] \n");

		printf("\nProcess image and change contrast: \n ");
		printf("prvhq -iw [in width] -ih [in height] "\
			"-idev [in device] "\
			"-ifile [input file] "\
			"-ofile [output file] "\
			"-contrast [number] \n");

		printf("\nProcess image without cfa: \n ");
		printf("prvhq -iw [in width] -ih [in height] "\
			"-idev [in device] "\
			"-ifile [input file] "\
			"-ofile [output file] "\
			"-turnoff_cfa \n");

		printf("\nProcess image without Noise filter: \n ");
		printf("prvhq -iw [in width] -ih [in height] "\
			"-idev [in device] "\
			"-ifile [input file] "\
			"-ofile [output file] "\
			"-turnoff_nf \n");

		printf("\nProcess image with maximum red green or blue " \
			"White ballance : \n ");
		printf("prvhq -iw [in width] -ih [in height] "\
			"-idev [in device] "\
			"-ifile [input file] "\
			"-ofile [output file] "\
			"-wbal [red/green/blue]\n");

		return -1;
	}

	if (parse_prepare(argc, argv, cmd_line, COUNT_OF(cmd_line)))
		return -1;

	/* index of options for output file name */
	opt_idx = get_option_index("-ifile", cmd_line, COUNT_OF(cmd_line));

	in_data = fopen(cmd_line[opt_idx].o_dflt.v_char, "rb");

	if (in_data == NULL) {
		printf("ERROR opening input file!\n");
		return -EACCES;
	}

	/* index of options for output file name */
	opt_idx = get_option_index("-ofile", cmd_line, COUNT_OF(cmd_line));

	out_data = fopen(cmd_line[opt_idx].o_dflt.v_char, "wb");

	if (out_data == NULL) {
		printf("ERROR opening output file!\n");
		return -EACCES;
	}

	/* Read input image parameters */
	opt_idx = get_option_index("-iw", cmd_line, COUNT_OF(cmd_line));
	in_width = cmd_line[opt_idx].o_dflt.v_word;
	printf(" optindex %d in_width %d \n", opt_idx, in_width);

	opt_idx = get_option_index("-ih", cmd_line, COUNT_OF(cmd_line));
	in_height = cmd_line[opt_idx].o_dflt.v_word;
	printf(" optindex %d in_height %d \n", opt_idx, in_height);

	if (in_height <= 0 || in_width <= 0) {
		error(-1, ECANCELED, "Input size out of range.");
		return -1;
	}

	/* index of device name */
	opt_idx = get_option_index("-dev", cmd_line, COUNT_OF(cmd_line));
	fd = open(cmd_line[opt_idx].o_dflt.v_char, O_RDWR);

	if (fd < 0) {
		error(-1, ECANCELED, "Can't open device.");
		return -1;
	}

	ret_val = ioctl(fd, PREV_GET_PARAM, &params);
	if (ret_val) {
		printf("\nCan't get parameters for ISP Previewer\n");
		return ret_val;
	}

	printf("Test features read are = 0x%x\n", params.features);
	/* For Averager */
	params.average = NO_AVE;
	/* Pix YCC Output Format */
	params.pix_fmt = YCPOS_YCrYCb;  /* UYVY */
	/* Image size */
	params.size_params.hstart = 0;
	params.size_params.vstart = 0;
	params.size_params.hsize = in_width;
	params.size_params.vsize = in_height;
	params.size_params.pixsize = PREV_INWIDTH_10BIT;
	/* Read Line Offset */
	params.size_params.in_pitch = ALIGN(params.size_params.hsize * 2, 32);
	/* Write Line Offset. This parameter is ignored */
	params.size_params.out_pitch = ALIGN(params.size_params.hsize * 2, 32);

	params.features = 0;

    params.contrast = ISPPRV_CONTRAST_DEF;
    params.brightness = ISPPRV_BRIGHT_DEF;

	opt_idx = get_option_index("-brightness", cmd_line, COUNT_OF(cmd_line));
	if (cmd_line[opt_idx].entered) {

		params.brightness = atoi(cmd_line[opt_idx].o_dflt.v_char);

		if (!(strcmp(cmd_line[opt_idx].o_dflt.v_char, "minimum")))
			params.brightness = ISPPRV_BRIGHT_LOW;
		if (!(strcmp(cmd_line[opt_idx].o_dflt.v_char, "maximum")))
			params.brightness = ISPPRV_BRIGHT_HIGH;
		if (!(strcmp(cmd_line[opt_idx].o_dflt.v_char, "default")))
			params.brightness = ISPPRV_BRIGHT_DEF;
		printf("params.brightness is set to %s \n", \
				cmd_line[opt_idx].o_dflt.v_char);
	}

	opt_idx = get_option_index("-contrast", cmd_line, COUNT_OF(cmd_line));
	if (cmd_line[opt_idx].entered) {

		params.brightness = atoi(cmd_line[opt_idx].o_dflt.v_char);

		if (!(strcmp(cmd_line[opt_idx].o_dflt.v_char, "minimum")))
			params.contrast = ISPPRV_CONTRAST_LOW;
		if (!(strcmp(cmd_line[opt_idx].o_dflt.v_char, "maximum")))
			params.contrast = ISPPRV_CONTRAST_HIGH;
		if (!(strcmp(cmd_line[opt_idx].o_dflt.v_char, "default")))
			params.contrast = ISPPRV_CONTRAST_DEF;
		printf("params.contrast is set to %s \n", \
				cmd_line[opt_idx].o_dflt.v_char);
	}

	params.cfa.cfa_table = cfa_coef_table;

    params.ytable = luma_enhance_table;

	params.gtable.redtable = redgamma_table;
	params.gtable.greentable = greengamma_table;
	params.gtable.bluetable = bluegamma_table;

   opt_idx = get_option_index("-turnoff_cfa", cmd_line, COUNT_OF(cmd_line));
	if (!cmd_line[opt_idx].entered) {
		params.cfa.cfa_gradthrs_vert = 0x28;
		params.cfa.cfa_gradthrs_horz = 0x28;

		params.features |= PREV_CFA;
		printf("Using default cfa.\n");
	} else {
		printf("Disable default cfa. \n");
    }

	opt_idx = get_option_index("-turnoff_nf", cmd_line, COUNT_OF(cmd_line));
	if (!cmd_line[opt_idx].entered) {
		params.nf.spread = 0x3;
		params.features |= PREV_NOISE_FILTER;
		printf("Using Noise filter Spread=%d. \n", params.nf.spread);
	} else {
		printf("Disable Noise fileter. \n");
    }

	params.wbal.dgain = 0x100;
	params.wbal.coef0 = 0x23;
	params.wbal.coef1 = 0x20;
	params.wbal.coef2 = 0x20;
	params.wbal.coef3 = 0x39;

	opt_idx = get_option_index("-wbal", cmd_line, COUNT_OF(cmd_line));
	if (cmd_line[opt_idx].entered) {
		/* we assume that we have RGGB color pattern */
		if (!(strcmp(cmd_line[opt_idx].o_dflt.v_char, "red"))) {
			printf("Setting red wbal to max.");
			params.wbal.coef0 = 0xFF;
		}

		if (!(strcmp(cmd_line[opt_idx].o_dflt.v_char, "green"))) {
			printf("Setting green wbal to max.");
			params.wbal.coef1 = 0xFF;
			params.wbal.coef2 = 0xFF;
		}
		if (!(strcmp(cmd_line[opt_idx].o_dflt.v_char, "blue"))) {
			printf("Setting blue wbal to max.");
			params.wbal.coef3 = 0xFF;
		}
	}

	ret_val = ioctl(fd, PREV_SET_PARAM, &params);

	if (ret_val) {
		perror("PREV_SET_PARAM");
		return ret_val;
	}
	/* Input Buffer size, 10 bits per pixel = 2 bytes per pixel */
	sizeimage = params.size_params.hsize * params.size_params.vsize * 2;

	ret_val = ioctl(fd, PREV_GET_CROPSIZE, &outsize);
	if (ret_val) {
		printf("\nPREV_GET_CROPSIZE failed\n");
		return ret_val;
	}
	printf("\nOutput size is %d x %d pixels\n", outsize.width,
		outsize.height);
	/* Output buffer size = hsize * vsize * bytesperpixel */
	out_img_sz = outsize.width * outsize.height * 2;

	creqbuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	creqbuf.memory = memtype;
	creqbuf.count = 1;
	if (ioctl(fd, PREV_REQBUF, &creqbuf) < 0) {
		perror("PREV_REQBUF");
		return -1;
	}

	vbuffer.type = creqbuf.type;
	vbuffer.memory = creqbuf.memory;
	vbuffer.index = 0;
	if (ioctl(fd, PREV_QUERYBUF, &vbuffer) < 0) {
		perror("PREV_QUERYBUF");
		return -1;
	}

	if (memtype == V4L2_MEMORY_USERPTR) {
		ibuffer_length = vbuffer.length;
		posix_memalign(&ibuffer, PAGE_SIZE, ibuffer_length);
		ibuffer_aligned = ibuffer;
		vbuffer.flags = 0;
		vbuffer.m.userptr = (unsigned int)ibuffer_aligned;
	} else {
		ibuffer = mmap(NULL, vbuffer.length, PROT_READ |
				PROT_WRITE, MAP_SHARED, fd, vbuffer.m.offset);

		if (ibuffer == MAP_FAILED) {
			perror("mmap");
			return -1;
		}
		vbuffer.m.userptr = (unsigned int)ibuffer;
		ibuffer_aligned = ibuffer;
	}
	fprintf(stdout, "buf length=%d, addr=0x%x \n", ibuffer_length,
		ibuffer_aligned);

	/* Copy input file to buffer */
	printf("Copying input file to buffer... \n");
	ret_val = fread(ibuffer_aligned, 1, sizeimage, in_data);
	if (ret_val != sizeimage) {
		printf("Bytes read = %d of %d \n", ret_val,
			sizeimage);
		perror("fread");
		return;
	}
	printf("      Done!\n");

	if (ioctl(fd, PREV_QUEUEBUF, &vbuffer) < 0) {
		perror("PREV_QUEUEBUF");
		return -1;
	}

	printf("Performing PREVIEW...\n");
	ret_val = ioctl(fd, PREV_PREVIEW, NULL);
	if (ret_val) {
		printf("Error in ioctl PREV_PREVIEW\n");
		return ret_val;
	}

   printf("Writing to file ... \n");

	ret_val = fwrite(ibuffer_aligned, 1, out_img_sz, out_data);
	if (ret_val != out_img_sz) {
		printf("Bytes read = %d out of %d \n", ret_val,
			out_img_sz);
		perror("fread");
		return;
	}

	/* We're done. Now bring all to default.*/

	/* For Averager */
	params.average = NO_AVE;
	/* Pix YCC Output Format */
	params.pix_fmt = YCPOS_YCrYCb;  /* UYVY */
	/* Image size */
	params.size_params.hstart = 0;
	params.size_params.vstart = 0;
	params.size_params.hsize = in_width;
	params.size_params.vsize = in_height;
	params.size_params.pixsize = PREV_INWIDTH_10BIT;
	/* Read Line Offset */
	params.size_params.in_pitch = ALIGN(params.size_params.hsize * 2, 32);
	/* Write Line Offset. This parameter is ignored */
	params.size_params.out_pitch = ALIGN(params.size_params.hsize * 2, 32);
	/* White Balance */
	params.wbal.dgain = 0x100;
	params.wbal.coef0 = 0x23;
	params.wbal.coef1 = 0x20;
	params.wbal.coef2 = 0x20;
	params.wbal.coef3 = 0x39;
	/* Black Adjustment */
	params.blk_adj.red = 0;
	params.blk_adj.green = 0;
	params.blk_adj.blue = 0;

	params.features = PREV_CFA | PREV_DEFECT_COR |
				PREV_NOISE_FILTER;

	/* Clear other tables */
	params.cfa.cfa_table = cfa_coef_table;
	params.gtable.redtable = redgamma_table;
	params.gtable.greentable = greengamma_table;
	params.gtable.bluetable = bluegamma_table;
    params.ytable = luma_enhance_table;

    params.contrast = ISPPRV_CONTRAST_DEF;
    params.brightness = ISPPRV_BRIGHT_DEF;

	params.cfa.cfa_gradthrs_vert = 0x28;
	params.cfa.cfa_gradthrs_horz = 0x28;

	params.nf.spread = 0x3;

	ret_val = ioctl(fd, PREV_SET_PARAM, &params);

	if (ret_val) {
		perror("PREV_SET_PARAM");
		return ret_val;
	}

	if (memtype == V4L2_MEMORY_USERPTR)
		free(ibuffer);
	else
		munmap(ibuffer, vbuffer.length);

	fclose(in_data);
	fclose(out_data);
	close(fd);
	printf("Preview completed.\n");
	return 0;
}
