
/*
 *  Visual operations for a truecolor fbdev visual
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
#include "util.h"


#define CREATE_COMPONENT_TABLE(tn, cn)					\
    do {								\
	tn ## _bits = fb_var.cn.length;					\
	tn ## _len = 1<<tn ## _bits;					\
	tn ## _pixel = create_component_table(tn ## _len,		\
					      fb_var.cn.offset,		\
					      fb_var.cn.msb_right,	\
					      fb_var.bits_per_pixel);	\
    } while (0);

void truecolor_create_tables(void)
{
    pixel_t *table;
    pixel_t pixel;
    u32 i;

    /* Truecolor */
    CREATE_COMPONENT_TABLE(red, red);
    CREATE_COMPONENT_TABLE(green, green);
    CREATE_COMPONENT_TABLE(blue, blue);
    CREATE_COMPONENT_TABLE(alpha, transp);

    /* Grayscale */
    gray_bits = min(min(red_bits, green_bits), blue_bits);
    gray_len = 1<<gray_bits;
    table = malloc(gray_len*sizeof(pixel_t));
    for (i = 0; i < gray_len; i++) {
	pixel = rgb_pixel(CONVERT_RANGE(i, gray_len-1, red_len-1),
			  CONVERT_RANGE(i, gray_len-1, green_len-1),
			  CONVERT_RANGE(i, gray_len-1, blue_len-1));
	table[i] = pixel;
    }
    gray_pixel = table;

    /* Monochrome */
    black_pixel = rgb_pixel(0, 0, 0);
    white_pixel = rgb_pixel(red_len-1, green_len-1, blue_len-1);
}

#undef CREATE_COMPONENT_TABLE


    /*
     *  Initialisation
     */

static int truecolor_init(void)
{
    if (fb_fix.visual != FB_VISUAL_TRUECOLOR || fb_var.grayscale)
	return 0;

    truecolor_create_tables();

    Message("Available visuals:\n");
    Message("  Monochrome\n");
    Message("  Grayscale %d\n", gray_len);
    Message("  Truecolor %d:%d:%d:%d\n", red_bits, green_bits, blue_bits,
	    alpha_bits);

    return 1;
}


    /*
     *  Set visual
     */

static int truecolor_set_visual(enum visual_id id)
{
    switch (id) {
	case VISUAL_GENERIC:
	case VISUAL_MONO:
	case VISUAL_GRAYSCALE:
	case VISUAL_TRUECOLOR:
	    break;

	default:
	    return 0;
    }
    return 1;
}


    /*
     *  Generic mode
     */

pixel_t truecolor_match_color(const rgba_t *color, rgba_t *error)
{
    rgba_t approx;
    u32 r, g, b, a;

    r = COMPRESS_FROM_16BIT(color->r, red_len-1);
    g = COMPRESS_FROM_16BIT(color->g, green_len-1);
    b = COMPRESS_FROM_16BIT(color->b, blue_len-1);
    a = COMPRESS_FROM_16BIT(color->a, alpha_len-1);
    if (error) {
	approx.r = EXPAND_TO_16BIT(r, red_len-1);
	approx.g = EXPAND_TO_16BIT(g, green_len-1);
	approx.b = EXPAND_TO_16BIT(b, blue_len-1);
	approx.a = EXPAND_TO_16BIT(a, alpha_len-1);
	color_sub(error, color, &approx);
    }
    return rgba_pixel(r, g, b, a);
}


    /*
     *  Operations
     */

const struct visops truecolor_visops = {
    .name =		"truecolor",
    .init =		truecolor_init,
    .set_visual =	truecolor_set_visual,
    .match_color =	truecolor_match_color,
};

