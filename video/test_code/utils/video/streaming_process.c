#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <linux/ioctl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <linux/videodev2.h>
#include <linux/errno.h>
#include <errno.h>
#include <string.h>
#include <signal.h>

#include <unistd.h>
#include <sys/resource.h>

#include "lib.h"

struct child_args {
	int priority;
	int file_descriptor;
	void *value;
};

void set_control(void *process_args)
{
	int which = PRIO_PROCESS;
	int result;
	struct child_args *args;
	int *degree = (int *)args->value;
	struct v4l2_control control;
	memset(&control, 0 , sizeof(control));
	control.id = V4L2_CID_ROTATE;

	args = (struct child_args *)process_args;

	printf("New process created\n");

	control.value = *degree;
	args = (struct child_args *)process_args;

	id_t pid;
	pid = getpid();
	result = setpriority(which, pid, args->priority);

	while (1) {
		result = ioctl(args->file_descriptor,
			 VIDIOC_S_CTRL, &control);

		if (result != 0)
			perror("VIDIOC_S_CTRL");
	}
	printf("Rotation set to %d degree\n", degree);
	printf("Process with priority=%d and value=%d closed\n",
		args->priority, args->value);
}

void set_win(void *process_args)
{
	int which = PRIO_PROCESS;
	int result;
	struct child_args *args;
	int degree;
	struct v4l2_format *format;
	int pix_field = V4L2_FIELD_NONE;

	args = (struct child_args *)process_args;

	printf("New process created\n");

	format = (struct v4l2_format *)args->value;

	format->type = V4L2_BUF_TYPE_VIDEO_OVERLAY;

	/*V4L2_FIELD_SEQ_TB == 5, V4L2_FIELD_NONE == 1 */
	(format->fmt).win.field  = pix_field;

	args = (struct child_args *)process_args;

	id_t pid;
	pid = getpid();
	result = setpriority(which, pid, args->priority);

	while (1) {
		result = ioctl(args->file_descriptor, VIDIOC_S_FMT, format);

		if (result != 0)
			perror("VIDIOC_S_FMT");
	}
	printf("Process with priority=%d and value=%d closed\n",
		args->priority, args->value);
}


