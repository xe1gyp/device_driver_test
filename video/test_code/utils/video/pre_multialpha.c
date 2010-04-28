/*
 * saFbdevDisplay.c
 *
 * This is a Fbdev sample application to show the display functionality
 * The app puts a swapping horizontal bar on the display device in various
 * shades of colors. This application runs RGB565 format with size VGA.
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
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <string.h>
#include <linux/fb.h>
#include <stdlib.h>
/******************************************************************************
 Macros

 MAXLOOPCOUNT	   : Display loop count
 WIDTH		   : Width of the output image.
 HEIGHT		   : Height of the image.
 BITS_PER_PIXEL	   : Number of bits per pixel
 RED_LENGTH, GREEN_LENGTH
 BLUE_LENGTH	   : Lengths of red, green and blue color components in pixels
 RED_OFFSET, GREEN_OFFSET,
 BLUE_OFFSET	   : Offsets of red, green and blue color component in 16 bits
******************************************************************************/

#define MAXLOOPCOUNT		5
#define WIDTH			800
#define HEIGHT			480
#define BITS_PER_PIXEL		32
#define RED_LENGTH		8
#define GREEN_LENGTH		8
#define BLUE_LENGTH		8
#define TRANSP_LENGTH		8
#define RED_OFFSET		16
#define GREEN_OFFSET		8
#define BLUE_OFFSET		0
#define TRANSP_OFFSET		24
#define WIDTH_VIRTUAL		WIDTH
#define HEIGHT_VIRTUAL		HEIGHT

static char display_dev_name[30] = {"/dev/fb0"};

static short ycbcr[2][8] = {
	{
		(0x1F << 11) | (0x3F << 5) | (0x1F),
		(0x00 << 11) | (0x00 << 5) | (0x00),
		(0x1F << 11) | (0x00 << 5) | (0x00),
		(0x00 << 11) | (0x3F << 5) | (0x00),
		(0x00 << 11) | (0x00 << 5) | (0x1F),
		(0x1F << 11) | (0x3F << 5) | (0x00),
		(0x1F << 11) | (0x00 << 5) | (0x1F),
		(0x00 << 11) | (0x3F << 5) | (0x1F),
	}, {
		(0x00 << 11) | (0x3F << 5) | (0x1F),
		(0x1F << 11) | (0x00 << 5) | (0x1F),
		(0x1F << 11) | (0x3F << 5) | (0x00),
		(0x00 << 11) | (0x00 << 5) | (0x1F),
		(0x00 << 11) | (0x3F << 5) | (0x00),
		(0x1F << 11) | (0x00 << 5) | (0x00),
		(0x00 << 11) | (0x00 << 5) | (0x00),
		(0x1F << 11) | (0x3F << 5) | (0x1F),
	}
};

/* This function is used to fill up buffer with color bars. */
void fill_color_bar(unsigned char *addr, int width, int height, int index)
{
	unsigned short *start = (unsigned short *)addr;
	unsigned int size = width * (height / 8);
	int i, j;

	if (index) {
		for (i = 0 ; i < 8 ; i++) {
			for (j = 0 ; j < size / 2 ; j++) {
				*start = ycbcr[1][i];
				start++;
			}
		}
	} else {
		for (i = 0 ; i < 8 ; i++) {
			for (j = 0 ; j < size / 2 ; j++) {
				*start = ycbcr[0][i];
				start++;
			}
		}
	}
}

void pre_multiply_data(unsigned int *addr, int width, int height,
						unsigned int prealpha_data){
	unsigned long int i;
	unsigned int *lcd_addr;

	lcd_addr = addr;
	width = width / 4;

	for (i = 0 ; i < (width*height - 1) ; i++) {
		if (i < 128000) {
			*lcd_addr = prealpha_data;
			lcd_addr++;
		} else if (i >= 128000 && i < 256000) {
			*lcd_addr =  prealpha_data;
			lcd_addr++;
		} else {
			*lcd_addr = prealpha_data;
			lcd_addr++;
		}
	}
}

