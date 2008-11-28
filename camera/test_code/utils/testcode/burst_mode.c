/* =========================================================================
*             Texas Instruments OMAP(TM) Platform Software
*  (c) Copyright Texas Instruments, Incorporated.  All Rights Reserved.
*
*  Use of this software is controlled by the terms and conditions found 
*  in the license agreement under which this software has been supplied.
* ========================================================================== */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <linux/ioctl.h>
#include <linux/fb.h>
#include <linux/videodev.h>
#include <linux/errno.h>
#include <sys/mman.h>
#include <string.h>
    
#define VIDIOC_S_OMAP2_ROTATION		_IOW ('V', 3, int)
#define VIDEO_DEVICE1 "/dev/video1"
#define VIDEO_DEVICE2 "/dev/video2"

#define COLOR_LEVEL 0
#define SEPIA_LEVEL 1
#define BW_LEVEL 2

#define DEF_CONT_LEVEL 2
#define DEF_BRT_LEVEL 1

#define DEFAULT_PIXEL_FMT "YUYV"
#define DEFAULT_VIDEO_SIZE "QCIF"
#define DEFAULT_FILE_NAME "output.yuv"
#define SKIPPED_FRAMES 10
#define DEFAULT_FRAMERATE 15

/* have to align at 32 bytes */ 
#define ALIGN 1
static void usage(void) 
{
	printf("Burst Mode Test Case\n");
	printf("Usage: burst_mode [camDevice] [pixelFormat] [<sizeW> <sizeH>]"
			" [<numberOfFrames>] [<file>] [<colorEffect>]\n");
	printf("   [camDevice] Camera device to be open\n\t 1:Micron sensor "
					"2:OV sensor\n");
	printf("   [pixelFormat] set the pixelFormat to use. \n\tSupported:"
					" YUYV, UYVY, RGB565, RGB555, RGB565X,"
							" RGB555X, RAW10 \n");
	printf("   [sizeW] Set the video width\n");
	printf("   [sizeH] Set the video heigth\n");
	printf("\tOptionally size can be specified using standard name sizes"
							"(VGA,PAL,etc)\n");
	printf("\tIf size is NOT specified QCIF used as default\n");
	printf("   [numberOfFrames] Number of Frames to be captured\n");
	printf("   [file] Optionally captured image can be saved to file "
								"<file>\n");
	printf("    If no file is specified output.yuv file is the default\n");
	printf("   [colorEffect] BW      The image captured with "
						"Black & White effect\n");
	printf("                 SEPIA   The image captured with Sepia "
								"effect\n");
	printf("                 If not specified COLOR effect is the "
							"default Option\n");
}

struct {
	void *start;
	void *start_aligned;
	size_t length;
} *cbuffers;

