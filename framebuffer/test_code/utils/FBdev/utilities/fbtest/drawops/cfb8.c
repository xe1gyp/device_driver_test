
/*
 *  Low-level drawing operations for a cfb8 frame buffer
 *
 *  (C) Copyright 2001-2002 Geert Uytterhoeven
 *
 *  This file is subject to the terms and conditions of the GNU General Public
 *  License. See the file COPYING in the main directory of this archive for
 *  more details.
 */

#include "types.h"
#include "drawops.h"
#include "fb.h"


static u8 *screen;
static u32 screen_width;

static int cfb8_init(void)
{
    if (fb_fix.type != FB_TYPE_PACKED_PIXELS || fb_var.bits_per_pixel != 8)
	return 0;

    screen = fb;
    screen_width = fb_fix.line_length ? fb_fix.line_length
				      : fb_var.xres_virtual;
    return cfb_init();
}

static void cfb8_setpixel(u32 x, u32 y, pixel_t pixel)
{
    screen[y*screen_width+x] = pixel;
}

static pixel_t cfb8_getpixel(u32 x, u32 y)
{
    return screen[y*screen_width+x];
}

const struct drawops cfb8_drawops = {
    .name =		"cfb8 (8 bpp packed pixels)",
    .init =		cfb8_init,
    .set_pixel =	cfb8_setpixel,
    .get_pixel =	cfb8_getpixel,
    .draw_hline =	cfb_draw_hline,
    .fill_rect =	cfb_fill_rect,
    .copy_rect =	cfb_copy_rect,
};

