
/*
 *  Low-level drawing operations for a cfb2 frame buffer
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
#include "util.h"


static u8 *screen;
static u32 screen_width;

static int cfb2_init(void)
{
    if (fb_fix.type != FB_TYPE_PACKED_PIXELS || fb_var.bits_per_pixel != 2)
	return 0;

    screen = fb;
    screen_width = fb_fix.line_length ? fb_fix.line_length
				      : fb_var.xres_virtual/4;
    return cfb_init();
}

static void cfb2_setpixel(u32 x, u32 y, pixel_t pixel)
{
    int shift = 2*(3- (x & 3));
    u8 *p = &screen[y*screen_width+x/4];
    u8 mask = 3 << shift;
    *p = pixel << shift | (*p & ~mask);
}

static pixel_t cfb2_getpixel(u32 x, u32 y)
{
    return (screen[y*screen_width+x/4] >> (2*(3- (x & 3)))) & 15;
}

const struct drawops cfb2_drawops = {
    .name =		"cfb2 (2 bpp packed pixels)",
    .init =		cfb2_init,
    .set_pixel =	cfb2_setpixel,
    .get_pixel =	cfb2_getpixel,
    .draw_hline =	cfb_draw_hline,
    .fill_rect =	cfb_fill_rect,
    .copy_rect =	cfb_copy_rect,
};

