
/*
 *  Low-level drawing operations for a cfb4 frame buffer
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

static int cfb4_init(void)
{
    if (fb_fix.type != FB_TYPE_PACKED_PIXELS || fb_var.bits_per_pixel != 4)
	return 0;

    screen = fb;
    screen_width = fb_fix.line_length ? fb_fix.line_length
				      : fb_var.xres_virtual/2;
    return cfb_init();
}

static void cfb4_setpixel(u32 x, u32 y, pixel_t pixel)
{
    u8 *p = &screen[y*screen_width+x/2];
    if (x & 1)
	*p = pixel | (*p & 0xf0);
    else
	*p = (pixel << 4) | (*p & 0x0f);
}

static pixel_t cfb4_getpixel(u32 x, u32 y)
{
    u8 d = screen[y*screen_width+x/2];
    return (x & 1) ? (d & 0x0f) : (d >> 4);
}

const struct drawops cfb4_drawops = {
    .name =		"cfb4 (4 bpp packed pixels)",
    .init =		cfb4_init,
    .set_pixel =	cfb4_setpixel,
    .get_pixel =	cfb4_getpixel,
    .draw_hline =	cfb_draw_hline,
    .fill_rect =	cfb_fill_rect,
    .copy_rect =	cfb_copy_rect,
};

