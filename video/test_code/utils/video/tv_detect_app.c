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
	return 0;
}
static int check_tv_connection(){
	int fd;
	char* tv_check;
	tv_check = malloc (sizeof(char)*1);
	fd = open ("/sys/class/display_control/omap_disp_control/tv_state",O_RDWR);		
	read(fd, tv_check,1);
	close(fd);
	return atoi(tv_check);
}
int main (int argc, char *argv[])
{
	int vid, fd, fd2, ret;
	int link;	
	
	fd2 = open ("/sys/class/display_control/omap_disp_control/video2",O_RDWR);		
	write(fd2, "tv",2);
	
	while (!check_tv_connection()){
		printf("TV not connected \n");
		sleep(TV_DELAY_CHECK);	
	}

	vid = 2;
	fd = open ((vid == 1)?VIDEO_DEVICE1:VIDEO_DEVICE2, O_RDONLY) ;
	
	if (fd <= 0) {
		printf("Could not open %s\n", (vid == 1)?VIDEO_DEVICE1:VIDEO_DEVICE2);
		return -1;
	}
	else
		printf("openned %s\n", (vid == 1)?VIDEO_DEVICE1:VIDEO_DEVICE2);

	ret = ioctl (fd, VIDIOC_G_OMAP2_LINK, &link);
	if (ret < 0) {
		perror ("VIDIOC_G_OMAP2_LINK");
		return 0;
	}
	printf("link status: V%d is %s linked to another layer\n", vid,
		(link == 1)? "": "not");

	link = 1;
	ret = ioctl (fd, VIDIOC_S_OMAP2_LINK, &link);
	if (ret < 0) {
		perror ("VIDIOC_S_OMAP2_LINK");
		return 0;
	}
	printf("linked!\n");

	if (ioctl(fd, VIDIOC_STREAMON, &link) == -1){
		perror("VIDIOC_STREAMON");
		return;
	}

	while (check_tv_connection()){
		printf("TV connected \n");
		sleep(TV_DELAY_CHECK);	
	}

	printf("TV not connected \n");
	
	if (ioctl(fd, VIDIOC_STREAMOFF, &link) == -1){
		perror("VIDIOC_STREAMOFF");
		return;
	}
	link = 0;
	ret = ioctl (fd, VIDIOC_S_OMAP2_LINK, &link);
	if (ret < 0) {
		perror ("VIDIOC_S_OMAP2_LINK");
		return 0;
	}
	write(fd2, "lcd",3);
	printf("unlinked!\n");

	close(fd) ;
	close(fd2);
}
