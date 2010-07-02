/**
 * Copyright (C) 2010 MM-SOL ltd. All Rights Reserved.
 *
 * Test application for "CCP2" pipeline module,
 * implemented in Texas Instruments "OMAP3" kernels.
 *
 * Authors:
 * 	 Atanas Filipov <afilipov@mm-sol.com>
 *
 * Use of this software is controlled by the terms and conditions found
 * in the license agreement under which this software has been supplied.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <error.h>
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/param.h>
#include <sys/ioctl.h>
#include <linux/videodev2.h>
#include <mach/isp_user.h>

#include "cmd_options.h"
#include "file_operations.h"

#define USED_BUFF 1

#define BYTES_PER_WINDOW	16
#define DIGITAL_GAIN_DEFAULT	0x100
#define DIGITAL_GAIN_MAX	0x3FF
#define BLUE_GAIN_DEFAULT	0x94
#define BLUE_GAIN_MAX		0xFF
#define RED_GAIN_DEFAULT	0x68
#define RED_GAIN_MAX		0xFF
#define GB_GAIN_DEFAULT		0x5C
#define GB_GAIN_MAX		0xFF
#define GR_GAIN_DEFAULT		0x5C
#define GR_GAIN_MAX		0xFF

struct options cmd_line[] = {
    { "-iw"    , e_WORD, (union default_u) 3280,
	" Image width ", 0 },
    { "-ih"    , e_WORD, (union default_u) 2464,
	" Image height ", 0 },
    { "-ow"    , e_WORD, (union default_u) 1280,
	" Output image width ", 0 },
    { "-oh"    , e_WORD, (union default_u) 960,
	" Output image height " },
    { "-of"    , e_CHAR, (union default_u) "UYVY",
	" Input image format", 0 },
    { "-idev"  , e_CHAR, (union default_u) "/dev/video10",
	" Input video device", 0 },
    { "-odev"  , e_CHAR, (union default_u) "/dev/video6",
	" Output video device", 0 },
    { "-ifile" , e_CHAR, (union default_u) NULL,
	" Input image file name", 0 },
    { "-ofile" , e_CHAR, (union default_u) NULL,
	" Output image file name", 0 },
    { "-zoom" , e_WORD, (union default_u) 10,
	" Zoom factor ", 0 },
    { "-brightness"  , e_CHAR, (union default_u) "nothing",
	" Set brightness level. \n\t\t Possible parameters: \
	Minimum, maximum or default \n\t\t (or number from 0 to 255)", 0 },
    { "-contrast"  , e_CHAR, (union default_u) "nothing",
	" Set contrast level. \n\t\t Possible parameters: Minimum, \
	maximum or default \n\t\t (or number from 0 to 255)", 0 },
    { "-color"  , e_CHAR, (union default_u) "nothing",
	" Select color effect. Possible parameters: BW or SEPIA", 0 },
    { "-outputinfo"  , e_CHAR, (union default_u) "nothing",
	" Select the output information from the kernel for verification.\n\t\t\
	Possible parameters: video_querycap sensor_info", 0 },
    { "-h3a"  , e_CHAR, (union default_u) "nothing",
	" Obtain h3a buffer. Possible parameters: obtain", 0 },
    { "-csi2mem"  , e_CHAR, (union default_u) "nothing",
	" Test just the CSI to Memory part of the pipe", 0 },
    { "-mem2ccp"  , e_CHAR, (union default_u) "nothing",
	" Test just the Memory to ccp part of the pipe", 0 },
};

static struct v4l2_buffer src_buff;
static struct v4l2_buffer dst_buff;

struct {
	void *start;
	size_t length;
} *cbuffers;


static int display_h3a_stats(unsigned int num_windows,
			unsigned int buff_prev_size,
			struct isph3a_aewb_data *aewb_data_user,
			struct isph3a_aewb_config *aewb_config_user)
{
	int i, data8, data2, window, unsat_cnt;
	__u16 *buff_preview = NULL;

	printf("H3A AE/AWB: buffer to display = %d data pointer = %p\n",
		buff_prev_size, aewb_data_user->h3a_aewb_statistics_buf);
	printf("num_windows = %d\n", num_windows);
	printf("ver_windows = %d\n", aewb_config_user->ver_win_count);
	printf("hor_windows = %d\n", aewb_config_user->hor_win_count);
	printf("plus one row of black windows\n");

	unsat_cnt = 0;
	buff_preview = (__u16 *)aewb_data_user->h3a_aewb_statistics_buf;
	for (i = 0; i < (buff_prev_size); i++) {
		data8 = (i + 1) % 8;
		data2 = (i + 1) % 2;
		window = (i + 1) / 8;
		printf("%05d ", buff_preview[i]);
		if (0 == data8) {
			if (((window > 1) && (0 == (window % 9))) ||
				(window == ((num_windows + (num_windows / 8) +
				((num_windows % 8) ? 1 : 0))))) {
				printf("   Unsaturated block count\n");
				unsat_cnt++;
			} else {
				printf("    Window %5d\n", (window - 1) -
					unsat_cnt);
			}
		}
		if (0 == data2)
			printf("\n");
	}
	return 0;
}

static int wait_for_h3a_event(int cfd, fd_set *excfds,
			struct v4l2_event *cam_ev)
{
	int ret;

	do {
		ret = pselect(cfd + 1, NULL, NULL, excfds, NULL, NULL);
		if (ret < 0) {
			perror("cam select()");
			return -1;
		}

		ret = ioctl(cfd, VIDIOC_DQEVENT, cam_ev);
		if (ret < 0) {
			perror("cam DQEVENT");
			return -1;
		}
	} while (cam_ev->type != V4L2_EVENT_OMAP3ISP_AEWB);

	return 0;
}


static int crop(int cfd, int zoomfactor)
{
	struct v4l2_cropcap cropcap;
	struct v4l2_crop crop;
	int ret;

	printf(" Set zoom factor: %1.1f\n", (float)zoomfactor/10.0f);

	cropcap.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	ret = ioctl(cfd, VIDIOC_CROPCAP, &cropcap);
	if (ret != 0) {
		perror("VIDIOC_CROPCAP");
		return -1;
	}

	printf(" Video Crop bounds (%d, %d) (%d, %d), "
		" defrect (%d, %d) (%d, %d)\n",
		cropcap.bounds.left, cropcap.bounds.top,
		cropcap.bounds.width, cropcap.bounds.height,
		cropcap.defrect.left, cropcap.defrect.top,
		cropcap.defrect.width, cropcap.defrect.height);

	crop.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	ret = ioctl(cfd, VIDIOC_G_CROP, &crop);
	if (ret != 0) {
		perror("VIDIOC_G_CROP");
		return -1;
	}

	printf(" Old Crop (%d, %d) (%d, %d)\n",
			crop.c.left, crop.c.top,
			crop.c.width, crop.c.height);

	crop.c.left = (cropcap.defrect.width -
			(cropcap.defrect.width * 10) / zoomfactor) / 2;
	crop.c.top = (cropcap.defrect.height -
			(cropcap.defrect.height * 10) / zoomfactor) / 2;
	crop.c.width = (cropcap.defrect.width * 10) / zoomfactor;
	crop.c.height = (cropcap.defrect.height * 10) / zoomfactor;

	printf(" Requested Crop  (%d, %d) (%d, %d)\n",
			crop.c.left, crop.c.top,
			crop.c.width, crop.c.height);

	ret = ioctl(cfd, VIDIOC_S_CROP, &crop);

	if (ret != 0) {
		perror("VIDIOC_S_CROP");
		return -1;
	}

	/* Read back actual crop settings*/
	ret = ioctl(cfd, VIDIOC_G_CROP, &crop);
	if (ret != 0) {
		perror("VIDIOC_G_CROP");
		return -1;
	}
	printf(" Negotiated Crop (%d, %d) (%d, %d)\n\n",
			crop.c.left, crop.c.top,
			crop.c.width, crop.c.height);

	return 0;
}

