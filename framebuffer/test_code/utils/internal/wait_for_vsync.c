/* 
 * Framebuffer driver 'wait_for_vsync' test program
 * 
 * Copyright(C) 2005 Texas Instruments, Inc.
 * Author(s) : Nishant Kamat
 */ 

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <string.h>
#include <linux/fb.h>
#define FBIO_WAITFORVSYNC       _IOW('F', 0x20, u_int32_t)

#define WAIT_FOR_VSYNC	1
/* If wait_for_vsync is used, the movement of the bar on the screen is smooth. 
 * Otherwise it is jumpy or flickering.
 */

#define ITERATIONS 1000000000

int fd;
unsigned char *data;
struct fb_var_screeninfo var;
struct fb_fix_screeninfo fix;

int show_frame(int buf_no)
{
	unsigned char *buf;
	static int start;
	int width = var.yres >> 4;	/* 1/16th of the screen height */
	struct fb_var_screeninfo v;
	int i = 0, j = 0, ret = 0;
	char color = 0xAA;

	memcpy(&v, &var, sizeof(v));
	v.yoffset = v.yres * buf_no;
	buf = data + v.yoffset * fix.line_length;

	/* clear the frame */
	memset((void *)buf, 0, fix.line_length * var.yres);
	
	/* draw a horizontal bar on the frame */
	for (i = start; i < start + width; i++) {
		for (j = 0; j < var.xres * var.bits_per_pixel/8; j++)
			buf[i*fix.line_length + j] = color;
	}	
	
	/* pan to that frame */
	if (ret = ioctl (fd, FBIOPAN_DISPLAY, &v)) {
		printf("ioctl FBIOPAN_DISPLAY failed\n");	
		return ret;
	}	
	
	/* move the position of the bar for the next frame */
	start = start + 1;
	if (start + width > var.yres)
		start = 0;
	
	return 0;
}

int movie(int num_buf, int wait_vsync)
{
	int buf_no = 0;
	int k = 0, ret = 0;
	
	for (k = 0; k < ITERATIONS; k++) {
	
		if (ret = show_frame(buf_no))
			return ret;
			
		if (wait_vsync) {
			if (ret = ioctl (fd, FBIO_WAITFORVSYNC, 0)) {
				printf("ioctl FBIO_WAITFORVSYNC failed\n");
				return ret;
			}
		}
		buf_no = (++buf_no) % num_buf;
	}
	return 0;
}

int init_fb(char *devname)
{
	int num_buf = 1;
	int ret = 0;

	/* Open framebuffer device */
	fd = open(devname, O_RDWR);
	if (fd <= 0) {
		printf("Could not open device\n");
	}
	if (ret = ioctl (fd, FBIOGET_FSCREENINFO, &fix)) {
		printf("ioctl FBIOGET_FSCREENINFO failed\n");
		return ret;
	}
	if (ret = ioctl (fd, FBIOGET_VSCREENINFO, &var)) {
		printf("ioctl FBIOGET_VSCREENINFO failed\n");
		return ret;
	}

	data = (unsigned char *)mmap (0,
				fix.line_length * var.yres_virtual,
				(PROT_READ|PROT_WRITE),
				MAP_SHARED, fd, 0) ;
	if (!data) {
		printf("mmap failed for %d x %d  of %d\n", \
			fix.line_length, var.yres_virtual, fix.smem_len);
		return -ENOMEM;
	}
	num_buf = var.yres_virtual / var.yres;

	return num_buf;
}	

int main(int argc, char *argv[])
{
	int i = 0, ret = 0;
	int num_buf;
	
	if (!argv[1]) {
		printf("/dev/fbN parameter missing\n");	
		return -ENODEV;
	}
	if ((num_buf = init_fb(argv[1])) < 0) {
		printf("Could not initialize dev %s\n", argv[1]);
		return -1;
	}	
	ret = movie(num_buf, WAIT_FOR_VSYNC);
	close(fd);
	return ret;
}
