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
	return 0;
}

int main (int argc, char *argv[])
{
	int output_dev, fd, ret;
	struct omap24xxvout_bgcolor bgcolor = {0};

	if (argc < 3)
		return usage();

	output_dev = atoi(argv[1]) + 3; 
	
	if ((output_dev != OMAP24XX_OUTPUT_LCD) && (output_dev != OMAP24XX_OUTPUT_TV)){
		printf("Output device should be 1 for LCD or 2 for TV \n");
		return usage();
	}

	fd = open (VIDEO_DEVICE1, O_RDONLY) ;
	if (fd <= 0) {
		printf("Could not open %s\n",VIDEO_DEVICE1);
		return -1;
	}
	else
		printf("openned %s\n", VIDEO_DEVICE1);

        bgcolor.output_dev = output_dev; 
        bgcolor.color      = atoi(argv[2]);
 

	ret = ioctl (fd,VIDIOC_S_OMAP2_BGCOLOR, &bgcolor);

	if (ret < 0) {
		perror ("VIDIOC_S_OMAP2_BGCOLOR");
		return 0;
	}

	printf("Back Ground color is set to %x \n",bgcolor.color);
	close(fd) ;
}
