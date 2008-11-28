
/*
 *  Low-level drawing operations for an iplan2 frame buffer
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
static u32 next_line;

static int iplan2_init(void)
{
    if (fb_fix.type != FB_TYPE_INTERLEAVED_PLANES || fb_fix.type_aux != 2)
	return 0;

    screen = fb;
    next_line = fb_fix.line_length
	? fb_fix.line_length
	: fb_var.bits_per_pixel*fb_var.xres_virtual/8;
    return 1;
}

static void iplan2_setpixel(u32 x, u32 y, pixel_t pixel)
{
    u16 *p = (u16 *)(screen+y*next_line+fb_var.bits_per_pixel*(x & -16));
    u16 mask = 0x8000 >> (x & 15);
    int i = fb_var.bits_per_pixel;
    while (1) {
	if (pixel & 1)
	    *p++ |= mask;
	else
	    *p++ &= ~mask;
	if (!--i)
	    break;
	pixel >>= 1;
    }
}

static pixel_t iplan2_getpixel(u32 x, u32 y)
{
    pixel_t pixel = 0, bit = 1;
    u16 *p = (u16 *)(screen+y*next_line+fb_var.bits_per_pixel*(x & -16));
    u16 mask = 0x8000 >> (x & 15);
    int i = fb_var.bits_per_pixel;
    while (1) {
	if (*p++ & mask)
	    pixel |= bit;
	if (!--i)
	    return pixel;
	bit <<= 1;
    }
}

const struct drawops iplan2_drawops = {
    .name =		"iplan2 (Atari interleaved bitplanes)",
    .init =		iplan2_init,
    .set_pixel =	iplan2_setpixel,
    .get_pixel =	iplan2_getpixel,
};