int app_main()
{
	int display_fd;
	struct fb_fix_screeninfo fixinfo;
	struct fb_var_screeninfo varinfo, org_varinfo;
	int buffersize, ret;
	unsigned char *buffer_addr;
	int i;

	/* Open the display device */
	display_fd = open(display_dev_name, O_RDWR);
	if (display_fd <= 0) {
		perror("Could not open device\n");
		return -1;
	}

	/* Get fix screen information. Fix screen information gives
	 * fix information like panning step for horizontal and vertical
	 * direction, line length, memory mapped start address and length etc.
	 */
	ret = ioctl(display_fd, FBIOGET_FSCREENINFO, &fixinfo);
	if (ret < 0) {
		perror("Error reading fixed information.\n");
		goto exit1;
	}
	printf("\nFix Screen Info:\n");
	printf("----------------\n");
	printf("Line Length - %d\n", fixinfo.line_length);
	printf("Physical Address = %lx\n", fixinfo.smem_start);
	printf("Buffer Length = %d\n", fixinfo.smem_len);

	/* Get variable screen information. Variable screen information
	 * gives informtion like size of the image, bites per pixel,
	 * virtual size of the image etc. */
	ret = ioctl(display_fd, FBIOGET_VSCREENINFO, &varinfo);
	if (ret < 0) {
		perror("Error reading variable information.\n");
		goto exit1;
	}
	printf("\nVar Screen Info:\n");
	printf("----------------\n");
	printf("Xres - %d\n", varinfo.xres);
	printf("Yres - %d\n", varinfo.yres);
	printf("Xres Virtual - %d\n", varinfo.xres_virtual);
	printf("Yres Virtual - %d\n", varinfo.yres_virtual);
	printf("Bits Per Pixel - %d\n", varinfo.bits_per_pixel);
	printf("Pixel Clk - %d\n", varinfo.pixclock);
	printf("Rotation - %d\n", varinfo.rotate);

	memcpy(&org_varinfo, &varinfo, sizeof(varinfo));

	/* Change screen resolution and bits per pixel. Application can
	 * change resolution parameters, buffer format parameters,
	 * rotation parameters, buffer size parameters and timing parameters
	 * through FBIOPUT_VSCREENINFO ioctl. */
	/*
	 * NOTE: Due to known issue of the FBDEV that, driver changes the
	 * params for rotation internally, so application not supposed to
	 * chnage the params (width and height, etc)
	 */
	if ((varinfo.rotate == 1) || (varinfo.rotate == 3)) {
		varinfo.xres = HEIGHT;
		varinfo.yres = WIDTH;
		varinfo.xres_virtual = HEIGHT_VIRTUAL;
		varinfo.yres_virtual = WIDTH_VIRTUAL;
	} else {
		varinfo.xres = WIDTH;
		varinfo.yres = HEIGHT;
		varinfo.xres_virtual = WIDTH_VIRTUAL;
		varinfo.yres_virtual = HEIGHT_VIRTUAL;
	}
	varinfo.bits_per_pixel = BITS_PER_PIXEL;
	varinfo.red.length = RED_LENGTH;
	varinfo.green.length = GREEN_LENGTH;
	varinfo.blue.length = BLUE_LENGTH;
	varinfo.transp.length = TRANSP_LENGTH;
	varinfo.red.offset = RED_OFFSET;
	varinfo.green.offset = GREEN_OFFSET;
	varinfo.blue.offset = BLUE_OFFSET;
	varinfo.transp.offset = TRANSP_OFFSET;

	ret = ioctl(display_fd, FBIOPUT_VSCREENINFO, &varinfo);
	if (ret < 0) {
		perror("Error writing variable information.\n");
		goto exit1;
	}

	/* It is better to get fix screen information again. its because
	 * changing variable screen info may also change fix screen info. */
	ret = ioctl(display_fd, FBIOGET_FSCREENINFO, &fixinfo);
	if (ret < 0) {
		perror("Error reading fixed information.\n");
		goto exit2;
	}

	/* Mmap the driver buffers in application space so that application
	 * can write on to them. Driver allocates contiguous memory for
	 * three buffers. These buffers can be displayed one by one. */
	buffersize = fixinfo.line_length * varinfo.yres;
	buffer_addr = (unsigned char *)mmap(0, buffersize,
			(PROT_READ|PROT_WRITE),
			MAP_SHARED, display_fd, 0);

	if (buffer_addr == MAP_FAILED) {
		printf("MMap failed\n");
		ret = -ENOMEM;
		goto exit2;
	}

	unsigned int 	test_data[18] = { 0x00000000,
					0x32320000,
					0x64640000,
					0x96960000,
					0xc8c80000,
					0xffff0000,
					0x00000000,
					0x32003200,
					0x64006400,
					0x96009600,
					0xc800c800,
					0xff00ff00,
					0x00000000,
					0x32000032,
					0x64000064,
					0x96000096,
					0xc80000c8,
					0xff0000ff,};

	unsigned int run_count;
	unsigned int prealpha_data;
	for (run_count = 0 ; run_count < 18; run_count++) {
		prealpha_data = test_data[run_count];

		/* Main loop */
		for (i = 0 ; i < MAXLOOPCOUNT ; i++) {
			pre_multiply_data((unsigned int *)buffer_addr ,
					(fixinfo.line_length),
					varinfo.yres, prealpha_data);
		sleep(1);
		}
	}
	ret = 0;

	munmap(buffer_addr, buffersize);

exit2:
	/* It is better to revert back to original configuration */
exit1:
	close(display_fd);
	return ret;
}

int main()
{
	return app_main();
}
