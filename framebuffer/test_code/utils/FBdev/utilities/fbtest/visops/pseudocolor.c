
/*
 *  Visual operations for a pseudocolor fbdev visual
 *
 *  (C) Copyright 2001-2002 Geert Uytterhoeven
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
#include "color.h"
#include "clut.h"
#include "util.h"


static int pseudocolor_cmap;

static void pseudocolor_update_cmap(void);


void pseudocolor_create_tables(u32 bpp)
{
    /* Pseudocolor */
    idx_bits = bpp;
    idx_len = 1<<bpp;
    idx_pixel = create_component_table(idx_len, fb_var.red.offset,
				       fb_var.red.msb_right,
				       fb_var.bits_per_pixel);
    clut = malloc(idx_len*sizeof(rgba_t));

    /* Grayscale */
    gray_bits = idx_bits;
    gray_len = idx_len;
    gray_pixel = idx_pixel;

    /* Monochrome */
    black_pixel = idx_pixel[0];
    white_pixel = idx_pixel[1];

    /* Truecolor/Directcolor emulation */
    if (bpp >= 3) {
	red_bits = green_bits = blue_bits = bpp/3;
	switch (bpp % 3) {
	    case 2:
		red_bits++;
	    case 1:
		green_bits++;
	}

	red_len = 1<<red_bits;
	green_len = 1<<green_bits;
	blue_len = 1<<blue_bits;
	red_pixel = create_component_table(red_len, green_bits+blue_bits,
					   fb_var.red.msb_right, bpp);
	green_pixel = create_component_table(green_len, blue_bits,
					   fb_var.red.msb_right, bpp);
	blue_pixel = create_component_table(blue_len, 0, fb_var.red.msb_right,
					   bpp);
    }
}


    /*
     *  Initialisation
     */

static int pseudocolor_init(void)
{
    if (fb_fix.visual != FB_VISUAL_PSEUDOCOLOR || fb_var.grayscale)
	return 0;

    pseudocolor_create_tables(fb_var.bits_per_pixel);

    Message("Available visuals:\n");
    Message("  Monochrome\n");
    Message("  Grayscale %d\n", gray_len);
    Message("  Pseudocolor %d\n", idx_len);
    if (idx_len >= 8) {
	Message("  Truecolor %d:%d:%d:%d\n", red_bits, green_bits, blue_bits,
		alpha_bits);
	Message("  Directcolor %d:%d:%d:%d\n", red_bits, green_bits, blue_bits,
		alpha_bits);
    }

    return 1;
}


    /*
     *  Monochrome
     */

static void pseudocolor_set_mono(void)
{
    clut[0].r = clut[0].g = clut[0].b = 0x0000; clut[0].a = 0xffff;
    clut[1].r = clut[1].g = clut[1].b = 0xffff; clut[1].a = 0xffff;
    pseudocolor_cmap = 1;
    pseudocolor_update_cmap();
}


    /*
     *  Grayscale
     */

static void pseudocolor_set_grayscale(void)
{
    clut_create_linear(clut, idx_len);
    pseudocolor_cmap = 1;
    pseudocolor_update_cmap();
}


    /*
     *  Truecolor
     */

static int pseudocolor_set_truecolor(void)
{
    if (idx_len < 8)
	return 0;

    clut_create_rgbcube(clut, red_len, green_len, blue_len);
    pseudocolor_cmap = 1;
    pseudocolor_update_cmap();
    return 1;
}


    /*
     *  Directcolor
     */

static int pseudocolor_set_directcolor(void)
{
    if (idx_len < 8)
	return 0;

    pseudocolor_cmap = 0;
    return 1;
}


int pseudocolor_set_visual(enum visual_id id)
{
    switch (id) {
	case VISUAL_PSEUDOCOLOR:
	    pseudocolor_cmap = 1;
	    break;

	case VISUAL_GENERIC:
	    pseudocolor_cmap = 1;
	    clut_init_nice();
	    break;

	case VISUAL_MONO:
	    pseudocolor_set_mono();
	    break;

	case VISUAL_GRAYSCALE:
	    pseudocolor_set_grayscale();
	    break;

	case VISUAL_TRUECOLOR:
	    return pseudocolor_set_truecolor();

	case VISUAL_DIRECTCOLOR:
	    return pseudocolor_set_directcolor();

	default:
	    return 0;
    }
    return 1;
}


    /*
     *  Set the colormap from the CLUT
     */

static void pseudocolor_update_cmap(void)
{
    u32 i, r, g, b;

    if (pseudocolor_cmap) {
	for (i = 0; i < idx_len; i++) {
	    fb_cmap.red[i] = clut[i].r;
	    fb_cmap.green[i] = clut[i].g;
	    fb_cmap.blue[i] = clut[i].b;
	    if (fb_cmap.transp)
		fb_cmap.transp[i] = clut[i].a;
	}
    } else {
	i = 0;
	for (r = 0; r < red_len; r++) {
	    for (g = 0; g < green_len; g++) {
		for (b = 0; b < blue_len; b++) {
		    fb_cmap.red[i] = clut[r].r;
		    fb_cmap.green[i] = clut[g].g;
		    fb_cmap.blue[i] = clut[b].b;
		    if (fb_cmap.transp)
			fb_cmap.transp[i] = 0xffff;
		    i++;
		}
	    }
	}
    }
    fb_set_cmap();
}


    /*
     *  Find a matching color in generic mode
     */

pixel_t pseudocolor_match_color(const rgba_t *color, rgba_t *error)
{
    u32 idx;

    idx = color_find(color, clut, idx_len);
    if (error)
	color_sub(error, color, &clut[idx]);
    return idx_pixel[idx];
}

const struct visops pseudocolor_visops = {
    .name =		"pseudocolor",
    .init =		pseudocolor_init,
    .set_visual =	pseudocolor_set_visual,
    .update_cmap =	pseudocolor_update_cmap,
    .match_color =	pseudocolor_match_color,
};

