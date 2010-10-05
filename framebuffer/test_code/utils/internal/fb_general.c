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
//#include "meera2.h"

#define FBDEVICE "/dev/fb/0"
int fd;
struct fb_var_screeninfo var;
struct fb_fix_screeninfo fix;

int width, height;
char* data;

#define VERBOSE 1
#define QUIET	0

#include "fb_general_config.h"

/*----------------------------------------------------------------------------*/

#define show_cmap(X) 					\
do {							\
	int i;						\
	printf("%s : show_cmap()\n", __FUNCTION__);	\
	for (i=0; i<X; i++)				\
		printf("r 0x%x, g 0x%x, b 0x%x, t 0x%x\n", red[i], green[i], blue[i], transp[i]);	\
} while(0)

#define INIT_CMAP		\
	struct fb_cmap cmap;	\
	unsigned short red[256], blue[256], green[256], transp[256];\
	cmap.start = 0;		\
	cmap.len = 256;		\
	cmap.red = red;		\
	cmap.green = green;	\
	cmap.blue = blue;	\
	cmap.transp = transp;


void get_cmap()
{
	INIT_CMAP
	if (ioctl(fd, FBIOGETCMAP, &cmap))
		printf("FBIOGETCMAP ioctl failed!\n");

	show_cmap(256);
}

void set_cmap()
{
	int i;
	INIT_CMAP
	for (i=0; i<256; i++) {
		cmap.red[i] = cmap.green[i] = cmap.blue[i] = 0;
	}
	if (ioctl(fd, FBIOPUTCMAP, &cmap)) {
		printf("FBIOPUTCMAP ioctl failed!");
	}
}

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

/*----------------------------------------------------------------------------*/

static int shift(int xoffset, int yoffset)
{
	struct fb_var_screeninfo v;

	int ret = 0;

	if(ret = ioctl (fd, FBIOGET_VSCREENINFO, &v))
	{
		printf("getvar ioctl error \n");
		return ret;
	}
	v.xoffset = xoffset;
	v.yoffset = yoffset;

	if(ret = ioctl(fd, FBIOPUT_VSCREENINFO, &v))
	{
		printf("setvar ioctl error\n");
		return ret;
	}

	printf("Shifted by ( %d , %d )\n", xoffset, yoffset);
	return ret;
}

#define OMAP2_OUTPUT_LCD	4
#define OMAP2_OUTPUT_TV		5

#define H4_LCD_XRES	 	240
#define H4_LCD_YRES 		320
#define H4_LCD_PIXCLOCK_MAX	185186 /* freq 5.2 MHz */
#define H4_LCD_PIXCLOCK_MIN	138888 /* freq 7.4 MHz */

#define H4_TV_XRES		640
#define H4_TV_YRES		480

static int set_h4_var(int output_dev)
{
	struct fb_var_screeninfo *vp;
	int ret = 0;

        struct fb_var_screeninfo h4_lcd_var = {
		.xres		= H4_LCD_XRES,
		.yres		= H4_LCD_YRES,
		.xres_virtual	= H4_LCD_XRES,
		.yres_virtual	= H4_LCD_YRES*3,
		.xoffset	= 0,
		.yoffset	= 0,
		.bits_per_pixel	= 16,
		.grayscale	= 0,
		.red		= {11, 5, 0},
		.green		= { 5, 6, 0},
		.blue		= { 0, 5, 0},
		.transp		= { 0, 0, 0},
		.nonstd		= 0,
		.activate	= FB_ACTIVATE_NOW,
		.height		= -1,
		.width		= -1,
		.accel_flags	= 0,
		.pixclock	= H4_LCD_PIXCLOCK_MIN,/* picoseconds */
		.left_margin	= 40,		/* pixclocks */
		.right_margin	= 4,		/* pixclocks */
		.upper_margin	= 8,		/* line clocks */
		.lower_margin	= 2,		/* line clocks */
		.hsync_len	= 4,		/* pixclocks */
		.vsync_len	= 2,		/* line clocks */
		.sync		= 0,
		.vmode		= FB_VMODE_NONINTERLACED,
		.rotate		= 0,
		.reserved[0]	= 0,
	};

	struct fb_var_screeninfo h4_tv_var = {
		.xres		= H4_TV_XRES,
		.yres		= H4_TV_YRES,
		.xres_virtual	= H4_TV_XRES,
		.yres_virtual	= H4_TV_YRES*3,
		.xoffset	= 0,
		.yoffset	= 0,
		.bits_per_pixel	= 16,
		.grayscale	= 0,
		.red		= {11, 5, 0},
		.green		= { 5, 6, 0},
		.blue		= { 0, 5, 0},
		.transp		= { 0, 0, 0},
		.nonstd		= 0,
		.activate	= FB_ACTIVATE_NOW,
		.height		= -1,
		.width		= -1,
		.accel_flags	= 0,
		.pixclock	= H4_LCD_PIXCLOCK_MAX,/* picoseconds */
		.left_margin	= 40,		/* pixclocks */
		.right_margin	= 4,		/* pixclocks */
		.upper_margin	= 8,		/* line clocks */
		.lower_margin	= 2,		/* line clocks */
		.hsync_len	= 4,		/* pixclocks */
		.vsync_len	= 2,		/* line clocks */
		.sync		= 0,
		.vmode		= FB_VMODE_NONINTERLACED,
		.rotate		= 0,
		.reserved[0]	= 0,
	};

	if (output_dev == OMAP2_OUTPUT_TV)
		vp = &h4_tv_var;
	else // (output_dev == OMAP2_OUTPUT_LCD)
		vp = &h4_lcd_var;

	if(ret = ioctl(fd, FBIOPUT_VSCREENINFO, vp))
		printf("setvar ioctl error\n");

	return ret;
}

