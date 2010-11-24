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
#include <errno.h>
#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/videodev.h>


#include "lib.h"

static int usage(void)
{
	printf("Usage: setwin <video_device> <left> <top> <width> <height> [pix_field] \n");
	printf("[pix_field] is optional, set this to 5 for interlace support"
			"or set 1 for progressive \n");
	return 1;
}

int main(int argc, char *argv[])
{
	int video_device, file_descriptor, result;
	int pix_field = V4L2_FIELD_NONE;
	struct v4l2_format format;

	if (argc < 6)
		return usage();

	if (argc == 7) {
		if ((atoi(argv[6])== 5) || (atoi(argv[6]) == 1))
			pix_field = atoi(argv[6]);
		else {
			printf("Invalid pix_field value: Input 1 or 5 only\n");
			return usage();
		}
	}

	video_device = atoi(argv[1]);
        if ((video_device != 1) && (video_device != 2) && (video_device != 3) && (video_device != 4)) {
		printf("video_device has to be 1 or 2 or 3 or 4! video_device=%d, "
			"argv[1]=%s\n", video_device, argv[1]);
		return usage();
	}

	format.fmt.win.w.left = atoi(argv[2]);
	format.fmt.win.w.top = atoi(argv[3]);
	format.fmt.win.w.width = atoi(argv[4]);
	format.fmt.win.w.height = atoi(argv[5]);
	format.type = V4L2_BUF_TYPE_VIDEO_OVERLAY;
	format.fmt.win.field  = pix_field;	/*V4L2_FIELD_SEQ_TB == 5, V4L2_FIELD_NONE == 1 */

	file_descriptor =
                open((video_device == 1) ? VIDEO_DEVICE1 :
                        ((video_device == 2) ? VIDEO_DEVICE2 :
                        ((video_device == 3) ? VIDEO_DEVICE3 : WB_DEV)),
		O_RDWR);
	if (file_descriptor <= 0) {
		printf("Could not open %s\n",
			(video_device == 1) ? VIDEO_DEVICE1 :
			((video_device == 2) ? VIDEO_DEVICE2 : VIDEO_DEVICE3));
		return 1;
	} else
		printf("openned %s\n",
                        (video_device == 1) ? VIDEO_DEVICE1 :
                        ((video_device == 2) ? VIDEO_DEVICE2 :
                        ((video_device == 3) ? VIDEO_DEVICE3 : WB_DEV)));

	result = ioctl(file_descriptor, VIDIOC_S_FMT, &format);
	if (result != 0) {
		perror("VIDIOC_S_FMT");
		return 1;
	}

        if (video_device == 4)
                format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        else
                format.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;

        result = show_info(format.type, file_descriptor);
	close(file_descriptor);

	return result;
}
