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
#include <unistd.h>
#include <errno.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <linux/fb.h>
#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/videodev.h>

#include <time.h>

#define PREVIEW_ROTATION_NO		0
#define PREVIEW_ROTATION_0		1
#define PREVIEW_ROTATION_90		2
#define PREVIEW_ROTATION_180		3
#define PREVIEW_ROTATION_270		4
#define INPUT_CCDC    0
#define INPUT_MEMORY  1
#define VIDEO_DEVICE1 "/dev/v4l/video1"
#define VIDEO_DEVICE2 "/dev/v4l/video2"
#define VIDIOC_S_OVERLAY_ROT		_IOW ('O', 1,  int)
#define VIDIOC_G_OVERLAY_ROT		_IOR ('O', 2,  int)

#define V4L2_CID_PRIVATE_ISP_HIST_CFG     (V4L2_CID_PRIVATE_BASE + 8)
#define V4L2_CID_PRIVATE_ISP_HIST_REQ     (V4L2_CID_PRIVATE_BASE + 9)
#define BYTES_PER_WINDOW	16

/* Flags for update field */
#define REQUEST_STATISTICS      (1 << 0)
#define SET_COLOR_GAINS         (1 << 1)
#define SET_DIGITAL_GAIN        (1 << 2)
#define SET_EXPOSURE            (1 << 3)
#define SET_ANALOG_GAIN         (1 << 4)

#define BINS_32                 0x0
#define BINS_64                 0x1
#define BINS_128                0x2
#define BINS_256                0x3

#define MAX_REGIONS             0x4
#define MAX_WB_GAIN             255
#define MIN_WB_GAIN             0x0
#define MAX_BIT_WIDTH           14
#define MIN_BIT_WIDTH           8

#define HIST_MEM_SIZE           1024

#ifndef u32
#define u32 unsigned long
#endif				/* u32 */

#ifndef u16
#define u16 unsigned short
#endif				/* u16 */

#ifndef u8
#define u8 unsigned char
#endif				/* u8 */

//#define CAP_UTILS
#ifdef CAP_UTILS
#include "CapUtils.h"
#else

#define DEFAULT_PIXEL_FMT "YUYV"
#define DEFAULT_VIDEO_SIZE "QCIF"

struct isp_hist_config {
	u8 hist_source;		/* CCDC or Memory */
	u8 input_bit_width;	/* Needed o know the size per pixel */
	u8 hist_frames;		/* Numbers of frames to be processed
				   and accumulated */
	u8 hist_h_v_info;	/* frame-input width and height if source
				   is memory */
	u16 hist_radd;		/* frame-input address in memory */
	u16 hist_radd_off;	/* line-offset for frame-input */
	u16 bins;		/* number of bins: 32, 64, 128, or 256 */
	u16 wb_gain_R;		/* White Balance Field-to-Pattern Assignments */
	u16 wb_gain_RG;		/* White Balance Field-to-Pattern Assignments */
	u16 wb_gain_B;		/* White Balance Field-to-Pattern Assignments */
	u16 wb_gain_BG;		/* White Balance Field-to-Pattern Assignments */
	u8 num_regions;		/* number of regions to be configured */
	u16 reg0_hor;		/* Region 0 size and position */
	u16 reg0_ver;		/* Region 0 size and position */
	u16 reg1_hor;		/* Region 1 size and position */
	u16 reg1_ver;		/* Region 1 size and position */
	u16 reg2_hor;		/* Region 2 size and position */
	u16 reg2_ver;		/* Region 2 size and position */
	u16 reg3_hor;		/* Region 3 size and position */
	u16 reg3_ver;		/* Region 3 size and position */
};

struct isp_hist_data {
	u32 *hist_statistics_buf;	/* Pointer to pass to user */
};
#endif

#ifdef CAP_UTILS
#define PIX_PER_WINDOW 25
h3a_aewb_paxel_data_t h3a_stats[H3A_AEWB_MAX_WIN_NUM];
h3a_aewb_paxel_data_t h3a_avg[1];
#endif

u16 speed_test_results[40][2];