static int get_h4_rotated_var(struct fb_var_screeninfo *vp, int rotation)
{
	int ret = 0;
	unsigned int tmp;
	unsigned int xres =0,yres =0;

	if(ret = ioctl (fd, FBIOGET_VSCREENINFO, vp))
		return ret;

	if (rotation >= 0) {
		// rotation = -1 means no rotation support
		
	if(vp->rotate == 0 || vp->rotate == 180){
		xres = vp->xres;yres = vp->yres;
	}
	else
	if(vp->rotate == 90 || vp->rotate == 270){
		xres = vp->yres;yres = vp->xres;
	}
														  
	switch(rotation) {
		case 0:
		default:
			vp->xres 	= xres;
			vp->yres 	= yres;
			vp->xoffset	= 0;
			vp->yoffset	= 0;
			vp->rotate	= 0;
			break;
		case 90:
			vp->xres 	= yres;
			vp->yres 	= xres;
			vp->xoffset	= 0;
			vp->yoffset	= 0;
			vp->rotate	= 90;
			break;
		case 180:
			vp->xres 	= xres;
			vp->yres 	= yres;
			vp->xoffset	= 0;
			vp->yoffset	= 0;
			vp->rotate 	= 180;
			break;
		case 270:
			vp->xres 	= yres;
			vp->yres 	= xres;
			vp->xoffset	= 0;
			vp->yoffset	= 0;
			vp->rotate 	= 270;
			break;
		}
	}
	return ret;
}

int rotate(int deg, int verbose)
{
	int ret = 0;
	struct fb_var_screeninfo v;

	if(ret = get_h4_rotated_var(&v, deg))
		return ret;

	if(ret = ioctl(fd, FBIOPUT_VSCREENINFO, &v))
		return ret;
	if(ret = ioctl (fd, FBIOGET_VSCREENINFO, &v))
		return ret;
	else if(verbose)
		show_screeninfo(&v, &fix);

	return ret;
}

int mirror(int on_off, int verbose)
{
	int ret = 0;
	struct fb_var_screeninfo v;

	printf ("This mirroring function is NOT for use with 16.x/18.x & 12.x releases\n\
	For those releases please use fb_mirror instead of this.");
	if(ret = ioctl (fd, FBIOGET_VSCREENINFO, &v))
		return ret;
	v.reserved[0] = on_off;
	if(ret = ioctl(fd, FBIOPUT_VSCREENINFO, &v))
		return ret;
	else if(verbose)
		show_screeninfo(&v, &fix);
	return ret;
}