static int ccp_setup_user_input(struct v4l2_pix_format *fmt, struct options *cl,
							unsigned cl_items_count)
{
	/* Read input image parameters */
	int opt_idx = get_option_index("-iw", cl, cl_items_count);
	int in_hsize = cl[opt_idx].o_dflt.v_word;

	opt_idx = get_option_index("-ih", cl, cl_items_count);
	int in_vsize = cl[opt_idx].o_dflt.v_word;

	if (in_hsize <= 0 || in_vsize <= 0) {
		error(-1, ECANCELED, "Input size out of range.");
	switch (cmd_line[opt_idx].o_type)		return -1;
	}

	/* clear parameters */
	memset(fmt, 0, sizeof(struct v4l2_pix_format));
	/* determining 16 bit or 8 bit data */
	fmt->pixelformat	= V4L2_PIX_FMT_SRGGB10;
	/* input frame horizontal/vertical size */
	fmt->width			= in_hsize;
	fmt->height			= in_vsize;

	return 0;
}

static int ccp_setup_user_output(struct v4l2_pix_format *fmt,
				struct options *cl, unsigned cl_items_count)
{
	/* Read output image parameters */
	int opt_idx = get_option_index("-ow", cl, cl_items_count);
	int out_hsize = cl[opt_idx].o_dflt.v_word;

	opt_idx = get_option_index("-oh", cl, cl_items_count);
	int out_vsize = cl[opt_idx].o_dflt.v_word;

	if (out_hsize <= 0 || out_vsize <= 0) {
		error(-1, ECANCELED, "Output size out of range.");
		return -1;
	}

	/* clear parameters */
	memset(fmt, 0, sizeof(struct v4l2_pix_format));
	/* output frame horizontal/verticals size */
	fmt->width	= out_hsize;
	fmt->height	= out_vsize;

	/* output frame format */
	opt_idx = get_option_index("-of", cl, cl_items_count);
	if (strcmp(cl[opt_idx].o_dflt.v_char, "RAW") == 0) {
		fmt->pixelformat = V4L2_PIX_FMT_SRGGB10;
	} else if (strcmp(cl[opt_idx].o_dflt.v_char, "YUYV") == 0) {
		fmt->pixelformat = V4L2_PIX_FMT_YUYV;
	} else if (strcmp(cl[opt_idx].o_dflt.v_char, "UYVY") == 0) {
		fmt->pixelformat = V4L2_PIX_FMT_UYVY;
	} else {
		error(-1, ECANCELED, "Unknown output format.");
		return -1;
	}