int main(int argc, char *argv[]) 
{
	struct screen_info_struct {
		int fd;
		char *data;
		int width;
		int height;
		struct fb_fix_screeninfo fbfix;
		struct fb_var_screeninfo info;
	} screen_info;
	
	void *src_start;
	struct v4l2_capability capability;
	struct v4l2_format cformat;
	struct v4l2_requestbuffers creqbuf;
	struct v4l2_buffer cfilledbuffer;
	struct v4l2_queryctrl queryctrl;
	struct v4l2_control control;
	int fd, i, ret, count = 1, memtype = V4L2_MEMORY_USERPTR;
	int fd_save = 0;
	int index = 1, vid = 1, set_video_img = 0;
	int degree;
	int j;
	int device = 1;
	int colorLevel;
	char * pixelFmt;
	char * fileName;
	
	colorLevel=COLOR_LEVEL;
	
	if ((argc > 1) && (!strcmp(argv[1], "?"))) {
		usage();
		return 0;
	}
	
	/**********************************************************************/
	
	if (argc > index) {
		device = atoi(argv[index]);
		index++;
	}
	
	if ((fd = open_cam_device(O_RDWR,device)) <= 0) {
		printf ("Could not open the cam device\n");
		return -1;
	}

	if (argc > index){
	pixelFmt=argv[index];
	index++;
		if (argc > index){
			ret = validateSize(argv[index]);
			if (ret == 0) {
				ret = cam_ioctl(fd,pixelFmt,argv[index]);
				if (ret < 0) {
					usage();
					return -1;
				}
			}
			else {
				index++;
				if (argc > (index)) {
					ret = cam_ioctl(fd,pixelFmt,
						argv[index-1], argv[index]);
					if (ret < 0) {
						usage();
						return -1;
					}
				}
				else {
					printf("Invalid size\n");
					usage();
					return -1;
				}
			}
			index++;
		}
		else {
			printf("Setting QCIF as video size, default value\n");
			ret = cam_ioctl(fd,pixelFmt,DEFAULT_VIDEO_SIZE);
			if (ret < 0)
				return -1;
		}
	}
	else {
		printf("Setting pixel format and video size with default"
								" values\n");
		ret = cam_ioctl(fd, DEFAULT_PIXEL_FMT, DEFAULT_VIDEO_SIZE);
		if (ret < 0)
			return -1;
	}
	
	/**********************************************************************/
	if (cformat.fmt.pix.width == 2592 && cformat.fmt.pix.height == 1944)
		ret = setFramerate(fd,13);
	else 
		ret = setFramerate(fd,DEFAULT_FRAMERATE);
	
	if (ret < 0) {
		printf("Error setting framerate");
		return -1;
	}
	
	if (argc > index)
		count = atoi(argv[index]);

	printf("Frames: %d\n", count);
	index++;

	if (count >= 32 || count <= 0) {
		printf("Camera driver only support max 32 buffers, "
			"you request %d\n", count);
		return -1;
	}
	
	if (argc > index) {
		fileName = argv[index];
	}
	else {
		fileName = DEFAULT_FILE_NAME;
	}
	
	if ((fd_save = creat(fileName, O_RDWR)) <= 0) {
			printf("Can't create file %s\n", fileName);
			fd_save = 0;
	}
	else {
		printf("The captured frames will be saved into: %s\n",fileName);
	}
		
	index++;
	
	if (argc > index) {
		if(!strcmp(argv[index], "BW")){
			colorLevel=BW_LEVEL;
			printf("Using black & white color level: %d\n",colorLevel);
		}
		else {
			if(!strcmp(argv[index], "SEPIA")) {
				colorLevel=SEPIA_LEVEL;
				printf("Using SEPIA color level: %d\n",colorLevel);
			}
			else
				if(!strcmp(argv[index], "SEPIA"))
					colorLevel=SEPIA_LEVEL;
				else{
					printf("Invalid Color Effect: argv[%d]"
						"=%s",index,argv[index]);
					usage();
					return 0;
				}
		}
	}

	index++;
	
	if (ioctl(fd, VIDIOC_QUERYCAP, &capability) < 0) {
		perror("VIDIOC_QUERYCAP");
		return -1;
	}
	if (capability.capabilities & V4L2_CAP_STREAMING)
		printf("The driver is capable of Streaming!\n");	
	else {
		printf("The driver is not capable of Streaming!\n");
		return -1;
	}
	cformat.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	ret = ioctl(fd, VIDIOC_G_FMT, &cformat);
	if (ret < 0) {
		perror("VIDIOC_G_FMT");
		return -1;
	}
	printf("Camera Image width = %d, Image height = %d, size = %d\n",
		 cformat.fmt.pix.width, cformat.fmt.pix.height,
		 cformat.fmt.pix.sizeimage);
	
	creqbuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	creqbuf.memory = memtype;
	creqbuf.count = count;
	
	printf("Requesting %d buffers of type %s\n", creqbuf.count, 
		 (memtype ==
		  V4L2_MEMORY_USERPTR) ? "V4L2_MEMORY_USERPTR" :
		 "V4L2_MEMORY_MMAP");
	if (ioctl(fd, VIDIOC_REQBUFS, &creqbuf) < 0) {
		perror("VIDEO_REQBUFS");
		return -1;
	}
	printf("Camera Driver allowed buffers reqbuf.count = %d\n",
		 creqbuf.count);
	if (creqbuf.count != count) {
		count = creqbuf.count;
	}
	cbuffers = calloc(creqbuf.count, sizeof(*cbuffers));
	
	/* mmap driver memory or allocate user memory, and queue each buffer */ 
	for (i = 0; i < creqbuf.count; ++i) {
		struct v4l2_buffer buffer;
		buffer.type = creqbuf.type;
		buffer.memory = creqbuf.memory;
		buffer.index = i;
		if (ioctl(fd, VIDIOC_QUERYBUF, &buffer) < 0) {
			perror("VIDIOC_QUERYBUF");
			return -1;
		}
		if (memtype == V4L2_MEMORY_USERPTR) {
			cbuffers[i].length = buffer.length;
			if (cbuffers[i].length & 0xfff) {
				cbuffers[i].length =
				    (cbuffers[i].length & 0xfffff000) + 0x1000;
			}
			cbuffers[i].start = malloc(cbuffers[i].length);
			cbuffers[i].start_aligned =
			    (void *)((unsigned int)(cbuffers[i].start) &
				     (0xffffffe0)) + 0x20;
			buffer.length = cbuffers[i].length;
			buffer.m.userptr =
			    (unsigned int)cbuffers[i].start_aligned;
			printf("User Buffer [%d].start = %x  length = %d\n",
				 i, cbuffers[i].start, cbuffers[i].length);
		} else {
			cbuffers[i].length = buffer.length;
			cbuffers[i].start =
			    mmap(NULL, buffer.length, PROT_READ | PROT_WRITE,
				 MAP_SHARED, fd, buffer.m.offset);
			if (cbuffers[i].start == MAP_FAILED) {
				perror("mmap");
				return -1;
			}
			printf("Mapped Buffers[%d].start = %x  length = %d\n",
				i, cbuffers[i].start, cbuffers[i].length);
			buffer.m.userptr = (unsigned int)cbuffers[i].start;
			buffer.length = cbuffers[i].length;
		}
		if (ioctl(fd, VIDIOC_QBUF, &buffer) < 0) {
			perror("CAMERA VIDIOC_QBUF");
			return -1;
		}
	}
	
	/* capture 1000 frames or when we hit the passed number of frames */ 
	cfilledbuffer.type = creqbuf.type;
	cfilledbuffer.memory = memtype;
	
	/* query color capability*/
	memset(&queryctrl,0,sizeof(queryctrl));
	
	queryctrl.id=V4L2_CID_PRIVATE_BASE;
	if(ioctl(fd,VIDIOC_QUERYCTRL, &queryctrl) == -1){
	   printf("COLOR effect is not supported!\n");
	}

	control.id = V4L2_CID_PRIVATE_BASE;
	if (ioctl(fd, VIDIOC_G_CTRL, &control) == -1) {
		printf("VIDIOC_G_CTRL failed!\n");
	}
	printf("Color effect at the beginning of the test is supported, min %d," 
		"max %d.\nCurrent color is level is %d\n",
		queryctrl.minimum, queryctrl.maximum, control.value);
		
	/* setting Contrast, Brightness & Color options*/ 
	
	control.id = V4L2_CID_CONTRAST;
	control.value = DEF_CONT_LEVEL;
	if (ioctl(fd, VIDIOC_S_CTRL, &control) == -1) {
		printf("VIDIOC_S_CTRL CONTRAST failed!\n");
	}
	control.id = V4L2_CID_BRIGHTNESS;
	control.value = DEF_BRT_LEVEL;
	if (ioctl(fd, VIDIOC_S_CTRL, &control) == -1) {
		printf("VIDIOC_S_CTRL BRIGHTNESS failed!\n");
	}
	
	control.id = V4L2_CID_PRIVATE_BASE;
	control.value = colorLevel;
	if (ioctl(fd, VIDIOC_S_CTRL, &control) == -1) {
		printf("VIDIOC_S_CTRL COLOR failed!\n");
	}
	
	i = 0;
	
	control.id = V4L2_CID_PRIVATE_BASE;
        if (ioctl(fd, VIDIOC_G_CTRL, &control) == -1) {
                printf("VIDIOC_G_CTRL failed!\n");
        }

	printf("Color effect values after setup is supported, min %d,"
		"max %d.\nCurrent color is level is %d\n",
                queryctrl.minimum, queryctrl.maximum, control.value);

	/* turn on streaming */ 
	if (ioctl(fd, VIDIOC_STREAMON, &creqbuf.type) < 0) {
		perror("VIDIOC_STREAMON");
		return -1;
	}
	
	if(device == 2) {
		count+=SKIPPED_FRAMES;
		printf("OV Sensor used, skipping %d frames\n",SKIPPED_FRAMES);
	}
	
	while (i < 1000) {
		/* De-queue the next avaliable buffer */ 
		while (ioctl(fd, VIDIOC_DQBUF, &cfilledbuffer) < 0) {
			perror("VIDIOC_DQBUF");
			printf(" ERROR HAS OCCURED\n");
		}
		
		i++;
		
		if (i == count) {
			printf("Cancelling the streaming capture...\n");
			creqbuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
			if (ioctl(fd, VIDIOC_STREAMOFF, &creqbuf.type) < 0) {
				perror("VIDIOC_STREAMOFF");
				return -1;
			}
			printf("Done\n");
			break;
		}
		
		while (ioctl(fd, VIDIOC_QBUF, &cfilledbuffer) < 0) {
			perror("CAM VIDIOC_QBUF");
		}
	}
	/* we didn't turn off streaming yet */ 
	if (count == -1) {
		creqbuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		if (ioctl(fd, VIDIOC_STREAMOFF, &creqbuf.type) == -1) {
			perror("VIDIOC_STREAMOFF");
			return -1;
		}
	}
	if (device == 2)
		i-=SKIPPED_FRAMES;
	printf("Captured %d frames!\n", i);
	printf("Start writing to file\n");
	if (fd_save > 0) {
		for (i = 0; i < count; i++)
			write(fd_save, cbuffers[i].start_aligned,
			       cformat.fmt.pix.sizeimage);
	}
	printf("Completed writing to file\n");
	for (i = 0; i < creqbuf.count; i++) {
		if (cbuffers[i].start) {
			if (memtype == V4L2_MEMORY_USERPTR)
				free(cbuffers[i].start);		
			else
				munmap(cbuffers[i].start, cbuffers[i].length);
		}
	}
	
	free(cbuffers);
	close(fd);
	if (fd_save > 0)
		close(fd_save);
}
