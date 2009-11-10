#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <linux/videodev2.h>
#include <string.h>
#include <signal.h>
#include <sys/resource.h>
#include <sys/wait.h>

#define VIDEO_DEVICE1 "/dev/video1"
#define VIDEO_DEVICE2 "/dev/video2"

#define DEFAULT_PIXEL_FMT "YUYV"
#define DEFAULT_VIDEO_SIZE "CIF"
#define DSS_FRAME_START		3

struct child_args {
	int priority;
	int value;
	int fd;
	int device;
};

static void usage(void)
{
	printf("process_cam [child priority 1]"
					" [child priority 2] [dev]\n");
	printf("   To start streaming capture of 1000 frames\n");
	/*printf("   [camDevice] Camera device to be open\n\t 1:Micron sensor "
					"2:OV sensor\n");*/
	printf("   [child priority 1] is the priority of the first child "
			" process created with the lower brigthness value\n"
			"\tDefault priority = 20\n");
	printf("   [child priority 2] is the priority of the second child "
			" process created with the higher brigthness value\n"
			"\tDefault priority = -10\n");
	printf("   Default pixel format = YUYV\n");
	printf("   Default size = QCIF\n");
	printf("   [device] Camera device to be open\n\t 1:Micron sensor "
					"2:OV sensor 3:IMX046\n");
}

void set_control(void *t_args)
{
	int which = PRIO_PROCESS;
	int ret;
	int retn;
	struct v4l2_control control_config;
	struct child_args *args = (struct child_args *)t_args;
	int i;
	id_t pid;

	pid = getpid();
	retn = setpriority(which, pid, args->priority);

	args->fd = open_cam_device(O_RDWR, args->device);

	if (args->fd <= 0)
		printf("Could not open the cam device inside the process\n");

	control_config.id = V4L2_CID_BRIGHTNESS;
	control_config.value = args->value;
	/* set h3a params */
	printf("Setting brigtness from child with priority=%d ;"
		" control value=%d\n", args->priority, args->value);
	while (1) {
		ret = ioctl(args->fd, VIDIOC_S_CTRL, &control_config);
		if (ret < 0)
			printf("VIDIOC_S_CTRL Error: %d, ", ret);
	}

	close(args->fd);
	printf("File descriptor for priority %d closed\n", args->priority);

	exit(0);
}

