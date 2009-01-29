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
#include <linux/unistd.h>

#include "lib.h"

#define TV_DELAY_CHECK 2 

static int usage(void)
{
	printf("Usage: setlink <vid>\n");
	return 1;
}

static int check_tv_connection()
{
	int file_descriptor;
	char* tv_check;
	tv_check = malloc (sizeof(char)*1);
	file_descriptor =
		open("/sys/class/display_control/omap_disp_control/tv_state",
		O_RDWR);
	read(file_descriptor, tv_check, 1);
	close(file_descriptor);
	return atoi(tv_check);
}

int main(int argc, char *argv[])
{
	int video_device, file_descriptor, output_device, result;
	int link;

	output_device =
		open("/sys/class/display_control/omap_disp_control/video2",
		O_RDWR);
	write(output_device, "tv", 2);

	while (!check_tv_connection()) {
		printf("TV not connected \n");
		sleep(TV_DELAY_CHECK);
	}

	video_device = 2;
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

	result = ioctl(file_descriptor, VIDIOC_G_OMAP2_LINK, &link);
	if (result != 0) {
		perror("VIDIOC_G_OMAP2_LINK");
		return 1;
	}

	printf("link status: V%d is %s linked to another layer\n", video_device,
		(link == 1) ? "" : "not");

	link = 1;
	result = ioctl(file_descriptor, VIDIOC_S_OMAP2_LINK, &link);
	if (result != 0) {
		perror("VIDIOC_S_OMAP2_LINK");
		return 1;
	}
	printf("linked!\n");

	result = ioctl(file_descriptor, VIDIOC_STREAMON, &link);
	if (result != 0) {
		perror("VIDIOC_STREAMON");
		return 1;
	}

	while (check_tv_connection()) {
		printf("TV connected \n");
		sleep(TV_DELAY_CHECK);
	}

	printf("TV not connected \n");

	result = ioctl(file_descriptor, VIDIOC_STREAMOFF, &link);
	if (result != 0) {
		perror("VIDIOC_STREAMOFF");
		return 1;
	}

	link = 0;
	result = ioctl(file_descriptor, VIDIOC_S_OMAP2_LINK, &link);
	if (result != 0) {
		perror("VIDIOC_S_OMAP2_LINK");
		return 1;
	}
	write(output_device, "lcd", 3);
	printf("unlinked!\n");

	close(file_descriptor);
	close(output_device);
	return 0;
}
