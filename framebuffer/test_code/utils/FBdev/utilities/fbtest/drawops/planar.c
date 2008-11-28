
/*
 *  Low-level drawing operations for a planar frame buffer
 *
 *  This driver supports 3 pixel formats:
 *    - monochrome packed pixels (mfb)
 *    - normal bitplanes (afb)
 *    - interleaved bitplanes (ilbm)
 *
 *  (C) Copyright 2001-2002 Geert Uytterhoeven
 *
 *  This file is subject to the terms and conditions of the GNU General Public
 *  License. See the file COPYING in the main directory of this archive for
 *  more details.
 */

#include "types.h"
#include "drawops.h"
#include "bitstream.h"
#include "fb.h"


static u8 *screen;
static u32 next_line;
static u32 next_plane;

static int planar_init(void)
{
    u32 len;

    len = fb_fix.line_length ? fb_fix.line_length : fb_var.xres_virtual/8;
    switch (fb_fix.type) {
	case FB_TYPE_PACKED_PIXELS:
	    if (fb_var.bits_per_pixel != 1)
		return 0;
	    /* mfb */
	    next_line = len;
	    break;

	case FB_TYPE_PLANES:
	    /* afb */
	    next_line = len;
	    next_plane = len*fb_var.yres_virtual;
	    break;

	case FB_TYPE_INTERLEAVED_PLANES:
	    if (fb_fix.type_aux != len)
		return 0;
	    /* ilbm */
	    next_line = len*fb_var.yres_virtual;
	    next_plane = len;
	    break;

	default:
	    return 0;
	    break;
    }
    screen = fb;
    return 1;
}

static void planar_setpixel(u32 x, u32 y, pixel_t pixel)
{
    u8 *p, mask;
    int i;

    p = screen+y*next_line+(x/8);
    mask = 0x80 >> (x & 7);
    i = fb_var.bits_per_pixel;
    while (1) {
	if (pixel & 1)
	    *p |= mask;
	else
	    *p &= ~mask;
	if (!--i)
	    break;
	pixel >>= 1;
	p += next_plane;
    }
}

static pixel_t planar_getpixel(u32 x, u32 y)
{
    pixel_t pixel = 0, bit = 1;
    u8 *p, mask;
    int i;

    p = screen+y*next_line+(x/8);
    mask = 0x80 >> (x & 7);
    i = fb_var.bits_per_pixel;
    while (1) {
	if (*p & mask)
	    pixel |= bit;
	if (!--i)
	    return pixel;
	bit <<= 1;
	p += next_plane;
    }
}

static inline void fill_one_line(unsigned long *dst, int dst_idx, u32 n,
				 pixel_t pixel)
{
    int i = fb_var.bits_per_pixel;
    while (1) {
	dst += dst_idx >> SHIFT_PER_LONG;
	dst_idx &= (BITS_PER_LONG-1);
	bitfill32(dst, dst_idx, pixel & 1 ? ~0 : 0, n);
	if (!--i)
	    break;
	pixel >>= 1;
	dst_idx += next_plane*8;
    }
}

static void planar_draw_hline(u32 x, u32 y, u32 length, pixel_t pixel)
{
    unsigned long *dst;
    int dst_idx;

    dst = (unsigned long *)((unsigned long)fb & ~(BYTES_PER_LONG-1));
    dst_idx = ((unsigned long)fb & (BYTES_PER_LONG-1))*8;
    dst_idx += y*next_line*8+x;
    fill_one_line(dst, dst_idx, length, pixel);
}

static void planar_fill_rect(u32 x, u32 y, u32 width, u32 height,
			     pixel_t pixel)
{
    unsigned long *dst;
    int dst_idx;

    dst = (unsigned long *)((unsigned long)fb & ~(BYTES_PER_LONG-1));
    dst_idx = ((unsigned long)fb & (BYTES_PER_LONG-1))*8;
    dst_idx += y*next_line*8+x;
    while (height--) {
	fill_one_line(dst, dst_idx, width, pixel);
	dst_idx += next_line*8;
    }
}