int change_mode(int bpp, int verbose)
{
	int ret = 0;
	struct fb_var_screeninfo v;

	if(ret = ioctl (fd, FBIOGET_VSCREENINFO, &v))
		return ret;

	switch (bpp) {
		case 8:
			v.bits_per_pixel= 8;
			v.grayscale	= 0;
			v.red		= (struct fb_bitfield) {0, 8, 0};
			v.green		= (struct fb_bitfield) {0, 8, 0};
			v.blue		= (struct fb_bitfield) {0, 8, 0};
			v.transp	= (struct fb_bitfield) {0, 0, 0};
			break;
		case 24:
			v.bits_per_pixel= 32;
			v.grayscale	= 0;
			v.red		= (struct fb_bitfield) {16, 8, 0};
			v.green		= (struct fb_bitfield) {8, 8, 0};
			v.blue		= (struct fb_bitfield) {0, 8, 0};
			v.transp	= (struct fb_bitfield) {0, 0, 0};
			break;
		case 32:
			v.bits_per_pixel= 32;
			v.grayscale	= 0;
			v.red		= (struct fb_bitfield) {24, 8, 0};
			v.green		= (struct fb_bitfield) {16, 8, 0};
			v.blue		= (struct fb_bitfield) {8, 8, 0};
			v.transp	= (struct fb_bitfield) {0, 8, 0};
			break;			
		case 16:
		default:
			v.bits_per_pixel= 16;
			v.grayscale	= 0;
			v.red		= (struct fb_bitfield) {11, 5, 0};
			v.green		= (struct fb_bitfield) {5, 6, 0};
			v.blue		= (struct fb_bitfield) {0, 5, 0};
			v.transp	= (struct fb_bitfield) {0, 0, 0};
			break;
	}
	printf("Change mode - trying \n");
	if(ret = ioctl(fd, FBIOPUT_VSCREENINFO, &v))
		return ret;
	if(ret = ioctl (fd, FBIOGET_VSCREENINFO, &v))
		return ret;
	if(verbose)
		show_screeninfo(&v, &fix);
	printf("Change mode successful \n");
	return ret;
}

/* Provide either a factor to multiply the current display size or the exact required size. 
 * If both are provided, the factor will take precedence.
 */
int set_display_size(unsigned int x, unsigned int y, int numerator, int denominator)
{
	int ret = 0;
	struct fb_var_screeninfo v;

	if(ret = ioctl (fd, FBIOGET_VSCREENINFO, &v))
		return ret;
	
	if ((numerator > 0) && (denominator > 0)) {
		x = v.xres * numerator / denominator;
		y = v.yres * numerator / denominator;
	} 
	v.xres = x;
	v.yres = y;
	if(ret = ioctl(fd, FBIOPUT_VSCREENINFO, &v)) {
		printf("Error changing display size to %d x %d\n", x, y);
		return ret;
	}
	printf("Display size changed to %d x %d\n", x, y);
	usleep(1000);
	return 0;	
}

int test_display_size()
{
	int ret = 0, max = 0, i = 0;
	struct fb_var_screeninfo v;

	if(ret = ioctl (fd, FBIOGET_VSCREENINFO, &v))
		return ret;

	/* Check if all xres values from 1 to v.xres work */
	for (i = 16; i <= v.xres; i++)
		set_display_size(i, v.yres, 0, 0);
	/* Reset to original */
	set_display_size(v.xres, v.yres, 0, 0);

	/* Check if all yres values from 1 to v.yres work */
	for (i = 16; i <= v.yres; i++)
		set_display_size(v.xres, i, 0, 0);
	/* Reset to original */
	set_display_size(v.xres, v.yres, 0, 0);
	
	/* Check if changing xres and yres together works */
	max = v.xres > v.yres ? v.xres : v.yres;
	for (i = 16; i <= max; i++)
		set_display_size((i <= v.xres)? i : v.xres,
				 (i <= v.yres)? i : v.yres, 0, 0);
	/* Reset to original */
	set_display_size(v.xres, v.yres, 0, 0);
}

#define BLANK	0
#define UNBLANK	1

void blank(int mode, int verbose)
{
	switch (mode) {
		case UNBLANK:
			if(ioctl(fd, FBIOBLANK, 0))
				if(verbose)
					printf("Unblanking ioctl failed\n");
			break;
		case BLANK:
		default:
			if(ioctl(fd, FBIOBLANK, 4))
				if(verbose)
					printf("Blanking ioctl failed\n");
			break;
	}
}

int flip_to_buffer(int i)
{
	int ret = 0;
	struct fb_var_screeninfo v;

	if(ret = ioctl (fd, FBIOGET_VSCREENINFO, &v))
		return ret;

	v.yoffset = v.yres * (i - 1);
	if(ret = ioctl(fd, FBIOPAN_DISPLAY, &v)) {
		printf("Error panning display to offsets (%d, %d)\n", v.xoffset, v.yoffset);
		return ret;
	}

	return 0;
}

