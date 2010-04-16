/* =============================================================================
*             Texas Instruments OMAP(TM) Platform Software
*  (c) Copyright Texas Instruments, Incorporated.  All Rights Reserved.
*
*  Use of this software is controlled by the terms and conditions found
*  in the license agreement under which this software has been supplied.
* ============================================================================*/

#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <linux/ioctl.h>
#include <sys/mman.h>
#include <linux/videodev2.h>
#include <linux/errno.h>
#include <errno.h>
#include <string.h>
#include <sys/time.h>
#include <mach/isp_user.h>
#include "kbget.h"
#include "snapshot_lsc.h"

/* For parameter parser */
#include <getopt.h>

#define DEFAULT_CAM_DEV				"/dev/video0"
#define DEFAULT_VID_DEV				"/dev/video1"
#define DEFAULT_PREVIEW_PIXFMT 		"YUYV"
#define DEFAULT_PREVIEW_WIDTH 		640
#define DEFAULT_PREVIEW_HEIGHT 		480
#define DEFAULT_PREVIEW_FPS 		30

#define DEFAULT_CAPTURE_PIXFMT 		"SGRBG10"
#define DEFAULT_CAPTURE_WIDTH 		1024
#define DEFAULT_CAPTURE_HEIGHT 		768
#define DEFAULT_CAPTURE_FPS 		10

#define DEFAULT_LSC_TEST			0
#define DEFAULT_FRAME_COUNT			-1

#define DSS_STREAM_START_FRAME		3

#define memtype				V4L2_MEMORY_USERPTR

/* H3A AEWB related declares */
#define BYTES_PER_WINDOW	16
#define use_aewb	1

static struct isph3a_aewb_config aewb_config_user;
static struct isph3a_aewb_data aewb_data_user;
static unsigned int aewb_buff_size;
static __u16 *aewb_stats_buff;

/* H3A AF related declares */
#define use_af		1

static struct af_configuration af_config_user;
static struct isp_af_data af_data_user;
static unsigned int af_buff_size;
static __u16 *af_stats_buff;

static void usage(void)
{
	printf("Usage:\n");
	printf("\tsnapshot <options>\n");
	printf("\t-c <device-node>\n"
			"\t\tCamera device node to open (default: "
			DEFAULT_CAM_DEV ")\n");
	printf("\t-v <device-node>\n"
			"\t\tVideo device node to open (default: "
			DEFAULT_VID_DEV ")\n");

	printf("\t-p <preview pixelFormat>\n"
			"\t\tPixel format for streaming. (default: "
			DEFAULT_PREVIEW_PIXFMT ")\n"
			"\t\tSupported: YUYV, UYVY, SGRBG10,"
			" SRGGB10, SBGGR10, SGBRG10\n");
	printf("\t-w <preview width>\n");
	printf("\t\tLCD preview width (default: %u)\n",
			DEFAULT_PREVIEW_WIDTH);
	printf("\t-h <preview height>\n");
	printf("\t\tLCD preview height (default: %u)\n",
			DEFAULT_PREVIEW_HEIGHT);
	printf("\t-f <preview fps>\n");
	printf("\t\tLCD preview frame rate (default: %u)\n",
			DEFAULT_PREVIEW_FPS);

	printf("\t-q <capture pixelFormat>\n"
			"\t\tPixel format for Snapshot. (default: "
			DEFAULT_CAPTURE_PIXFMT ")\n");
	printf("\t-x <capture width>\n");
	printf("\t\tSnapshot capture width (default: %u)\n",
			DEFAULT_CAPTURE_WIDTH);
	printf("\t-y <capture height>\n");
	printf("\t\tSnapshot capture height (default: %u)\n",
			DEFAULT_CAPTURE_HEIGHT);
	printf("\t-g <capture fps>\n");
	printf("\t\tSnapshot capture frame rate (default: %u)\n",
			DEFAULT_CAPTURE_FPS);

	printf("\t-l <lsc test>\n"
			"\t\tLSC Test: 1-enable, 0-disable "
			"(default: %d)\n", DEFAULT_LSC_TEST);

	printf("\t-n <frame count>\n"
			"\t\tNumber of frames to capture before exit:\n"
			"\t\t -1 - for unlimited capture "
			"(default: %d)\n", DEFAULT_FRAME_COUNT);
}

static void display_keys(void)
{
	printf("Keys:\n");
	printf("  c - Snapshot (Capture to file)\n");
	printf("  2 - Lens FOCUS_RELATIVE -1\n");
	printf("  3 - Lens FOCUS_RELATIVE +1\n");
	printf("  4 - Lens FOCUS_RELATIVE -5\n");
	printf("  5 - Lens FOCUS_RELATIVE +5\n");
	printf("  q - Quit\n");
}

