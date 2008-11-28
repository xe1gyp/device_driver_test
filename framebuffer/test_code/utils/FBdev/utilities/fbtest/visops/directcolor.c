
/*
 *  Visual operations for a directcolor fbdev visual
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


static u32 cmap_len;

static void directcolor_update_cmap(void);


    /*
     *  Initialisation
     */

static int directcolor_init(void)
{
    u32 minbflen, maxbflen, i;
    pixel_t *table;
    pixel_t pixel;

    if (fb_fix.visual != FB_VISUAL_DIRECTCOLOR || fb_var.grayscale)
	return 0;

    /* Truecolor */
    truecolor_create_tables();

    /* Pseudocolor */
    minbflen = min(min(fb_var.red.length, fb_var.green.length),
		   fb_var.blue.length);
    maxbflen = max(max(fb_var.red.length, fb_var.green.length),
		   max(fb_var.blue.length, fb_var.transp.length));
    idx_bits = minbflen;
    idx_len = 1<<idx_bits;
    table = malloc(idx_len*sizeof(pixel_t));
    for (i = 0; i < idx_len; i++) {
	pixel = rgb_pixel(i, i, i);
	table[i] = pixel;
    }
    idx_pixel = table;

    /* Grayscale */
    gray_bits = idx_bits;
    gray_len = idx_len;
    gray_pixel = table;

    /* Monochrome */
    black_pixel = idx_pixel[0];
    white_pixel = idx_pixel[1];

    /* Directcolor */
    cmap_len = 1<<maxbflen;
    clut = malloc(cmap_len*sizeof(rgba_t));

    Message("Available visuals:\n");
    Message("  Monochrome\n");
    Message("  Grayscale %d\n", gray_len);
    Message("  Pseudocolor %d\n", idx_len);
    Message("  Truecolor %d:%d:%d:%d\n", red_bits, green_bits, blue_bits,
	    alpha_bits);
    Message("  Directcolor %d:%d:%d:%d\n", red_bits, green_bits, blue_bits,
	    alpha_bits);

    return 1;
}


    /*
     *  Monochrome
     */

static void directcolor_set_mono(void)
{
    clut[0].r = clut[0].g = clut[0].b = 0x0000; clut[0].a = 0xffff;
    clut[1].r = clut[1].g = clut[1].b = 0xffff; clut[1].a = 0xffff;
    directcolor_update_cmap();
}


    /*
     *  Truecolor/Grayscale
     */

static void directcolor_set_linear(void)
{
    clut_create_linear(clut, idx_len);
    directcolor_update_cmap();
}


    /*
     *  Generic
     */

static int directcolor_set_visual(enum visual_id id)
{
    switch (id) {
	case VISUAL_MONO:
	    directcolor_set_mono();
	    break;

	case VISUAL_PSEUDOCOLOR:
	case VISUAL_DIRECTCOLOR:
	    break;

	case VISUAL_GENERIC:
	case VISUAL_GRAYSCALE:
	case VISUAL_TRUECOLOR:
	    directcolor_set_linear();
	    break;

	default:
	    return 0;
    }
    return 1;
}


    /*
     *  Set the colormap from the CLUT
     */

static void directcolor_update_cmap(void)
{
    u32 i;

    for (i = 0; i < cmap_len; i++) {
	if (i < red_len)
	    fb_cmap.red[i] = clut[i].r;
	if (i < green_len)
	    fb_cmap.green[i] = clut[i].g;
	if (i < blue_len)
	    fb_cmap.blue[i] = clut[i].b;
	if (fb_cmap.transp && i < alpha_len)
	    fb_cmap.transp[i] = clut[i].a;
    }
    fb_set_cmap();
}


    /*
     *  Operations
     */

const struct visops directcolor_visops = {
    .name =		"directcolor",
    .init =		directcolor_init,
    .set_visual =	directcolor_set_visual,
    .update_cmap =	directcolor_update_cmap,
    .match_color =	truecolor_match_color,
};