static int streaming_video(int output_device, int file_descriptor,
	int sleep_time, int priority1, int priority2)
{
	struct child_args t_args1, t_args2;
	int child1_pid, child2_pid;
	void *child1_stack;
	void *child2_stack;
	struct {
		void *start;
		size_t length;
	} *buffers;

	struct v4l2_capability capability;
	struct v4l2_format format, format_child1, format_child2;
	struct v4l2_buffer filledbuffer;
	struct v4l2_requestbuffers reqbuf;
	int i, count, result;

	child1_stack = (void *)malloc(4096);
	child2_stack = (void *)malloc(4096);

	if (child1_stack == NULL || child2_stack == NULL) {
		printf("ERROR: cannot create stack for child");
		exit(1);
	}

	format_child1.type = V4L2_BUF_TYPE_VIDEO_OVERLAY;
	result = ioctl(output_device, VIDIOC_G_FMT, &format_child1);
	if (result != 0) {
		perror("VIDIOC_G_FMT");
		return 1;
	}

	format_child2.fmt.win.w.left = format_child1.fmt.win.w.left;
	format_child2.fmt.win.w.top = format_child1.fmt.win.w.top +
					format_child1.fmt.win.w.height + 20;
	format_child2.fmt.win.w.width = format_child1.fmt.win.w.width;
	format_child2.fmt.win.w.height = format_child1.fmt.win.w.height;

	t_args1.priority = priority1;
	t_args1.value = (void *) &format_child1;
	t_args1.file_descriptor = output_device;

	t_args2.priority = priority2;
	t_args2.value = (void *)&format_child2;
	t_args2.file_descriptor = output_device;

	result = ioctl(output_device, VIDIOC_QUERYCAP, &capability);
	if (result != 0) {
		perror("VIDIOC_QUERYCAP");
		return 1;
	}

	if (capability.capabilities & V4L2_CAP_STREAMING == 0) {
		printf("VIDIOC_QUERYCAP indicated that "
			"driver is not capable of Streaming \n");
		return 1;
	}

	format.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
	result = ioctl(output_device, VIDIOC_G_FMT, &format);
	if (result != 0) {
		perror("VIDIOC_G_FMT");
		return 1;
	}

	reqbuf.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
	reqbuf.memory = V4L2_MEMORY_MMAP;
	reqbuf.count = 4;
	result = ioctl(output_device, VIDIOC_REQBUFS, &reqbuf);
	if (result != 0) {
		perror("VIDEO_REQBUFS");
		return 1;
	}
	printf("Driver allocated %d buffers when 4 are requested\n",
		reqbuf.count);

	buffers = calloc(reqbuf.count, sizeof(*buffers));
	for (i = 0; i < reqbuf.count ; ++i) {
		struct v4l2_buffer buffer;
		buffer.type = reqbuf.type;
		buffer.index = i;
		result = ioctl(output_device, VIDIOC_QUERYBUF, &buffer);
		if (result != 0) {
			perror("VIDIOC_QUERYBUF");
			return 1;
		}
#if 0
		printf("%d: buffer.length=%d, buffer.m.offset=%d\n",
				i, buffer.length, buffer.m.offset);
#endif
		buffers[i].length = buffer.length;
		buffers[i].start = mmap(NULL, buffer.length, PROT_READ|
						PROT_WRITE, MAP_SHARED,
						output_device,
						buffer.m.offset);
		if (buffers[i].start == MAP_FAILED) {
			perror("mmap");
			return 1;
		}
		printf("Buffers[%d].start = %x  length = %d\n", i,
			buffers[i].start, buffers[i].length);
	}


	filledbuffer.type = reqbuf.type;
	filledbuffer.memory = V4L2_MEMORY_MMAP;

	for (i = 0; i <= 1; i++) {

		filledbuffer.index = i;
		if (read(file_descriptor, buffers[i].start,
			format.fmt.pix.sizeimage) != format.fmt.pix.sizeimage) {
			perror("read");
			return 1;
		}

		result = ioctl(output_device, VIDIOC_QBUF, &filledbuffer);
		if (result != 0) {
			perror("VIDIOC_QBUF");
			return 1;
		}
	}

	result = ioctl(output_device, VIDIOC_STREAMON, &reqbuf.type);
	if (result != 0) {
		perror("VIDIOC_STREAMON");
		return 1;
	}

	count = 2;
	while (count < 2000) {
		/* delay some for frame rate control */
		if (sleep_time)
			sleep(sleep_time);

		if (count == 10) {
			printf("Invoking clone for child1\n");
			child1_pid = clone((void *)&set_win, child1_stack,
				SIGCHLD | CLONE_FS | CLONE_FILES |
				CLONE_SIGHAND | CLONE_VM, (void *)&t_args1);
			if (child1_pid == -1) {
				printf("error en funcion clone");
				exit(2);
			}
		}

		if (count == 15) {
			printf("Invoking clone for child2\n");
			child2_pid = clone((void *)&set_win, child2_stack,
				SIGCHLD | CLONE_FS | CLONE_FILES |
				CLONE_SIGHAND | CLONE_VM, (void *)&t_args2);
			if (child2_pid == -1) {
				printf("error en funcion clone");
				exit(2);
			}
		}

		if (count == 25) {
			kill(child2_pid, SIGKILL);
			printf("Child process 2 killed\n");
		}

		result = ioctl(output_device, VIDIOC_DQBUF, &filledbuffer);
		if (result != 0) {
			perror("VIDIOC_DQBUF");
			return 1;
		}

		i = read(file_descriptor,
			buffers[count%reqbuf.count].start,
			format.fmt.pix.sizeimage);
		if (i < 0) {
			perror("read");
			return 1;
		}
		if (i != format.fmt.pix.sizeimage)
			goto exit;/* we are done */

		filledbuffer.index = count % reqbuf.count;
		result = ioctl(output_device, VIDIOC_QBUF, &filledbuffer);
		if (result != 0) {
			perror("VIDIOC_QBUF2");
			return 1;
		}

		count++;
	}

	result = ioctl(output_device, VIDIOC_DQBUF, &filledbuffer);
	if (result != 0) {
		perror("VIDIOC_DQBUF2");
		return 1;
	}

exit:
	/* Killing child process*/
	kill(child1_pid, SIGKILL);
	printf("Child process 1 killed\n");

	for (i = 0; i < reqbuf.count; i++) {
		if (buffers[i].start)
			munmap(buffers[i].start, buffers[i].length);
	}

	result = ioctl(output_device, VIDIOC_STREAMOFF, &reqbuf.type);
	if (result != 0) {
		perror("VIDIOC_STREAMOFF");
		return 1;
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
	return 1;
}

int main(int argc, char *argv[])
{
	int video_device, file_descriptor, output_device, result;
	int sleep_time = 0;
	int priority1, priority2;

	priority1 = 10;
	priority2 = -10;

	printf("STREAMING_ACTUAL\n");

	if (argc < 3)
		return usage();

	video_device = atoi(argv[1]);
	if ((video_device != 1) && (video_device != 2) && (video_device != 3)) {
		printf("video_device has to be 1 or 2 or 3! video_device=%d, "
			"argv[1]=%s\n", video_device, argv[1]);
		return usage();
	}

	file_descriptor =
		open((video_device == 1) ? VIDEO_DEVICE1 :
			((video_device == 2) ? VIDEO_DEVICE2 : VIDEO_DEVICE3),
		O_RDWR);
	if (file_descriptor <= 0) {
		printf("Could not open %s\n",
			(video_device == 1) ? VIDEO_DEVICE1 :
			((video_device == 2) ? VIDEO_DEVICE2 : VIDEO_DEVICE3));
		return 1;
	} else
		printf("openned %s\n",
			(video_device == 1) ? VIDEO_DEVICE1 :
			((video_device == 2) ? VIDEO_DEVICE2 : VIDEO_DEVICE3));


	output_device = open(argv[2], O_RDONLY);
	if (output_device <= 0) {
		printf("Could not open input file %s\n", argv[2]);
		return 1;
	}

	if (argc > 3)
		sleep_time = atoi(argv[3]);

	if (argc > 4)
		priority1 = atoi(argv[4]);

	if (argc > 5)
		priority2 = atoi(argv[5]);

	result = streaming_video(file_descriptor, output_device, sleep_time,
		priority1, priority2);

	close(output_device);
	close(file_descriptor);
	return result;
}
