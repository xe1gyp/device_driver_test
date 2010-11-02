#include <stdio.h>
#include <fcntl.h>
#include <linux/ioctl.h>
#include <linux/fb.h>
#include <sys/mman.h>

#include <stdbool.h>
#include <linux/videodev2.h>
#include <linux/errno.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include "lib.h"
#define true 1

/* WB specific custom ioctls */
#define VIDIOC_CUSTOM_G_WB     _IOWR('V', 255, struct v4l2_writeback_ioctl_data)
#define VIDIOC_CUSTOM_S_WB     _IOW('V', 254, struct v4l2_writeback_ioctl_data)

/* shared globals */

/*
 * wb_buffers 	: mmaped writeback buffers
 * in_buffers 	: mmaped source buffers
 * disp_buffers	: mmaped buffers which display the writeback buffers
 */

struct {
	void *start;
	size_t length;
} *wb_buffers, *in_buffers, *disp_buffers;

struct v4l2_format format_in, format_wb, format_disp;
struct v4l2_buffer filledbuffer_in, filledbuffer_wb, filledbuffer_disp;
struct v4l2_requestbuffers reqbuf_in, reqbuf_wb, reqbuf_disp;


/* helper functions */
static int getBpp(unsigned long pixelformat)
{
	int bpp = 0;

	switch (pixelformat) {
	case V4L2_PIX_FMT_YUYV:
	case V4L2_PIX_FMT_UYVY:
	case V4L2_PIX_FMT_RGB565:
	case V4L2_PIX_FMT_RGB565X:
		bpp = 2;
		break;
	case V4L2_PIX_FMT_RGB24:
		bpp = 3;
		break;
	case V4L2_PIX_FMT_RGB32:
	case V4L2_PIX_FMT_BGR32:
		bpp = 4;
		break;
	case V4L2_PIX_FMT_NV12:
		bpp = 1;
		break;
	default:
		bpp = -1;
	}
	return bpp;
}

enum v4l2_writeback_source overlay_to_source(int overlay)
{
	switch (overlay) {
	case 1:
		return V4L2_WB_OVERLAY1;
	case 2:
		return V4L2_WB_OVERLAY2;
	case 3:
		return V4L2_WB_OVERLAY3;
	default:
		return 0;
	}
}

/* -----------set up input source video pipeline------------- */

