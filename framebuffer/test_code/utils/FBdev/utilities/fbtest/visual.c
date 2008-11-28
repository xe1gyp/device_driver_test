
/*
 *  Visual control
 *
 *  (C) Copyright 2001 Geert Uytterhoeven
 *
 *  This file is subject to the terms and conditions of the GNU General Public
 *  License. See the file COPYING in the main directory of this archive for
 *  more details.
 */

#include <stdlib.h>

#include "types.h"
#include "visual.h"
#include "visops.h"
#include "fb.h"
#include "util.h"


    /*
     *  Set the visual
     */

int visual_set(enum visual_id id)
{
    return visops.set_visual(id);
}


    /*
     *  Monochrome
     */

pixel_t black_pixel, white_pixel;


    /*
     *  Grayscale
     */

u32 gray_len, gray_bits;
const pixel_t *gray_pixel;


    /*
     *  CLUT for Pseudocolor and Directcolor
     */

u32 idx_len, idx_bits;
const pixel_t *idx_pixel;
rgba_t *clut;


void clut_update(void)
{
    if (visops.update_cmap)
	visops.update_cmap();
}


    /*
     *  RGB(A) for Truecolor and DirectColor
     */

u32 red_len, green_len, blue_len, alpha_len;
u32 red_bits, green_bits, blue_bits, alpha_bits;
const pixel_t *red_pixel, *green_pixel, *blue_pixel, *alpha_pixel;


    /*
     *  Reverse the bits in a 32-bit word
     */

static u32 reverse32(u32 x)
{
    x = ((x & 0xffff0000) >> 16) | ((x & 0x0000ffff) << 16);
    x = ((x & 0xff00ff00) >>  8) | ((x & 0x00ff00ff) <<  8);
    x = ((x & 0xf0f0f0f0) >>  4) | ((x & 0x0f0f0f0f) <<  4);
    x = ((x & 0xcccccccc) >>  2) | ((x & 0x33333333) <<  2);
    x = ((x & 0xaaaaaaaa) >>  1) | ((x & 0x55555555) <<  1);
    return x;
}


    /*
     *  Create an indexed table for one color component, based on the color
     *  bitfield
     */

pixel_t *create_component_table(u32 size, u32 offset, int msb_right, u32 bpp)
{
    pixel_t *table, pixel;
    int i;

    if (!size)
	return NULL;

    table = malloc(size*sizeof(pixel_t));
    for (i = 0; i < size; i++) {
	pixel = i<<offset;
	table[i] = msb_right ? reverse32(pixel)>>(32-bpp) : pixel;
    }
    return table;
}

