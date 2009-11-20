#include "omap24xxvout.h"

#define VIDEO_DEVICE1 "/dev/video1"
#define VIDEO_DEVICE2 "/dev/video2"

#define OMAP2_V1	1
#define OMAP2_V2	2

#define V4L2_CID_ROTATE (V4L2_CID_BASE+32)
#define V4L2_CID_BG_COLOR (V4L2_CID_BASE+33)

int show_info(int fd);

