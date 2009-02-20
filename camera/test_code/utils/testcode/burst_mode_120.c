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
#include <linux/videodev2.h>
#include <linux/errno.h>
#include <sys/mman.h>
#include <string.h>

#undef DEBUG
    
#define VIDIOC_S_OMAP2_ROTATION		_IOW ('V', 3, int)
#define VIDEO_DEVICE1 "/dev/video1"
#define VIDEO_DEVICE2 "/dev/video2"

#define COLOR_LEVEL 0
#define SEPIA_LEVEL 1
#define BW_LEVEL 2

#define DEF_CONT_LEVEL 2
#define DEF_BRT_LEVEL 1

#define DEFAULT_PIXEL_FMT 	"YUYV"
#define DEFAULT_VIDEO_SIZE 	"QVGA"
#define DEFAULT_FRAMERATE 	120
#define DEFAULT_FILE_NAME 	"output_120fps.yuv"
#define SKIPPED_FRAMES 10
#define CAM_DEVICE		1
#define CAPTURED_FRAMES		120
#define BUFFERS_REQUESTED	4

/* have to align at 32 bytes */ 
#define ALIGN 1
static void usage(void) 
{
	printf("Special Burst Mode Test Case: Capture 120 frames\n");
	printf("Usage: burst_mode_120 <[size W][size H]> <file_name>\n");
	printf("   [sizeW] Set the video width\n");
	printf("   [sizeH] Set the video heigth\n");
	printf("\tOptionally size can be specified using standard name sizes"
							"(QVGA,QCIF)\n");
	printf("   <file_name> Optionally can be defined the file name of "
							"saved frames\n");
	printf("    If no argument is given, size = QXGA & output_120fps.yuv");
	printf("Note: Default values\n"
			"camera device = 1 (MT9P012)" 
			"framerate = 120\n"
			"Frames to be captured=120\n"
			"Buffers to be used = 4\n");
}

struct {
	void *start;
	void *start_aligned;
	size_t length;
} *cbuffers, *tempBuffers;

