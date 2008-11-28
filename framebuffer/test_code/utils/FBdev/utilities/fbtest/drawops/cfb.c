/*
 *  Generic low-level drawing operations for a cfb frame buffer
 *
 *  (C) Copyright 2002 Geert Uytterhoeven
 *
 *  This file is subject to the terms and conditions of the GNU General Public
 *  License. See the file COPYING in the main directory of this archive for
 *  more details.
 */

#include "types.h"
#include "drawops.h"
#include "bitstream.h"
#include "fb.h"


#define EXP1(x)		0xffffffffU*x
#define EXP2(x)		0x55555555U*x
#define EXP4(x)		0x11111111U*0x ## x

static const u32 bpp1tab[2] = {
    EXP1(0), EXP1(1)
};

static const u32 bpp2tab[4] = {
    EXP2(0), EXP2(1), EXP2(2), EXP2(3)
};

static const u32 bpp4tab[16] = {
    EXP4(0), EXP4(1), EXP4(2), EXP4(3), EXP4(4), EXP4(5), EXP4(6), EXP4(7),
    EXP4(8), EXP4(9), EXP4(a), EXP4(b), EXP4(c), EXP4(d), EXP4(e), EXP4(f)
};


static u32 next_line;

int cfb_init(void)
{
    if (fb_fix.type != FB_TYPE_PACKED_PIXELS || fb_var.bits_per_pixel > 32)
	return 0;

    next_line =
	fb_fix.line_length ? fb_fix.line_length
			   : fb_var.xres_virtual*8/fb_var.bits_per_pixel;
    return 1;
}


    /*
     *  Expand a pixel value to a 32-bit pattern
     */

static inline u32 pixel_to_pat32(pixel_t pixel)
{
    u32 pat = pixel;

    switch (fb_var.bits_per_pixel) {
	case 1:
	    pat = bpp1tab[pat];
	    break;

	case 2:
	    pat = bpp2tab[pat];
	    break;

	case 4:
	    pat = bpp4tab[pat];
	    break;

	case 8:
	    pat |= pat << 8;
	    // Fall through
	case 16:
	    pat |= pat << 16;
	    // Fall through
	case 32:
	    break;
    }
    return pat;
}


    /*
     *  Expand a pixel value to a generic 32/64-bit pattern and rotate it to
     *  the correct start position
     */

static inline unsigned long pixel_to_pat(pixel_t pixel, int left)
{
    unsigned long pat = pixel;
    u32 bpp = fb_var.bits_per_pixel;
    int i;

    /* expand pixel value */
    for (i = bpp; i < BITS_PER_LONG; i *= 2)
	pat |= pat << i;

    /* rotate pattern to correct start position */
    pat = pat << left | pat >> (bpp-left);
    return pat;
}


void cfb_draw_hline(u32 x, u32 y, u32 length, pixel_t pixel)
{
    unsigned long *dst;
    int dst_idx, left;
    u32 bpp = fb_var.bits_per_pixel;

    dst = (unsigned long *)((unsigned long)fb & ~(BYTES_PER_LONG-1));
    dst_idx = ((unsigned long)fb & (BYTES_PER_LONG-1))*8;
    dst_idx += y*next_line*8+x*bpp;
    dst += dst_idx >> SHIFT_PER_LONG;
    dst_idx &= (BITS_PER_LONG-1);
    /* FIXME For now we support 1-32 bpp only */
    left = BITS_PER_LONG % bpp;
    if (!left) {
	u32 pat = pixel_to_pat32(pixel);
	bitfill32(dst, dst_idx, pat, length*bpp);
    } else {
	unsigned long pat = pixel_to_pat(pixel, (left-dst_idx) % bpp);
	bitfill(dst, dst_idx, pat, left, bpp-left, length*bpp);
    }
}

void cfb_fill_rect(u32 x, u32 y, u32 width, u32 height, pixel_t pixel)
{
    unsigned long *dst;
    int dst_idx, left;
    u32 bpp = fb_var.bits_per_pixel;

    dst = (unsigned long *)((unsigned long)fb & ~(BYTES_PER_LONG-1));
    dst_idx = ((unsigned long)fb & (BYTES_PER_LONG-1))*8;
    dst_idx += y*next_line*8+x*bpp;
    /* FIXME For now we support 1-32 bpp only */
    left = BITS_PER_LONG % bpp;
    if (!left) {
	u32 pat = pixel_to_pat32(pixel);
	while (height--) {
	    dst += dst_idx >> SHIFT_PER_LONG;
	    dst_idx &= (BITS_PER_LONG-1);
	    bitfill32(dst, dst_idx, pat, width*bpp);
	    dst_idx += next_line*8;
	}
    } else {
	unsigned long pat = pixel_to_pat(pixel, (left-dst_idx) % bpp);
	int right = bpp-left;
	int r;
	while (height--) {
	    dst += dst_idx >> SHIFT_PER_LONG;
	    dst_idx &= (BITS_PER_LONG-1);
	    bitfill(dst, dst_idx, pat, left, right, width*bpp);
	    r = (next_line*8) % bpp;
	    pat = pat << (bpp-r) | pat >> r;
	    dst_idx += next_line*8;
	}
    }
}

void cfb_copy_rect(u32 dx, u32 dy, u32 width, u32 height, u32 sx, u32 sy)
{
    unsigned long *dst, *src;
    int dst_idx, src_idx;
    u32 bpp = fb_var.bits_per_pixel;
    int rev_copy = 0;

    if (dy > sy || (dy == sy && dx > sx)) {
	dy += height;
	sy += height;
	rev_copy = 1;
    }
    dst = src = (unsigned long *)((unsigned long)fb & ~(BYTES_PER_LONG-1));
    dst_idx = src_idx = ((unsigned long)fb & (BYTES_PER_LONG-1))*8;
    dst_idx += dy*next_line*8+dx*bpp;
    src_idx += sy*next_line*8+sx*bpp;
    if (rev_copy) {
	while (height--) {
	    dst_idx -= next_line*8;
	    src_idx -= next_line*8;
	    dst += dst_idx >> SHIFT_PER_LONG;
	    dst_idx &= (BITS_PER_LONG-1);
	    src += src_idx >> SHIFT_PER_LONG;
	    src_idx &= (BITS_PER_LONG-1);
	    bitcpy_rev(dst, dst_idx, src, src_idx, width*bpp);
	}
    } else {
	while (height--) {
	    dst += dst_idx >> SHIFT_PER_LONG;
	    dst_idx &= (BITS_PER_LONG-1);
	    src += src_idx >> SHIFT_PER_LONG;
	    src_idx &= (BITS_PER_LONG-1);
	    bitcpy(dst, dst_idx, src, src_idx, width*bpp);
	    dst_idx += next_line*8;
	    src_idx += next_line*8;
	}
    }
}

