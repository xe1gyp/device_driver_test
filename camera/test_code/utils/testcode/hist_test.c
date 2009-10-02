/* =========================================================================
*             Texas Instruments OMAP(TM) Platform Software
*  (c) Copyright Texas Instruments, Incorporated.  All Rights Reserved.
*
*  Use of this software is controlled by the terms and conditions found
*  in the license agreement under which this software has been supplied.
* ========================================================================== */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/mman.h>
#include <linux/videodev2.h>
#include <mach/isp_user.h>

#define INPUT_CCDC		0
#define INPUT_MEMORY		1
#define VIDEO_DEVICE		"/dev/video2"

#define HIST_MEM_SIZE		1024

#define DEFAULT_PIXEL_FMT "YUYV"
#define DEFAULT_VIDEO_SIZE "QVGA"
#define DSS_STREAM_START_FRAME	3

__u16 speed_test_results[40][2];

void hist_output(__u32 *hist_data, int hist_cnt_bins, FILE *fp_out)
{
	int c0, c1, c2, c3;
	int color_offset;
	int region = 0, region_offset = 0;
	int k;

	printf("\nWriting statistics to hist_data.out file\n");
	color_offset = (256 >> (3 - hist_cnt_bins));
	printf("After color offset\n");
	fprintf(fp_out, " ===LINUX BASEPORT HISTOGRAM DRIVER OUTPUT===\n\n");
	fprintf(fp_out, "BIN value\tColor0\tColor1\tColor2\tColor3\n");
	fprintf(fp_out, "=========\t======\t======\t======\t======\n\n");

	for (k = 0; k < 256; k++) {
		if ((k != 0) && (k % color_offset == 0)) {
			region++;
			region_offset = 4 * color_offset * region;
			if (region > 3)
				return;

			fprintf(fp_out, "\t\t\t\t---  Region %d  ---\n",
				region);
		}
		c0 = hist_data[(k % color_offset) + region_offset];
		c1 = hist_data[(k % color_offset) + region_offset +
				color_offset];
		c2 = hist_data[(k % color_offset) + region_offset +
				(2 * color_offset)];
		c3 = hist_data[(k % color_offset) + region_offset +
				(3 * color_offset)];

		fprintf(fp_out, "%d\t\t%6x\t%6x\t%6x\t%6x\n",
			(k % color_offset), c0, c1, c2, c3);
	}
}

void usage()
{
	printf("hist_test: Test the support of generation "
		"and collection of histogram\n");
	printf("           statistics, depending of input source "
		"the parameters are:\n\n");
	printf("\t<camDevice> Camera device to be open\n\t\t 1:Micron sensor "
					"2:OV sensor\n");
	printf("\t<framerate> is the framerate to be used, if no value"
				" is given \n\t           30 fps is default\n");
	printf("If input source is CCDC module:\n");
	printf("\t<input>  Input source (in this case CCDC)\n");
	printf("\t<bins>   Number of bins (0-32bins, 1-64 bins, "
		"2-128 bins, 3-256 bins)\n");
	printf("\t<frames> Number of frames to process\n");
	printf("\t<bitw>   Needed to know the size per pixel\n\n");
	printf("If input source is memory (funtion not ready):\n");
	printf("\t<input>  Input source (in this case MEM)\n");
	printf("\t<bins>   Number of bins (0-32bins, 1-64 bins, "
		"2-128 bins, 3-256 bins)\n");
	printf("\t<frames> Number of frames to process\n");
	printf("\t<bitw>   Needed to know the size per pixel\n");
	printf("\t<addr>   Frame-input address in memory \n");
	printf("\t<offset> Line-offset for frame-input)\n");
	printf("\t<hv_inf> Frame-input width and height\n");
}

