/* =============================================================================
*             Texas Instruments OMAP(TM) Platform Software
*  (c) Copyright Texas Instruments, Incorporated.  All Rights Reserved.
*
*  Use of this software is controlled by the terms and conditions found
*  in the license agreement under which this software has been supplied.
* ============================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <linux/ioctl.h>
#include <sys/mman.h>
#include <linux/videodev2.h>
#include <linux/errno.h>
#include <errno.h>
#include <string.h>
#include <mach/isp_user.h>

/* Helper macros */
#define ARRAY_SIZE(a)	(sizeof(a) / sizeof(a[0]))
#define CLEAR(x)	memset(&(x), 0, sizeof(x))

#define V4L2_FMT_SHORTFORM(x, y, pix) \
	{ \
		.width = (x), \
		.height = (y), \
	}

struct v4l2_pix_format sizes2try_imx046[] = {
	V4L2_FMT_SHORTFORM(128, 96, V4L2_PIX_FMT_YUYV),
	V4L2_FMT_SHORTFORM(176, 144, V4L2_PIX_FMT_YUYV),
	V4L2_FMT_SHORTFORM(320, 240, V4L2_PIX_FMT_YUYV),
	V4L2_FMT_SHORTFORM(320, 280, V4L2_PIX_FMT_YUYV),
	V4L2_FMT_SHORTFORM(352, 288, V4L2_PIX_FMT_YUYV),
	V4L2_FMT_SHORTFORM(528, 384, V4L2_PIX_FMT_YUYV),
	V4L2_FMT_SHORTFORM(640, 480, V4L2_PIX_FMT_YUYV),
	V4L2_FMT_SHORTFORM(704, 576, V4L2_PIX_FMT_YUYV),
	V4L2_FMT_SHORTFORM(720, 480, V4L2_PIX_FMT_YUYV),
	V4L2_FMT_SHORTFORM(768, 576, V4L2_PIX_FMT_YUYV),
	V4L2_FMT_SHORTFORM(800, 600, V4L2_PIX_FMT_YUYV),
	V4L2_FMT_SHORTFORM(848, 480, V4L2_PIX_FMT_YUYV),
	V4L2_FMT_SHORTFORM(854, 480, V4L2_PIX_FMT_YUYV),
	V4L2_FMT_SHORTFORM(864, 480, V4L2_PIX_FMT_YUYV),
	V4L2_FMT_SHORTFORM(992, 560, V4L2_PIX_FMT_YUYV),
	V4L2_FMT_SHORTFORM(1152, 864, V4L2_PIX_FMT_YUYV),
	V4L2_FMT_SHORTFORM(1024, 768, V4L2_PIX_FMT_YUYV),
	V4L2_FMT_SHORTFORM(1280, 720, V4L2_PIX_FMT_YUYV),
	V4L2_FMT_SHORTFORM(1280, 768, V4L2_PIX_FMT_YUYV),
	V4L2_FMT_SHORTFORM(1280, 800, V4L2_PIX_FMT_YUYV),
	V4L2_FMT_SHORTFORM(1280, 960, V4L2_PIX_FMT_YUYV),
	V4L2_FMT_SHORTFORM(1280, 1024, V4L2_PIX_FMT_YUYV),
	V4L2_FMT_SHORTFORM(1366, 768, V4L2_PIX_FMT_YUYV),
	V4L2_FMT_SHORTFORM(1400, 1050, V4L2_PIX_FMT_YUYV),
	V4L2_FMT_SHORTFORM(1408, 576, V4L2_PIX_FMT_YUYV),
	V4L2_FMT_SHORTFORM(1440, 900, V4L2_PIX_FMT_YUYV),
	V4L2_FMT_SHORTFORM(1440, 1080, V4L2_PIX_FMT_YUYV),
	V4L2_FMT_SHORTFORM(1600, 1200, V4L2_PIX_FMT_YUYV),
	V4L2_FMT_SHORTFORM(1680, 1050, V4L2_PIX_FMT_YUYV),
	V4L2_FMT_SHORTFORM(1920, 1080, V4L2_PIX_FMT_YUYV),
	V4L2_FMT_SHORTFORM(1920, 1200, V4L2_PIX_FMT_YUYV),
	V4L2_FMT_SHORTFORM(1920, 1080, V4L2_PIX_FMT_YUYV),
	V4L2_FMT_SHORTFORM(2048, 1080, V4L2_PIX_FMT_YUYV),
	V4L2_FMT_SHORTFORM(2048, 1536, V4L2_PIX_FMT_YUYV),
	V4L2_FMT_SHORTFORM(2592, 1944, V4L2_PIX_FMT_YUYV),
	V4L2_FMT_SHORTFORM(2560, 1600, V4L2_PIX_FMT_YUYV),
	V4L2_FMT_SHORTFORM(2560, 2048, V4L2_PIX_FMT_YUYV),
	V4L2_FMT_SHORTFORM(4096, 2160, V4L2_PIX_FMT_YUYV),
};

