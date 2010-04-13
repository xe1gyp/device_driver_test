#include "omap24xxvout.h"

#define VIDEO_DEVICE1 "/dev/video1"
#define VIDEO_DEVICE2 "/dev/video2"

#define OMAP2_V1	1
#define OMAP2_V2	2

#define V4L2_CID_ROTATE 0x980922
#define V4L2_CID_BG_COLOR 0x980923

#define BASE_VIDIOC_PRIVATE     192             /* 192-255 are private */
#define VIDIOC_S_OMAP2_LINK      _IOW('V', 193, int)
#define VIDIOC_G_OMAP2_LINK      _IO('V', 194)

int show_info(int fd);