int main(int argc, char *argv[]) 
{
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
	int buffersRequested = BUFFERS_REQUESTED;
	
	colorLevel = COLOR_LEVEL;
	
	if ((argc > 1) && (!strcmp(argv[1], "?"))) {
		usage();
		return 0;
	}
	
	/**********************************************************************/
	
	device = CAM_DEVICE;
	if ((fd = open_cam_device(O_RDWR,device)) <= 0) {
		printf ("Could not open the cam device\n");
		return -1;
	}

	if (argc > index) {
		pixelFmt = argv[index];
		index++;
		if (argc > index){
			ret = validateSize(argv[index]);
			if (ret == 0) {
				ret = cam_ioctl(fd, pixelFmt, argv[index]);
				if (ret < 0) {
					usage();
					return -1;
				}
			} else {
				index++;
				if (argc > (index)) {
					ret = cam_ioctl(fd, pixelFmt,
						argv[index-1], argv[index]);
					if (ret < 0) {
						usage();
						return -1;
					}
				} else {
					printf("Invalid size\n");
					usage();
					return -1;
				}
			}
			index++;
		}
		else {
			printf("Setting QCIF as video size, default value\n");
			ret = cam_ioctl(fd, pixelFmt, DEFAULT_VIDEO_SIZE);
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
	
	ret = setFramerate(fd,DEFAULT_FRAMERATE);
	
	/**********************************************************************/

	count = CAPTURED_FRAMES;
	printf("Frames: %d\n", count);

	/*if (count >= 32 || count <= 0) {
		printf("Camera driver only support max 32 buffers, "
			"you request %d\n", count);
		return -1;
	}*/
	
	if (argc > index) {
		fileName = argv[index];
	} else {
		fileName = DEFAULT_FILE_NAME;
	}
	
	if ((fd_save = creat(fileName, O_RDWR)) <= 0) {
			printf("Can't create file %s\n", fileName);
			fd_save = 0;
	} else {
		printf("The captured frames will be saved into: %s\n",
						fileName);
	}
	
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
	creqbuf.count = BUFFERS_REQUESTED;

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
	/*if (creqbuf.count != count) {
		count = creqbuf.count;
	}*/
	cbuffers = calloc(creqbuf.count, sizeof(*cbuffers));
	tempBuffers = calloc(CAPTURED_FRAMES, sizeof(*tempBuffers));
#if 0
	for (i = 0; i < CAPTURED_FRAMES ; ++i) {
		tempBuffers[i].length = cformat.fmt.pix.sizeimage;
		/*if (tempBuffers[i].length & 0xfff) {
			tempBuffers[i].length = 
			(tempBuffers[i].length & 0xfffff000) + 0x1000;
		}*/
		tempBuffers[i].start = malloc(tempBuffers[i].length);
		/*tempBuffers[i].start_aligned =
			    (void *)((unsigned int)(tempBuffers[i].start) &
				     (0xffffffe0)) + 0x20;*/
	}
#endif
	
	
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
			printf("User Buffers[%d].start=[%x]start_aligned= "
				"= %x  length = %d\n",
				i, cbuffers[i].start, 
				cbuffers[i].start_aligned, 
				cbuffers[i].length);
		} else {
			cbuffers[i].length = buffer.length;
			cbuffers[i].start =
			    mmap(NULL, buffer.length, PROT_READ | PROT_WRITE,
				 MAP_SHARED, fd, buffer.m.offset);
			if (cbuffers[i].start == MAP_FAILED) {
				perror("mmap");
				return -1;
			}
			printf("Mapped Buffers[%d].start= %x  length = %d\n",
				i, cbuffers[i].start, 
				cbuffers[i].length);
			buffer.m.userptr = (unsigned int)cbuffers[i].start;
			buffer.length = cbuffers[i].length;
		}
		if (ioctl(fd, VIDIOC_QBUF, &buffer) < 0) {
			perror("CAMERA VIDIOC_QBUF");
			return -1;
		}
	}

	/* mmap driver memory or allocate user memory, and queue each buffer */ 
	for (i = 0; i < CAPTURED_FRAMES; ++i) {
		
		struct v4l2_buffer buffer2;
		buffer2.type = creqbuf.type;
		buffer2.memory = creqbuf.memory;
		buffer2.index = i%4;
		if (ioctl(fd, VIDIOC_QUERYBUF, &buffer2) < 0) {
			perror("VIDIOC_QUERYBUF");
			return -1;
		}
		
		if (memtype == V4L2_MEMORY_USERPTR) {
			tempBuffers[i].length = buffer2.length;
			if (tempBuffers[i].length & 0xfff) {
				tempBuffers[i].length =
				    (tempBuffers[i].length & 0xfffff000) + 0x1000;
			}
			tempBuffers[i].start = malloc(tempBuffers[i].length);
			tempBuffers[i].start_aligned =
			    (void *)((unsigned int)(tempBuffers[i].start) &
				     (0xffffffe0)) + 0x20;
			buffer2.length = tempBuffers[i].length;
			buffer2.m.userptr =
			    (unsigned int)tempBuffers[i].start_aligned;
#ifdef DEBUG
			printf("Temp Buffer [%d].start:[%x].start_aligned "
				"= %x  length = %d\n", i, 
				tempBuffers[i].start, 
				tempBuffers[i].start_aligned, 
				tempBuffers[i].length);
#endif
		}
	}
#ifdef DEBUG
	printf("After query temp buffer\n");
#endif
	/* turn on streaming */ 
	if (ioctl(fd, VIDIOC_STREAMON, &creqbuf.type) < 0) {
		perror("VIDIOC_STREAMON");
		return -1;
	}

	/* capture 120 frames or when we hit the passed number of frames */ 
	cfilledbuffer.type = creqbuf.type;
	cfilledbuffer.memory = memtype;

	i = 0;

	while (i < 1000) {
		/* De-queue the next avaliable buffer */ 
		while (ioctl(fd, VIDIOC_DQBUF, &cfilledbuffer) < 0) {
			perror("VIDIOC_DQBUF");
			printf(" ERROR HAS OCCURED\n");
		}
		
		/*write(fd_save, cbuffers[cfilledbuffer.index].start_aligned,
			       cformat.fmt.pix.sizeimage);*/
		memcpy(tempBuffers[i].start_aligned,
		cbuffers[cfilledbuffer.index].start_aligned,
		cfilledbuffer.length);
		
		i++;
#ifdef DEBUG
		printf("COUNT = %d\ni=%d\n",count,i);
#endif
		if (i == (count)) {
			printf("Cancelling the streaming capture...\n");
			creqbuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
			if (ioctl(fd, VIDIOC_STREAMOFF, &creqbuf.type) < 0) {
				perror("VIDIOC_STREAMOFF");
				return -1;
			}
			printf("Done\n");
			break;
		}
		
		while (ioctl(fd, VIDIOC_QBUF, &cfilledbuffer) < 0)
			perror("CAM VIDIOC_QBUF");
	}
	/* we didn't turn off streaming yet */ 
	if (count == -1) {
		creqbuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		if (ioctl(fd, VIDIOC_STREAMOFF, &creqbuf.type) == -1) {
			perror("VIDIOC_STREAMOFF");
			return -1;
		}
	}
	printf("Captured %d frames!\n", i);
	printf("Start writing to file\n");
	if (fd_save > 0) {
		for (i = 0; i < CAPTURED_FRAMES; i++)
			write(fd_save, tempBuffers[i].start_aligned,
			       cformat.fmt.pix.sizeimage);
	}
	printf("Completed writing to file\n");
	
	for (i = 0; i < CAPTURED_FRAMES ; i++) {
		if (tempBuffers[i].start)
			free(tempBuffers[i].start);
	}
	printf("tempBuffers Freed\n");
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
