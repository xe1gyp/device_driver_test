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
#include "isp_user.h"
#include "prev_wrap.h"

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

__u32 luma_enhance_table[] = {
	#include "luma_enhance_table.h"
};


static void usage(void)
{
	printf("prev_wrap <input_file> <input_width> <input_heigh> "
							"<output_file>\n");
	printf("\tCreate a new YUYV fotmat file named \"out_example.yuv\""
							"from a RAW file\n");
	printf("\t<input_file> Name of existing file\n");
	printf("\t<input_width> Width of existing file\n");
	printf("\t<input_height> Height of existing file\n");
	printf("\t<output_file> Name of file to be created\n");
}

int main(int argc, const char *argv[])
{
	int ret_val, fd;
	int sizeimage, out_img_sz;
	struct prev_params params;
	FILE *in_data, *out_data;
	struct v4l2_requestbuffers creqbuf;
	struct v4l2_buffer vbuffer;
	struct prev_cropsize outsize;
	void *ibuffer;
	void *ibuffer_aligned;
	int ibuffer_length;
	int i;
	int in_width, in_height;
	int memtype = V4L2_MEMORY_USERPTR;

	if (argc == 1 || (argc == 2 && !strcmp(argv[1], "?")) || argc > 5) {
		usage();
		return -1;
	}

	in_width = atoi(argv[2]);
	/* printf("%d\n", in_width); */
	in_height = atoi(argv[3]);
	/* printf("%d\n", in_height); */

	in_data = fopen(argv[1], "rb");
	if (in_data == NULL) {
		printf("ERROR opening input file!\n");
		return -EACCES;
	}

	out_data = fopen(argv[4], "wb");
	if (out_data == NULL) {
		printf("ERROR opening output file!\n");
		return -EACCES;
	}

	fd = open("/dev/omap-previewer", O_RDWR);
	if (fd == -1) {
		perror("Previewer");
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
	params.size_params.in_pitch = params.size_params.hsize * 2;
	/* Write Line Offset. This parameter is ignored */
	params.size_params.out_pitch = params.size_params.hsize * 2;
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
	/* RGB to RGB Blending */
	params.rgb2rgb = flr_rgb2rgb;
	/* RGB to YCbCr Blending */
	params.rgb2ycbcr = flr_prev_csc_normal;

	params.features = PREV_CFA | PREV_CHROMA_SUPPRESS | PREV_LUMA_ENHANCE
				| PREV_DEFECT_COR | PREV_NOISE_FILTER;

	/* Pass in Luma Table (ytable) */
	params.ytable = &luma_enhance_table[0];

	/* Clear other tables */
	params.cfa.cfa_table = NULL;
	params.gtable.redtable = NULL;
	params.gtable.greentable = NULL;
	params.gtable.bluetable = NULL;

	ret_val = ioctl(fd, PREV_SET_PARAM, &params);
	if (ret_val) {
		printf("\nWrong Parameters for ISP Previewer\n");
		return ret_val;
	}
	/* Input Buffer size, 10 bits per pixel = 2 bytes per pixel */
	sizeimage = params.size_params.hsize * params.size_params.vsize * 2;

	ret_val = ioctl(fd, PREV_GET_CROPSIZE, &outsize);
	if (ret_val) {
		printf("\nPREV_GET_CROPSIZE failed\n");
		return ret_val;
	}
	printf("\nOutput size is %d x %d pixels\n", outsize.hcrop,
		outsize.vcrop);
	/* Output buffer size = hsize * vsize * bytesperpixel */
	out_img_sz = outsize.hcrop * outsize.vcrop * 2;

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
		if (ibuffer_length & 0xfff)
			ibuffer_length = (ibuffer_length & 0xfffff000) + 0x1000;

		ibuffer = malloc(ibuffer_length+0x2000);
	        ibuffer_aligned = (void *)(((unsigned int)(ibuffer) + 0xfff) &
					   (0xfffff000));
	        vbuffer.flags = 0;
	        vbuffer.length = ibuffer_length;
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
	fprintf(stdout,"buf length=%d, addr=0x%x \n", ibuffer_length,
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

	printf("Reading output...\n");
	ret_val = fwrite(ibuffer_aligned, 1, out_img_sz, out_data);
	if (ret_val != out_img_sz) {
		printf("Bytes read = %d out of %d \n", ret_val,
			out_img_sz);
		perror("fread");
		return;
	}
	printf("Writen  %d  bytes out of %d to %s\n",
		ret_val, out_img_sz, argv[4]);

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
