
/*
 *  Low-level drawing operations for a cfb24 frame buffer
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

static int cfb24_init(void)
{
    if (fb_fix.type != FB_TYPE_PACKED_PIXELS || fb_var.bits_per_pixel != 24)
	return 0;

    screen = fb;
    screen_width = fb_fix.line_length ? fb_fix.line_length
				      : fb_var.xres_virtual*3;
    return cfb_init();
}

static void cfb24_setpixel(u32 x, u32 y, pixel_t pixel)
{
    u8 *dst;

    dst = &screen[y*screen_width+x*3];
    dst[0] = (pixel >> 16) & 0xff;
    dst[1] = (pixel >> 8) & 0xff;
    dst[2] = pixel & 0xff;
}

static pixel_t cfb24_getpixel(u32 x, u32 y)
{
    const u8 *src;

    src = &screen[y*screen_width+x*3];
    return (src[0] << 16) | (src[1] << 8) | src[2];
}

const struct drawops cfb24_drawops = {
    .name =		"cfb24 (24 bpp packed pixels)",
    .init =		cfb24_init,
    .set_pixel =	cfb24_setpixel,
    .get_pixel =	cfb24_getpixel,
    .draw_hline =	cfb_draw_hline,
    .fill_rect =	cfb_fill_rect,
    .copy_rect =	cfb_copy_rect,
};

