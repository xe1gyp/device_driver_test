#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h> 
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <linux/videodev.h>
#include <errno.h>
#include "ydis.h"



#include <unistd.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <string.h>

#include <sys/time.h>
#include <linux/fs.h>
#include <linux/kernel.h>
//

/* defines */

#define SUCCESS    0
#define FBDEVICE "/dev/fb0"

#define FB_ERROR_COULDNOT_OPEN_DEVICE     -10
#define FB_ERROR_DEVICE_NOT_READY         -11


#define VIDEO_DEVICE "/dev/video0"

#define CAM_ERROR_COULDNOT_OPEN_DEVICE   -30
#define CAM_ERROR_NO_MEMORY              -32
extern int errno;


/* locals */
struct screen_info
{
   int fd ;
   char *data ;
   int width ;
   int height ;
   struct fb_fix_screeninfo fbfix;
   struct fb_var_screeninfo info;
   int color ;
   int delay ;
} Screen_info ;

int fd;

/* locals */

struct video_info_app
{
   int fd ;
   struct  v4l2_capability cap ;
   struct  v4l2_format fmt ;
   char *data ;
} Video_info ;

int init_video (void)
{
    printf("inside init_video function \n");
    Video_info.fd = open (VIDEO_DEVICE, O_RDWR) ;
    if (Video_info.fd <= 0)
    {
        printf ("Could not open the device " VIDEO_DEVICE "\n") ;
        return -1;
    }
	printf("init Video_info.fd = %d \n", Video_info.fd);
    return (SUCCESS) ;
}

int Process_Video (int size)
{
    int i, j, n ;
    int offset = 64 ;
	char a = 0xff;
	char *ashu;
	ashu = (char *) malloc ( size );
	switch(size)
	{

		case 50688:
		{
			fd = open("/mmc/cameratest/camera_data_qcif", O_RDONLY);
			break;
		}

		case 38400:
		{
			fd = open("/mmc/cameratest/camera_data_qqvga", O_RDONLY);
			break;
		}

		case 153600:
		{
			fd = open("/mmc/cameratest/camera_data_qvga", O_RDONLY);
			break;
		}
	}
	if (fd < 0) {
		printf(" cannot open file to read data \n");
		exit (1);
	}
	
	printf("Video_info.fd = %d \n", Video_info.fd);

	n= read(fd, ashu, size);
	while( n >= size){
	if((i = 	write (Video_info.fd, ashu, size)) == -1)
	{
	printf("errno= %d", errno);
	perror("write");
	}
	n= read(fd, ashu, size);
 	sleep(1);
	
	}
	close(fd);
	
    return (SUCCESS) ;
}
 
int main (int argc, char *argv[])
{
    struct v4l2_format format;
    int count = 0;
    int color = 0xbb ;
	int ret = 0;
	int i=0;

    struct omap24xx_vid2_format vid2_format;
    if (init_video() != SUCCESS)
    {
        return -1 ;
    }

    format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    ret = ioctl (Video_info.fd, VIDIOC_G_FMT, &format);
    if (ret < 0) {
	printf ("ERROR\n");
	return 0;
    }

    if (!strcmp (argv[1], "QQCIF")) 
    {
	format.fmt.pix.width = 88;
	format.fmt.pix.height = 72;

    }

    if (!strcmp (argv[1], "QQVGA")) 
    {
	format.fmt.pix.width = 160;
	format.fmt.pix.height = 120;
	
    }

    if (!strcmp (argv[1], "QCIF"))
    {
        format.fmt.pix.width = 176;
        format.fmt.pix.height = 144;

    }

    if (!strcmp (argv[1], "QVGA")) 
    {
	format.fmt.pix.width = 320;
	format.fmt.pix.height = 240;
        
    }

    if (!strcmp (argv[1], "CIF"))
    {
        format.fmt.pix.width = 352;
        format.fmt.pix.height = 288;

    }

    if (!strcmp (argv[1], "VGA"))
    {
        format.fmt.pix.width = 640;
        format.fmt.pix.height = 480;

    }

    if (!strcmp (argv[1], "SXGA"))
    {
        format.fmt.pix.width = 1280;
        format.fmt.pix.height = 960;

    }

	format.fmt.pix.sizeimage = format.fmt.pix.width * format.fmt.pix.height * 2;
	format.fmt.pix.bytesperline = format.fmt.pix.width * 2;

	if (!strcmp (argv[2], "YUYV"))
	    format.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;

	if (!strcmp (argv[2], "RGB565"))
	    format.fmt.pix.pixelformat = V4L2_PIX_FMT_RGB565;

	if (!strcmp (argv[2], "RGB555"))
	    format.fmt.pix.pixelformat = V4L2_PIX_FMT_RGB555;

#if 0
    ret = ioctl (Video_info.fd, VIDIOC_S_FMT, &format);
    if (ret < 0) {
	perror ("VIDIOC_S_FMT");
	//printf ("ERROR\n");
	return 0;
	}

    ret = ioctl (Video_info.fd, VIDIOC_G_FMT, &format);
    if (ret < 0) {
        printf ("ERROR\n");
        return 0;
        }
#endif

    printf ("new picture width = %d\n",format.fmt.pix.width);
    printf ("new picture height = %d\n",format.fmt.pix.height);
    printf ("new picture pixelformat = %x\n",format.fmt.pix.pixelformat);
    printf ("new picture colorspace = %x\n",format.fmt.pix.colorspace);

	vid2_format.pix = format.fmt.pix;
	vid2_format.left = atoi(argv[3]);
	vid2_format.top = atoi(argv[4]);
    ret = ioctl (Video_info.fd, VIDIOC_S_VID2, &vid2_format);

	/* display the frame from static file to the video2 layer */
        Process_Video ( format.fmt.pix.width * format.fmt.pix.height *2) ;
	
	sleep(2);
	ret = ioctl (Video_info.fd, VIDIOC_S_VID2_DISABLE, &i);
    if (ret < 0) 
	perror ("VIDIOC_S_FMT");


}