static inline void expand_one_line(unsigned long *dst, int dst_idx, u32 n,
				   const u8 *data, pixel_t pixel0,
				   pixel_t pixel1)
{
    const unsigned long *src;
    int src_idx, i;

    i = fb_var.bits_per_pixel;
    while (1) {
	dst += dst_idx >> SHIFT_PER_LONG;
	dst_idx &= (BITS_PER_LONG-1);
	if ((pixel0 ^ pixel1) & 1) {
	    src = (unsigned long *)((unsigned long)data & ~(BYTES_PER_LONG-1));
	    src_idx = ((unsigned long)data & (BYTES_PER_LONG-1))*8;
	    if (pixel1 & 1)
		bitcpy(dst, dst_idx, src, src_idx, n);
	    else
		bitcpy_not(dst, dst_idx, src, src_idx, n);
	    /* set or clear */
	} else
	    bitfill32(dst, dst_idx, pixel1 & 1 ? ~0 : 0, n);
	if (!--i)
	    break;
	pixel0 >>= 1;
	pixel1 >>= 1;
	dst_idx += next_plane*8;
    }
}

static void planar_expand_bitmap(u32 x, u32 y, u32 width, u32 height,
				 const u8 *data, u32 pitch, pixel_t pixel0,
				 pixel_t pixel1)
{
    unsigned long *dst;
    int dst_idx;

    dst = (unsigned long *)((unsigned long)fb & ~(BYTES_PER_LONG-1));
    dst_idx = ((unsigned long)fb & (BYTES_PER_LONG-1))*8;
    dst_idx += y*next_line*8+x;
    while (height--) {
	expand_one_line(dst, dst_idx, width, data, pixel0, pixel1);
	dst_idx += next_line*8;
	data += pitch;
    }
}


static inline void copy_one_line(unsigned long *dst, int dst_idx,
				 unsigned long *src, int src_idx, u32 n)
{
    int i = fb_var.bits_per_pixel;
    while (1) {
	dst += dst_idx >> SHIFT_PER_LONG;
	dst_idx &= (BITS_PER_LONG-1);
	src += src_idx >> SHIFT_PER_LONG;
	src_idx &= (BITS_PER_LONG-1);
	bitcpy(dst, dst_idx, src, src_idx, n);
	if (!--i)
	    break;
	dst_idx += next_plane*8;
	src_idx += next_plane*8;
    }
}

static inline void copy_one_line_rev(unsigned long *dst, int dst_idx,
				     unsigned long *src, int src_idx, u32 n)
{
    int i = fb_var.bits_per_pixel;
    while (1) {
	dst += dst_idx >> SHIFT_PER_LONG;
	dst_idx &= (BITS_PER_LONG-1);
	src += src_idx >> SHIFT_PER_LONG;
	src_idx &= (BITS_PER_LONG-1);
	bitcpy_rev(dst, dst_idx, src, src_idx, n);
	if (!--i)
	    break;
	dst_idx += next_plane*8;
	src_idx += next_plane*8;
    }
}

static void planar_copy_rect(u32 dx, u32 dy, u32 width, u32 height, u32 sx,
			     u32 sy)
{
    unsigned long *dst, *src;
    int dst_idx, src_idx;
    int rev_copy = 0;

    if (dy > sy || (dy == sy && dx > sx)) {
	dy += height;
	sy += height;
	rev_copy = 1;
    }
    dst = src = (unsigned long *)((unsigned long)fb & ~(BYTES_PER_LONG-1));
    dst_idx = src_idx = ((unsigned long)fb & (BYTES_PER_LONG-1))*8;
    dst_idx += dy*next_line*8+dx;
    src_idx += sy*next_line*8+sx;
    if (rev_copy) {
	while (height--) {
	    dst_idx -= next_line*8;
	    src_idx -= next_line*8;
	    copy_one_line_rev(dst, dst_idx, src, src_idx, width);
	}
    } else {
	while (height--) {
	    copy_one_line(dst, dst_idx, src, src_idx, width);
	    dst_idx += next_line*8;
	    src_idx += next_line*8;
	}
    }
}

const struct drawops planar_drawops = {
    .name =		"planar (monochrome and (interleaved) bitplanes)",
    .init =		planar_init,
    .set_pixel =	planar_setpixel,
    .get_pixel =	planar_getpixel,
    .draw_hline =	planar_draw_hline,
    .fill_rect =	planar_fill_rect,
    .expand_bitmap =	planar_expand_bitmap,
    .copy_rect =	planar_copy_rect,
};