void hist_output ( u32 *hist_data, int hist_cnt_bins, FILE *fp_out)
{
	int c0,c1,c2,c3;
	int color_offset;
	int region = 0, region_offset = 0;
	int k;

	printf("\nWriting statistics to hist_data.out file\n");
	color_offset = (256 >> (3 - hist_cnt_bins));
	printf("After color offset");
	fprintf(fp_out," ===LINUX BASEPORT HISTOGRAM DRIVER OUTPUT===\n\n");
	fprintf(fp_out, "BIN value\tColor0\tColor1\tColor2\tColor3\n");
	fprintf(fp_out, "=========\t======\t======\t======\t======\n\n");
	
	for(k = 0; k < 256; k++)
	{
		if((k != 0) && (k % color_offset == 0) )
		{
			region++;
			region_offset = 4 * color_offset * region;
			if(region > 3)
				return;
			
			fprintf(fp_out, "\t\t\t\t---  Region %d  ---\n", region);
		}
		c0 = hist_data[(k % color_offset) + region_offset];
		c1 = hist_data[(k % color_offset) + region_offset + color_offset];
		c2 = hist_data[(k % color_offset) + region_offset + (2*color_offset)];
		c3 = hist_data[(k % color_offset) + region_offset + (3*color_offset)];
		
		fprintf(fp_out, "%d\t\t%6x\t%6x\t%6x\t%6x\n", (k % color_offset),c0,c1,c2,c3);
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
	int ret, cfd, vfd, enable = 1;
	int i, j = 0;
	struct isp_hist_config hist_user;
	struct isp_hist_data hist_data_user;
	u32 *buff_preview;// = NULL;
	u32 *buff_char = NULL;
	unsigned int buff_prev_size = 0;
	int data8, data2, window, unsat_cnt;
	int input, new_gain = 0, set_video_img = 0;
	int frame_number;
	int index = 1;
	int device = 1;
	FILE *fp_out;
	char * pixelFmt;
	int framerate = 15;
	
	struct {
		void *start;
		size_t length;
	} *vbuffers;
	struct v4l2_capability capability;
	struct v4l2_format cformat, vformat;
	struct v4l2_requestbuffers creqbuf, vreqbuf;
	struct v4l2_buffer cfilledbuffer, vfilledbuffer;
	struct v4l2_control control_hist_config, control_hist_request;
	//hist_data_user.hist_statistics_buf = calloc(1,sizeof(u32));
	
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
		printf("Framerate = %d\n",framerate);
		index++;
	}
	else {
		printf("Using framerate = 30, default value\n");
	}
	
	if ((cfd = open_cam_device(O_RDWR,device)) <= 0) {
		printf ("Could not open the cam device\n");
		return -1;
	}
	
	if (argc > index){
		pixelFmt=argv[index];
		index++;
		if (argc > index){
			ret = validateSize(argv[index]);
			if (ret == 0) { 
				ret = cam_ioctl(cfd,pixelFmt,argv[index]);
				if (ret < 0) {
					usage();
					return -1;
				}
			}
			else {
				index++;
				if (argc > (index)) {
					ret = cam_ioctl(cfd,pixelFmt,
						argv[index-1], argv[index]);
					if (ret < 0) {
						usage();
						return -1;
					}
				}
				else {
					printf("Invalid size\n");
					usage();
					return -1;
				}
			}
			index++;
		}
		else {
			printf("Setting QCIF as video size, default value\n");
			ret = cam_ioctl(cfd,pixelFmt,DEFAULT_VIDEO_SIZE);
			if (ret < 0)
				return -1;
		}
	}
	else {
		printf("Setting pixel format and video size with default"
								" values\n");
		ret = cam_ioctl(cfd, DEFAULT_PIXEL_FMT, DEFAULT_VIDEO_SIZE);
		if (ret < 0)
			return -1;
	}
	
	ret = setFramerate(cfd,framerate);
	if (ret < 0){
		printf("ERROR: VIDIOC_S_PARM ioctl cam\n");
		return -1;
	}

	vfd = open (VIDEO_DEVICE2, O_RDWR);
	if (vfd <= 0) {
		printf("Could not open %s\n", VIDEO_DEVICE2);
		return -1;
	}
	else
		printf("openned %s for rendering\n", VIDEO_DEVICE2);
		
	if (ioctl(vfd, VIDIOC_QUERYCAP, &capability) == -1) {
		perror("video VIDIOC_QUERYCAP");
		return -1;
	}
	if ( capability.capabilities & V4L2_CAP_STREAMING)
		printf("The video driver is capable of Streaming!\n");
	else {
		printf("The video driver is not capable of Streaming!\n");
		return -1;
	}	

 	if (ioctl(cfd, VIDIOC_QUERYCAP, &capability) < 0) {
		perror("VIDIOC_QUERYCAP");
		return -1;
	}
	if ( capability.capabilities & V4L2_CAP_STREAMING)
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
			cformat.fmt.pix.width,cformat.fmt.pix.height, 
						cformat.fmt.pix.sizeimage);

	vformat.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
	ret = ioctl(vfd, VIDIOC_G_FMT, &vformat);
	if (ret < 0) {
	    perror("video VIDIOC_G_FMT");
	    return -1;
	}
	printf("Video Image width = %d, Image height = %d, size = %d\n", 
			vformat.fmt.pix.width,vformat.fmt.pix.height, 
						vformat.fmt.pix.sizeimage);
	
	if ((cformat.fmt.pix.width!=vformat.fmt.pix.width) ||
	    (cformat.fmt.pix.height!=vformat.fmt.pix.height) ||
	    (cformat.fmt.pix.sizeimage!=vformat.fmt.pix.sizeimage)) {
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
		ret = ioctl (vfd, VIDIOC_S_FMT, &vformat);
		if (ret < 0) {
			perror ("video VIDIOC_S_FMT");
			return -1;
		}
		if ((cformat.fmt.pix.width!=vformat.fmt.pix.width) ||
			(cformat.fmt.pix.height!=vformat.fmt.pix.height) ||
			(cformat.fmt.pix.sizeimage!=
						vformat.fmt.pix.sizeimage) ||
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
		perror ("video VIDEO_REQBUFS");
		return;
	}
	printf("Video Driver allocated %d buffers when 4 are requested\n", 
								vreqbuf.count);

	vbuffers = calloc(vreqbuf.count, sizeof(*vbuffers));
	for (i = 0; i < vreqbuf.count ; ++i) {
		struct v4l2_buffer buffer;
		buffer.type = vreqbuf.type;
		buffer.index = i;
		if (ioctl(vfd, VIDIOC_QUERYBUF, &buffer) == -1){
			perror("video VIDIOC_QUERYBUF");
			return;
		}
#if 0
		printf("video %d: buffer.length=%d, buffer.m.offset=%d\n",
				i, buffer.length, buffer.m.offset);
#endif
		vbuffers[i].length= buffer.length;
		vbuffers[i].start = mmap(NULL, buffer.length, PROT_READ|
						PROT_WRITE, MAP_SHARED,
						vfd, buffer.m.offset);
		if (vbuffers[i].start == MAP_FAILED ){
			perror ("video mmap");
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
		perror ("cam VIDEO_REQBUFS");
		return -1;
	}
	printf("Camera Driver allowed %d buffers\n", creqbuf.count);

	for (i = 0; i < creqbuf.count; ++i) {
		struct v4l2_buffer buffer;
		buffer.type = creqbuf.type;
		buffer.memory = creqbuf.memory;
		buffer.index = i;
		if(ioctl(cfd, VIDIOC_QUERYBUF, &buffer) < 0){
			perror("cam VIDIOC_QUERYBUF");
			return -1;
		}

		buffer.flags = 0;
		buffer.m.userptr = (unsigned long) vbuffers[i].start;
		buffer.length = vbuffers[i].length;
	       	
		if (ioctl(cfd, VIDIOC_QBUF, &buffer) < 0) {
			perror("cam VIDIOC_QBUF");
			return -1;
		}		
	}

	/* turn on streaming on both drivers */
	if (ioctl(cfd, VIDIOC_STREAMON, &creqbuf.type) < 0 ) {
		perror("cam VIDIOC_STREAMON");
		return -1;
	}
	if (ioctl(vfd, VIDIOC_STREAMON, &vreqbuf.type) < 0 ) {
		perror("video VIDIOC_STREAMON");
		return -1;
	}

	/* caputure 1000 frames */
	cfilledbuffer.type = creqbuf.type;
	vfilledbuffer.type = vreqbuf.type;
	i = 0;
	vfilledbuffer.index = -1;
	sleep(1);

while (i < 2) {
		int aux = 0;
		/* De-queue the next filled buffer from camera */
		while (ioctl(cfd, VIDIOC_DQBUF, &cfilledbuffer) < 0) {
			perror("cam VIDIOC_DQBUF");
			printf("ERROR FROM CAM DQ\n");
			while (ioctl(vfd, VIDIOC_QBUF, &cfilledbuffer) < 0) {
				perror("VIDIOC_QBUF***");
			}
		}
		i++;

		if (vfilledbuffer.index == -1) {
			vfilledbuffer.index = 0;
		}

		/* Queue the new buffer to video driver for rendering */
		if (ioctl(vfd, VIDIOC_QBUF, &vfilledbuffer) == -1) {
			perror("video VIDIOC_QBUF");
			return;
		}

		/* De-queue the previous buffer from video driver */
		if (ioctl(vfd, VIDIOC_DQBUF, &vfilledbuffer) < 0) {
			perror("cam VIDIOC_DQBUF");
			return;
		}

		vfilledbuffer.index = cfilledbuffer.index;

		/* queue the buffer back to camera */
		while (ioctl(cfd, VIDIOC_QBUF, &cfilledbuffer) < 0) {
			perror("cam VIDIOC_QBUF");
		}
		
	/* ************************* HIST TEST **************************** */
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
				hist_user.bins = atoi(argv[2]);
				switch (hist_user.bins) {
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
				   hist_user.bins = atoi(argv[2]); */
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
				hist_user.bins = 0;
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
			hist_user.bins = BINS_256;
			/* number of regions to be configured 0 - 3 */
			hist_user.num_regions = 0;
		}

		hist_user.wb_gain_R = 32;	/* WB Field-to-Pattern Assignments */
		hist_user.wb_gain_RG = 32;	/* WB Field-to-Pattern Assignments */
		hist_user.wb_gain_B = 32;	/* WB Field-to-Pattern Assignments */
		hist_user.wb_gain_BG = 32;	/* WB Field-to-Pattern Assignments */

		hist_user.reg0_hor = 640;	/* Region 0 size and position */
		hist_user.reg0_ver = 480;	/* Region 0 size and position */
		//hist_user.reg1_hor = 320;	/* Region 0 size and position */
		//hist_user.reg1_ver = 240;	/* Region 0 size and position */

		
		/* set h3a params */
		control_hist_config.id = V4L2_CID_PRIVATE_ISP_HIST_CFG;
		control_hist_config.value = (int)&hist_user;
		/* set h3a params */
		ret = ioctl(cfd, VIDIOC_S_CTRL, &control_hist_config);
		
		if (ret < 0) {
			printf("Error: %d, ", ret);
			perror ("VIDIOC_ISP_HIST");
			return ret;

		}
		printf("VIDIOC_ISP_HIST successful\n");

		sleep(5);

		hist_data_user.hist_statistics_buf = (u32*) malloc(HIST_MEM_SIZE*4);
		control_hist_request.id = V4L2_CID_PRIVATE_ISP_HIST_REQ;
		control_hist_request.value = (int)&hist_data_user;
		ret = ioctl(cfd, VIDIOC_S_CTRL, &control_hist_request);
		if (ret < 0) {
			printf("VIDIOC_ISP_HISTREQ Error: %d, \n", ret);
			perror("VIDIOC_ISP_HISTREQ\n");
			return ret;
		}

		printf("\nHIST: buffer to display = %p data pointer = %p\n",
		       hist_data_user.hist_statistics_buf, hist_data_user.hist_statistics_buf);

		hist_output(hist_data_user.hist_statistics_buf, hist_user.bins, fp_out);		
		// Display stats

		free (hist_data_user.hist_statistics_buf);

/* ******************** ENDS H3A TEST ******************** */

	}

	sleep(1);

	enable = 0;

      	if (ioctl(cfd, VIDIOC_STREAMOFF, &creqbuf.type) == -1) {
              	perror("cam VIDIOC_STREAMOFF");
		return -1;
	}
       if (ioctl(vfd, VIDIOC_STREAMOFF, &vreqbuf.type) == -1) {
              	perror("video VIDIOC_STREAMOFF");
		return -1;
	}
	
	ret = setFramerate(cfd,30);
	if (ret < 0){
		printf("ERROR: VIDIOC_S_PARM ioctl cam\n");
		return -1;
	}

	//free (hist_data_user.hist_statistics_buf);

	printf("Preview stopped!\n");
	close(cfd);
	close(vfd);
}