	return 0;
}

int main(int argc, char **argv)
{
	/* command line argument index */
	int opt_idx;
	struct v4l2_capability		v4l2_cap;
	struct v4l2_requestbuffers	v4l2_req;
	struct v4l2_format		v4l2_fmt;
	struct v4l2_buffer 		buffer;
	int in_fd;
	int out_fd;
	struct v4l2_queryctrl queryctrl;
	struct v4l2_control control;
	int zoomFactor = 10;
	int colorLevel = V4L2_COLORFX_NONE;
	int count;
   int use_video5;

	if (argc == 1 || (argc == 2 && (!strcmp(argv[1], "?") ||
		!strcmp(argv[1], "-h") || !strcmp(argv[1], "--help")))) {
		fprintf(stderr, "\nUSAGE: command <options> \n");
		for (opt_idx = 0; opt_idx < COUNT_OF(cmd_line); opt_idx++) {
			printf(" \t %s %s ", cmd_line[opt_idx].o_symb, \
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

		printf("\nCapture RAW image: \n ");
		printf("virtsenhq -of RAW -iw [in width] -ih [in height] "\
			"-ow [out width] -oh [out height] "\
			"-idev [in device] -odev [out device] "\
			"-ofile [output file] \n");

		printf("\nCapture UYVY image with different zooms (crops):\n");
		printf("virtsenhq -of UYVY -iw [in width] -ih [in height] "\
			"-ow [out width] -oh [out height] "\
			"-idev [in device] -odev [out device] "\
			"-ofile [output file] "\
			"-zoom [20/30/40]\n");

		printf("\nCapture UYVY image with different "\
			"brightness level:\n");
		printf("virtsenhq -of UYVY -iw [in width] -ih [in height] "\
			"-ow [out width] -oh [out height] "\
			"-idev [in device] -odev [out device] "\
			"-ofile [output file] "\
			"-brightness [0-255]\n");

		printf("\nCapture UYVY image with different contrast level:\n");
		printf("virtsenhq -of UYVY -iw [in width] -ih [in height] "\
			"-ow [out width] -oh [out height] "\
			"-idev [in device] -odev [out device] "\
			"-ofile [output file] "\
			"-contrast [0-255]\n");

		printf("\nCapture RAW image with different colorfx level:\n");
		printf("virtsenhq -of UYVY -iw [in width] -ih [in height] "\
			"-ow [out width] -oh [out height] -of [output format] "\
			"-idev [in device] -odev [out device] "\
			"-ofile [output file] "\
			"-color [BW/RAW]\n");

		printf("\nOutput info from the virtual sensor:\n");
		printf("virtsenhq -idev [in device] -outputinfo" \
			"[video_querycap/sensor_info]\n");

		printf("\nObtain H3A buffer:\n");
		printf("virtsenhq -of UYVY -iw [in width] -ih [in height] "\
			"-ow [out width] -oh [out height] -of [output format] "\
			"-idev [in device] -odev [out device] "\
			"-ofile [output file] "\
			"-h3a [obtain]\n");

		printf("\nCapture RAW image from CSI to Memory:\n");
		printf("virtsenhq -of RAW -iw [in width] -ih [in height] "\
			"-ow [out width] -oh [out height]"\
			"-idev [in device] -odev [out device] "\
			"-ofile [output file] "\
			"-csi2mem \n");

		printf("\nCapture RAW image from Memory to ccp:\n");
		printf("virtsenhq -of RAW -iw [in width] -ih [in height] "\
			"-ow [out width] -oh [out height]"\
			"-idev [in device] -odev [out device] "\
			"-ofile [output file] "\
			"-mem2ccp \n");
		printf("\n");
		return -1;
	}

   use_video5 = 1;

	if (parse_prepare(argc, argv, cmd_line, COUNT_OF(cmd_line)))
		return -1;

	opt_idx = get_option_index("-outputinfo", cmd_line, COUNT_OF(cmd_line));
	if (cmd_line[opt_idx].entered)
		use_video5 = 0;

	opt_idx = get_option_index("-mem2ccp", cmd_line, COUNT_OF(cmd_line));
	if (cmd_line[opt_idx].entered)
		use_video5 = 0;

	if (use_video5) {
		/* Use video5 to give input for video10*/
		struct v4l2_format cformat;
		struct v4l2_requestbuffers creqbuf;
		struct v4l2_control control;
		struct v4l2_buffer buffer;

		int fd;
		int fd_save = 0;

		memset(&cformat, 0, sizeof(cformat));
		memset(&creqbuf, 0, sizeof(creqbuf));
		memset(&control, 0, sizeof(control));
		memset(&buffer, 0, sizeof(buffer));


		fd = open("/dev/video5", O_RDWR);
		if (fd <= 0) {
			printf("Could not open the cam device\n");
			return -1;
		}


		control.id = V4L2_CID_PRIVATE_OMAP3ISP_CSI2MEM;
		control.value = 1;
		if (ioctl(fd, VIDIOC_S_CTRL, &control) == -1) {
			printf("VIDIOC_S_CTRL failed!\n");
			return -1;
		} else {
			printf("V4L2_CID_PRIVATE_OMAP3ISP_CSI2MEM ="
				" %d\n", control.value);
		}

		cformat.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		cformat.fmt.pix.width = 3280;
		cformat.fmt.pix.height = 2464;
		cformat.fmt.pix.pixelformat = V4L2_PIX_FMT_UYVY;

		if (ioctl(fd, VIDIOC_S_FMT, &cformat)) {
			perror("VIDIOC_S_FMT");
			return -1;
		}

		creqbuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		creqbuf.memory = V4L2_MEMORY_USERPTR;
		creqbuf.count = 1;

		printf("Requesting %d \n", creqbuf.count);

		if (ioctl(fd, VIDIOC_REQBUFS, &creqbuf) < 0) {
			perror("VIDEO_REQBUFS");
			return -1;
		}
		printf("Camera Driver allowed buffers reqbuf.count = %d\n",
			creqbuf.count);

		cbuffers = calloc(creqbuf.count, sizeof(*cbuffers));

		/*
		mmap driver memory or allocate user memory,
		and queue each buffer
		*/

		buffer.type = creqbuf.type;
		buffer.memory = creqbuf.memory;
		buffer.index = 0;
		if (ioctl(fd, VIDIOC_QUERYBUF, &buffer) < 0) {
			perror("VIDIOC_QUERYBUF");
			return -1;
		}

		cbuffers[0].length = buffer.length + 0x1000;
		posix_memalign(&cbuffers[0].start, 0x1000, cbuffers[0].length);

		buffer.m.userptr = (unsigned int)cbuffers[0].start;

		printf("User Buffer [0].start = %x  length = %d\n",
			 cbuffers[0].start, cbuffers[0].length);

		if (ioctl(fd, VIDIOC_QBUF, &buffer) < 0) {
			perror("CAMERA VIDIOC_QBUF");
			return -1;
		}


		/* turn on streaming */
		if (ioctl(fd, VIDIOC_STREAMON, &creqbuf.type) < 0) {
			perror("VIDIOC_STREAMON");
			return -1;
		}

		/* De-queue the next avaliable buffer */
		if (ioctl(fd, VIDIOC_DQBUF, &buffer)) {
			perror("VIDIOC_DQBUF");
			return -1;
		}
		/* we didn't turn off streaming yet */

		creqbuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		if (ioctl(fd, VIDIOC_STREAMOFF, &creqbuf.type) == -1) {
			perror("VIDIOC_STREAMOFF");
			return -1;
		}

		control.id = V4L2_CID_PRIVATE_OMAP3ISP_CSI2MEM;
		control.value = 0;
		if (ioctl(fd, VIDIOC_S_CTRL, &control) == -1)
			printf("VIDIOC_S_CTRL failed!\n");
		else {
			printf("V4L2_CID_PRIVATE_OMAP3ISP_CSI2MEM ="
				" %d\n", control.value);
		}

		printf("Completed phase 1:\n");

		close(fd);

	}

    /* For csi2mem save the image here and exit*/
	opt_idx = get_option_index("-csi2mem", cmd_line, COUNT_OF(cmd_line));
	if (cmd_line[opt_idx].entered) {
		opt_idx = get_option_index("-ofile", cmd_line, \
				COUNT_OF(cmd_line));
		printf("Saving file from csi2mem \n");
	    Save_Image(cmd_line[opt_idx].o_dflt.v_char,
			       (char *) cbuffers[0].start, cbuffers[0].length);
		free(cbuffers);
		return 0;
    }

	/* index of input video device name */
	opt_idx = get_option_index("-idev", cmd_line, COUNT_OF(cmd_line));
	in_fd = open(cmd_line[opt_idx].o_dflt.v_char, O_RDWR);
	if (in_fd <= 0) {
		perror("Could not open the input device");
		return -1;
	}

	/*
	 * Input device operations
	 */
	memset(&v4l2_cap, 0, sizeof(v4l2_cap));
	/* V4L2 - Query capability */
	if (ioctl(in_fd, VIDIOC_QUERYCAP, &v4l2_cap) < 0) {
		perror("input: VIDIOC_QUERYCAP failed.");
		return -1;
	}


	if (!(v4l2_cap.capabilities &
	      (V4L2_CAP_STREAMING | V4L2_CAP_VIDEO_OUTPUT))) {
		printf("The video driver is not capable of "
		       "Streaming and recieving frames!\n");
		return -1;
	}

	opt_idx = get_option_index("-outputinfo", cmd_line, COUNT_OF(cmd_line));
	if (cmd_line[opt_idx].entered) {
		if (!(strcmp(cmd_line[opt_idx].o_dflt.v_char, \
							"video_querycap"))) {

			opt_idx = get_option_index("-idev", \
					cmd_line, COUNT_OF(cmd_line));

			printf("\t ======================================\n ");
			printf("\t %s name is %s. \n", \
			cmd_line[opt_idx].o_dflt.v_char, v4l2_cap.card);
		}
		if (!(strcmp(cmd_line[opt_idx].o_dflt.v_char, "sensor_info"))) {
			struct omap34xxcam_sensor_info sensor_info;

			if (ioctl(in_fd, \
			VIDIOC_PRIVATE_OMAP34XXCAM_SENSOR_INFO, \
			&sensor_info) < 0) {
				perror("output: \
				VIDIOC_PRIVATE_OMAP34XXCAM_SENSOR_INFO \
				failed.");
				return -1;
			}

			opt_idx = get_option_index("-idev", cmd_line, \
					COUNT_OF(cmd_line));

			printf("\t ======================================\n ");
			printf("\t Sensor info for %s \n", \
					cmd_line[opt_idx].o_dflt.v_char);
			printf("\t active size - ");
			printf("Left=%d Top=%d Widht=%d Height=%d\n", \
					sensor_info.active_size.left, \
					sensor_info.active_size.top, \
					sensor_info.active_size.width, \
					sensor_info.active_size.height);
			printf("\t full size - ");
			printf("Left=%d Top=%d Widht=%d Height=%d\n", \
					sensor_info.full_size.left, \
					sensor_info.full_size.top, \
					sensor_info.full_size.width, \
					sensor_info.full_size.height);
			printf("\t pixel size - ");
			printf(" %d:%d\n", sensor_info.pixel_size.width, \
					sensor_info.pixel_size.height);
			printf("\t current_xclk - %dMhz\n", \
					sensor_info.current_xclk);


		}
			close(in_fd);
			return 0;
	}

	/* Setup input parameters */
	ccp_setup_user_input(&v4l2_fmt.fmt.pix, cmd_line, COUNT_OF(cmd_line));
	v4l2_fmt.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
	/* V4L2 - Set format */
	if (ioctl(in_fd, VIDIOC_S_FMT, &v4l2_fmt) < 0) {
		perror("input: VIDIOC_S_FMT failed.");
		return -1;
	}
	fprintf(stderr, "\nIW:%4d IH:%4d\n", v4l2_fmt.fmt.pix.width,
			v4l2_fmt.fmt.pix.height);

	memset(&v4l2_req, 0, sizeof(v4l2_req));
	v4l2_req.type	= v4l2_fmt.type;
	v4l2_req.memory	= V4L2_MEMORY_USERPTR;
	v4l2_req.count	= USED_BUFF;
	/* V4L2 - Request buffers */
	if (ioctl(in_fd, VIDIOC_REQBUFS, &v4l2_req) < 0) {
		perror("input: VIDEO_REQBUFS failed.");
		return -1;
	}

	memset(&src_buff, 0, sizeof(src_buff));
	src_buff.type	= v4l2_req.type;
	src_buff.index	= 0;

	/* V4L2 - Query */
	if (ioctl(in_fd, VIDIOC_QUERYBUF, &src_buff) < 0) {
		perror("input: VIDIOC_QUERYBUF failed.");
		return -1;
	}

	opt_idx = get_option_index("-mem2ccp", cmd_line, COUNT_OF(cmd_line));
	if (cmd_line[opt_idx].entered) {

		src_buff.length	= ALIGN(v4l2_fmt.fmt.pix.width * 2 *
					v4l2_fmt.fmt.pix.height, 4096);

       opt_idx = get_option_index("-ifile", cmd_line, COUNT_OF(cmd_line));
       printf("loading image from %s \n", cmd_line[opt_idx].o_dflt.v_char);

		posix_memalign((void **) &src_buff.m.userptr, 4096, \
							src_buff.length);

       if (Load_Image(cmd_line[opt_idx].o_dflt.v_char,
			(char *)src_buff.m.userptr, src_buff.length)) {
			perror("Loading file");
       }

   } else {
       src_buff.m.userptr = (int)cbuffers[0].start;
   }

	/* V4L2 - Queue */
	if (ioctl(in_fd, VIDIOC_QBUF, &src_buff) < 0) {
		perror("input: VIDIOC_QBUF failed.");
		return -1;
	}

	/* index of input video device name */
	opt_idx = get_option_index("-odev", cmd_line, COUNT_OF(cmd_line));
	out_fd = open(cmd_line[opt_idx].o_dflt.v_char, O_RDWR);
	if (out_fd <= 0) {
		perror("Could not open the output device");
		return -1;
	}

	/*
	 * Output device operations
	 */
	memset(&v4l2_cap, 0, sizeof(v4l2_cap));
	if (ioctl(out_fd, VIDIOC_QUERYCAP, &v4l2_cap) < 0) {
		perror("output: VIDIOC_QUERYCAP failed.");
		return -1;
	}
	if (!(v4l2_cap.capabilities &
		  (V4L2_CAP_STREAMING | V4L2_CAP_VIDEO_CAPTURE))) {
		printf("The video driver is not capable of "
		       "Streaming and recieving frames!\n");
		return -1;
	}

	opt_idx = get_option_index("-color", cmd_line, COUNT_OF(cmd_line));
	if (cmd_line[opt_idx].entered) {
		if (!(strcmp(cmd_line[opt_idx].o_dflt.v_char, "BW")))
			colorLevel = V4L2_COLORFX_BW;
		if (!(strcmp(cmd_line[opt_idx].o_dflt.v_char, "SEPIA")))
			colorLevel = V4L2_COLORFX_SEPIA;

		queryctrl.id = V4L2_CID_COLORFX;
		if (ioctl(out_fd, VIDIOC_QUERYCTRL, &queryctrl) == -1) {
			printf("COLOR effect is not supported!\n");
			return -1;
		}

		control.id = V4L2_CID_COLORFX;
		if (ioctl(out_fd, VIDIOC_G_CTRL, &control) == -1)
			printf("VIDIOC_G_CTRL failed!\n");

		printf("Color effect at the beginning of the test " \
			"is supported - min %d, max %d default_value %d.\n",
			queryctrl.minimum, queryctrl.maximum, \
			queryctrl.default_value);

		control.value = colorLevel;
		if (ioctl(out_fd, VIDIOC_S_CTRL, &control) == -1)
			printf("VIDIOC_S_CTRL COLOR failed!\n");

		if (ioctl(out_fd, VIDIOC_G_CTRL, &control) == -1)
			printf("VIDIOC_G_CTRL failed!\n");

		printf("Color effect values after setup is supported, min %d, "
			"max %d.\nCurrent color is level is %d\n",
			queryctrl.minimum, queryctrl.maximum, control.value);

	}

	opt_idx = get_option_index("-brightness", cmd_line, COUNT_OF(cmd_line));
	if (cmd_line[opt_idx].entered) {

		queryctrl.id = V4L2_CID_BRIGHTNESS;
		if (ioctl(out_fd, VIDIOC_QUERYCTRL, &queryctrl) == -1) {
			printf("V4L2_CID_BRIGHTNESS is not supported!\n");
			return -1;
		}

		control.id = V4L2_CID_BRIGHTNESS;
		if (ioctl(out_fd, VIDIOC_G_CTRL, &control) == -1)
			printf("VIDIOC_G_CTRL failed!\n");

		printf("Brightness is supported - minimum %d, "
			"maximum %d and default value %d.\n",
			queryctrl.minimum, queryctrl.maximum, \
			queryctrl.default_value);

		control.value = atoi(cmd_line[opt_idx].o_dflt.v_char);

		if (!(strcmp(cmd_line[opt_idx].o_dflt.v_char, "minimum")))
			control.value = queryctrl.minimum;
		if (!(strcmp(cmd_line[opt_idx].o_dflt.v_char, "maximum")))
			control.value = queryctrl.maximum;
		if (!(strcmp(cmd_line[opt_idx].o_dflt.v_char, "default")))
			control.value = queryctrl.default_value;

		if (ioctl(out_fd, VIDIOC_S_CTRL, &control) == -1)
			printf("VIDIOC_S_CTRL contrast failed!\n");
		else
			printf("Setting brightness to %d\n", control.value);
	} else {
		/* Setting the default value reported by the kernel */
		queryctrl.id = V4L2_CID_BRIGHTNESS;
		if (ioctl(out_fd, VIDIOC_QUERYCTRL, &queryctrl) == -1)
			printf("V4L2_CID_BRIGHTNESS is not supported!\n");

		control.id = V4L2_CID_BRIGHTNESS;
		control.value = queryctrl.default_value;
		if (ioctl(out_fd, VIDIOC_S_CTRL, &control) == -1)
			printf("VIDIOC_S_CTRL brightness failed!\n");
		else
			printf("Setting brightness to default \n");
	}

	opt_idx = get_option_index("-contrast", cmd_line, COUNT_OF(cmd_line));
	if (cmd_line[opt_idx].entered) {

		queryctrl.id = V4L2_CID_CONTRAST;
		if (ioctl(out_fd, VIDIOC_QUERYCTRL, &queryctrl) == -1) {
			printf("V4L2_CID_CONTRAST is not supported!\n");
			return -1;
		}

		control.id = V4L2_CID_CONTRAST;
		if (ioctl(out_fd, VIDIOC_G_CTRL, &control) == -1)
			printf("VIDIOC_G_CTRL failed!\n");

		printf("Contrast is supported - minimum %d, "
			"maximum %d and default value %d.\n",
			queryctrl.minimum, queryctrl.maximum, \
					queryctrl.default_value);

		control.value = atoi(cmd_line[opt_idx].o_dflt.v_char);

		if (!(strcmp(cmd_line[opt_idx].o_dflt.v_char, "minimum")))
			control.value = queryctrl.minimum;
		if (!(strcmp(cmd_line[opt_idx].o_dflt.v_char, "maximum")))
			control.value = queryctrl.maximum;
		if (!(strcmp(cmd_line[opt_idx].o_dflt.v_char, "default")))
			control.value = queryctrl.default_value;

		if (ioctl(out_fd, VIDIOC_S_CTRL, &control) == -1)
			printf("VIDIOC_S_CTRL contrast failed!\n");
		else
			printf("Settings contrast to %d\n", control.value);
	} else {
		/* Setting the default value reported by the kernel */
		queryctrl.id = V4L2_CID_CONTRAST;
		if (ioctl(out_fd, VIDIOC_QUERYCTRL, &queryctrl) == -1)
			printf("V4L2_CID_CONTRAST is not supported!\n");
		control.id = V4L2_CID_CONTRAST;
		control.value = queryctrl.default_value;
		if (ioctl(out_fd, VIDIOC_S_CTRL, &control) == -1)
			printf("VIDIOC_S_CTRL contrast failed!\n");
		else
			printf("Settings contrast to default \n");
	}

	/* Setup input parameters */
	ccp_setup_user_output(&v4l2_fmt.fmt.pix, cmd_line, COUNT_OF(cmd_line));
	v4l2_fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

	/* V4L2 - Set format */
	if (ioctl(out_fd, VIDIOC_S_FMT, &v4l2_fmt) < 0) {
		perror("output: VIDIOC_S_FMT failed.");
		return -1;
	}


	opt_idx = get_option_index("-zoom", cmd_line, COUNT_OF(cmd_line));
	if (cmd_line[opt_idx].entered) {
		zoomFactor = cmd_line[opt_idx].o_dflt.v_word;
		if (crop(out_fd, zoomFactor) < 0)
			return -1;

	}
	fprintf(stderr, "\nOW:%4d OH:%4d\n", v4l2_fmt.fmt.pix.width,
			v4l2_fmt.fmt.pix.height);


	memset(&v4l2_req, 0, sizeof(v4l2_req));
	v4l2_req.type	= v4l2_fmt.type;
	v4l2_req.memory	= V4L2_MEMORY_USERPTR;
	v4l2_req.count	= USED_BUFF;

	/* V4L2 - Request buffers */
	if (ioctl(out_fd, VIDIOC_REQBUFS, &v4l2_req) < 0) {
		perror("output: VIDEO_REQBUFS failed.");
		return -1;
	}

	memset(&dst_buff, 0, sizeof(dst_buff));
	dst_buff.type	= v4l2_req.type;
	dst_buff.index	= 0;

	/* V4L2 - Query */
	if (ioctl(out_fd, VIDIOC_QUERYBUF, &dst_buff) < 0) {
		perror("output: VIDIOC_QUERYBUF failed.");
		return -1;
	}

	/* allocate output data buffer */
	posix_memalign((void **) &dst_buff.m.userptr, PAGE_SIZE,
				   dst_buff.length);
	if (!dst_buff.m.userptr) {
		perror("input: Can't allocate memory");
		return -1;
	}


	/* V4L2 - Queue */
	if (ioctl(out_fd, VIDIOC_QBUF, &dst_buff) < 0) {
		perror("input: VIDIOC_QBUF failed.");
		return -1;
	}

	opt_idx = get_option_index("-h3a", cmd_line, COUNT_OF(cmd_line));
	if (cmd_line[opt_idx].entered &&
	   (!(strcmp(cmd_line[opt_idx].o_dflt.v_char, "obtain")))) {

		/* V4L2 Video Event handling */
		struct v4l2_event_subscription cam_sub;
		struct v4l2_event cam_ev;
		fd_set excfds;
		int ret;

		unsigned int num_windows = 0;
		unsigned int buff_size = 0;
		struct isph3a_aewb_config aewb_config_user;
		struct isph3a_aewb_data aewb_data_user;
		__u8 *stats_buff = NULL;
		unsigned int buff_prev_size = 0;
		int frame_number;

		/* H3A params */
		aewb_config_user.saturation_limit = 0x1FF;
		aewb_config_user.win_height = 10;
		aewb_config_user.win_width = 10;
		aewb_config_user.ver_win_count = 2;
		aewb_config_user.hor_win_count = 3;
		aewb_config_user.ver_win_start = 3;
		aewb_config_user.hor_win_start = 10;
		aewb_config_user.blk_ver_win_start = 30;
		/* blk_win_height MUST NOT be the same height as win_height*/
		aewb_config_user.blk_win_height = 8;
		aewb_config_user.subsample_ver_inc = 2;
		aewb_config_user.subsample_hor_inc = 2;
		aewb_config_user.alaw_enable = 1;
		aewb_config_user.aewb_enable = 1;

		/* set h3a params */
		ret = ioctl(out_fd, VIDIOC_PRIVATE_ISP_AEWB_CFG, \
						&aewb_config_user);
		if (ret < 0) {
			printf("Error: %d, ", ret);
			perror("ISP_AEWB_CFG 1");
			return ret;
		}

		/************************************************************/
		/* Subscribe to internal SCM AEWB_DONE event */

		cam_sub.type = V4L2_EVENT_OMAP3ISP_AEWB;

		ret = ioctl(out_fd, VIDIOC_SUBSCRIBE_EVENT, &cam_sub);
		if (ret < 0)
			perror("subscribe()");

		printf("Subscribed for SCM AEWB_DONE event.\n");

		/* Init file descriptor list to check with select call */
		FD_ZERO(&excfds);
		FD_SET(out_fd, &excfds);

		v4l2_req.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
		/* V4L2 - Stream on */
		if (ioctl(in_fd, VIDIOC_STREAMON, &v4l2_req.type) < 0) {
			perror("input: VIDIOC_STREAMON failed.");
			return -1;
		}

		v4l2_req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

		/* V4L2 - Stream on */
		if (ioctl(out_fd, VIDIOC_STREAMON, &v4l2_req.type) < 0) {
			perror("output: VIDIOC_STREAMON failed.");
			return -1;
		}

		/* V4L2 - DeQueue */
		if (ioctl(out_fd, VIDIOC_DQBUF, &dst_buff) < 0) {
			perror("output: VIDIOC_DQBUF failed.");
			return -1;
		}

		/* V4L2 - Queue */
		if (ioctl(out_fd, VIDIOC_QBUF, &dst_buff) < 0) {
			perror("input: VIDIOC_QBUF failed.");
			return -1;
		}

		/************************************************************/
		/* Wait for H3A event */

		printf("Syncup on frame number, before starting streaming\n");
		ret = wait_for_h3a_event(out_fd, &excfds, &cam_ev);
		if (ret != 0) {
			perror("Failed wait_for_h3a_event()");
			return -1;
		}
		/************************************************************/

		num_windows = ((aewb_config_user.ver_win_count
				* aewb_config_user.hor_win_count)
				+ aewb_config_user.hor_win_count);
		buff_size = ((num_windows + (num_windows / 8) +
			((num_windows % 8) ? 1 : 0)) * BYTES_PER_WINDOW);
		buff_prev_size = (buff_size / 2);

		stats_buff = malloc(buff_size);

		aewb_data_user.h3a_aewb_statistics_buf = NULL;

		/* Digital gain in U10Q8 format */
		aewb_data_user.dgain = 0x100;	/* Gain = 1.000 */
		/* Particular gains in U8Q5 format */
		aewb_data_user.wb_gain_b = 0x94;
		aewb_data_user.wb_gain_r = 0x68;
		aewb_data_user.wb_gain_gb = 0x5C;
		aewb_data_user.wb_gain_gr = 0x5C;

		aewb_data_user.update = (SET_COLOR_GAINS | SET_DIGITAL_GAIN);
		aewb_data_user.frame_number = 8; /* dummy */

		printf("Setting first parameters \n");
		ret = ioctl(out_fd, VIDIOC_PRIVATE_ISP_AEWB_REQ, \
							&aewb_data_user);
		if (ret < 0) {
			perror("ISP_AEWB_REQ 1");
			return ret;
		}

		/* Request stats */
		aewb_data_user.frame_number = aewb_data_user.curr_frame - 1;
		frame_number = aewb_data_user.frame_number;
		printf("Requesting stats for frame %d\n", frame_number);

		aewb_data_user.update = REQUEST_STATISTICS;
		aewb_data_user.h3a_aewb_statistics_buf = stats_buff;
		ret = ioctl(out_fd,  VIDIOC_PRIVATE_ISP_AEWB_REQ, \
							&aewb_data_user);

		/* Display stats */
		if (!ret)
			display_h3a_stats(num_windows,
						buff_prev_size,
						&aewb_data_user,
						&aewb_config_user);
		else
			printf("No stats, current frame is %d.\n",
				aewb_data_user.curr_frame);

	} else {
		v4l2_req.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
		/* V4L2 - Stream on */
		if (ioctl(in_fd, VIDIOC_STREAMON, &v4l2_req.type) < 0) {
			perror("input: VIDIOC_STREAMON failed.");
			return -1;
		}

		v4l2_req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

		/* V4L2 - Stream on */
		if (ioctl(out_fd, VIDIOC_STREAMON, &v4l2_req.type) < 0) {
			perror("output: VIDIOC_STREAMON failed.");
			return -1;
		}
	}

	/* V4L2 - DeQueue */
	if (ioctl(out_fd, VIDIOC_DQBUF, &dst_buff) < 0) {
		perror("output: VIDIOC_DQBUF failed.");
		return -1;
	}

	/* index of options for input file name */
	opt_idx = get_option_index("-ofile", cmd_line, COUNT_OF(cmd_line));

	/* Save file */
	Save_Image(cmd_line[opt_idx].o_dflt.v_char,
			   (char *) dst_buff.m.userptr, dst_buff.length);

	v4l2_req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

	/* V4L2 - Stream off */
	if (ioctl(out_fd, VIDIOC_STREAMOFF, &v4l2_req.type) < 0) {
		perror("output: VIDIOC_STREAMOFF failed.");
		return -1;
	}

	v4l2_req.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
	/* V4L2 - Stream off */
	if (ioctl(in_fd, VIDIOC_STREAMOFF, &v4l2_req.type) < 0) {
		perror("input: VIDIOC_STREAMOFF failed.");
		return -1;
	}

	if (src_buff.m.userptr)
		free((void *) src_buff.m.userptr);
	if (dst_buff.m.userptr)
		free((void *) dst_buff.m.userptr);
	free(cbuffers);

	printf("Exiting...\n");
	close(in_fd);
	close(out_fd);

	return 0;
}
