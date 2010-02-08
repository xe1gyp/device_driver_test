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
	printf("Usage: setbgcolor <output device [1: LCD 2: TV]> <24 bit RGB value>\n");
	return 1;
}

int main(int argc, char *argv[])
{
	int output_device, file_descriptor, result;
	unsigned int bgcolor = 0;
	struct v4l2_control control;
	memset(&control, 0 , sizeof(control));
	control.id = V4L2_CID_BG_COLOR;

	if (argc < 3)
		return usage();

	output_device = atoi(argv[1]) + 3;
	
	if ((output_device != OMAP24XX_OUTPUT_LCD) &&
		(output_device != OMAP24XX_OUTPUT_TV)) {
		printf("Output device should be 1 for LCD or 2 for TV \n");
		return usage();
	}

	file_descriptor = open(VIDEO_DEVICE1, O_RDONLY);
	if (file_descriptor <= 0) {
		printf("Could not open %s\n",VIDEO_DEVICE1);
		return 1;
	} else {
		printf("openned %s\n", VIDEO_DEVICE1);
	}


	bgcolor = atoi(argv[2]);
	control.value = bgcolor;
	result = ioctl(file_descriptor, VIDIOC_S_CTRL, &control);

	if (result != 0) {
		perror("VIDIOC_S_CTRL");
		return 1;
	}

	printf("Back Ground color is set to %x \n", bgcolor);
	close(file_descriptor);

	return 0;
}
