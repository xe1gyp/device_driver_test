/* ================================================================================
*             Texas Instruments OMAP(TM) Platform Software
*  (c) Copyright Texas Instruments, Incorporated.  All Rights Reserved.
*
*  Use of this software is controlled by the terms and conditions found 
*  in the license agreement under which this software has been supplied.
* ================================================================================ */

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
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/videodev.h>

#define PREVIEW_ROTATION_NO		0
#define PREVIEW_ROTATION_0		1
#define PREVIEW_ROTATION_90		2
#define PREVIEW_ROTATION_180		3
#define PREVIEW_ROTATION_270		4
#define VIDIOC_S_OVERLAY_ROT		_IOW ('O', 1,  int)
#define VIDIOC_G_OVERLAY_ROT		_IOR ('O', 2,  int)

static void usage(void)
{
	printf("overlay [0|1|2] [rotno|rot0|rot90|rot180|rot270]\n");
	printf("\tTo stop/start camera preview. 0: stop, 1: start on V1, 2: start on V2\n");
	printf("\tPreview can optionally rotate the image and will last 5 mintues\n");
}

int main (int argc, char *argv[])
{
	int ret, fd, enable = 1, rotation = -1, oldrotation;

	if ((argc > 1) && (!strcmp(argv[1], "?"))) {
		usage();
		return 0;
	}

	if (argc >= 2) {
		if (!strcmp(argv[1], "rot0"))
			rotation = 1;
		else if (!strcmp(argv[1], "rot90"))
			rotation = 2;
		else if (!strcmp(argv[1], "rot180"))
			rotation = 3;
		else if (!strcmp(argv[1], "rot270"))
			rotation = 4;
		else if (!strcmp(argv[1], "rotno"))
			rotation = 0;
		else if ((!strcmp(argv[1], "0")) ||
			 (!strcmp(argv[1], "1")) ||
			 (!strcmp(argv[1], "2")))
			enable = atoi(argv[1]);
		else {
			usage();
			return 0;
		}
	}
	if (!rotation && argc >= 3) {
		if (!strcmp(argv[2], "rot0"))
			rotation = 1;
		else if (!strcmp(argv[2], "rot90"))
			rotation = 2;
		else if (!strcmp(argv[2], "rot180"))
			rotation = 3;
		else if (!strcmp(argv[2], "rot270"))
			rotation = 4;
		else if (!strcmp(argv[2], "rotno"))
			rotation = 0;
	}

	if (enable != 0 && enable != 1 && enable != 2) {
		usage();
		return 0;
	}

	if ((fd = open_cam_device(O_RDWR)) <= 0) {
		printf ("Could not open the cam device\n");
		return -1;
	}
	
	ret = ioctl (fd, VIDIOC_G_OVERLAY_ROT, &oldrotation);
	if (ret == 0) {
	      	if (oldrotation)
			printf("Old Rotation: %d\n",(oldrotation-1)*90);
		else
			printf("Old Rotation: No\n");
	}

	if (enable & rotation != -1) { 
		ret = ioctl (fd, VIDIOC_S_OVERLAY_ROT, &rotation);
		if (ret == 0) {
			ret = ioctl (fd, VIDIOC_G_OVERLAY_ROT, &rotation);
			if (ret != 0)
				perror("VIDIOC_G_OVERLAY_ROT");
			else {
			      	if (rotation)
					printf("Rotation %d ...\n",(rotation-1)*90);
				else
					printf("No rotation ...\n");
			}
		}
	} 
	
	ret = ioctl (fd, VIDIOC_OVERLAY, &enable);
	if (ret != 0) {
		perror("VIDIOC_OVERLAY");
		return -1;
	}

	if (enable > 0) {
		printf("Previewing on Video%d\n", enable);
		sleep(300);

		enable = 0;
		ret = ioctl (fd, VIDIOC_OVERLAY, &enable);
		if (ret != 0)	{
			perror("VIDIOC_OVERLAY");
			return -1;
		}
	}

	printf("Preview stopped!\n");
	close(fd);
}