int main(int argc, char *argv[])
{
	int ret, cfd, vfd;
	int i;
	struct isp_hist_config hist_user;
	struct isp_hist_data hist_data_user;
	int set_video_img = 0;
	int index = 1;
	int device = 1;
	FILE *fp_out;
	char *pixelFmt;
	int framerate = 15;

	struct {
		void *start;
		size_t length;
	} *vbuffers;
	struct v4l2_capability capability;
	struct v4l2_format cformat, vformat;
	struct v4l2_requestbuffers creqbuf, vreqbuf;
	struct v4l2_buffer cfilledbuffer, vfilledbuffer;

	if (argc > index) {
		if ((!strcmp(argv[1], "?")) ||
		    ((argc != 5) && !strcmp(argv[1], "CCDC")) ||
		    ((argc != 8) && !strcmp(argv[1], "MEM"))) {
			usage();
			return 0;
		}
	}
	fp_out = fopen("hist_data.out", "wb");
	if (fp_out == NULL) {
		printf("ERROR opening output file!\n");
		return -EACCES;
	}

	if (argc > index) {
		device = atoi(argv[index]);
		index++;
	}

	if (argc > index) {
		framerate = atoi(argv[index]);
		printf("Framerate = %d\n", framerate);
		index++;
	} else
		printf("Using framerate = 30, default value\n");

	cfd = open_cam_device(O_RDWR, device);
	if (cfd <= 0) {
		printf("Could not open the cam device\n");
		return -1;
	}

	if (argc > index) {
		pixelFmt = argv[index];
		index++;
		if (argc > index) {
			ret = validateSize(argv[index]);
			if (ret == 0) {
				ret = cam_ioctl(cfd, pixelFmt, argv[index]);
				if (ret < 0) {
					usage();
					return -1;
				}
			} else {
				index++;
				if (argc > (index)) {
					ret = cam_ioctl(cfd, pixelFmt,
						argv[index-1], argv[index]);
					if (ret < 0) {
						usage();
						return -1;
					}
				} else {
					printf("Invalid size\n");
					usage();
					return -1;
				}
			}
			index++;
		} else {
			printf("Setting QCIF as video size, default value\n");
			ret = cam_ioctl(cfd, pixelFmt, DEFAULT_VIDEO_SIZE);
			if (ret < 0)
				return -1;
		}
	} else {
		printf("Setting pixel format and video size with default"
								" values\n");
		ret = cam_ioctl(cfd, DEFAULT_PIXEL_FMT, DEFAULT_VIDEO_SIZE);
		if (ret < 0)
			return -1;
	}

	ret = setFramerate(cfd, framerate);
	if (ret < 0) {
		printf("ERROR: VIDIOC_S_PARM ioctl cam\n");
		return -1;
	}

	vfd = open(VIDEO_DEVICE, O_RDWR);
	if (vfd <= 0) {
		printf("Could not open %s\n", VIDEO_DEVICE);
		return -1;
	} else
		printf("openned %s for rendering\n", VIDEO_DEVICE);

	if (ioctl(vfd, VIDIOC_QUERYCAP, &capability) == -1) {
		perror("video VIDIOC_QUERYCAP");
		return -1;
	}
	if (capability.capabilities & V4L2_CAP_STREAMING)
		printf("The video driver is capable of Streaming!\n");
	else {
		printf("The video driver is not capable of Streaming!\n");
		return -1;
	}

	if (ioctl(cfd, VIDIOC_QUERYCAP, &capability) < 0) {
		perror("VIDIOC_QUERYCAP");
		return -1;
	}
	if (capability.capabilities & V4L2_CAP_STREAMING)
		printf("The camera driver is capable of Streaming!\n");
	else {
		printf("The camera driver is not capable of Streaming!\n");
		return -1;
	}

	cformat.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	ret = ioctl(cfd, VIDIOC_G_FMT, &cformat);
	if (ret < 0) {
		perror("cam VIDIOC_G_FMT");
		return -1;
	}
	printf("Camera Image width = %d, Image height = %d, size = %d\n",
			cformat.fmt.pix.width, cformat.fmt.pix.height,
			cformat.fmt.pix.sizeimage);

	vformat.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
	ret = ioctl(vfd, VIDIOC_G_FMT, &vformat);
	if (ret < 0) {
		perror("video VIDIOC_G_FMT");
		return -1;
	}
	printf("Video Image width = %d, Image height = %d, size = %d\n",
			vformat.fmt.pix.width, vformat.fmt.pix.height,
						vformat.fmt.pix.sizeimage);

	if ((cformat.fmt.pix.width != vformat.fmt.pix.width) ||
	    (cformat.fmt.pix.height != vformat.fmt.pix.height)) {
		printf("image sizes don't match!\n");
		set_video_img = 1;
	}
	if (cformat.fmt.pix.pixelformat != vformat.fmt.pix.pixelformat) {
		printf("pixel formats don't match!\n");
		set_video_img = 1;
	}

	if (set_video_img) {
		printf("set video image the same as camera image ...\n");
		vformat.fmt.pix.width = cformat.fmt.pix.width;
		vformat.fmt.pix.height = cformat.fmt.pix.height;
		vformat.fmt.pix.sizeimage = cformat.fmt.pix.sizeimage;
		vformat.fmt.pix.pixelformat = cformat.fmt.pix.pixelformat;
		ret = ioctl(vfd, VIDIOC_S_FMT, &vformat);
		if (ret < 0) {
			perror("video VIDIOC_S_FMT");
			return -1;
		}
		if ((cformat.fmt.pix.width != vformat.fmt.pix.width) ||
			(cformat.fmt.pix.height != vformat.fmt.pix.height) ||
			(cformat.fmt.pix.pixelformat !=
						 vformat.fmt.pix.pixelformat)) {
			printf("can't make camera and video image "
							"compatible!\n");
			return 0;
		}
	}

	vreqbuf.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
	vreqbuf.memory = V4L2_MEMORY_MMAP;
	vreqbuf.count = 4;
	if (ioctl(vfd, VIDIOC_REQBUFS, &vreqbuf) == -1) {
		perror("video VIDEO_REQBUFS");
		return;
	}
	printf("Video Driver allocated %d buffers when 4 are requested\n",
								vreqbuf.count);

	vbuffers = calloc(vreqbuf.count, sizeof(*vbuffers));
	for (i = 0; i < vreqbuf.count ; ++i) {
		struct v4l2_buffer buffer;
		buffer.type = vreqbuf.type;
		buffer.index = i;
		if (ioctl(vfd, VIDIOC_QUERYBUF, &buffer) == -1) {
			perror("video VIDIOC_QUERYBUF");
			return;
		}
		vbuffers[i].length = buffer.length;
		vbuffers[i].start = mmap(NULL, buffer.length, PROT_READ|
						PROT_WRITE, MAP_SHARED,
						vfd, buffer.m.offset);
		if (vbuffers[i].start == MAP_FAILED) {
			perror("video mmap");
			return;
		}
		printf("Video Buffers[%d].start = %x  length = %d\n", i,
					vbuffers[i].start, vbuffers[i].length);
	}

	creqbuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	creqbuf.memory = V4L2_MEMORY_USERPTR;
	creqbuf.count = 4;
	printf("Requesting %d buffers of type V4L2_MEMORY_USERPTR\n",
								creqbuf.count);
	if (ioctl(cfd, VIDIOC_REQBUFS, &creqbuf) < 0) {
		perror("cam VIDEO_REQBUFS");
		return -1;
	}
	printf("Camera Driver allowed %d buffers\n", creqbuf.count);

	for (i = 0; i < creqbuf.count; ++i) {
		struct v4l2_buffer buffer;
		buffer.type = creqbuf.type;
		buffer.memory = creqbuf.memory;
		buffer.index = i;
		if (ioctl(cfd, VIDIOC_QUERYBUF, &buffer) < 0) {
			perror("cam VIDIOC_QUERYBUF");
			return -1;
		}

		buffer.flags = 0;
		buffer.m.userptr = (unsigned long)vbuffers[i].start;
		buffer.length = vbuffers[i].length;

		if (ioctl(cfd, VIDIOC_QBUF, &buffer) < 0) {
			perror("cam VIDIOC_QBUF");
			return -1;
		}
	}

	/* turn on streaming on both drivers */
	if (ioctl(cfd, VIDIOC_STREAMON, &creqbuf.type) < 0) {
		perror("cam VIDIOC_STREAMON");
		return -1;
	}

	/* caputure 1000 frames */
	cfilledbuffer.type = creqbuf.type;
	vfilledbuffer.type = vreqbuf.type;
	i = 0;

	while (i < 2+DSS_STREAM_START_FRAME) {
		int aux = 0;
		/* De-queue the next avaliable buffer */
		while (ioctl(cfd, VIDIOC_DQBUF, &cfilledbuffer) < 0)
			perror("cam VIDIOC_DQBUF");

		vfilledbuffer.index = cfilledbuffer.index;
		vfilledbuffer.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
		vfilledbuffer.memory = V4L2_MEMORY_MMAP;
		vfilledbuffer.m.userptr = 
			(unsigned int)(vbuffers[cfilledbuffer.index].start);
		vfilledbuffer.length = cfilledbuffer.length;
		if (ioctl(vfd, VIDIOC_QBUF, &vfilledbuffer) < 0) {
			perror("dss VIDIOC_QBUF");
			return -1;
		}
		i++;

		if (i == DSS_STREAM_START_FRAME) {
			/* Turn on streaming for video */
			if (ioctl(vfd, VIDIOC_STREAMON, &vreqbuf.type)) {
				perror("dss VIDIOC_STREAMON");
				return -1;
			}
		}

		if (i >= DSS_STREAM_START_FRAME) {
			/* De-queue the previous buffer from video driver */
			if (ioctl(vfd, VIDIOC_DQBUF, &vfilledbuffer)) {
				perror("dss VIDIOC_DQBUF");
				return;
			}

			cfilledbuffer.index = vfilledbuffer.index;
			while (ioctl(cfd, VIDIOC_QBUF, &cfilledbuffer) < 0)
				perror("cam VIDIOC_QBUF");


	/* ************************* HIST TEST **************************** */

		/* Start the hist test after the DSS stream-on */
		/* HIST params */
		if (argc > 1
		    && (!strcmp(argv[1], "CCDC") || !strcmp(argv[1], "MEM"))) {
			if (!strcmp(argv[1], "CCDC")) {
				hist_user.hist_source = INPUT_CCDC;
				hist_user.input_bit_width = atoi(argv[4]);
				hist_user.hist_frames = atoi(argv[3]);
				hist_user.hist_h_v_info = 0;
				hist_user.hist_radd = 0;
				hist_user.hist_radd_off = 0;
				hist_user.hist_bins = atoi(argv[2]);
				switch (hist_user.hist_bins) {
				case 0:
				case 1:
					hist_user.num_regions = 3;
					break;
				case 2:
					hist_user.num_regions = 1;
					break;
				case 3:
					hist_user.num_regions = 0;
					break;
				}

			} else {
				/*hist_user.hist_source = INPUT_MEMORY;
				   hist_user.input_bit_width = atoi(argv[4]);
				   hist_user.hist_frames = atoi(argv[3]);
				   hist_user.hist_h_v_info = atoi(argv[7]);
				   hist_user.hist_radd = atoi(argv[5]);
				   hist_user.hist_radd_off = atoi(argv[6]);
				   hist_user.hist_bins = atoi(argv[2]); */
				printf("Using default values for Histogram\n");
				/* CCDC or Memory */
				hist_user.hist_source = 0;
				/* Needed o know the size per pixel */
				hist_user.input_bit_width = 10;
				/* # of frames to process and accumulate */
				hist_user.hist_frames = 10;
				/* frame-input w and h, if source is memory */
				hist_user.hist_h_v_info = 0;
				/* frame-input address in memory */
				hist_user.hist_radd = 0;
				/* line-offset for frame-input */
				hist_user.hist_radd_off = 0;
				/* number of bins: 32, 64, 128, or 256 0 - 3 */
				hist_user.hist_bins = 0;
				/* number of regions to be configured 0 - 3 */
				hist_user.num_regions = 3;
			}
		} else {
			printf("Using default values for Histogram\n");
			/* CCDC or Memory */
			hist_user.hist_source = 0;
			/* Needed o know the size per pixel */
			hist_user.input_bit_width = 10;
			/* Numbers of frames to be processed and accumulated */
			hist_user.hist_frames = 5;
			/* frame-input width and height if source is memory */
			hist_user.hist_h_v_info = 0;
			/* frame-input address in memory */
			hist_user.hist_radd = 0;
			/* line-offset for frame-input */
			hist_user.hist_radd_off = 0;
			/* number of bins: 32, 64, 128, or 256 0 - 3 */
			hist_user.hist_bins = BINS_256;
			/* number of regions to be configured 0 - 3 */
			hist_user.num_regions = 0;
		}

		hist_user.wb_gain_R = 32; /* WB Field-to-Pattern Assignments */
		hist_user.wb_gain_RG = 32; /* WB Field-to-Pattern Assignments */
		hist_user.wb_gain_B = 32; /* WB Field-to-Pattern Assignments */
		hist_user.wb_gain_BG = 32; /* WB Field-to-Pattern Assignments */

		hist_user.reg0_hor = 640;	/* Region 0 size and position */
		hist_user.reg0_ver = 480;	/* Region 0 size and position */
		/* hist_user.reg1_hor = 320; */	/* Region 0 size and position */
		/* hist_user.reg1_ver = 240; */	/* Region 0 size and position */

		/* set h3a params */
		ret = ioctl(cfd, VIDIOC_PRIVATE_ISP_HIST_CFG, &hist_user);
		if (ret < 0) {
			printf("Error: %d, ", ret);
			perror("VIDIOC_ISP_HIST");
			return ret;

		}
		printf("VIDIOC_ISP_HIST successful\n");

		sleep(5);

		hist_data_user.hist_statistics_buf =
					(__u32 *)malloc(HIST_MEM_SIZE * 4);
		ret = ioctl(cfd, VIDIOC_PRIVATE_ISP_HIST_REQ, &hist_data_user);
		if (ret < 0) {
			printf("VIDIOC_ISP_HISTREQ Error: %d, \n", ret);
			perror("VIDIOC_ISP_HISTREQ\n");
			return ret;
		}

		printf("\nHIST: buffer to display = %p data pointer = %p\n",
		       hist_data_user.hist_statistics_buf,
		       hist_data_user.hist_statistics_buf);

		hist_output(hist_data_user.hist_statistics_buf,
			    hist_user.hist_bins, fp_out);
		/* Display stats */

		free(hist_data_user.hist_statistics_buf);
	}
/* ******************** ENDS H3A TEST ******************** */

	}


	if (ioctl(cfd, VIDIOC_STREAMOFF, &creqbuf.type) == -1) {
		perror("cam VIDIOC_STREAMOFF");
		return -1;
	}
	if (ioctl(vfd, VIDIOC_STREAMOFF, &vreqbuf.type) == -1) {
		perror("video VIDIOC_STREAMOFF");
		return -1;
	}

	for (i = 0; i < vreqbuf.count; i++) {
		if (vbuffers[i].start)
			munmap(vbuffers[i].start, vbuffers[i].length);
	}

	free(vbuffers);
	close(cfd);
	close(vfd);
}
