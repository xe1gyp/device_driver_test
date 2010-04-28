/*
 * saFbdevRotation.c
 *
 * This is a Framebuffer sample application to show the display functionality
 * The app puts a moving horizontal bar on the display device in various
 * shades of colors.
 * This application is an extenson of saMmapDisplay application and it demos the
 * rotation on graphics pipelines using VRFB.
 *
 * Copyright (C) 2009 Texas Instruments Incorporated - http://www.ti.com/
 *
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the
 *    distribution.
 *
 *    Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
*/

/******************************************************************************
  Header File Inclusion
 ******************************************************************************/
#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <stdio.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <string.h>

#define omap_rotation_index(rotation_deg) \
			((rotation_deg == 0) || (rotation_deg == 90) || \
			(rotation_deg == 180) || (rotation_deg == 270)) ? \
			(rotation_deg/90) : rotation_deg

#define omap_rotation_angle(rotation_idx) \
			((rotation_idx == 0) || (rotation_idx == 1) || \
			(rotation_idx == 2) || (rotation_idx == 3)) ? \
			(rotation_idx*90) : rotation_idx

#define WIDTH			800
#define HEIGHT			480
#define WIDTH_VIRTUAL		WIDTH
#define HEIGHT_VIRTUAL		HEIGHT

#define DEF_FB	"/dev/fb0"

static short ycbcr[8] = {
		(0x1F << 11) | (0x3F << 5) | (0x1F),
		(0x00 << 11) | (0x00 << 5) | (0x00),
		(0x1F << 11) | (0x00 << 5) | (0x00),
		(0x00 << 11) | (0x3F << 5) | (0x00),
		(0x00 << 11) | (0x00 << 5) | (0x1F),
		(0x1F << 11) | (0x3F << 5) | (0x00),
		(0x1F << 11) | (0x00 << 5) | (0x1F),
		(0x00 << 11) | (0x3F << 5) | (0x1F),
};

/* This function is used to fill up buffer with color bars. */
void fill_color_bar(unsigned char *addr, int width, int height, int pitch)
{
	unsigned short *start = (unsigned short *)addr;
	int i, j, k;

	for (i = 0; i < 8; i++) {
		for (j = 0; j < (height / 8); j++) {
			for (k = 0 ; k < width; k++)
				*(start + k) = ycbcr[i];

			start += (pitch/2);
		}
	}
}
static int usage()
{
	printf("Usage: setrotation  framebuffer device name </dev/fb0 \
		or /dev/fb1 etc> rotation angle <0/90/180/270>  \n");
	return 1;
}

int main(int argc, char *argp[])
{
	char shortoptions[] = "d:r:", fbstr[20] = DEF_FB;
	int index = 0, c;
	int rotate = 0, fb, buffersize;
	struct fb_var_screeninfo var, org_var;
	struct fb_fix_screeninfo fix;
	unsigned char *buffer_addr;

	if (argc < 3)
		return usage();

	rotate = atoi(argp[2]);

	fb = open(argp[1], O_RDWR);
	if (fb <= 0) {
		perror("Error: cannot open framebuffer  device.\n");
		exit(1);
	}

	if (ioctl(fb, FBIOGET_FSCREENINFO, &fix)) {
		perror("Error reading fixed information.\n");
		exit(2);
	}
	printf("Fix Screen Info:\n");
	printf("----------------\n");
	printf("Line Length - %d\n", fix.line_length);
	printf("Physical Address = %lx\n", fix.smem_start);
	printf("Buffer Length = %d\n", fix.smem_len);

	if (ioctl(fb, FBIOGET_VSCREENINFO, &var)) {
		perror("Error reading variable information.\n");
		exit(3);
	}
	printf("\nVar Screen Info:\n");
	printf("----------------\n");
	printf("Xres - %d\n", var.xres);
	printf("Yres - %d\n", var.yres);
	printf("Xres Virtual - %d\n", var.xres_virtual);
	printf("Yres Virtual - %d\n", var.yres_virtual);
	printf("Bits Per Pixel - %d\n", var.bits_per_pixel);
	printf("Pixel Clk - %d\n", var.pixclock);
	printf("Rotation - %d\n", omap_rotation_angle(var.rotate));

	/* Copy the original var */
	memcpy(&org_var, &var, sizeof(var));
	printf("before mmap");
	buffersize = fix.line_length * var.yres;
	buffer_addr = (unsigned char *)mmap(0, buffersize,
			(PROT_READ|PROT_WRITE),	MAP_SHARED, fb, 0);
	if (buffer_addr == MAP_FAILED) {
		printf("MMap failed\n");
		return -1;
	}
	fill_color_bar(buffer_addr, var.xres,
				var.yres, fix.line_length);
	munmap(buffer_addr, buffersize);

	sleep(5);

	var.rotate = omap_rotation_index(rotate);
	if ((var.rotate == 1) || (var.rotate == 3)) {
		var.xres = HEIGHT;
		var.yres = WIDTH;
		var.xres_virtual = HEIGHT_VIRTUAL;
		var.yres_virtual = WIDTH_VIRTUAL;
	} else {
		var.xres = WIDTH;
		var.yres = HEIGHT;
		var.xres_virtual = WIDTH_VIRTUAL;
		var.yres_virtual = HEIGHT_VIRTUAL;
	}
	if (ioctl(fb, FBIOPUT_VSCREENINFO, &var) < 0) {
		perror("Error:FBIOPUT_VSCREENINFO\n");
		exit(4);
	}

	if (ioctl(fb, FBIOGET_FSCREENINFO, &fix)) {
		perror("Error reading fixed information.\n");
		exit(2);
	}
	printf("Fix Screen Info:\n");
	printf("----------------\n");
	printf("Line Length - %d\n", fix.line_length);
	printf("Physical Address = %lx\n", fix.smem_start);
	printf("Buffer Length = %d\n", fix.smem_len);

	if (ioctl(fb, FBIOGET_VSCREENINFO, &var)) {
		perror("Error reading variable information.\n");
		exit(3);
	}
	printf("\nVar Screen Info:\n");
	printf("----------------\n");
	printf("Xres - %d\n", var.xres);
	printf("Yres - %d\n", var.yres);
	printf("Xres Virtual - %d\n", var.xres_virtual);
	printf("Yres Virtual - %d\n", var.yres_virtual);
	printf("Bits Per Pixel - %d\n", var.bits_per_pixel);
	printf("Pixel Clk - %d\n", var.pixclock);
	printf("Rotation - %d\n", omap_rotation_angle(var.rotate));

	buffersize = fix.line_length * var.yres;
	buffer_addr = (unsigned char *)mmap(0, buffersize,
			(PROT_READ|PROT_WRITE),	MAP_SHARED, fb, 0);
	if (buffer_addr == MAP_FAILED) {
		printf("MMap failed\n");
		return -1;
	}
	fill_color_bar(buffer_addr, var.xres,
				var.yres, fix.line_length);
	munmap(buffer_addr, buffersize);

	sleep(5);

	/* It is better to revert back to original configuration */
	if (ioctl(fb, FBIOPUT_VSCREENINFO, &org_var)) {
		perror("Error restoring variable information.\n");
		exit(3);
	}

	close(fb);
	return 0;
}

