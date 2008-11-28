#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <linux/ioctl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <linux/videodev.h>
#include <linux/errno.h>
#include <errno.h>
#include <string.h>
#include <signal.h>

#include <unistd.h>
#include <sys/resource.h>

#include "lib.h"

struct child_args{
	int priority;
	int value;
	int fd;
};

void set_control(void *t_args)
{
	int which = PRIO_PROCESS;
	int ret;
	int cfd, retn;
	struct v4l2_control control_config;
	struct child_args * args;
	int i, degree;

	args=(struct child_args *)t_args;

	printf("New process created\n");
	
	degree = args->value;
	args=(struct child_args *)t_args;

	id_t pid;
	pid = getpid();
    	retn = setpriority(which, pid, args->priority);
    	
	while (1)
	{
		ret = ioctl (args->fd, VIDIOC_S_OMAP2_ROTATION, &degree);
		if (ret < 0) {
			perror ("VIDIOC_S_OMAP2_ROTATION");
			//return 0;
		}
	}
	printf("Rotation set to %d degree\n",degree);
	printf("Process with priority=%d and value=%d closed\n",
				args->priority,args->value);

}

static void streaming_video(int ofd, int ifd, int sleep_time, int priority1, 
						int priority2)
{
	struct child_args t_args1, t_args2, t_arg3;
	int child1_pid, child2_pid;
	void * child1_stack;
	void * child2_stack;
	int ret1, ret2;
	struct {
		void *start;
		size_t length;
	} *buffers;

	struct v4l2_capability capability;
	struct v4l2_format format;
	struct v4l2_buffer filledbuffer;
	struct v4l2_requestbuffers reqbuf;
	int i, count, ret;

	//printf("Allocating space for the child stack 1\n");
	if ( (child1_stack = (void *) malloc(4096)) == NULL) {
		printf("ERROR: cannot create stack for child");
		exit(1);
	}
	//printf("Direccion de memoria de child1_stack=%x\n",child1_stack);

	//printf("Allocating space for the child stack 2\n");
	if ( (child2_stack = (void *) malloc(4096)) == NULL) {
		printf("ERROR: cannot create stack for child");
		exit(1);
	}
	
	t_args1.priority = priority1;
	t_args1.value = 0;
	t_args1.fd=ofd;

	t_args2.priority = priority2;
	t_args2.value = 180;
	t_args2.fd=ofd;

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
	ret=ioctl(ofd, VIDIOC_REQBUFS, &reqbuf);
	if (ret == -1) {
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
		/*else {
			for (i=0; i < 2000000; i++);
		}*/

		if (count == 10) {
			printf("Invoking clone for child1\n");
			child1_pid = clone( (void *)&set_control, child1_stack,
				SIGCHLD | CLONE_FS | CLONE_FILES | 
				CLONE_SIGHAND | CLONE_VM, (void *)&t_args1);
			if ( child1_pid == -1 ) {
			printf( "error en funcion clone" );
			exit( 2 );
			}
		}
		
		if (count == 15) {
			printf("Invoking clone for child2\n");
			child2_pid = clone( (void *)&set_control, child2_stack,
				SIGCHLD | CLONE_FS | CLONE_FILES | 
				CLONE_SIGHAND | CLONE_VM, (void *)&t_args2);
			if ( child2_pid == -1 ) {
			printf( "error en funcion clone" );
			exit( 2 );
			}			
		}
		
		if (count == 25){
			kill(child2_pid,SIGKILL);
			printf("Child process 2 killed\n");
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
			goto exit;//return;//break; /* we are done */
		}

		filledbuffer.index = count % reqbuf.count;			
		if (ioctl(ofd, VIDIOC_QBUF, &filledbuffer) == -1){
			perror ("VIDIOC_QBUF2");
			return;
		}

 		count++;
	}
	
	/* Killing child process*/
	kill(child1_pid,SIGKILL);
	printf("Child process 1 killed\n");

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
	printf("Usage: streaming <vid> <inputfile> [<n>] [child priority 1] "
					"[child priority 2]\n");
	printf("   [child priority 1] is the priority of the first child "
			" process created with rotation = 0\n"
			"\tDefault priority = 20\n");
	printf("   [child priority 2] is the priority of the second child "
			" process created with rotation = 180\n"
			"\tDefault priority = -10\n");
	return 0;
}

int main (int argc, char *argv[])
{
	int vid, fd, fd2, ret;
	int sleep_time = 0;
	int priority1, priority2;
	
	priority1 = 10; priority2 = -10;
	
	printf("STREAMING_ACTUAL\n");

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

	if (argc > 3)
		sleep_time = atoi(argv[3]);
	
	if (argc > 4)
		priority1 = atoi(argv[4]);
		
	if (argc > 5)
		priority2 = atoi(argv[5]);
	
	streaming_video(fd, fd2, sleep_time, priority1, priority2);
	
	close(fd2);
	close(fd);
}




