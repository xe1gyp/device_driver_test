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
	printf("Usage: enablecolkey < o/p device [1:LCD 2:TV]> <enable/disable [1:0]>\n");
	return 0;
}

int main (int argc, char *argv[])
{
	int output_dev, fd, ret,state=0;

	if (argc < 3)
		return usage();

	output_dev = atoi(argv[1]) + 3; 
	
	if ((output_dev != OMAP24XX_OUTPUT_LCD) && (output_dev != OMAP24XX_OUTPUT_TV)){
		printf("Output device should be 1 for LCD or 2 for TV \n");
		return usage();
	}

	state	= atoi(argv[2]);
	
	if((state != 1) && (state != 0)){
		printf("Color Key should be enabled [1] or disabled [2]\n");
		return usage();
	}

	fd = open (VIDEO_DEVICE1, O_RDONLY) ;
	if (fd <= 0) {
		printf("Could not open %s\n",VIDEO_DEVICE1);
		return -1;
	}
	else
		printf("openned %s\n", VIDEO_DEVICE1);

	printf("out %d state %d\n",output_dev,state);
	if(state == 1)
	{
		ret = ioctl (fd,VIDIOC_OMAP2_COLORKEY_ENABLE, &output_dev);
		if (ret < 0) {
			perror ("VIDIOC_OMAP2_COLORKEY_ENABLE");
			return 0;
		}
	}
	else{
		ret = ioctl (fd,VIDIOC_OMAP2_COLORKEY_DISABLE, &output_dev);
		if (ret < 0) {
			perror ("VIDIOC_OMAP2_COLORKEY_DISABLE");
			return 0;
		}
	}

	close(fd);
}
