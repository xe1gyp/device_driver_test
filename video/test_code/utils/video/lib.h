#include "omap24xxvout.h"

#define VIDEO_DEVICE1 "/dev/video1"
#define VIDEO_DEVICE2 "/dev/video2"
#define VIDEO_DEVICE3 "/dev/video3"
#define WB_DEV	 "/dev/video4"

#define OMAP2_V1	1
#define OMAP2_V2	2
#define OMAP2_V3        3

#ifndef OMAP_4430
#define ROTATE_OFFSET 32
#define BG_COLOR_OFFSET 33

#define V4L2_CID_ROTATE (V4L2_CID_BASE+ROTATE_OFFSET)
#define V4L2_CID_BG_COLOR (V4L2_CID_BASE+BG_COLOR_OFFSET)
#endif

int show_info(enum v4l2_buf_type type, int file_descriptor);