static int setup_source_pipeline(int in_device, int num_buffers)
{
	struct v4l2_capability capability;
	struct v4l2_control control;
	int i, result, bytesperline;
	__u32 in_width, in_height, in_format;

	/* specifies window size specified
	 * in source pipeline(source pipeline
	 * scaling
	 */
	struct v4l2_format format_win;

	printf("setting up source pipeline\n");

	result = ioctl(in_device, VIDIOC_QUERYCAP, &capability);
	if (result != 0) {
		perror("VIDIOC_QUERYCAP");
		return 1;
	}

	if (capability.capabilities & V4L2_CAP_STREAMING == 0) {
	printf("VIDIOC_QUERYCAP indicated that output driver is not "
		"capable of Streaming \n");
		return 1;
	}

	format_in.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
	result = ioctl(in_device, VIDIOC_G_FMT, &format_in);

	if (result != 0) {
		perror("VIDIOC_G_FMT");
		return result;
	}

	in_height = format_in.fmt.pix.height;
	in_width = format_in.fmt.pix.width;
	in_format = format_in.fmt.pix.pixelformat;
	bytesperline = format_in.fmt.pix.bytesperline;

	printf("G_FMT(pix) for input pipeline : pix_height:%d, pix_width:%d,"
		"pix_format:%d, bytesperline:%d\n", in_height, in_width,
		in_format, bytesperline);

	format_win.type = V4L2_BUF_TYPE_VIDEO_OVERLAY;
	result = ioctl(in_device, VIDIOC_G_FMT, &format_win);
	if (result != 0) {
		perror("VIDIOC_G_FMT");
		return result;
	}

	printf("G_FMT(win) for input pipeline : win_height:%d, win_width:%d \n",
		format_win.fmt.win.w.height, format_win.fmt.win.w.width);

	reqbuf_in.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
	reqbuf_in.memory = V4L2_MEMORY_MMAP;
	reqbuf_in.count = num_buffers;

	result = ioctl(in_device, VIDIOC_REQBUFS, &reqbuf_in);
	if (result != 0) {
		perror("VIDEO_REQBUFS");
		return 1;
	}
	printf("Driver allocated %d INPUT pipeline buffers when %d were requested\n",
		reqbuf_in.count, num_buffers);

	in_buffers = calloc(reqbuf_in.count, sizeof(*in_buffers));

	for (i = 0; i < reqbuf_in.count ; ++i) {
		struct v4l2_buffer buffer;
		buffer.type = reqbuf_in.type;
		buffer.index = i;

		result = ioctl(in_device, VIDIOC_QUERYBUF, &buffer);

		if (result != 0) {
			perror("VIDIOC_QUERYBUF");
			return 1;
		}
		printf("%d: input pipeline buffer.length=%d, buffer.m.offset=%d\n",
			i, buffer.length, buffer.m.offset);
		in_buffers[i].length = buffer.length;
		in_buffers[i].start = mmap(NULL, buffer.length, PROT_READ|
						PROT_WRITE, MAP_SHARED,
						in_device, buffer.m.offset);
		if (in_buffers[i].start == MAP_FAILED) {
			perror("mmap");
			return 1;
		}
		printf("input pipeline mmaped bufs[%d].start = %x"
			" length = %d\n", i, in_buffers[i].start,
			in_buffers[i].length);
	}

	/*enable writeback for this in_device*/
	memset(&control, 0 , sizeof(control));
	control.id = V4L2_CID_WB;
	control.value = 1;

	result = ioctl(in_device, VIDIOC_S_CTRL, &control);
	if (result != 0) {
		printf("VIDIOC_S_CTRL trouble\n");
	}

	filledbuffer_in.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
	filledbuffer_in.memory = V4L2_MEMORY_MMAP;
	filledbuffer_in.flags = 0;

	return 0;
}

/* ------------set up wb pipeline------------- */

static int setup_wb_pipeline(int wb_device, int in_overlay, int num_buffers)
{
	struct v4l2_capability capability;
	struct v4l2_writeback_ioctl_data wb;
	int i, result;

	result = ioctl(wb_device, VIDIOC_QUERYCAP, &capability);
	if (result != 0) {
		perror("VIDIOC_QUERYCAP");
		return 1;
	}

	if (capability.capabilities & V4L2_CAP_STREAMING == 0) {
		printf("VIDIOC_QUERYCAP indicated that driver is not"
			" capable of Streaming \n");
		return 1;
	}

	if (capability.capabilities & V4L2_CAP_VIDEO_CAPTURE) {
		printf("VIDIOC_QUERYCAP indicated that driver is a"
			" capture device !\n");
	}

	format_wb.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

	result = ioctl(wb_device, VIDIOC_G_FMT, &format_wb);
	if (result != 0) {
		printf("wb_g_fmt not done!\n");
		perror("VIDIOC_G_FMT");
		return result;
	}
	printf("\nwb_g_fmt:width %d height %d pxlfmt %d bpp %d\n\n", format_wb.fmt.pix.width,
		format_wb.fmt.pix.height, format_wb.fmt.pix.pixelformat, format_wb.fmt.pix.bytesperline);

	result = ioctl(wb_device, VIDIOC_CUSTOM_G_WB, &wb);
	if (result != 0) {
            perror("VIDIOC_G_WB");
            return result;
	}

	printf("wb device g_wb ioctl :wb_enabled = %d, wb_source = %d, wb_capturemode = %d, wb_source_type = %d\n",
		wb.enabled, wb.source, wb.capturemode, wb.source_type);

	wb.enabled = true;
	wb.source = overlay_to_source(in_overlay);
	wb.capturemode = V4L2_WB_CAPTURE_ALL;
	wb.source_type = V4L2_WB_SOURCE_OVERLAY;

	result = ioctl(wb_device, VIDIOC_CUSTOM_S_WB, &wb);
	if (result != 0) {
		perror("VIDIOC_S_WB");
		return result;
	}

	reqbuf_wb.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	reqbuf_wb.memory = V4L2_MEMORY_MMAP;
	reqbuf_wb.count = num_buffers;

	result = ioctl(wb_device, VIDIOC_REQBUFS, &reqbuf_wb);
	if (result != 0) {
		perror("VIDEO_REQBUFS");
		return 1;
	}

	printf("Driver allocated %d capture writeback buffers when %d are"
		" requested\n", reqbuf_wb.count, num_buffers);

	wb_buffers = calloc(reqbuf_wb.count, sizeof(*wb_buffers));
	for (i = 0; i < reqbuf_wb.count ; ++i) {
		struct v4l2_buffer buffer;
		buffer.type = reqbuf_wb.type;
		buffer.index = i;

		result = ioctl(wb_device, VIDIOC_QUERYBUF, &buffer);
		if (result != 0) {
			perror("VIDIOC_QUERYBUF");
			return 1;
		}
		printf("%d: buffer.length=%d, buffer.m.offset=%d\n",
				i, buffer.length, buffer.m.offset);
		wb_buffers[i].length = buffer.length;
		wb_buffers[i].start = mmap(NULL, buffer.length, PROT_READ|
						PROT_WRITE|PROT_EXEC, MAP_SHARED,
						wb_device, buffer.m.offset);
		if (wb_buffers[i].start == MAP_FAILED) {
			perror("mmap");
			return 1;
		}

		printf("WB bufs[%d].start = %x  length = %d\n", i,
			wb_buffers[i].start, wb_buffers[i].length);
	}

	filledbuffer_wb.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	filledbuffer_wb.memory = V4L2_MEMORY_MMAP;
	filledbuffer_wb.flags = 0;

	return 0;
}

