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
#include <linux/videodev2.h>

#include "lib.h"

static int usage(void)
{
	printf("Usage: showinfo <video_device>\n");
	return 1;
}

int main(int argc, char *argv[])
{
	int video_device, file_descriptor, result;

	if (argc < 2)
		return usage();

	video_device = atoi(argv[1]);
        if ((video_device != 1) && (video_device != 2) && (video_device != 3) && (video_device != 4)) {
                printf("video_device has to be 1 or 2 or 3 or 4!\n");
		return usage();
	}

	file_descriptor =
                open((video_device == 1) ? VIDEO_DEVICE1 :
                        ((video_device == 2) ? VIDEO_DEVICE2 :
                        ((video_device == 3) ? VIDEO_DEVICE3 : WB_DEV)),
		O_RDONLY);

	if (file_descriptor <= 0) {
		printf("Could not open %s\n",
                        (video_device == 1) ? VIDEO_DEVICE1 :
                        ((video_device == 2) ? VIDEO_DEVICE2 :
                        ((video_device == 3) ? VIDEO_DEVICE3 : WB_DEV)));
		return 1;
	} else {
		printf("openned %s\n",
                        (video_device == 1) ? VIDEO_DEVICE1 :
                        ((video_device == 2) ? VIDEO_DEVICE2 :
                        ((video_device == 3) ? VIDEO_DEVICE3 : WB_DEV)));
	}

        if (video_device == 4)
                result = show_info(V4L2_BUF_TYPE_VIDEO_CAPTURE, file_descriptor);
        else
                result = show_info(V4L2_BUF_TYPE_VIDEO_OUTPUT, file_descriptor);
	close(file_descriptor);
	return result;
}
