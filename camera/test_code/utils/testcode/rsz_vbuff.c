#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <errno.h>
#include <linux/videodev2.h>
#include "rsz_vbuff.h"

#define FMT			RSZ_PIX_FMT_YUYV;

static __u16 gRDRV_reszFilter4TapHighQuality[] = {
	0x0000, 0x0100, 0x0000, 0x0000,
	0x03FA, 0x00F6, 0x0010, 0x0000,
	0x03F9, 0x00DB, 0x002C, 0x0000,
	0x03FB, 0x00B3, 0x0053, 0x03FF,
	0x03FD, 0x0082, 0x0084, 0x03FD,
	0x03FF, 0x0053, 0x00B3, 0x03FB,
	0x0000, 0x002C, 0x00DB, 0x03F9,
	0x0000, 0x0010, 0x00F6, 0x03FA
};

static __u16 gRDRV_reszFilter7TapHighQuality[] = {
	0x0004, 0x0023, 0x005A, 0x0058,
	0x0023, 0x0004, 0x0000, 0x0000,
	0x0002,	0x0018, 0x004d, 0x0060,
	0x0031, 0x0008, 0x0000, 0x0000,
	0x0001, 0x000f,	0x003f, 0x0062,
	0x003f, 0x000f,	0x0001, 0x0000,
	0x0000, 0x0008, 0x0031,	0x0060,
	0x004d, 0x0018, 0x0002, 0x0000
};

static void usage(void)
{
	printf("rsz_vbuff <input_file> <output_file> <input_width> "
			"<input_height> <output_width> <output_height>\n");
	printf("\tResizer driver to change a file of size\n");
	printf("\t<input_file> Name of existing file\n");
	printf("\t<output_file> Name of the new file with the new size\n");
	printf("\t<input_width> Width of existing file\n");
	printf("\t<input_height> Height of existing file\n");
	printf("\t<output_width> Width of new file\n");
	printf("\t<output_height> Height of new file\n");
}