static void dump_sensor_info(int cfd)
{
	struct omap34xxcam_sensor_info sens_info;

	printf("Getting Sensor Info...\n");
	if (ioctl(cfd, VIDIOC_PRIVATE_OMAP34XXCAM_SENSOR_INFO,
		  &sens_info) < 0) {
		printf("VIDIOC_PRIVATE_OMAP34XXCAM_SENSOR_INFO not"
		       " supported.\n");
		return;
	}
	printf("  Sensor xclk:       %d Hz\n", sens_info.current_xclk);
	printf("  Max Base size:     %d x %d\n",
		sens_info.full_size.width,
		sens_info.full_size.height);
	printf("  Current Base size: %d x %d\n",
		sens_info.active_size.width,
		sens_info.active_size.height);
}

static int h3a_aewb_init(int cfd)
{
	unsigned int num_windows;
	int ret = 0;

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

	/************************************************************/
	/* Set params */
	ret = ioctl(cfd, VIDIOC_PRIVATE_ISP_AEWB_CFG, &aewb_config_user);
	if (ret < 0)
		goto out_err;

	num_windows = ((aewb_config_user.ver_win_count
			* aewb_config_user.hor_win_count)
			+ aewb_config_user.hor_win_count);
	aewb_buff_size = ((num_windows +
			   (num_windows / 8) +
			   ((num_windows % 8) ? 1 : 0)) *
			  BYTES_PER_WINDOW);

	aewb_stats_buff = malloc(aewb_buff_size);

	if (!aewb_stats_buff)
		ret = -ENOMEM;

	printf("h3a_aewb_init: Successfully initted AE & AWB SCM\n");
	fflush(stdout);
out_err:
	return ret;
}

static int h3a_aewb_close(int cfd)
{
	unsigned int num_windows;
	int ret = 0;

	aewb_config_user.aewb_enable = 0;

	ret = ioctl(cfd, VIDIOC_PRIVATE_ISP_AEWB_CFG, &aewb_config_user);
	if (ret < 0)
		goto out_err;

	free(aewb_stats_buff);

	printf("h3a_aewb_init: Successfully disabled AE & AWB SCM\n");
	fflush(stdout);
out_err:
	return ret;
}

static int h3a_aewb_request_frame_num(int cfd, int *frame_num)
{
	int ret = 0;

	aewb_data_user.update = 0;
	ret = ioctl(cfd,  VIDIOC_PRIVATE_ISP_AEWB_REQ, &aewb_data_user);
	*frame_num = aewb_data_user.curr_frame;

	return ret;
}

static int h3a_aewb_request_stats(int cfd, int *frame_num)
{
	int ret = 0;

	aewb_data_user.update = REQUEST_STATISTICS;
	aewb_data_user.h3a_aewb_statistics_buf = aewb_stats_buff;
	aewb_data_user.frame_number = *frame_num;

	ret = ioctl(cfd,  VIDIOC_PRIVATE_ISP_AEWB_REQ, &aewb_data_user);
	*frame_num = aewb_data_user.curr_frame;

	return ret;
}

static int h3a_af_init(int cfd)
{
	unsigned int num_windows;
	int ret = 0, index;

	/* AF params */
	af_config_user.alaw_enable = H3A_AF_ALAW_ENABLE;	/* Enable Alaw */
	af_config_user.hmf_config.enable = H3A_AF_HMF_DISABLE;
	af_config_user.iir_config.hz_start_pos = 0;
	af_config_user.paxel_config.height = 16;
	af_config_user.paxel_config.width = 16;
	af_config_user.paxel_config.line_incr = 0;
	af_config_user.paxel_config.vt_start = 0;
	af_config_user.paxel_config.hz_start = 2;
	af_config_user.paxel_config.hz_cnt = 8;
	af_config_user.paxel_config.vt_cnt = 8;
	af_config_user.af_config = H3A_AF_CFG_ENABLE;
	af_config_user.hmf_config.threshold = 0;
	/* Set Accumulator mode */
	af_config_user.mode = ACCUMULATOR_SUMMED;

	for (index = 0; index < 11; index++) {
		af_config_user.iir_config.coeff_set0[index] = 12;
		af_config_user.iir_config.coeff_set1[index] = 12;
	}

	/************************************************************/
	/* Set params */
	ret = ioctl(cfd, VIDIOC_PRIVATE_ISP_AF_CFG, &af_config_user);
	if (ret < 0)
		goto out_err;

	af_buff_size = (af_config_user.paxel_config.hz_cnt + 1) *
		       (af_config_user.paxel_config.vt_cnt + 1) *
		       AF_PAXEL_SIZE;

	af_stats_buff = malloc(af_buff_size);

	if (!af_stats_buff)
		ret = -ENOMEM;

	printf("h3a_af_init: Successfully initted AF SCM\n");
	fflush(stdout);
out_err:
	return ret;
}

