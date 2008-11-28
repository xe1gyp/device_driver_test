
#define VIDIOC_S_VID2          _IOWR ('V', 90, struct omap24xx_vid2_format)
#define VIDIOC_S_VID2_DISABLE          _IOWR ('V', 91, int)

struct omap24xx_vid2_format{
                struct v4l2_pix_format pix;
                __s32   left;   /* following two members are defined to */
                __s32   top;    /* position the video2 layer on the lcd */

};