#define NUM_SIZES	ARRAY_SIZE(sizes2try_imx046)

__u32 formats2try_imx046[] = {
	/* V4L2_PIX_FMT_YUYV, */
	V4L2_PIX_FMT_UYVY,
	/* V4L2_PIX_FMT_SRGGB10, */
};

#define NUM_FMTS	ARRAY_SIZE(formats2try_imx046)

struct v4l2_fract fps2try_imx046[] = {
	/* {.numerator = 1, .denominator = 10}, */
	/* {.numerator = 1, .denominator = 15}, */
	{.numerator = 1, .denominator = 30},
};

#define NUM_FPS		ARRAY_SIZE(fps2try_imx046)

int main(int argc, char **argv)
{
	int fd, count_fps, count_fmt, count_size;
	struct v4l2_capability cap;

	fd = open("/dev/video5", O_RDWR);
	if (fd <= 0) {
		printf("Could not open device node '/dev/video5'\n");
		return -1;
	}

	if (ioctl(fd, VIDIOC_QUERYCAP, &cap)) {
		perror("VIDIOC_QUERYCAP");
		return -1;
	}
	if (!(cap.capabilities & V4L2_CAP_STREAMING)) {
		printf("The device node '/dev/video5' is not capable of"
		       " streaming!\n");
		return -1;
	}
	printf("The device node '/dev/video5' is capable of Streaming!\n");

	for (count_fmt = 0; count_fmt < NUM_FMTS; count_fmt++) {
		for (count_fps = 0; count_fps < NUM_FPS; count_fps++) {
			struct v4l2_streamparm parm;

			CLEAR(parm);

			parm.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
			parm.parm.capture.timeperframe = fps2try_imx046[count_fps];

			if (ioctl(fd, VIDIOC_S_PARM, &parm)) {
				perror("VIDIOC_S_PARM");
				return -1;
			}

			printf("Test with ");
			switch (formats2try_imx046[count_fmt]) {
			case V4L2_PIX_FMT_YUYV:
				printf("V4L2_PIX_FMT_YUYV");
				break;
			case V4L2_PIX_FMT_UYVY:
				printf("V4L2_PIX_FMT_UYVY");
				break;
			case V4L2_PIX_FMT_SRGGB10:
				printf("V4L2_PIX_FMT_SRGGB10");
				break;
			default:
				printf("(Unknown[%d])", formats2try_imx046[count_fmt]);
			}

			printf(" pixel format @ %d fps (returned %d fps by driver)\n\n",
			       fps2try_imx046[count_fps].denominator /
			       fps2try_imx046[count_fps].numerator,
			       parm.parm.capture.timeperframe.denominator /
			       parm.parm.capture.timeperframe.numerator);

			printf("+---------+---------+-------------------------------------------------------+-----------------------------------------------------------+------+\n"
			       "|         |         |                     Sensor Info                       |                        Crop Capability                    |Pixel |\n"
			       "|  S_FMT  | G_GMT   |-------------------+-------------------+-----+---------+-----------------------------------------------------------+aspect|\n"
			       "|         |         |        Full       |        Active     |Pixel|Pix clock|       Bounds      |      Default      |      Current      |ratio |\n"
			       "+---------+---------+-------------------+-------------------+-----+---------+-------------------+-------------------+-------------------+------+\n"
			       "|  W   H  |  W   H  |   L    T   W    H |   L    T   W    H | W H |   Hz    |   L    T   W    H |   L    T   W    H |   L    T   W    H | W  H |\n"
			       "+---------+---------+-------------------+-------------------+-----+---------+-------------------+-------------------+-------------------+------+\n");
			for (count_size = 0; count_size < NUM_SIZES; count_size++) {
				struct v4l2_format fmt;
				struct v4l2_cropcap cropcap;
				struct v4l2_crop crop;
				struct omap34xxcam_sensor_info sens_info;

				CLEAR(fmt);

				fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
				fmt.fmt.pix = sizes2try_imx046[count_size];
				fmt.fmt.pix.pixelformat = formats2try_imx046[count_fmt];

				if (ioctl(fd, VIDIOC_S_FMT, &fmt)) {
					perror("VIDIOC_S_FMT");
					return -1;
				}

				/* Intended size */
				printf("|%4d %4d",
				       sizes2try_imx046[count_size].width,
				       sizes2try_imx046[count_size].height);

				/* Obtained size */
				printf("|%4d %4d",
				       fmt.fmt.pix.width,
				       fmt.fmt.pix.height);

				CLEAR(sens_info);

				if (ioctl(fd, VIDIOC_PRIVATE_OMAP34XXCAM_SENSOR_INFO, &sens_info)) {
					perror("VIDIOC_PRIVATE_OMAP34XXCAM_SENSOR_INFO");
					return -1;
				}

				/* Sensor Info - Full size */
				printf("|%4d %4d %4d %4d",
				       sens_info.full_size.left,
				       sens_info.full_size.top,
				       sens_info.full_size.width,
				       sens_info.full_size.height);

				/* Sensor Info - Active size */
				printf("|%4d %4d %4d %4d",
				       sens_info.active_size.left,
				       sens_info.active_size.top,
				       sens_info.active_size.width,
				       sens_info.active_size.height);

				/* Sensor Info - Pixel size */
				printf("| %1d %1d ",
				       sens_info.pixel_size.width,
				       sens_info.pixel_size.height);

				/* Sensor Info - Pixel clock */
				printf("|%9d",
				       sens_info.current_xclk);

				CLEAR(cropcap);
				cropcap.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

				if (ioctl(fd, VIDIOC_CROPCAP, &cropcap)) {
					perror("VIDIOC_CROPCAP");
					return -1;
				}

				/* Crop Capability - Bounds */
				printf("|%4d %4d %4d %4d",
				       cropcap.bounds.left,
				       cropcap.bounds.top,
				       cropcap.bounds.width,
				       cropcap.bounds.height);

				/* Crop Capability - Default */
				printf("|%4d %4d %4d %4d",
				       cropcap.defrect.left,
				       cropcap.defrect.top,
				       cropcap.defrect.width,
				       cropcap.defrect.height);

				CLEAR(crop);
				crop.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

				if (ioctl(fd, VIDIOC_G_CROP, &crop)) {
					perror("VIDIOC_G_CROP");
					return -1;
				}

				/* Crop Capability - Current */
				printf("|%4d %4d %4d %4d",
				       crop.c.left,
				       crop.c.top,
				       crop.c.width,
				       crop.c.height);

				/* Pixel aspect ratio */
				printf("|%2d %2d ",
				       cropcap.pixelaspect.numerator,
				       cropcap.pixelaspect.denominator);

				printf("|\n");
			}
			printf("+---------+---------+-------------------+-------------------+-----+---------+-------------------+-------------------+-------------------+------+\n\n");
		}
	}

	close(fd);
}