int test_triple_buffer()
{
	int width=0, height=0;
	int i=0, j=0, k=0;
	int num_buf = 3;
	unsigned char col_8;
	
	width = fix.line_length;
	height = var.yres;

	data = (unsigned char *)mmap (0,
				width*height*num_buf,
				(PROT_READ|PROT_WRITE),
				MAP_SHARED, fd, 0) ;
	if (!data) {
		printf("MMap failed for %d x %d  of %d\n", width, height*num_buf, fix.smem_len);
		return -ENOMEM;
	}

	/* fill buffer 2 with col_8 = 0xAA */
	col_8 = 0xAA;
	for(i=var.yres; i<var.yres*2; i++)			//vertical
		for(j=0; j<var.xres*var.bits_per_pixel/8; j++)	//horizontal
			data[i*fix.line_length + j] = col_8;
	
	/* fill buffer 3 with col_8 = 0x22 */
	col_8 = 0x22;
	for(i=var.yres*2; i<var.yres*3; i++)			//vertical
		for(j=0; j<var.xres*var.bits_per_pixel/8; j++)	//horizontal
			data[i*fix.line_length + j] = col_8;

	munmap(data, fix.line_length * var.yres);
	
	flip_to_buffer(2);
	sleep(1);
	flip_to_buffer(3);
	sleep(1);
	flip_to_buffer(1);

	return 0;
}

