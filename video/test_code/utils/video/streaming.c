#include <stdio.h>
#include <fcntl.h>
#include <linux/ioctl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <linux/videodev.h>
#include <linux/errno.h>
#include <errno.h>

#include "lib.h"


static void streaming_video(int ofd, int ifd, int sleep_time)
{
	struct {
		void *start;
		size_t length;
	} *buffers;

	struct v4l2_capability capability;
	struct v4l2_format format;
	struct v4l2_buffer filledbuffer;
	struct v4l2_requestbuffers reqbuf;
	int i, count, ret;

	if (ioctl(ofd, VIDIOC_QUERYCAP, &capability) == -1) {
		perror("VIDIOC_QUERYCAP");
		return;
	}

	if (capability.capabilities & V4L2_CAP_STREAMING == 0) {
		printf("VIDIOC_QUERYCAP indicated that driver is not capable of Streaming \n");
		return;
	}	

	format.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
	ret = ioctl(ofd, VIDIOC_G_FMT, &format);
	if (ret < 0) {
	    perror("VIDIOC_G_FMT");
	    return;
	}
	reqbuf.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
	reqbuf.memory = V4L2_MEMORY_MMAP;
	reqbuf.count = 4;
	if (ioctl(ofd, VIDIOC_REQBUFS, &reqbuf) == -1) {
		perror ("VIDEO_REQBUFS");
		return;
	}
	printf("Driver allocated %d buffers when 4 are requested\n", reqbuf.count);

	buffers = calloc(reqbuf.count, sizeof(*buffers));
	for (i = 0; i < reqbuf.count ; ++i) {
		struct v4l2_buffer buffer;
		buffer.type = reqbuf.type;
		buffer.index = i;
		if (ioctl(ofd, VIDIOC_QUERYBUF, &buffer) == -1){
			perror("VIDIOC_QUERYBUF");
			return;
		}
#if 0
		printf("%d: buffer.length=%d, buffer.m.offset=%d\n",
				i, buffer.length, buffer.m.offset);
#endif
		buffers[i].length= buffer.length;
		buffers[i].start = mmap(NULL, buffer.length, PROT_READ|
						PROT_WRITE, MAP_SHARED,
						ofd, buffer.m.offset);
		if (buffers[i].start == MAP_FAILED ){
			perror ("mmap");
			return;
		}
		printf("Buffers[%d].start = %x  length = %d\n", i,buffers[i].start, buffers[i].length);		
	}


	if (ioctl(ofd, VIDIOC_STREAMON, &reqbuf.type) == -1){
		perror("VIDIOC_STREAMON");
		return;
	}

	if (read(ifd, buffers[0].start, format.fmt.pix.sizeimage) != format.fmt.pix.sizeimage){
		perror ("read");
		return;
	}
	filledbuffer.type = reqbuf.type;
	filledbuffer.index = 0;
	if (ioctl(ofd, VIDIOC_QBUF, &filledbuffer) == -1){
		perror ("VIDIOC_QBUF");
		return;
	}

	count = 1;
	while (count < 2000) {
		/* delay some for frame rate control */
		if (sleep_time)
			sleep(sleep_time);
		else {
			for (i=0; i < 2000000; i++);
		}

		if (ioctl(ofd, VIDIOC_DQBUF, &filledbuffer) == -1){
			perror ("VIDIOC_DQBUF");
			return;
		}

		i = read(ifd, buffers[count%reqbuf.count].start, format.fmt.pix.sizeimage);
		if (i < 0) {
			perror ("read");
			return;
		}
		if (i != format.fmt.pix.sizeimage){
			goto exit; /* we are done */
		}

		filledbuffer.index = count % reqbuf.count;			
		if (ioctl(ofd, VIDIOC_QBUF, &filledbuffer) == -1){
			perror ("VIDIOC_QBUF2");
			return;
		}

 		count++;
	}

	if (ioctl(ofd, VIDIOC_DQBUF, &filledbuffer) == -1){
		perror ("VIDIOC_DQBUF2");
		return;
	}

exit:

	for (i = 0; i < reqbuf.count; i++) {
		if (buffers[i].start)
			munmap (buffers[i].start, buffers[i].length);
	}

	if (ioctl(ofd, VIDIOC_STREAMOFF, &reqbuf.type) == -1) {
		perror("VIDIOC_STREAMOFF");
		return;
	}

}


static int usage(void)
{
	printf("Usage: streaming <vid> <inputfile> [<n>]\n");
	return 0;
}

int main (int argc, char *argv[])
{
	int vid, fd, fd2, ret;
	int sleep_time = 0;

	if (argc < 3)
		return usage();

	vid = atoi(argv[1]);
	if ((vid != 1) && (vid != 2)){
		printf("vid has to be 1 or 2! vid=%d, argv[1]=%s\n", vid, argv[1]);
		return usage();
	}
	
	fd = open ((vid == 1)?VIDEO_DEVICE1:VIDEO_DEVICE2, O_RDWR) ;
	if (fd <= 0) {
		printf("Could not open %s\n", (vid == 1)?VIDEO_DEVICE1:VIDEO_DEVICE2);
		return -1;
	}
	else
		printf("openned %s\n", (vid == 1)?VIDEO_DEVICE1:VIDEO_DEVICE2);

	fd2 = open(argv[2], O_RDONLY) ;
	if (fd2 <= 0) {
		printf("Could not open input file %s\n", argv[2]);
		return -1;
	}

	if (argc == 4)
		sleep_time = atoi(argv[3]);
	streaming_video(fd, fd2, sleep_time);
	
	close(fd2);
	close(fd);
}