static int h3a_af_close(int cfd)
{
	unsigned int num_windows;
	int ret = 0;

	af_config_user.af_config = H3A_AF_CFG_DISABLE;
	ret = ioctl(cfd, VIDIOC_PRIVATE_ISP_AF_CFG, &af_config_user);
	if (ret < 0)
		goto out_err;

	free(af_stats_buff);

	printf("h3a_af_init: Successfully disabled AF SCM\n");
	fflush(stdout);
out_err:
	return ret;
}

static int h3a_af_request_frame_num(int cfd, int *frame_num)
{
	int ret = 0;

	af_data_user.update = 0;
	ret = ioctl(cfd,  VIDIOC_PRIVATE_ISP_AF_REQ, &af_data_user);
	*frame_num = af_data_user.curr_frame;

	return ret;
}

static int h3a_af_request_stats(int cfd, int *frame_num)
{
	int ret = 0;

	af_data_user.update = REQUEST_STATISTICS;
	af_data_user.af_statistics_buf = af_stats_buff;
	af_data_user.frame_number = *frame_num;

	ret = ioctl(cfd,  VIDIOC_PRIVATE_ISP_AF_REQ, &af_data_user);
	*frame_num = af_data_user.curr_frame;

	return ret;
}

int main(int argc, char **argv)
{
	struct buffers {
		void *start;
		size_t length;
	} *vbuffers, *cbuffers;
	struct v4l2_capability capability;
	struct v4l2_format cfmt, vfmt;
	struct v4l2_requestbuffers creqbuf, vreqbuf;
	struct v4l2_buffer cfilledbuffer, vfilledbuffer;
	struct v4l2_control control;
	int cfd, vfd, i;
	int quit_flag = 0, snap_flag = 0;
	char *camdev = DEFAULT_CAM_DEV;
	char *viddev = DEFAULT_VID_DEV;
	int prvw = DEFAULT_PREVIEW_WIDTH, prvh = DEFAULT_PREVIEW_HEIGHT;
	int prvfps = DEFAULT_PREVIEW_FPS;
	char *prvpix = DEFAULT_PREVIEW_PIXFMT;
	int capw = DEFAULT_CAPTURE_WIDTH, caph = DEFAULT_CAPTURE_HEIGHT;
	int capfps = DEFAULT_CAPTURE_FPS;
	char *cappix = DEFAULT_CAPTURE_PIXFMT;
	int use_lsc = DEFAULT_LSC_TEST;
	int lsc_toggle = 0, count_max = DEFAULT_FRAME_COUNT;
	int aewb_curr_frame, af_curr_frame;

	opterr = 0;

	while (1) {
		static struct option long_options[] = {
			{"camdev",	required_argument,	0, 'c'},
			{"pixprv",	required_argument,	0, 'p'},
			{"wprv",	required_argument,	0, 'w'},
			{"hprv",	required_argument,	0, 'h'},
			{"fpsprv",	required_argument,	0, 'f'},
			{"pixcap",	required_argument,	0, 'q'},
			{"wcap",	required_argument,	0, 'x'},
			{"hcap",	required_argument,	0, 'y'},
			{"fpscap",	required_argument,	0, 'g'},
			{"viddev",	required_argument,	0, 'v'},
			{"lsc",		required_argument,	0, 'l'},
			{"nbr",		required_argument,	0, 'n'},
			{0, 0, 0, 0}
		};
		int option_index = 0;
		static int c;

		c = getopt_long_only(argc, argv, "c:p:w:h:f:q:x:y:g:v:l:n:a",
				long_options, &option_index);

		/* Detect the end of the options. */
		if (c == -1)
			break;

		switch (c) {
		case 'c':
			camdev = optarg;
			break;
		case 'p':
			prvpix = optarg;
			break;
		case 'w':
			prvw = atoi(optarg);
			break;
		case 'h':
			prvh = atoi(optarg);
			break;
		case 'f':
			prvfps = atoi(optarg);
			break;
		case 'q':
			cappix = optarg;
			break;
		case 'x':
			capw = atoi(optarg);
			break;
		case 'y':
			caph = atoi(optarg);
			break;
		case 'g':
			capfps = atoi(optarg);
			break;
		case 'v':
			viddev = optarg;
			break;
		case 'l':
			use_lsc = atoi(optarg);
			break;
		case 'n':
			count_max = atoi(optarg);
			break;
		case '?':
			if ((optopt == 'c') ||
			    (optopt == 'p') ||
			    (optopt == 'w') ||
			    (optopt == 'h') ||
			    (optopt == 'f') ||
			    (optopt == 'q') ||
			    (optopt == 'x') ||
			    (optopt == 'y') ||
			    (optopt == 'g') ||
			    (optopt == 'v') ||
				(optopt == 'n') ||
				(optopt == 'l')) {
				fprintf(stderr,
					"Option -%c requires an argument.\n",
					optopt);
				usage();
			} else if (isprint(optopt)) {
				fprintf(stderr,
					"Unknown option `-%c'.\n",
					optopt);
				usage();
			} else {
				fprintf(stderr,
					"Unknown option character `\\x%x'.\n",
					optopt);
				usage();
			}
			return 1;
		default:
			abort();
		}
	}

	/********************************************************************/
	/* Camera: Open handle to camera driver */

	cfd = open(camdev, O_RDWR);
	if (cfd <= 0) {
		printf("Could not open the cam device %s\n", camdev);
		return -1;
	}

	printf("Openned %s for capturing\n", viddev);

	/********************************************************************/
	/* Video: Open handle to DSS */

	vfd = open(viddev, O_RDWR);
	if (vfd <= 0) {
		printf("Could no open the device %s\n", viddev);
		return -1;
	}

	printf("Openned %s for rendering\n", viddev);

	/********************************************************************/
	/* Video: Query Capability */

	if (ioctl(vfd, VIDIOC_QUERYCAP, &capability) == -1) {
		perror("dss VIDIOC_QUERYCAP");
		return -1;
	}
	if (!(capability.capabilities & V4L2_CAP_STREAMING)) {
		printf("The video driver is not capable of "
		       "Streaming!\n");
		return -1;
	}
	printf("The video driver is capable of Streaming!\n");

restart_streaming:
	printf("Restart streaming...\n");

	/********************************************************************/
	/* Camera: Set Frame rate to 30fps */

	printf("Set Camera frame rate to %ufps...\n", DEFAULT_PREVIEW_FPS);
	if (setFramerate(cfd, prvfps) < 0) {
		printf("Error setting framerate = %d\n", DEFAULT_PREVIEW_FPS);
		return -1;
	}

	/********************************************************************/
	/* Camera: Set Format & Size */

	printf("Set Camera format & size...\n");

	/* get the current format of the video capture */
	cfmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	if (ioctl(cfd, VIDIOC_G_FMT, &cfmt) < 0) {
		perror("VIDIOC_G_FMT");
		return -1;
	}

	cfmt.fmt.pix.width = prvw;
	cfmt.fmt.pix.height = prvh;

	if (!strcmp(prvpix, "YUYV"))
		cfmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
	else if (!strcmp(prvpix, "UYVY"))
		cfmt.fmt.pix.pixelformat = V4L2_PIX_FMT_UYVY;
	else if (!strcmp(prvpix, "SGRBG10"))
		cfmt.fmt.pix.pixelformat = V4L2_PIX_FMT_SGRBG10;
	else if (!strcmp(prvpix, "SRGGB10"))
		cfmt.fmt.pix.pixelformat = V4L2_PIX_FMT_SRGGB10;
	else if (!strcmp(prvpix, "SBGGR10"))
		cfmt.fmt.pix.pixelformat = V4L2_PIX_FMT_SBGGR10;
	else if (!strcmp(prvpix, "SGBRG10"))
		cfmt.fmt.pix.pixelformat = V4L2_PIX_FMT_SGBRG10;
	else {
		printf("unsupported pixel format!\n");
		return -1;
	}

	if (ioctl(cfd, VIDIOC_S_FMT, &cfmt) < 0) {
		perror("cam VIDIOC_S_FMT");
		return -1;
	}

	/********************************************************************/
	/* Camera: Query Capability */

	printf("Query Camera Capabilities...\n");
	if (ioctl(cfd, VIDIOC_QUERYCAP, &capability) < 0) {
		perror("cam VIDIOC_QUERYCAP");
		return -1;
	}
	if (!(capability.capabilities & V4L2_CAP_STREAMING)) {
		printf("The Camera driver is not capable of Streaming!\n");
		return -1;
	}
	printf("The Camera driver is capable of Streaming!\n");

	/********************************************************************/
	/* Camera: Get Pixel Format */

	cfmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	if (ioctl(cfd, VIDIOC_G_FMT, &cfmt) < 0) {
		perror("cam VIDIOC_G_FMT");
		return -1;
	}
	printf("Camera Image width = %d, Image height = %d, size = %d\n",
			cfmt.fmt.pix.width,
			cfmt.fmt.pix.height,
			cfmt.fmt.pix.sizeimage);

	/********************************************************************/
	/* Video: Get Pixel Format */

	vfmt.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
	if (ioctl(vfd, VIDIOC_G_FMT, &vfmt) < 0) {
		perror("dss VIDIOC_G_FMT");
		return -1;
	}
	printf("Video Image width = %d, Image height = %d, size = %d\n",
			vfmt.fmt.pix.width, vfmt.fmt.pix.height,
			vfmt.fmt.pix.sizeimage);

	/********************************************************************/
	/* Check if size and pixel formats match */

	if ((cfmt.fmt.pix.width != vfmt.fmt.pix.width) ||
		(cfmt.fmt.pix.height != vfmt.fmt.pix.height) ||
		(cfmt.fmt.pix.pixelformat !=
				vfmt.fmt.pix.pixelformat)) {
		printf("image specs don't match!\n");
		printf("set video image the same as camera image...\n");
		vfmt.fmt.pix.width = cfmt.fmt.pix.width;
		vfmt.fmt.pix.height = cfmt.fmt.pix.height;
		vfmt.fmt.pix.sizeimage = cfmt.fmt.pix.sizeimage;
		vfmt.fmt.pix.pixelformat = cfmt.fmt.pix.pixelformat;

		if (ioctl(vfd, VIDIOC_S_FMT, &vfmt) < 0) {
			perror("dss VIDIOC_S_FMT");
			return -1;
		}

		printf("New Image & Video sizes, after "
		"equaling:\nCamera Image width = %d, Image "
		"height = %d, size = %d\n",
		cfmt.fmt.pix.width, cfmt.fmt.pix.height,
		cfmt.fmt.pix.sizeimage);
		printf("Video Image width = %d, Image "
				"height = %d, size = %d\n",
				vfmt.fmt.pix.width,
				vfmt.fmt.pix.height,
				vfmt.fmt.pix.sizeimage);

		if ((cfmt.fmt.pix.width != vfmt.fmt.pix.width) ||
			(cfmt.fmt.pix.height != vfmt.fmt.pix.height) ||
			(cfmt.fmt.pix.pixelformat !=
				vfmt.fmt.pix.pixelformat)) {
			printf("can't make camera and video image "
				"compatible!\n");
			return 0;
		}
	}

	/********************************************************************/
	/* Video: Request number of available buffers */

	vreqbuf.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
	vreqbuf.memory = V4L2_MEMORY_MMAP;
	vreqbuf.count = 4;
	if (ioctl(vfd, VIDIOC_REQBUFS, &vreqbuf) == -1) {
		perror("dss VIDEO_REQBUFS");
		return;
	}
	printf("Video Driver allocated %d buffers when 4 are "
			"requested\n", vreqbuf.count);

	/********************************************************************/
	/* Video: Query buffer & mmap buffer address */

	vbuffers = calloc(vreqbuf.count, sizeof(*vbuffers));
	for (i = 0; i < vreqbuf.count; ++i) {
		struct v4l2_buffer buffer;
		buffer.type = vreqbuf.type;
		buffer.index = i;
		if (ioctl(vfd, VIDIOC_QUERYBUF, &buffer) == -1) {
			perror("dss VIDIOC_QUERYBUF");
			return;
		}
		vbuffers[i].length = buffer.length;
		vbuffers[i].start = mmap(NULL, buffer.length, PROT_READ
					| PROT_WRITE, MAP_SHARED,
					 vfd,
					buffer.m.offset);
		if (vbuffers[i].start == MAP_FAILED) {
			perror("dss mmap");
			return;
		}
		printf("Video Buffers[%d].start = %x  length = %d\n",
			i, vbuffers[i].start, vbuffers[i].length);
	}

	/********************************************************************/
	/* Camera: Request number of available buffers */

	creqbuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	creqbuf.memory = memtype;
	creqbuf.count = 4;
	printf("Requesting %d buffers of type %s\n", creqbuf.count,
		(memtype == V4L2_MEMORY_USERPTR) ? "V4L2_MEMORY_USERPTR" :
							"V4L2_MEMORY_MMAP");
	if (ioctl(cfd, VIDIOC_REQBUFS, &creqbuf) < 0) {
		perror("cam VIDEO_REQBUFS");
		return -1;
	}
	printf("Camera Driver allowed buffers reqbuf.count = %d\n",
		creqbuf.count);

	/********************************************************************/
	/* Camera: Query for each buffer & mmap buffer addresses */

	cbuffers = calloc(creqbuf.count, sizeof(*cbuffers));

	for (i = 0; i < creqbuf.count; ++i) {
		struct v4l2_buffer buffer;
		buffer.type = creqbuf.type;
		buffer.memory = creqbuf.memory;
		buffer.index = i;
		if (ioctl(cfd, VIDIOC_QUERYBUF, &buffer) < 0) {
			perror("cam VIDIOC_QUERYBUF");
			return -1;
		}
		if (memtype == V4L2_MEMORY_USERPTR) {
			buffer.flags = 0;
			buffer.m.userptr = (unsigned int)vbuffers[i].start;
			buffer.length = vbuffers[i].length;
		} else {
			cbuffers[i].length = buffer.length;
			cbuffers[i].start = vbuffers[i].start;
			printf("Mapped Buffers[%d].start = %x  length = %d\n",
				i, cbuffers[i].start, cbuffers[i].length);

			buffer.m.userptr = (unsigned int)cbuffers[i].start;
			buffer.length = cbuffers[i].length;
		}

		if (ioctl(cfd, VIDIOC_QBUF, &buffer) < 0) {
			perror("cam VIDIOC_QBUF");
			return -1;
		}
	}

	/********************************************************************/
	/* Init and Configure LSC */
	if (use_lsc) {
		lsc_init_table();
		lsc_update_table(cfd, 1);
	}

	/********************************************************************/
	/* Init and Configure AE & AWB SCM */
	if (use_aewb) {
		if (h3a_aewb_init(cfd)) {
			perror("h3a_aewb_init");
			return -1;
		}
	}

	/********************************************************************/
	/* Init and Configure AF SCM */
	if (use_af) {
		if (h3a_af_init(cfd)) {
			perror("h3a_af_init");
			return -1;
		}
	}

	dump_sensor_info(cfd);

	/********************************************************************/
	/* Start Camera streaming */

	if (ioctl(cfd, VIDIOC_STREAMON, &creqbuf.type) < 0) {
		perror("cam VIDIOC_STREAMON");
		return -1;
	}


	/********************************************************************/
	/* Display some information to the user */

	printf("Streaming %d x %d...\n",
			cfmt.fmt.pix.width,
			cfmt.fmt.pix.height);
	if (use_lsc == 0)
		display_keys();
	/********************************************************************/
	/* Start streaming loop */

	cfilledbuffer.type = creqbuf.type;
	vfilledbuffer.type = vreqbuf.type;
	i = 0;
	while (1) {
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

		/* Syncup with internal AEWB frame count */
		if (use_aewb) {
			if (i == 0) {
				h3a_aewb_request_frame_num(cfd, &aewb_curr_frame);
				aewb_curr_frame -=1;
			}
			if (h3a_aewb_request_stats(cfd, &aewb_curr_frame))
				perror("AEWB");
		}

		/* Syncup with internal AF frame count */
		if (use_af) {
			if (i == 0) {
				h3a_af_request_frame_num(cfd, &af_curr_frame);
				af_curr_frame -=1;
			}
			if (h3a_af_request_stats(cfd, &af_curr_frame))
				perror("AF");
		}

		/* Toggle LSC every second */
		if (use_lsc && ((i % prvfps) == 0)) {
			lsc_toggle = lsc_toggle ? 0 : 1;
			lsc_update_table(cfd, lsc_toggle);
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
				return -1;
			}
		}

		if ((use_lsc == 0) && kbhit()) {
			int input = getch();
			if (input == '2') {
				control.id = V4L2_CID_FOCUS_RELATIVE;
				control.value = -1;
				if (ioctl(cfd, VIDIOC_S_CTRL, &control) == -1)
					perror("Error CID_FOCUS_RELATIVE: ");
				printf("Lens position: -1\n");
			} else if (input == '3') {
				control.id = V4L2_CID_FOCUS_RELATIVE;
				control.value = 1;
				if (ioctl(cfd, VIDIOC_S_CTRL, &control) == -1)
					perror("Error CID_FOCUS_RELATIVE: ");
				printf("Lens position: +1\n");
			} else if (input == '4') {
				control.id = V4L2_CID_FOCUS_RELATIVE;
				control.value = -5;
				if (ioctl(cfd, VIDIOC_S_CTRL, &control) == -1)
					perror("Error CID_FOCUS_RELATIVE: ");
				printf("Lens position: -5\n");
			} else if (input == '5') {
				control.id = V4L2_CID_FOCUS_RELATIVE;
				control.value = 5;
				if (ioctl(cfd, VIDIOC_S_CTRL, &control) == -1)
					perror("Error CID_FOCUS_RELATIVE: ");
				printf("Lens position: +5\n");
			} else if (input == 'c') {
				snap_flag = 1;
			} else if (input == 'q') {
				quit_flag = 1;
			}
		}

		/* Have we reached the frame count limit ? */
		if (i >= count_max && count_max != -1)
			quit_flag = 1;

		if (quit_flag | snap_flag) {
			printf("Cancelling the streaming capture...\n");

			creqbuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
			if (ioctl(cfd, VIDIOC_STREAMOFF, &creqbuf.type) < 0) {
				perror("cam VIDIOC_STREAMOFF");
				return -1;
			}

			if (ioctl(vfd, VIDIOC_STREAMOFF, &vreqbuf.type) == -1) {
				perror("dss VIDIOC_STREAMOFF");
				return -1;
			}

			break;
		}

		if (i >= DSS_STREAM_START_FRAME) {
			cfilledbuffer.index = vfilledbuffer.index;
			while (ioctl(cfd, VIDIOC_QBUF, &cfilledbuffer) < 0)
				perror("cam VIDIOC_QBUF");
		}
	}

	/********************************************************************/
	/* Cleanup */

	/* Unmap video buffers */
	for (i = 0; i < vreqbuf.count; i++) {
		if (vbuffers[i].start)
			munmap(vbuffers[i].start, vbuffers[i].length);
	}
	free(vbuffers);

	/* Cleanup structure that holds info about camera buffers */
	free(cbuffers);

	/* Stop AE & AWB */
	if (use_aewb) {
		if (h3a_aewb_close(cfd)) {
			perror("h3a_aewb_close");
			return -1;
		}
	}

	/* Stop AF */
	if (use_af) {
		if (h3a_af_close(cfd)) {
			perror("h3a_af_close");
			return -1;
		}
	}

	/********************************************************************/
	/* Take snapshot ? */

	if (snap_flag) {
		int ret;

		ret = snapshot(cfd, cappix, capw, caph, capfps);
		if (ret)
			return ret;
		snap_flag = 0;
		goto restart_streaming;
	}

	/********************************************************************/
	/* LSC: Disable and cleanup resources */

	if (use_lsc) {
		lsc_update_table(cfd, 0);
		lsc_cleanup();
	}

	close(vfd);
	close(cfd);
}

