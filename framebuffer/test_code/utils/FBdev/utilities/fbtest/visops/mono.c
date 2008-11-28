
/*
 *  Visual operations for a monochrome fbdev visual
 *
 *  (C) Copyright 2001-2002 Geert Uytterhoeven
 *
 *  This file is subject to the terms and conditions of the GNU General Public
 *  License. See the file COPYING in the main directory of this archive for
 *  more details.
 */

#include "types.h"
#include "visual.h"
#include "visops.h"
#include "fb.h"
#include "color.h"
#include "clut.h"
#include "util.h"


static rgba_t mono_clut[2];

static pixel_t mono_gray_pixel[2];


    /*
     *  Initialisation
     */

static int mono_init(void)
{
    switch (fb_fix.visual) {
	case FB_VISUAL_MONO01:
	    black_pixel = 1;
	    white_pixel = 0;
	    break;

	case FB_VISUAL_MONO10:
	    black_pixel = 0;
	    white_pixel = 1;
	    break;

	default:
	    return 0;
    }

    /* Monochrome */
    mono_clut[black_pixel] = clut_mono[0];
    mono_clut[white_pixel] = clut_mono[1];

    /* Grayscale */
    gray_len = 2;
    gray_bits = 1;
    mono_gray_pixel[0] = black_pixel;
    mono_gray_pixel[1] = white_pixel;
    gray_pixel = mono_gray_pixel;

    Message("Available visuals:\n");
    Message("  Monochrome\n");
    Message("  Grayscale %d\n", gray_len);

    return 1;
}


    /*
     *  Set the visual
     */

static int mono_set_visual(enum visual_id id)
{
    switch (id) {
	case VISUAL_GENERIC:
	case VISUAL_MONO:
	case VISUAL_GRAYSCALE:
	    break;

	default:
	    return 0;
    }
    return 1;
}


    /*
     *  Generic
     */

static pixel_t mono_match_color(const rgba_t *color, rgba_t *error)
{
    u32 idx;

    idx = color_find(color, mono_clut, 2);
    if (error)
	color_sub(error, color, &mono_clut[idx]);
    return idx;
}


    /*
     *  Operations
     */

const struct visops mono_visops = {
    .name =		"monochrome",
    .init =		mono_init,
    .set_visual =	mono_set_visual,
    .match_color =	mono_match_color,
};

