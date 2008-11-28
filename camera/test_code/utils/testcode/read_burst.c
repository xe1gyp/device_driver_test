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
#define VIDIOC_BURSTON                  _IOW ('O', 3, int)
#define VIDIOC_BURSTOFF                 _IOW ('O', 4, int)
#define VIDIOC_R_BURST                  _IOR ('O', 5, char)
#define FBDEVICE "/dev/fb/0"

static void usage(void)
{
	printf("read_burst <no_requested> <no_reads> [<file>]\n");
	printf("\tTo capture n images\n");
	printf("\tOptionally the captured images can be saved to file <file>\n");
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
	struct v4l2_format format;
	int fd, fd_save = 0, ret = 0;
	int no_requested, no_reads;
	int no_captures, i;
	char *out_file = NULL;

	if ((argc > 1) && (!strcmp(argv[1], "?"))) {
		usage();
		return 0;
	}

	screen_info.fd = open(FBDEVICE, O_RDWR);
	if (screen_info.fd <= 0) {
		printf("Could not open the device " FBDEVICE "\n");
        	return -1;
	}

	if (argc >= 3) {
		no_requested = atoi(argv[1]);
		no_reads = atoi(argv[2]);
	}
	else {
		usage();
		return 0;
	}
	if (argc >= 4) {
		out_file = malloc(sizeof(char)*(strlen(argv[2]+strlen(argv[3])+1)));
	}

	ret = ioctl(screen_info.fd, FBIOGET_FSCREENINFO, &screen_info.fbfix);
	if (ret != 0) {
		perror("FBIOGET_FSCREENINFO");
		return -1;
	}
	ioctl (screen_info.fd, FBIOGET_VSCREENINFO, &screen_info.info) ;
	screen_info.width = screen_info.info.xres ;
	screen_info.height = screen_info.info.yres ;
	printf ("physical address of frame buffer : %x\n", screen_info.fbfix.smem_start);
	printf ("Screen Width = %d, Height = %d\n", screen_info.width, screen_info.height);

	screen_info.data = (unsigned char *)mmap (0,
                                     screen_info.width*screen_info.height*2,
                                     (PROT_READ|PROT_WRITE),
                                     MAP_SHARED, screen_info.fd, 0);

	if ((fd = open_cam_device(O_RDONLY)) <= 0) {
		printf ("Could not open the cam device\n");
		return -1;
	}
	 
	format.type = V4L2_BUF_TYPE_STILL_CAPTURE;
	ret = ioctl(fd, VIDIOC_G_FMT, &format);
	if (ret < 0) {
		perror("VIDIOC_G_FMT with V4L2_BUF_TYPE_STILL_CAPTURE");
		format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		ret = ioctl(fd, VIDIOC_G_FMT, &format);
		if (ret < 0) {
			perror("VIDIOC_G_FMT with V4L2_BUF_TYPE_VIDEO_CAPTURE");
			return -1;
		}
	}

	ret=ioctl(fd, VIDIOC_BURSTON, &no_requested);
	if (ret<0) {
		perror("BURSTON");
		return -1;
	}

	no_captures = (no_requested < no_reads) ? no_reads : no_requested;
	
	for (i=0; i<no_captures; i++) {	
		/* Want our buffer aligned at 0x20 so we can use camera driver zero-copy */
    		data_start = buf = malloc(format.fmt.pix.sizeimage + 0x20);
		if (buf == NULL) {
			printf("can't allocate memory!\n");
			return -1;
		}
		while ((((int)data_start) & 0x1f) != 0) data_start++;  
		printf("User buffer start = 0x%lx\n", data_start);

		if (i == no_reads) {
			ioctl(fd, VIDIOC_BURSTOFF, &no_requested);
		}
		ret=ioctl(fd, VIDIOC_R_BURST, data_start);
		if (ret <= 0) {
			perror("READ");
			return -1;
		}
	
		rotate_image(data_start, format.fmt.pix.width, format.fmt.pix.height,
		screen_info.data, screen_info.width, screen_info.height); 
 
		if (out_file) {
			sprintf(out_file, "%s%d", argv[3], (i+1));
			if ((fd_save = creat(out_file, O_RDWR)) > 0) {
				write(fd_save, data_start, format.fmt.pix.width*format.fmt.pix.height*2); 
				close(fd_save);
				printf("Saved the captured image to %s\n", out_file);
			}
			else {
				printf("Can't create file %s\n", out_file);
				fd_save = 0;
			}
		}
	
		free(buf);
	}

	munmap(screen_info.data, screen_info.width*screen_info.height*2);
	close(screen_info.fd);
	close(fd);
	
}

