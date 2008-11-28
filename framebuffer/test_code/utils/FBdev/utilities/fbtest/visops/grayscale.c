
/*
 *  Visual operations for a truecolor fbdev visual
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
#include "util.h"


void grayscale_create_tables(void)
{
    gray_bits = fb_var.bits_per_pixel;
    gray_len = 1<<fb_var.bits_per_pixel;
    gray_pixel = create_component_table(gray_len, fb_var.red.offset,
					fb_var.red.msb_right, gray_bits);
}


    /*
     *  Initialisation
     */

static int grayscale_init(void)
{
    if (fb_fix.visual != FB_VISUAL_TRUECOLOR || !fb_var.grayscale)
	return 0;

    /* Grayscale */
    grayscale_create_tables();

    /* Monochrome */
    black_pixel = gray_pixel[0];
    white_pixel = gray_pixel[gray_len-1];

    Message("Available visuals:\n");
    Message("  Monochrome\n");
    Message("  Grayscale %d\n", gray_len);

    return 1;
}


    /*
     *  Set visual
     */

static int grayscale_set_visual(enum visual_id id)
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
     *  Generic mode
     */

pixel_t grayscale_match_color(const rgba_t *color, rgba_t *error)
{
    rgba_t approx;
    u32 g;

    g = CONVERT_RANGE(color->r+color->g+color->b, 3*65535, gray_len-1);
    if (error) {
	approx.r = EXPAND_TO_16BIT(g, gray_len-1);
	approx.g = EXPAND_TO_16BIT(g, gray_len-1);
	approx.b = EXPAND_TO_16BIT(g, gray_len-1);
	approx.a = 0xffff;
	color_sub(error, color, &approx);
    }
    return gray_pixel[g];
}


    /*
     *  Operations
     */

const struct visops grayscale_visops = {
    .name =		"grayscale",
    .init =		grayscale_init,
    .set_visual =	grayscale_set_visual,
    .match_color =	grayscale_match_color,
};