/* -------------display pipeline configuration------------------ */

static int setup_disp_pipeline(int disp_device, int num_buffers)
{
	struct v4l2_capability capability;
	int i, result;

	result = ioctl(disp_device, VIDIOC_QUERYCAP, &capability);
	if (result != 0) {
		perror("VIDIOC_QUERYCAP");
		return 1;
	}

	if (capability.capabilities & V4L2_CAP_STREAMING == 0) {
		printf("VIDIOC_QUERYCAP indicated that driver is not"
			" capable of Streaming \n");
		return 1;
	}

	format_disp.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
	result = ioctl(disp_device, VIDIOC_G_FMT, &format_disp);

	if (result != 0) {
		perror("VIDIOC_G_FMT");
		return result;
	}

	printf("\n format_disp pix attribs: width=%0x, height=%0x,"
		" imagesize=%0x, format=%0x\n", format_disp.fmt.pix.width,
		format_disp.fmt.pix.height, format_disp.fmt.pix.sizeimage,
		format_disp.fmt.pix.pixelformat);

	reqbuf_disp.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
	reqbuf_disp.memory = V4L2_MEMORY_MMAP;
	reqbuf_disp.count = num_buffers;

	result = ioctl(disp_device, VIDIOC_REQBUFS, &reqbuf_disp);
	if (result != 0) {
		perror("VIDEO_REQBUFS");
		return 1;
	}

	printf("Driver allocated %d output display buffers when %d are"
		" requested\n", reqbuf_disp.count, num_buffers);

	disp_buffers = calloc(reqbuf_disp.count, sizeof(*disp_buffers));

	for (i = 0; i < reqbuf_disp.count ; ++i) {
		struct v4l2_buffer buffer;
		buffer.type = reqbuf_disp.type;
		buffer.index = i;

		result = ioctl(disp_device, VIDIOC_QUERYBUF, &buffer);
		if (result != 0) {
			perror("VIDIOC_QUERYBUF");
			return 1;
		}
		printf("%d: buffer.length=%d, buffer.m.offset=%d\n",
				i, buffer.length, buffer.m.offset);
		disp_buffers[i].length = buffer.length;
		disp_buffers[i].start = mmap(NULL, buffer.length, PROT_READ|
						PROT_WRITE, MAP_SHARED,
						disp_device, buffer.m.offset);
		if (disp_buffers[i].start == MAP_FAILED) {
			perror("mmap");
			return 1;
		}

		printf("Buffers[%d].start = %x  length = %d\n", i,
			disp_buffers[i].start, disp_buffers[i].length);
	}


	filledbuffer_disp.type = reqbuf_disp.type;
	filledbuffer_disp.memory = V4L2_MEMORY_MMAP;
	filledbuffer_disp.flags = 0;

	return 0;
}
static int streaming_video(int in_device, int wb_device, int file_descriptor,
		int disp_device, int in_overlay, int sleep_time,
		int num_buffers)
{
	int i, j, count_vout, count_wb, count_disp;
	int result = 0, bpp_in, bpp_disp, bytesperline_in, bytesperline_disp;
	struct v4l2_control control;

	result = setup_source_pipeline(in_device, num_buffers);
	if (result != 0) {
		printf("source pipeline setup failed\n");
		goto err_src;
	}

	result = setup_wb_pipeline(wb_device, in_overlay, num_buffers);
	if (result != 0) {
		printf("wb pipeline setup failed\n");
		goto err_wb;
	}

	result = setup_disp_pipeline(disp_device, num_buffers);
	if (result != 0) {
		printf("display pipeline setup failed\n");
		goto err_disp;
	}

	bytesperline_in = format_in.fmt.pix.bytesperline;
	bpp_in = getBpp(format_in.fmt.pix.pixelformat);

	if (-1 == bpp_in) {
		perror("format not supported");
		goto err_disp;
	}

	if (-1 == bpp_disp) {
		perror("format not supported");
		goto err_disp;
	}

	bytesperline_disp = format_disp.fmt.pix.bytesperline;
	bpp_disp = getBpp(format_disp.fmt.pix.pixelformat);

	/* queue num_buffers for input and wb pipelines */
	for (i = 0; i <= num_buffers - 1; i++) {
		filledbuffer_in.index = i;
		filledbuffer_wb.index = i;
		//bytesperline_in = (format_in.fmt.pix.width * bpp_in + 4096 - 1) & ~(4096 - 1);
		printf("\nfirst bpp = %0x,bytesperline  = %0x\n", bpp_in, bytesperline_in);

		for (j = 0; j < format_in.fmt.pix.height; j++) {
			if (read(file_descriptor, in_buffers[i].start + (j*bytesperline_in),
				(format_in.fmt.pix.width * bpp_in)) != (format_in.fmt.pix.width * bpp_in)) {
				printf("current address = 0x%0x\n",
					in_buffers[i].start + (j*bytesperline_in));
				perror("first read");
				return 1;
			}
		}

		if (V4L2_PIX_FMT_NV12 == format_in.fmt.pix.pixelformat) {
			int bytesperline_uv = (format_in.fmt.pix.width * 2 +
				4096 - 1) & ~(4096 - 1);
			for (j = format_in.fmt.pix.height;
					j < (format_in.fmt.pix.height * 3 / 2); j++) {
				if (read(file_descriptor, in_buffers[i].start +
					(j*bytesperline_uv),
					(format_in.fmt.pix.width))
					!= (format_in.fmt.pix.width)) {
					perror("UV read");
					break;
				}
			}
		}

		result = ioctl(in_device, VIDIOC_QBUF, &filledbuffer_in);
		if (result != 0) {
			perror("VIDIOC_QBUF");
			return 1;
		}

		result = ioctl(wb_device, VIDIOC_QBUF, &filledbuffer_wb);
		if (result != 0) {
			perror("VIDIOC_QBUF_WB");
			return 1;
		}

	}

	result = ioctl(in_device, VIDIOC_STREAMON, &reqbuf_in.type);
	if (result != 0) {
		printf("streamon error with vout device\n");
		goto exit;
	}

	result = ioctl(wb_device, VIDIOC_STREAMON, &reqbuf_wb.type);
	if (result != 0) {
		printf("streamon error with wb device\n");
		goto exit;
	}
	/* Queue Dequeue loop */
	count_vout = reqbuf_in.count;
	count_wb = reqbuf_wb.count;
	count_disp = 0;

	while (count_vout < 2000) {
		int height;
		/* delay some for frame rate control */
		if (sleep_time)
			sleep(sleep_time);
		else {
			for (i = 0; i < 2000000; i++)
				;
		}

		result = ioctl(in_device, VIDIOC_DQBUF, &filledbuffer_in);
		if (result != 0) {
			printf("error with VIDIOC_DQBUFn");
		}

		result = ioctl(wb_device, VIDIOC_DQBUF, &filledbuffer_wb);
		if (result != 0) {
			printf("error with VIDIOC_DQBUF_WB\n");
		}

		/* copy newly dequed wb buffer to display buffer and
		 * then queue it to the display vout device */

		if (V4L2_PIX_FMT_NV12 == format_disp.fmt.pix.pixelformat)
			height = format_disp.fmt.pix.height * 3 / 2;
		else
			height = format_disp.fmt.pix.height;

		for (j = 0; j < height; j++) {
			memcpy(disp_buffers[count_disp%reqbuf_disp.count].start + (j * bytesperline_disp),
				wb_buffers[count_wb%reqbuf_wb.count].start + (j * bytesperline_disp),
				format_disp.fmt.pix.width * bpp_disp);
		}

		filledbuffer_disp.index = count_disp % reqbuf_disp.count;

		result = ioctl(disp_device, VIDIOC_QBUF, &filledbuffer_disp);

		if (result != 0) {
			perror("VIDIOC_QBUF");
			return 1;
		}

		count_disp++;

		/* start streamon for display vout once num_buffers buffers
		 * are queued */
		if (count_disp == num_buffers){
			result = ioctl(disp_device, VIDIOC_STREAMON, &reqbuf_disp.type);
			if (result != 0) {
				perror("VIDIOC_STREAMON");
				return 1;
			}
		}

		if (count_disp >= num_buffers) {
			printf("waiting vidioc_dqbuf %d\n", count_disp);
			result = ioctl(disp_device, VIDIOC_DQBUF, &filledbuffer_disp);
			if (result != 0) {
				perror("VIDIOC_DQBUF");
				return 1;
			}
		}

		/* queue next source buffer */
		for (j = 0; j < format_in.fmt.pix.height; j++) {
			i = read(file_descriptor, in_buffers[count_vout%reqbuf_in.count].start +
				(j*bytesperline_in), format_in.fmt.pix.width * bpp_in);

			if (i < 0) {
				printf("in_buf current address = 0x%0x\n",
					in_buffers[count_vout%reqbuf_in.count].start + (j*bytesperline_in));
				perror("second read");
				return 1;
			}
			if (i != format_in.fmt.pix.width * bpp_in) {
				printf("going to exit?\n");
				goto exit;	/* we are done */
			}
		}

		if (V4L2_PIX_FMT_NV12 == format_in.fmt.pix.pixelformat) {
			int bytesperline_uv = (format_in.fmt.pix.width * 2 +
				4096 - 1) & ~(4096 - 1);
			for (j = format_in.fmt.pix.height;
					j < (format_in.fmt.pix.height * 3 / 2); j++) {
				i = read(file_descriptor,
					in_buffers[count_vout%reqbuf_in.count].start +
					(j*bytesperline_uv), format_in.fmt.pix.width);

				if (i < 0) {
					printf("current address = 0x%0x\n",
						in_buffers[count_vout%reqbuf_in.count].start + (j*bytesperline_uv));
					perror("UV read in loop");
					return 1;
				}

				if (i != format_in.fmt.pix.width * bpp_in) {
					printf("going to exit?\n");
					goto exit;	/* we are done */
				}
			}
		}

		filledbuffer_in.index = count_vout % reqbuf_in.count;
		if (ioctl(in_device, VIDIOC_QBUF, &filledbuffer_in) != 0) {
			perror("VIDIOC_QBUF2");
			return 1;
		}

		count_vout++;

		filledbuffer_wb.index = count_wb % reqbuf_wb.count;

		if (ioctl(wb_device, VIDIOC_QBUF, &filledbuffer_wb) != 0) {
			perror("VIDIOC_QBUF2_WB");
			return 1;
		}

		count_wb++;
	}
exit:
	result = ioctl(in_device, VIDIOC_STREAMOFF, &reqbuf_in.type);
	if (result != 0) {
		perror("VIDIOC_STREAMOFF");
		return 1;
	}

	result = ioctl(wb_device, VIDIOC_STREAMOFF, &reqbuf_wb.type);
	if (result != 0) {
		perror("VIDIOC_STREAMOFF_WB");
		return 1;
	}

		result = ioctl(disp_device, VIDIOC_STREAMOFF, &reqbuf_disp.type);
	if (result != 0) {
		perror("VIDIOC_STREAMOFF_WB");
		return 1;
	}

err_disp:
	/* cleanup display pipeline */
	for (i = 0; i < reqbuf_disp.count; i++) {
		if (disp_buffers[i].start)
			munmap(disp_buffers[i].start, disp_buffers[i].length);
	}
err_wb:
	/* cleanup wb pipeline */
	for (i = 0; i < reqbuf_wb.count; i++) {
		if (wb_buffers[i].start)
			munmap(wb_buffers[i].start, wb_buffers[i].length);
	}
err_src:
	/* cleanup source pipeline */
	for (i = 0; i < reqbuf_in.count; i++) {
		if (in_buffers[i].start)
			munmap(in_buffers[i].start, in_buffers[i].length);
	}

	/*disable writeback for this in_device*/
	memset(&control, 0 , sizeof(control));
	control.id = V4L2_CID_WB;
	control.value = 0;

	result = ioctl(in_device, VIDIOC_S_CTRL, &control);
	if (result != 0) {
		printf("VIDIOC_S_CTRL trouble\n");

	}

	return result;
}