int main(int argc, const char *argv[])
{
	int in_hsize, in_vsize, out_hsize, out_vsize;
	int ret_val, fd, sizeimage;
	struct rsz_resize convert;
	struct rsz_params params;
	FILE *in_data;
	FILE *out_data;
	void *in_start, *out_start;
	int i;
	struct v4l2_requestbuffers creqbuf;
	struct v4l2_buffer vbuffer;

	if (argc == 1 || (argc == 2 && !strcmp(argv[1], "?")) || argc > 7) {
		usage();
		return -1;
	}
	in_hsize = atoi(argv[3]);
	printf("%s", argv[3]);
	in_vsize = atoi(argv[4]);
	printf("%s", argv[4]);
	out_hsize = atoi(argv[5]);
	printf("%s", argv[5]);
	out_vsize = atoi(argv[6]);
	printf("%s", argv[6]);

	in_data = fopen(argv[1], "rb");
	if (in_data == NULL) {
		printf("ERROR opening input file!\n");
		return -EACCES;
	}

	out_data = fopen(argv[2], "wb");
	if (out_data == NULL) {
		printf("ERROR opening output file!\n");
		return -EACCES;
	}

	fd = open("/dev/omap-resizer", O_RDWR);
	if (fd == -1) {
		printf("Error opening Resizer\n");
		return -1;
	}
	/* printf("Resizer opened\n"); */
	/* Set Parameters */
	params.in_hsize = in_hsize;
	params.in_vsize = in_vsize;
	params.in_pitch = params.in_hsize * 2;
	params.inptyp = RSZ_INTYPE_YCBCR422_16BIT;
	params.vert_starting_pixel = 0;
	params.horz_starting_pixel = 0;

	params.cbilin = 0; /* We are going to do downsampling, 0.75x*/
	params.pix_fmt = FMT;
	params.out_hsize = out_hsize;
	params.out_vsize = out_vsize;
	params.out_pitch = params.out_hsize * 2;

	params.hstph = 0;
	params.vstph = 0;
	/* As we are downsizing, we put */

	for (i = 0; i < 32; i++)
		params.tap4filt_coeffs[i] =
		    gRDRV_reszFilter4TapHighQuality[i];

	for (i = 0; i < 32; i++)
		params.tap7filt_coeffs[i] =
		    gRDRV_reszFilter7TapHighQuality[i];

	params.yenh_params.type = 0;
	params.yenh_params.gain = 0;
	params.yenh_params.slop = 0;
	params.yenh_params.core = 0;
	/* printf("Sending parameters to Resizer Wrapper\n"); */
	ret_val = ioctl(fd, RSZ_S_PARAM, &params);
	if (ret_val) {
		printf("\nWrong Parameters for ISP Resizer\n");
		return ret_val;
	}

	/*ret_val = ioctl(fd, RSZ_G_PARAM, &params);
	if (ret_val) {
		printf("\nWrong requesting params for ISP Resizer\n");
		return ret_val;
	}*/

	/*printf(
	"params.in_hsize = %d\n \
	params.in_vsize = %d\n \
	params.in_pitch = %d\n\
	params.inptyp = %d\n\
	params.vert_starting_pixel = %d\n\
	params.horz_starting_pixel = %d\n\
	params.cbilin = %d\n\
	params.pix_fmt = %d\n\
	params.out_hsize = %d\n\
	params.out_vsize = %d\n\
	params.out_pitch = %d\n\
	params.hstph = %d\n\
	params.vstph = %d\n\
	params.yenh_params.type = %d\n\
	params.yenh_params.gain = %d\n\
	params.yenh_params.slop = %d\n\
	params.yenh_params.core = %d\n",\
	params.in_hsize,\
	params.in_vsize,\
	params.in_pitch,\
	params.inptyp,\
	params.vert_starting_pixel,\
	params.horz_starting_pixel,\
	params.cbilin,\
	params.pix_fmt,\
	params.out_hsize,\
	params.out_vsize,\
	params.out_pitch,\
	params.hstph,\
	params.vstph,\
	params.yenh_params.type,\
	params.yenh_params.gain,\
	params.yenh_params.slop,\
	params.yenh_params.core);*/



	/* Buffer size, 10 bits per pixel = 2 bytes per pixel */
	sizeimage = params.in_hsize * params.in_vsize * 2;
	creqbuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	creqbuf.memory = V4L2_MEMORY_MMAP; /* V4L2_MEMORY_USERPTR; */

	creqbuf.count = ((params.in_hsize > params.out_hsize) ? 1 : 2);

	sleep(2);

	/* Request input buffer */
	ret_val = ioctl(fd, RSZ_REQBUF, &creqbuf);
	if (ret_val < 0) {
		printf("Error requesting input buffer, retval= %d\n", ret_val);
		return ret_val;
	} else
		printf("Allocated INPUT buffer of %d bytes\n", sizeimage);

	vbuffer.type = creqbuf.type;
	vbuffer.memory = creqbuf.memory;
	vbuffer.index = 0;

	/* This IOCTL just updates buffer */
	ret_val = ioctl(fd, RSZ_QUERYBUF, &vbuffer);
	if (ret_val) {
		printf("Unable to query input buffer %d\n", vbuffer.index);
		return ret_val;
	}

	in_start = mmap(NULL, vbuffer.length, PROT_READ |
						PROT_WRITE, MAP_SHARED, fd,
						vbuffer.m.offset);
	printf("vbuffer.m.offset 1= %d\n", vbuffer.m.offset);
	if (in_start == MAP_FAILED) {
		printf("mmap error!\n");
		return -1;
	}
	vbuffer.m.userptr = (unsigned int)in_start;
	printf("Mapped Buffer.start = %x  length = %d\n",
		in_start, vbuffer.length);

	if (ioctl(fd, RSZ_QUEUEBUF, &vbuffer) < 0) {
		perror("RSZ_QUEUEBUF");
		return -1;
	}

	/* upscaling second buffer */
	if (creqbuf.count == 2) {
		vbuffer.index = 1;

		/* This IOCTL just updates buffer */
		ret_val = ioctl(fd, RSZ_QUERYBUF, &vbuffer);
		if (ret_val) {
			printf("Unable to query output buffer %d\n",
				vbuffer.index);
			return ret_val;
		}

		out_start = mmap(NULL, vbuffer.length,
					PROT_READ | PROT_WRITE,
					MAP_SHARED, fd,
					vbuffer.m.offset);
		printf("vbuffer.m.offset 2= %d\n", vbuffer.m.offset);
		if (out_start == MAP_FAILED) {
			printf("mmap error!\n");
			return -1;
		}
		vbuffer.m.userptr = (unsigned int)out_start;
		printf("Mapped Buffer.start = %x  length = %d\n",
			out_start, vbuffer.length);

		if (ioctl(fd, RSZ_QUEUEBUF, &vbuffer) < 0) {
			perror("RSZ_QUEUEBUF");
			return -1;
		}
	}

	/* Copying input file to input buffer */
	ret_val = fread(in_start, 1, sizeimage, in_data);
	if (ret_val != sizeimage) {
		printf("Bytes read = %d of %d \n", ret_val,
			convert.in_buf.size);
		perror("fread");
		return;
	}
	printf("INPUT Buffer filled! \n");

	ret_val = ioctl(fd, RSZ_RESIZE, NULL);
	if (ret_val) {
		printf("Error in ioctl RSZ_RESIZE\n");
		return ret_val;
	}
	/* dwnscaling */
	if (params.in_hsize > params.out_hsize)
		ret_val = fwrite(in_start, 1, out_hsize * out_hsize * 2,
								out_data);
	else
	/* upscaling */
		ret_val = fwrite(out_start, 1, out_hsize * out_vsize * 2,
								out_data);

	printf("Written %d  bytes of %d to out.yuv\n",
		ret_val, out_hsize * out_vsize * 2);
exit:
	fclose(in_data);
	fclose(out_data);
	printf("Resize completed \n");
	close(fd);
	return 0;
}
