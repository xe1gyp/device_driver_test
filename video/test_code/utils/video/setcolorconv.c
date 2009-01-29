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
	printf("Usage: setcolorconv <layer 1/2> <e1> <e2> .... <e9>\n");
	return 1;
}

int main(int argc, char *argv[])
{
	int video_device, file_descriptor, result;
	struct omap24xxvout_colconv ccmtx, ccmrx;

	if (argc < 11)
		return usage();

	video_device = atoi(argv[1]);
	if ((video_device != 1) && (video_device != 2)) {
		printf("video_device has to be 1 or 2!\n");
		return usage();
	}
	
	file_descriptor =
		open((video_device == 1) ? VIDEO_DEVICE1 : VIDEO_DEVICE2,
		O_RDONLY);
	if (file_descriptor <= 0) {
		printf("Could not open %s\n",
		(video_device == 1) ? VIDEO_DEVICE1 : VIDEO_DEVICE2);
		return 1;
	}
	else
		printf("openned %s\n",
		(video_device == 1) ? VIDEO_DEVICE1 : VIDEO_DEVICE2);

	ccmtx.RY  = atoi(argv[2]);
	ccmtx.RCr = atoi(argv[3]);
	ccmtx.RCb = atoi(argv[4]);
	ccmtx.GY  = atoi(argv[5]);
	ccmtx.GCr = atoi(argv[6]);
	ccmtx.GCb = atoi(argv[7]);
	ccmtx.BY  = atoi(argv[8]);
	ccmtx.BCr = atoi(argv[9]);
	ccmtx.BCb = atoi(argv[10]);

	result = ioctl(file_descriptor, VIDIOC_S_OMAP2_COLORCONV, &ccmtx);
	if (result != 0) {
		perror("VIDIOC_S_OMAP2_COLORCONV");
		return 1;
	}

	result = ioctl(file_descriptor, VIDIOC_G_OMAP2_COLORCONV, &ccmrx);
	if (result != 0) {
		perror("VIDIOC_G_OMAP2_COLORCONV");
		return 1;
	}

	printf(" The Color conversion values are set to \n");
	printf("%d %d %d\n%d %d %d\n%d %d %d\n",
		ccmrx.RY,ccmrx.RCr,ccmrx.RCb ,
		ccmrx.GY,ccmrx.GCr,ccmrx.GCb ,
		ccmrx.BY,ccmrx.BCr,ccmrx.BCb );

	close(file_descriptor);

	return 0;
}