int main(int argc, char *argv[])
{
	struct child_args t_args1, t_args2;
	int child1_pid, child2_pid;
	int child1_status, child2_status;
	void *child1_stack;
	void *child2_stack;
	struct {
		void *start;
		size_t length;
	} *vbuffers, *cbuffers;
	struct v4l2_capability capability;
	struct v4l2_format cformat, vformat;
	struct v4l2_requestbuffers creqbuf, vreqbuf;
	struct v4l2_buffer cfilledbuffer, vfilledbuffer;
	struct v4l2_queryctrl qc;
	struct v4l2_control control;
	int vfd, cfd;
	int i, ret, count = -1, memtype = V4L2_MEMORY_USERPTR;
	int index = 1, vid = 1, set_video_img = 0;
	int device = 1;
	int framerate = 30;

	if ((argc > 1) && (!strcmp(argv[1], "?"))) {
		usage();
		return 0;
	}

	printf("Allocating space for the child stack 1\n");
	child1_stack = (void *)malloc(4096);
	if (!child1_stack) {
		printf("ERROR: cannot create stack for child");
		exit(1);
	}

	printf("Allocating space for the child stack 2\n");
	child2_stack = (void *)malloc(4096);
	if (!child2_stack) {
		printf("ERROR: cannot create stack for child");
		exit(1);
	}

	if (argc > index) {
		t_args1.priority = atoi(argv[index]);
		index++;
	}

	if (argc > index) {
		t_args2.priority = atoi(argv[index]);
		index++;
	}

	if (argc > index) {
		device = atoi(argv[index]);
		index++;
	}


	cfd = open_cam_device(O_RDWR, device);
	if (cfd <= 0) {
		printf("Could not open the cam device\n");
		return -1;
	}

	printf("Setting pixel format and video size with default "
					"values\n");
	ret = cam_ioctl(cfd, DEFAULT_PIXEL_FMT, DEFAULT_VIDEO_SIZE);
	if (ret < 0)
		return -1;
	ret = setFramerate(cfd, framerate);
	if (ret < 0) {
		printf("Error setting framerate = %d\n", framerate);
		return -1;
	}

	count = 400;
	printf("Frames: %d\n", count);

	if (count >= 400 || count <= 0)
		count = -1;

	vfd = open((vid == 1) ? VIDEO_DEVICE1 : VIDEO_DEVICE2, O_RDWR);
	if (vfd <= 0) {
		printf("Could no open the device %s\n",
			(vid == 1) ? VIDEO_DEVICE1 : VIDEO_DEVICE2);
		vid = 0;
	} else
		printf("openned %s for rendering\n",
			(vid == 1) ? VIDEO_DEVICE1 : VIDEO_DEVICE2);

	if (ioctl(vfd, VIDIOC_QUERYCAP, &capability) == -1) {
		perror("dss VIDIOC_QUERYCAP");
		return -1;
	}
	if (capability.capabilities & V4L2_CAP_STREAMING)
		printf("The video driver is capable of Streaming!\n");
	else {
		printf("The video driver is not capable of "
					"Streaming!\n");
		return -1;
	}

	if (ioctl(cfd, VIDIOC_QUERYCAP, &capability) < 0) {
		perror("cam VIDIOC_QUERYCAP");
		return -1;
	}
	if (capability.capabilities & V4L2_CAP_STREAMING)
		printf("The driver is capable of Streaming!\n");
	else {
		printf("The driver is not capable of Streaming!\n");
		return -1;
	}

	cformat.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	ret = ioctl(cfd, VIDIOC_G_FMT, &cformat);
	if (ret < 0) {
		perror("cam VIDIOC_G_FMT");
		return -1;
	}
	printf("Camera Image width = %d, Image height = %d, size = %d\n",
			cformat.fmt.pix.width, cformat.fmt.pix.height,
					cformat.fmt.pix.sizeimage);

	vformat.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
	ret = ioctl(vfd, VIDIOC_G_FMT, &vformat);
	if (ret < 0) {
		perror("dss VIDIOC_G_FMT");
		return -1;
	}
	printf("Video Image width = %d, Image height = %d, size = %d\n",
		vformat.fmt.pix.width, vformat.fmt.pix.height,
		vformat.fmt.pix.sizeimage);

	if ((cformat.fmt.pix.width != vformat.fmt.pix.width) ||
	    (cformat.fmt.pix.height != vformat.fmt.pix.height)) {
		printf("image sizes don't match!\n");
		set_video_img = 1;
	}
	if (cformat.fmt.pix.pixelformat !=
	    vformat.fmt.pix.pixelformat) {
		printf("pixel formats don't match!\n");
		set_video_img = 1;
	}

	if (set_video_img) {
		printf("set video image the same as camera image ..\n");
		vformat.fmt.pix.width = cformat.fmt.pix.width;
		vformat.fmt.pix.height = cformat.fmt.pix.height;
		vformat.fmt.pix.sizeimage = cformat.fmt.pix.sizeimage;
		vformat.fmt.pix.pixelformat =
					cformat.fmt.pix.pixelformat;
		ret = ioctl(vfd, VIDIOC_S_FMT, &vformat);
		if (ret < 0) {
			perror("dss VIDIOC_S_FMT");
			return -1;
		}
		printf("New Image & Video sizes, after "
			"equaling:\nCamera Image width = %d, "
			"Image height = %d, size = %d\n",
			cformat.fmt.pix.width, cformat.fmt.pix.height,
			cformat.fmt.pix.sizeimage);
		printf("Video Image width = %d, Image height "
			"= %d, size = %d\n",
			vformat.fmt.pix.width, vformat.fmt.pix.height,
			vformat.fmt.pix.sizeimage);

		if ((cformat.fmt.pix.width != vformat.fmt.pix.width) ||
		    (cformat.fmt.pix.height !=
		     vformat.fmt.pix.height) ||
		    (cformat.fmt.pix.pixelformat !=
		     vformat.fmt.pix.pixelformat)) {
			printf("can't make camera and video image "
				"compatible!\n");
			return 0;
		}
	}

	vreqbuf.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
	vreqbuf.memory = V4L2_MEMORY_MMAP;
	vreqbuf.count = 4;
	if (ioctl(vfd, VIDIOC_REQBUFS, &vreqbuf) == -1) {
		perror("dss VIDEO_REQBUFS");
		return;
	}
	printf("Video Driver allocated %d buffers when 4 are "
			"requested\n", vreqbuf.count);

	vbuffers = calloc(vreqbuf.count, sizeof(*vbuffers));
	for (i = 0; i < vreqbuf.count; ++i) {
		struct v4l2_buffer buffer;
		buffer.type = vreqbuf.type;
		buffer.index = i;
		if (ioctl(vfd, VIDIOC_QUERYBUF, &buffer) == -1) {
			perror("dss VIDIOC_QUERYBUF");
			return;
		}
		vbuffers[i].length = buffer.length;
		vbuffers[i].start = mmap(NULL, buffer.length,
					 PROT_READ | PROT_WRITE,
					 MAP_SHARED,
					 vfd,
					 buffer.m.offset);
		if (vbuffers[i].start == MAP_FAILED) {
			perror("dss mmap");
			return;
		}
		printf("Video Buffers[%d].start = %x  length = %d\n",
			i, vbuffers[i].start, vbuffers[i].length);

	}

	creqbuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	creqbuf.memory = memtype;
	creqbuf.count = 4;
	printf("Requesting %d buffers of type %s\n", creqbuf.count,
		(memtype == V4L2_MEMORY_USERPTR) ? "V4L2_MEMORY_USERPTR" :
						"V4L2_MEMORY_MMAP");
	if (ioctl(cfd, VIDIOC_REQBUFS, &creqbuf) < 0) {
		perror("cam VIDEO_REQBUFS");
		return -1;
	}
	printf("Camera Driver allowed buffers reqbuf.count = %d\n",
						creqbuf.count);


	cbuffers = calloc(creqbuf.count, sizeof(*cbuffers));
	/* mmap driver memory or allocate user memory, and queue each buffer */
	for (i = 0; i < creqbuf.count; ++i) {
		struct v4l2_buffer buffer;
		buffer.type = creqbuf.type;
		buffer.memory = creqbuf.memory;
		buffer.index = i;
		if (ioctl(cfd, VIDIOC_QUERYBUF, &buffer) < 0) {
			perror("cam VIDIOC_QUERYBUF");
			return -1;
		}
		if (memtype == V4L2_MEMORY_USERPTR) {
			buffer.flags = 0;
			buffer.m.userptr = (unsigned int)vbuffers[i].start;
			buffer.length = vbuffers[i].length;
		} else {
			cbuffers[i].length = buffer.length;
			cbuffers[i].start = vbuffers[i].start;
			printf("Mapped Buffers[%d].start = %x  length = %d\n",
				i, cbuffers[i].start, cbuffers[i].length);

			buffer.m.userptr = (unsigned int)cbuffers[i].start;
			buffer.length = cbuffers[i].length;
		}

		if (ioctl(cfd, VIDIOC_QBUF, &buffer) < 0) {
			perror("cam VIDIOC_QBUF");
			return -1;
		}
	}

	/* turn on streaming */
	if (ioctl(cfd, VIDIOC_STREAMON, &creqbuf.type) < 0) {
		perror("cam VIDIOC_STREAMON");
		return -1;
	}

	/* capture 1000 frames or when we hit the passed nmuber of frames */
	cfilledbuffer.type = creqbuf.type;
	vfilledbuffer.type = vreqbuf.type;
	i = 0;

	/* Query brightness limits */
	qc.id = V4L2_CID_BRIGHTNESS;
	if (ioctl(cfd, VIDIOC_QUERYCTRL, &qc) > 0) {
		perror("Error VIDIOC_QUERYCTRL");
		goto exit;
	}
	printf("Query %s: min=%d max=%d step=%d\n",
			qc.name, qc.minimum, qc.maximum, qc.step);


	/* Set values to pass to new processes */
	t_args1.priority = 20;
	t_args1.value = qc.minimum;
	t_args1.device = device;

	t_args2.priority = -10;
	t_args2.value = qc.maximum;
	t_args2.device = device;


	/* Get current brightness value */
	control.id = V4L2_CID_BRIGHTNESS;
	if (ioctl(cfd, VIDIOC_G_CTRL, &control) != 0) {
		perror("");
		goto exit;
	}
	printf("Current brightness is %d\n", control.value);


	/* get priority of current process */
	id_t pid;
	int retn;
	pid = getpid();
	retn = getpriority(PRIO_PROCESS, pid);

	printf("Priority of parent process: %d\n", retn);
	while (i < 1000) {
		/*Creating threads to execute the same function*/
		if (i == 100) {
			printf("Invoking clone for child1\n");
			child1_pid = fork();
			if (child1_pid == 0) {
				set_control((void *)&t_args1);
			} else if (child1_pid == -1) {
				printf("error in clone function");
				exit(2);
			}
		}
		if (i == 200) {
			printf("Invoking clone for child2\n");
			child2_pid = fork();
			if (child2_pid == 0)
				set_control((void *)&t_args2);
			else if (child2_pid == -1) {
				printf("error in clone function");
				exit(2);
			}
		}
		if (i == 300) {
			printf("Killing Child process 2 (pid=%i)\n",
				child2_pid);
			kill(child2_pid, SIGKILL);
		}
		/* De-queue the next avaliable buffer */
		while (ioctl(cfd, VIDIOC_DQBUF, &cfilledbuffer) < 0)
			perror("cam VIDIOC_DQBUF");

		vfilledbuffer.index = cfilledbuffer.index;
		vfilledbuffer.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
		vfilledbuffer.memory = V4L2_MEMORY_MMAP;
		vfilledbuffer.m.userptr = 
			(unsigned int)(vbuffers[cfilledbuffer.index].start);
		vfilledbuffer.length = cfilledbuffer.length;
		if (ioctl(vfd, VIDIOC_QBUF, &vfilledbuffer) < 0) {
			perror("dss VIDIOC_QBUF");
			return -1;
		}
		i++;

		if (i == 3) {
			/* Turn on streaming for video */
			if (ioctl(vfd, VIDIOC_STREAMON, &vreqbuf.type)) {
				perror("dss VIDIOC_STREAMON");
				return -1;
			}
		}

		if (i >= 3) {
			/* De-queue the previous buffer from video driver */
			if (ioctl(vfd, VIDIOC_DQBUF, &vfilledbuffer)) {
				perror("dss VIDIOC_DQBUF");
				return;
			}
		}

		if (i == count) {
			printf("Cancelling the streaming capture...\n");
			creqbuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
			if (ioctl(cfd, VIDIOC_STREAMOFF, &creqbuf.type) < 0) {
				perror("cam VIDIOC_STREAMOFF");
				return -1;
			}
			if (ioctl(vfd, VIDIOC_STREAMOFF,
				  &vreqbuf.type) == -1) {
				perror("dss VIDIOC_STREAMOFF");
				return -1;
			}

			printf("Done\n");
			break;
		}

		if (i >= 3) {
			cfilledbuffer.index = vfilledbuffer.index;
			while (ioctl(cfd, VIDIOC_QBUF, &cfilledbuffer) < 0)
				perror("cam VIDIOC_QBUF");
		}
	}

	waitpid(child2_pid, &child2_status, 0);
	if (WIFEXITED(child2_status))
		printf("child2 exit status %i\n", child2_status);

	/* Kill child1 process*/
	printf("Killing Child process 1 (pid=%i)\n",
				child1_pid);
	setpriority(PRIO_PROCESS, child1_pid, 0);
	kill(child1_pid, SIGKILL);

	waitpid(child1_pid, &child1_status, 0);
	if (WIFEXITED(child1_status))
		printf("child1 exit status %i\n", child1_status);


	printf("Captured %d frames!\n", i);

	/* Set back to old brightness value */
	control.id = V4L2_CID_BRIGHTNESS;
	ioctl(cfd, VIDIOC_G_CTRL, &control);

	/* we didn't turn off streaming yet */
	if (count == -1) {
		creqbuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		if (ioctl(cfd, VIDIOC_STREAMOFF, &creqbuf.type) == -1) {
			perror("cam VIDIOC_STREAMOFF");
			return -1;
		}
		if (ioctl(vfd, VIDIOC_STREAMOFF, &vreqbuf.type) == -1) {
			perror("dss VIDIOC_STREAMOFF");
			return -1;
		}
	}
exit:
	for (i = 0; i < vreqbuf.count; i++) {
		if (vbuffers[i].start)
			munmap(vbuffers[i].start, vbuffers[i].length);
	}

	free(vbuffers);

	for (i = 0; i < creqbuf.count; i++) {
		if (cbuffers[i].start) {
			if (memtype == V4L2_MEMORY_USERPTR)
				free(cbuffers[i].start);
			else
				munmap(cbuffers[i].start, cbuffers[i].length);
		}
	}

	free(cbuffers);

	close(vfd);
	close(cfd);
}