int mmap_test()
{
	int width=0, height=0;
	int i=0, j=0, k=0;
	int count=0;

	width = fix.line_length;
	height = var.yres;

	do {
	printf("Trying to mmap %d x %d  of %d\n", width, height, fix.smem_len);
	data = (unsigned char *)mmap (0,
				width*height,
				(PROT_READ|PROT_WRITE),
				MAP_SHARED, fd, 0) ;
	printf("Mmap'ed - %d x %d  of %d\n", width, height, fix.smem_len);
	printf("Res - %d x %d of %d bpp \n", var.xres, var.yres, var.bits_per_pixel);
	for(i=0; i<var.yres; i++) {				//vertical
		for(j=0; j<var.xres; j++)	//horizontal
			for(k=0; k<var.bits_per_pixel/8; k++)	//pixel bytes
				data[i*fix.line_length + j*var.bits_per_pixel/8 + k] = 0xff;
//		printf("wrote line %d  ...xres...\n", i);
	}

	munmap(data, fix.line_length * var.yres);
	} while (count++ < 100);

	printf("Unmmap'ed - %d x %d  of %d\n", width, height, fix.smem_len);

	printf("Trying to mmap %d of %d\n", fix.smem_len, fix.smem_len);
	data = (unsigned char *)mmap (0,
				fix.smem_len,
				(PROT_READ|PROT_WRITE),
				MAP_SHARED, fd, 0) ;
	printf("Mmap'ed %d of %d\n", fix.smem_len, fix.smem_len);
/*	for(i=0; i<fix.smem_len; i++) 	// whole mmap'ed space
		data[i*fix.line_length + j*var.bits_per_pixel/8 + k] = 0x1f;
	printf("wrote whole mmap'ed space %d\n", fix.smem_len);
*/
	for(i=0; i<var.yres; i++) {	// whole mmap'ed space
		for(j=0; j<var.xres * var.bits_per_pixel/8; j++)
			data[i*fix.line_length + j] = 0x1f;
		printf("wrote line %d  ...xres... ", i);
		for(j=var.xres * var.bits_per_pixel/8; j<fix.line_length; j++)
			data[i*fix.line_length + j] = 0x1f;
		printf(" ...fix.line_length \n");
	}
	printf("|____________ yres x fix.line_length __________|\n");

	for(i=var.yres * fix.line_length; i<fix.smem_len; i++) {	// remaining mmap'ed space
		data[i] = 0x1f;
	}
	printf("wrote whole mmap'ed space %d\n", fix.smem_len);
	return 0;
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

/*
void show_bmp()
{
	int i;
	for (i=0; i<width*height; i++)
		data[i] = (char)meera2[i];
}
*/

void flood_fill (unsigned char col)
{
	int i;
	for (i=0; i<width*height; i++)
		data[i] = col;
}

void close_screen()
{
	munmap(data, width*height);
	close(fd);
}


int main(int argc, char **argv)
{
	int deg = -1;
	int bpp = 16;
	int xoff, yoff;

	printf("%s\n", argv[0]);

	if (init_screen() < 0)
		return -1;

#ifndef FUNCTION_BLANK
	ioctl (fd, FBIOGET_VSCREENINFO, &var) ;
	ioctl (fd, FBIOGET_FSCREENINFO, &fix) ;
	show_screeninfo(&var, &fix);
#endif

#ifdef FUNCTION_BLANK
	if (!argv[1]) {
		printf("Parameter: Blank (-b) / unblank (-u) / Stress (-stress)?\n");
		return;
	}
	if (!strcmp(argv[1], "-stress"))
		do {
			blank(BLANK, QUIET);
			blank(UNBLANK, QUIET);
		} while(1);
	else if (!strcmp(argv[1], "-u"))
		blank(UNBLANK, VERBOSE);
	else
		blank(BLANK, VERBOSE);
#endif

#ifdef FUNCTION_MMAP
	mmap_test();
#endif

#ifdef FUNCTION_SIZE
	if (argv[1] && argv[2]) {
		if (!strcmp(argv[1], "x") || !strcmp(argv[1], "X")) {
			int numerator, denominator;
			numerator = strtoul(argv[2], NULL, 10);
			denominator = strtoul(argv[3], NULL, 10);
			printf("Setting display size x %d/%d\n", numerator, denominator);
			set_display_size(0, 0, numerator, denominator);
		}
	}
	else
		test_display_size();
#endif
set_display_size(864, 480, 1, 1);
#ifdef FUNCTION_SHIFT
	xoff = yoff = 100;
	if (argv[1])
		xoff = strtoul(argv[1], NULL, 10);
	if (argv[2])
		yoff = strtoul(argv[2], NULL, 10);
	printf("shifting...\n");
	shift(xoff, yoff);
#endif

#ifdef FUNCTION_MIRROR
	if (!argv[1]) {
		printf("Parameter: 0 (mirroring off) or 1 (mirroring on)\n");
		return;
	} else {
		int m = strtoul(argv[1], NULL, 10);
		(m == 0) ? mirror(m, QUIET) : mirror(1, QUIET);
	}
#endif

#ifdef	FUNCTION_ROTATE
	if (!argv[1]) {
		printf("Parameter: Rotation angle [0 | 90 | 180 | 270) / Stress (-stress)?\n");
		return;
	}

	if (!strcmp(argv[1], "-stress"))
		do {
			rotate(0, QUIET);
			rotate(90, QUIET);
			rotate(180, QUIET);
			rotate(270, QUIET);
		} while(1);
	else if (!strcmp(argv[1], "0"))
		deg = 0;
	else if (!strcmp(argv[1], "90"))
		deg = 90;
	else if (!strcmp(argv[1], "180"))
		deg = 180;
	else if (!strcmp(argv[1], "270"))
		deg = 270;

	if (!rotate(deg, QUIET))
		printf("Rotated by %d degrees \n", deg);
	else
		printf("Rotation by %d degrees failed!\n", deg);
#endif

#ifdef FUNCTION_BPP
	if (!argv[1]) {
		printf("Parameter: Bits-per-pixel (8 | 16) / Stress (-stress)?\n");
		return;
	}
	if (!strcmp(argv[1], "-stress"))
		do {
			change_mode(8, QUIET);
			change_mode(16, QUIET);
		} while(1);
	else if (!strcmp(argv[1], "8"))
		bpp = 8;
	else if (!strcmp(argv[1], "24"))
		bpp = 24;
	else if (!strcmp(argv[1], "32"))
		bpp = 32;		
	else
		bpp = 16;
	if (change_mode(bpp, VERBOSE) < 0)
		printf("Change mode failed \n");
#endif

#ifdef FUNCTION_CMAP
	get_cmap();
//	show_bmp();
	flood_fill(0x11);
	usleep(200000);
	set_cmap();
	get_cmap();
//	flood_fill(0);
#endif

#ifdef FUNCTION_OUTPUTDEV
	if (!argv[1]) {
		printf("Parameter: 0 (lcd) or 1 (tv)\n");
		return;
	} else {
		int m = strtoul(argv[1], NULL, 10);
		(m == 0) ? set_h4_var(OMAP2_OUTPUT_LCD) : set_h4_var(OMAP2_OUTPUT_TV);
	}
#endif

#ifdef FUNCTION_FLIPBUF
	test_triple_buffer();
#endif
	printf("-----------------------------------\n");
	close(fd);
}