static int usage(void)
{
	printf("Usage: streaming_wb <vout_input_overlay> <inputfile>"
		" <vout_display_overlay> [<delay-n>] [<num_buffers]\n");
	return 1;
}

int main(int argc, char *argv[])
{
	/*
	 * in_device		: video device which is the source of wb pipeline
	 * in_overlay		: corresponding overlay
	 * wb_device		: writeback capture device
	 * disp_device		: video device which shows the content stored by wb
	 * disp_overlay		: corresponding overlay
	 * file_descriptor	: file_descriptor of source frames
	 */
	int in_device, in_overlay, wb_device, disp_device, disp_overlay,
		file_descriptor, result;
	char *in_name, *wb_name, *disp_name;

	/* request four buffers by default */
	int sleep_time = 0, num_buffers = 2;

	if (argc < 3)
		return usage();

	in_overlay = atoi(argv[1]);

	if (in_overlay < 1 || in_overlay > 3) {
		printf("input overlay has to be 1-3! in_overlay=%d,"
		" argv[2]=%s\n", in_overlay, argv[1]);
		return usage();
	}

	in_name = (in_overlay == 1) ? VIDEO_DEVICE1 :
        ((in_overlay == 2) ? VIDEO_DEVICE2 : VIDEO_DEVICE3);

	wb_name = WB_DEV;

	in_device = open(in_name, O_RDWR);
	if (in_device <= 0) {
		printf("Could not open %s\n", in_name);
		return 1;
	} else {
		printf("openned %s\n", in_name);
	}

	wb_device = open(wb_name, O_RDWR);
	if (wb_device <= 0) {
		printf("Could not open %s\n", wb_name);
		return 1;
	} else {
		printf("openned %s\n", wb_name);
	}

	file_descriptor = open(argv[2], O_RDONLY);

	if (file_descriptor <= 0) {
		printf("Could not open input file %s\n", argv[2]);
		return 1;
	}

	disp_overlay = atoi(argv[3]);
	disp_name = (disp_overlay == 1) ? VIDEO_DEVICE1 :
		((disp_overlay == 2) ? VIDEO_DEVICE2 : VIDEO_DEVICE3);

	if (disp_overlay == in_overlay) {
		printf("input and output overlays can't be the same\n");
		return 1;
	}

	disp_device = open(disp_name, O_RDWR);
	if (disp_device <= 0) {
		printf("Could not open wb_destination file %s\n", argv[3]);
		return 1;
	} else {
		 printf("openned %s\n", disp_name);
	}

	if (argc > 4)
		sleep_time = atoi(argv[4]);

	if (argc > 5)
		num_buffers = atoi(argv[5]);

	result = streaming_video(in_device, wb_device, file_descriptor, disp_device,
			in_overlay, sleep_time, num_buffers);

	close(in_device);
	close(wb_device);
	close(file_descriptor);
	close(disp_device);

	return result;
}

