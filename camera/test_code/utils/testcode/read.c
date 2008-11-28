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

static void usage(void)
{
	printf("read [(vid)] [<file>]\n");
	printf("\tTo capture an one-shot image\n");
	printf("\t[vid] is the video pipeline t be used. Valid vid is 1(default)or 2\n");
	printf("\tusing 0 for [vid] forces usage of framebuffer\n");
	printf("\tOptionally the captured image can be saved to file <file>\n");
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
	int fd, fd_save = 0, ret = 0;	
	int vid = 1, index = 1, set_video_img = 0;
	int degree;
	struct v4l2_buffer buffer;
	struct v4l2_requestbuffers vreqbuf;

	if ((argc > 1) && (!strcmp(argv[1], "?"))) {
		usage();
		return 0;
	}

	if (argc > 1) {
		vid = atoi(argv[index]);
		if ((vid != 1) && (vid != 2)) {
			if (vid != 0) 
				printf("vid has to be 1 or 2! vid=%d, argv[1]=%s\n",vid, argv[1]);
			else
				index++;
			printf("framebuffer will be used\n");
			vid = 0;
		}
		else
			index++;
	}
	
	if (vid != 0) {
		screen_info.fd = open((vid==1)?VIDEO_DEVICE1:VIDEO_DEVICE2, O_RDWR);
		if (screen_info.fd <= 0) {
			printf("Could no open the device %s\n",(vid==1)?VIDEO_DEVICE1:VIDEO_DEVICE2);
			vid = 0;
		}
		else
			printf("openned %s for rendering\n", (vid == 1)? VIDEO_DEVICE1:VIDEO_DEVICE2);
	}
	if (vid == 0) {
		screen_info.fd = open(FBDEVICE, O_RDWR);
		if (screen_info.fd <= 0) {
			printf("Could not open the device " FBDEVICE "\n");
        		return -1;
		}
	}

	if (argc >= (index + 1)) {
		if ((fd_save = creat(argv[index], O_RDWR)) <= 0) {
			printf("Can't create file %s\n", argv[index]);
			fd_save = 0;
		}    
	}

	if (vid != 0) {
		if (ioctl(screen_info.fd, VIDIOC_QUERYCAP, &capability) == -1) {
			perror("video VIDIOC_QUERYCAP");
			return -1;
		}
		if ( capability.capabilities & V4L2_CAP_STREAMING)
			printf("The video driver is capable of Streaming!\n");
		else {
			printf("The video driver is not capable of Streaming!\n");
			return -1;
		}
	}
	else {
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
	printf("Camera Image width = %d, Image height = %d, size = %d\n", 
		cformat.fmt.pix.width,cformat.fmt.pix.height,cformat.fmt.pix.sizeimage);

	/* Want our buffer aligned at 0x20 so we can use camera driver zero-copy */
    	data_start = buf = malloc(cformat.fmt.pix.sizeimage + 0x20);
	if (buf == NULL) {
		printf("can't allocate memory!\n");
		return -1;
	}
	while ((((int)data_start) & 0x1f) != 0) data_start++;  
	printf("User buffer start = 0x%lx\n", data_start);

	ret = read(fd, data_start, cformat.fmt.pix.sizeimage);
	if (ret <= 0) {
		perror("READ");
		return -1;
	}

	if (vid != 0) {
		vformat.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
		ret = ioctl(screen_info.fd, VIDIOC_G_FMT, &vformat);
		if (ret < 0) {
		    perror("video VIDIOC_G_FMT");
		    return -1;
		}
		printf("Video Image width = %d, Image height = %d, size = %d\n", 
				vformat.fmt.pix.width,vformat.fmt.pix.height, vformat.fmt.pix.sizeimage);
	
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

		degree = 270;
		ret = ioctl (screen_info.fd, VIDIOC_S_OMAP2_ROTATION, &degree);
		if (ret < 0) {
			perror ("VIDIOC_S_OMAP2_ROTATION");
			return 0;
		}
		printf("Rotation set to %d degree\n",degree);

		if (set_video_img) {
			printf("set video image the same as camera image ...\n");
			vformat.fmt.pix.width = cformat.fmt.pix.width;
			vformat.fmt.pix.height = cformat.fmt.pix.height;
			vformat.fmt.pix.sizeimage = cformat.fmt.pix.sizeimage;
			vformat.fmt.pix.pixelformat = cformat.fmt.pix.pixelformat;

			degree = 270;
                        ret = ioctl (screen_info.fd, VIDIOC_S_OMAP2_ROTATION, &degree);
                        if (ret < 0) {
                                perror ("VIDIOC_S_OMAP2_ROTATION");
                                return 0;
                        }
                        printf("Rotation set to %d degree\n",degree);

			ret = ioctl (screen_info.fd, VIDIOC_S_FMT, &vformat);
			if (ret < 0) {
				perror ("video VIDIOC_S_FMT");
				return -1;
			}

			if ((cformat.fmt.pix.width!=vformat.fmt.pix.width) ||
		    		(cformat.fmt.pix.height!=vformat.fmt.pix.height) ||
	    			(cformat.fmt.pix.sizeimage!=vformat.fmt.pix.sizeimage) ||
	    			(cformat.fmt.pix.pixelformat != vformat.fmt.pix.pixelformat)) {
				printf("can't make camera and video image compatible!\n");
				return 0;
			}	 
		}

		vreqbuf.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
		vreqbuf.memory = V4L2_MEMORY_MMAP;
		vreqbuf.count = 1;
		if (ioctl(screen_info.fd, VIDIOC_REQBUFS, &vreqbuf) == -1) {
			perror ("video VIDEO_REQBUFS");
			return;
		}
		printf("Video Driver allocated 1 buffer\n");

		buffer.type = vreqbuf.type;
		buffer.index = 0;
		if (ioctl(screen_info.fd, VIDIOC_QUERYBUF, &buffer) == -1){
			perror("video VIDIOC_QUERYBUF");
			return;
		}
		data_start = mmap(NULL, buffer.length, PROT_READ|
						PROT_WRITE, MAP_SHARED,
						screen_info.fd, buffer.m.offset);
		if (data_start == MAP_FAILED ){
			perror ("video mmap");
			return;
		}

		if (ioctl(screen_info.fd, VIDIOC_STREAMON, &vreqbuf.type) < 0 ) {
			perror("video VIDIOC_STREAMON");
			return -1;
		}

		read(fd, data_start, cformat.fmt.pix.sizeimage);
		if (ret < 0) {
			perror("READ");
			return -1;
		} 

		/* Queue the new buffer to video driver for rendering */
		if (ioctl(screen_info.fd, VIDIOC_QBUF, &buffer) == -1){
			perror ("video VIDIOC_QBUF");
			return;
		}
		
		sleep(5);

		if (ioctl(screen_info.fd, VIDIOC_STREAMOFF, &vreqbuf.type) == -1) {
			perror("video VIDIOC_STREAMOFF");
			return -1;
		}

		munmap(data_start, buffer.length);
	}
	else {
		rotate_image(data_start, cformat.fmt.pix.width, cformat.fmt.pix.height,
			screen_info.data, screen_info.width, screen_info.height); 
	}

	if (fd_save > 0) {
		write(fd_save, data_start, cformat.fmt.pix.width*cformat.fmt.pix.height*2); 
		close(fd_save);
		printf("Saved the captured image to %s\n", argv[1]);
	}

	free(buf);
	munmap(screen_info.data, screen_info.width*screen_info.height*2);
	close(screen_info.fd);
	close(fd);
}

