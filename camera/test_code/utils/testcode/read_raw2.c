/* ================================================================================
*             Texas Instruments OMAP(TM) Platform Software
*  (c) Copyright Texas Instruments, Incorporated.  All Rights Reserved.
*
*  Use of this software is controlled by the terms and conditions found 
*  in the license agreement under which this software has been supplied.
* ================================================================================ */

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <string.h>

#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/videodev.h>

#define V4L2_BUF_TYPE_STILL_CAPTURE 	V4L2_BUF_TYPE_PRIVATE
#define VIDIOC_S_OMAP2_ROTATION         _IOW ('V', 3,  int)
#define FBDEVICE "/dev/fb0"
#define VIDEO_DEVICE1 "/dev/video1"
#define VIDEO_DEVICE2 "/dev/video2"

#define V4L2_PIX_FMT_SGRBG10   v4l2_fourcc('B','A','1','0') /* 10bit raw     */

static void usage(void)
{
	printf("read [(vid)] [<file>]\n");
	printf("\tTo capture an one-shot image\n");
	printf("\t[vid] is the video pipeline t be used. Valid vid is 1(default)or 2\n");
	printf("\tusing 0 for [vid] forces usage of framebuffer\n");
	printf("\tOptionally the captured image can be saved to file <filename>\n");
}
 
int main(int argc, char *argv[])
{
	struct screen_info_struct {
		int fd ;
		char *data ;
		int width ;
		int height ;
		struct fb_fix_screeninfo fbfix;
		struct fb_var_screeninfo info;
	} screen_info ;

    	void *buf, *data_start;
	struct v4l2_capability capability;
	struct v4l2_format cformat, vformat;
      	int fd, ret = 0;	
	int index = 0, set_video_img = 0;
	int degree;
	struct v4l2_buffer buffer;
	struct v4l2_requestbuffers vreqbuf;
	FILE *fd_save = NULL;
	if ((argc > 1) && (!strcmp(argv[1], "?"))) {
		usage();
		return 0;
	}


	if ((fd = open_cam_device(O_RDONLY)) <= 0) {
		printf ("Could not open the cam device\n");
		return -1;
	}
 
	cformat.type = V4L2_BUF_TYPE_STILL_CAPTURE;
	ret = ioctl(fd, VIDIOC_G_FMT, &cformat);
	if (ret < 0) {
		perror("VIDIOC_G_FMT with V4L2_BUF_TYPE_STILL_CAPTURE");
		cformat.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		ret = ioctl(fd, VIDIOC_G_FMT, &cformat);
		if (ret < 0) {
			perror("VIDIOC_G_FMT with V4L2_BUF_TYPE_VIDEO_CAPTURE");
			return -1;
		}
        }

        if (!strcmp (argv[++index], "QQCIF")) {
            cformat.fmt.pix.width = 88;
            cformat.fmt.pix.height = 72;
        }
        else if (!strcmp (argv[index], "QQVGA")) {
            cformat.fmt.pix.width = 160;
            cformat.fmt.pix.height = 120;
        }
        else if (!strcmp (argv[index], "QCIF")) {
            cformat.fmt.pix.width = 176;
            cformat.fmt.pix.height = 144;
        }
        else if (!strcmp (argv[index], "QVGA")) {
            cformat.fmt.pix.width = 320;
            cformat.fmt.pix.height = 240;
        }
        else if (!strcmp (argv[index], "CIF")) {
            cformat.fmt.pix.width = 352;
            cformat.fmt.pix.height = 288;
        }
        else if (!strcmp (argv[index], "VGA")) {
            cformat.fmt.pix.width = 640;
            cformat.fmt.pix.height = 480;
        }
        else if (!strcmp (argv[index], "SVGA")) {
            cformat.fmt.pix.width = 800;
            cformat.fmt.pix.height = 600;
        }
        else if (!strcmp (argv[index], "SXGA")) {
            cformat.fmt.pix.width = 1280;
            cformat.fmt.pix.height = 960;
        }
        else if (!strcmp (argv[index], "UXGA")) {
            cformat.fmt.pix.width = 1600;
            cformat.fmt.pix.height = 1200;
        }
        else if (argc > 3) {
            cformat.fmt.pix.width = atoi(argv[index]);
            cformat.fmt.pix.height = atoi(argv[++index]);
        }
        else {
            printf("unsupported size!\n");
            return -1;
        }


        if (!strcmp (argv[++index], "YUYV"))
            cformat.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
        else if (!strcmp (argv[index], "UYVY"))
            cformat.fmt.pix.pixelformat = V4L2_PIX_FMT_UYVY;
        else if (!strcmp (argv[index], "RGB565"))
            cformat.fmt.pix.pixelformat = V4L2_PIX_FMT_RGB565;
        else if (!strcmp (argv[index], "RGB555"))
            cformat.fmt.pix.pixelformat = V4L2_PIX_FMT_RGB555;
        else if (!strcmp (argv[index], "RGB565X"))
            cformat.fmt.pix.pixelformat = V4L2_PIX_FMT_RGB565X;
        else if (!strcmp (argv[index], "RGB555X"))
            cformat.fmt.pix.pixelformat = V4L2_PIX_FMT_RGB555X;
        else if (!strcmp (argv[index], "RAW10"))
            cformat.fmt.pix.pixelformat = V4L2_PIX_FMT_SGRBG10;
        else {
            printf("unsupported cformat!\n");
            return -1;
        }

        /* set size & cformat of the picture captured */
        ret = ioctl(fd, VIDIOC_S_FMT, &cformat);
        if (ret < 0) {
            perror ("VIDIOC_S_FMT");
            return -1;
        }

	cformat.type = V4L2_BUF_TYPE_STILL_CAPTURE;
	ret = ioctl(fd, VIDIOC_G_FMT, &cformat);
        if (ret < 0) {
            perror("VIDIOC_G_FMT with V4L2_BUF_TYPE_STILL_CAPTURE");
            return -1;
	}

        printf("Camera Image width = %d, Image height = %d, Size = %d Format = %x\n", 
                cformat.fmt.pix.width,cformat.fmt.pix.height,
                cformat.fmt.pix.sizeimage,
                cformat.fmt.pix.pixelformat);

        if ((fd_save = fopen(argv[++index], "w")) <= 0) {
            printf("Can't create file %s\n", argv[index]);
            fd_save = 0;
            return -1;
        }    

        /* Want our buffer aligned at 0x20 so we can use camera driver zero-copy */
        data_start = buf = malloc(cformat.fmt.pix.sizeimage + 0x20);
        if (buf == NULL) {
            printf("can't allocate memory!\n");
            return -1;
        }
        while ((((int)data_start) & 0x1f) != 0) data_start++;  
        printf("User buffer start = 0x%lx\n", data_start);

        //sleep(1);
        ret = read(fd, data_start, cformat.fmt.pix.sizeimage);
        if (ret <= 0) {
            perror("READ");
            return -1;
        }

        if (fd_save) {
            //		fwrite(fd_save, data_start, cformat.fmt.pix.width*cformat.fmt.pix.height*2); 
            fwrite(data_start, cformat.fmt.pix.sizeimage, 1 , fd_save);
            fflush(fd_save);
            printf("Saved the captured image to %s\n", argv[3]);
        }

        if(fd_save)
            fclose(fd_save);
        close(fd);

        {
            FILE *fp = fopen("test.dat","wb");
            unsigned short word = 0xABCD;
            if(fp == NULL)
            {
                printf("cannot open file \n");
                exit(-1);
            }
            fwrite(&word,1,sizeof(word),fp);
            fclose(fp);

        }

}

