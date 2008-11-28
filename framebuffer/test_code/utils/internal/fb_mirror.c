/*
 * Framebuffer dirver 'mirroring' test program for 16.x/18.x & 12.x releases
 */

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <string.h>
#include "fb.h"
#include <stdlib.h>

#define FBIO_MIRROR	_IOW('F', 0x21, u_int32_t)

#define FBDEVICE "/dev/fb0"
int fd;
struct fb_var_screeninfo var;
struct fb_fix_screeninfo fix;

int width, height;
char* data;

#define VERBOSE 1
#define QUIET	1

/*----------------------------------------------------------------------------*/
static int show_screeninfo(struct fb_var_screeninfo *vinfo,
			struct fb_fix_screeninfo *finfo)
{
		printf("fb0 Fixed Info:\n"
				"   %s  @ 0x%lx, len=0x%x, line=0x%x (%d) bytes,\n",
			finfo->id,
			finfo->smem_start,
			finfo->smem_len,
			finfo->line_length,
			finfo->line_length);

		printf(
				"   Geometry - %u x %u, %u bpp%s\n",
			vinfo->xres,
			vinfo->yres,
			vinfo->bits_per_pixel,
			vinfo->grayscale ? ", greyscale" : "");

		printf("   Color - offset:length:msb_right \n");


		printf("   Greyscale %d\n",vinfo->grayscale);
		printf("   Nonstd %d\n",vinfo->nonstd);
		printf("   Red %d:%d:%d\n",vinfo->red.offset,vinfo->red.length, vinfo->red.msb_right);
	        printf("   Green %d:%d:%d\n",vinfo->green.offset,vinfo->green.length, vinfo->green.msb_right);
		printf("   Blue %d:%d:%d\n",vinfo->blue.offset,vinfo->blue.length, vinfo->blue.msb_right);
		printf("   Transparent %d:%d:%d\n",vinfo->transp.offset,vinfo->transp.length, vinfo->transp.msb_right);

		if ( finfo->visual == FB_VISUAL_TRUECOLOR) {
			printf(		"visual: FB_VISUAL_TRUECOLOR\n");
		} else {
			printf(		"visual: %d\n", finfo->visual);
		}


		if ( (vinfo->height != -1) ||
				(vinfo->width != -1)) {
			printf(
				"   %d mm high x %d mm wide\n",
				vinfo->height,
				vinfo->width);
		}
}

int mirror(int on_off, int verbose)
{
	int ret = 0;
	struct fb_var_screeninfo v;

	printf ("This mirroring function is for use with 16.x/18.x & 12.x releases\n\
	For other releases please use fbmirror instead of this.\n");
	if(ret = ioctl(fd, FBIO_MIRROR, &on_off))
		return ret;
	else if(verbose)
		show_screeninfo(&v, &fix);
	return ret;
}

int init_screen()
{
	int i=0, j=0, k=0;
	int count = 0;

	fd = open(FBDEVICE, O_RDWR);
	if (fd <= 0) {
		printf("Could not open device\n");
		return -1;
	}
	return 0;
}

void close_screen()
{
	munmap(data, width*height);
	close(fd);
}

int main(int argc, char **argv)
{
	printf("%s\n", argv[1]);

	if (init_screen() < 0)
		return -1;

	if (!argv[1]) {
		printf("Parameter: 0 (mirroring off) or 1 (mirroring on)\n");
		return;
	} else {
		int m = strtoul(argv[1], NULL, 10);
		(m == 0) ? mirror(m, QUIET) : mirror(1, QUIET);
	}

	printf("-----------------------------------\n");
	close(fd);
}