/********************************************************************/
/********************************************************************/

int snapshot(int cfd, char *pixelFmt, int w, int h, int fps)
{
	struct {
		void *start;
		size_t length;
	} *cbuffers;

	static unsigned int snap_count = 1;
	struct v4l2_capability capability;
	struct v4l2_format cfmt;
	struct v4l2_requestbuffers creqbuf;
	struct v4l2_buffer cfilledbuffer;
	int i, count = 1;
	int fd_save = 0;
	char filename[64];
	int file_is_yuv = 0, file_is_raw = 0;

	printf("\nTaking snapshot...\n");

	/********************************************************************/
	/* Set frame rate */
	if (setFramerate(cfd, fps) < 0) {
		printf("Error setting framerate");
		return -1;
	}

	/********************************************************************/
	/* Set snapshot frame format */

	/* get the current format of the video capture */
	cfmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	if (ioctl(cfd, VIDIOC_G_FMT, &cfmt) < 0) {
		perror("VIDIOC_G_FMT");
		return -1;
	}

	cfmt.fmt.pix.width = w;
	cfmt.fmt.pix.height = h;

	if (!strcmp(pixelFmt, "YUYV")) {
		cfmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
		file_is_yuv = 1;
	} else if (!strcmp(pixelFmt, "UYVY")) {
		cfmt.fmt.pix.pixelformat = V4L2_PIX_FMT_UYVY;
		file_is_yuv = 1;
	} else if (!strcmp(pixelFmt, "SGRBG10")) {
		cfmt.fmt.pix.pixelformat = V4L2_PIX_FMT_SGRBG10;
		file_is_raw = 1;
	} else if (!strcmp(pixelFmt, "SRGGB10")) {
		cfmt.fmt.pix.pixelformat = V4L2_PIX_FMT_SRGGB10;
		file_is_raw = 1;
	} else if (!strcmp(pixelFmt, "SBGGR10")) {
		cfmt.fmt.pix.pixelformat = V4L2_PIX_FMT_SBGGR10;
		file_is_raw = 1;
	} else if (!strcmp(pixelFmt, "SGBRG10")) {
		cfmt.fmt.pix.pixelformat = V4L2_PIX_FMT_SGBRG10;
		file_is_raw = 1;
	} else {
		printf("unsupported pixel format!\n");
		return -1;
	}

	if (ioctl(cfd, VIDIOC_S_FMT, &cfmt) < 0) {
		perror("cam VIDIOC_S_FMT");
		return -1;
	}

	/********************************************************************/
	/* Open image output file
	 * Create filename to be compatible with PYUV image viewer
	 * Filename format details:
	 * http://dsplab.diei.unipg.it/~baruffa/dvbt/pyuvhelp/doc2.htm#control
	 */

	sprintf(filename, "snap%04X_%dx%d_%iHz_%s_%s.%s", snap_count,
		cfmt.fmt.pix.width, cfmt.fmt.pix.height,
		fps,
		file_is_yuv ? "8b_I422" : "10bpp",
		file_is_yuv ? pixelFmt : "RGGB",
		file_is_yuv ? "yuv" : (file_is_raw ? "raw" : "dat"));

	/* Create a file with 644 permissions */
	fd_save = creat(filename,
			O_RDWR | S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
	if (fd_save <= 0) {
		printf("Can't create file %s\n", filename);
		return -1;
	}
	printf("The captured frames will be saved into: %s\n", filename);

	/********************************************************************/
	/* Camera: Get format */

	cfmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	if (ioctl(cfd, VIDIOC_G_FMT, &cfmt) < 0) {
		perror("VIDIOC_G_FMT");
		return -1;
	}
	printf("Camera Image width = %d, Image height = %d, size = %d\n",
		 cfmt.fmt.pix.width, cfmt.fmt.pix.height,
		 cfmt.fmt.pix.sizeimage);

	/********************************************************************/
	/* Camera: Request pointers to buffers */

	creqbuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	creqbuf.memory = memtype;
	creqbuf.count = count;

	printf("Requesting %d buffers of type %s\n", creqbuf.count,
		(memtype == V4L2_MEMORY_USERPTR) ? "V4L2_MEMORY_USERPTR" :
						"V4L2_MEMORY_MMAP");
	if (ioctl(cfd, VIDIOC_REQBUFS, &creqbuf) < 0) {
		perror("VIDEO_REQBUFS");
		return -1;
	}
	printf("Camera Driver allowed buffers reqbuf.count = %d\n",
		creqbuf.count);
	if (creqbuf.count != count)
		count = creqbuf.count;

	/********************************************************************/
	/* Camera: Allocate user memory, and queue each buffer */

	cbuffers = calloc(creqbuf.count, sizeof(*cbuffers));

	for (i = 0; i < creqbuf.count; ++i) {
		struct v4l2_buffer buffer;
		buffer.type = creqbuf.type;
		buffer.memory = creqbuf.memory;
		buffer.index = i;
		if (ioctl(cfd, VIDIOC_QUERYBUF, &buffer) < 0) {
			perror("VIDIOC_QUERYBUF");
			return -1;
		}

		/* V4L2_MEMORY_USERPTR */
		cbuffers[i].length = buffer.length;
		posix_memalign(&cbuffers[i].start, 0x1000, cbuffers[i].length);
		buffer.m.userptr = (unsigned int)cbuffers[i].start;
		printf("User Buffer [%d].start = %x  length = %d\n",
			 i, cbuffers[i].start, cbuffers[i].length);

		 if (ioctl(cfd, VIDIOC_QBUF, &buffer) < 0) {
			perror("CAMERA VIDIOC_QBUF");
			return -1;
		}
	}

	cfilledbuffer.type = creqbuf.type;
	cfilledbuffer.memory = memtype;


	/********************************************************************/
	/* Camera: turn on streaming  */

	if (ioctl(cfd, VIDIOC_STREAMON, &creqbuf.type) < 0) {
		perror("VIDIOC_STREAMON");
		return -1;
	}

	/********************************************************************/
	/* Display some information to the user */

	dump_sensor_info(cfd);

	printf("Snapshot size %d x %d...\n",
			cfmt.fmt.pix.width,
			cfmt.fmt.pix.height);

	/********************************************************************/
	/* Camera: Queue/Dequeue one time  */

	i = 0;
	while (1) {
		/* De-queue the next avaliable buffer */
		while (ioctl(cfd, VIDIOC_DQBUF, &cfilledbuffer) < 0) {
			perror("VIDIOC_DQBUF");
			printf(" ERROR HAS OCCURED\n");
		}

		i++;

		if (i == count) {
			printf("Cancelling the streaming capture...\n");
			creqbuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
			if (ioctl(cfd, VIDIOC_STREAMOFF, &creqbuf.type) < 0) {
				perror("VIDIOC_STREAMOFF");
				return -1;
			}
			printf("Done\n");
			break;
		}

		while (ioctl(cfd, VIDIOC_QBUF, &cfilledbuffer) < 0)
			perror("CAM VIDIOC_QBUF");
	}

	/********************************************************************/
	/* Save buffer to file  */

	printf("Captured %d frame!\n", i);
	printf("Start writing to file\n");
	for (i = 0; i < count; i++)
		write(fd_save, cbuffers[i].start,
			cfmt.fmt.pix.width * cfmt.fmt.pix.height * 2);
	printf("Completed writing to file: %s\n", filename);
	close(fd_save);

	/********************************************************************/
	/* Camera: Cleanup  */

	for (i = 0; i < creqbuf.count; i++) {
		if (cbuffers[i].start) {
			/* V4L2_MEMORY_USERPTR */
			free(cbuffers[i].start);
		}
	}

	free(cbuffers);
	snap_count++;

	return 0;
}
