
/*
 *  Visual operations for an Amiga HAM/HAM8 (Hold And Modify) fbdev visual
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
#include "util.h"


    /*
     *  Initialisation
     */

static int ham_init(void)
{
    if (fb_fix.visual != FB_VISUAL_PSEUDOCOLOR || fb_var.grayscale ||
	(fb_var.bits_per_pixel != 6 && fb_var.bits_per_pixel != 8) ||
	fb_var.nonstd != FB_NONSTD_HAM)
	return 0;

    /*
     *  FIXME: for now we don't use the HAM features and treat it just like
     *         pseudocolor
     */
    pseudocolor_create_tables(fb_var.bits_per_pixel-2);

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

    return 0;
}

const struct visops ham_visops = {
    .name =		"Amiga HAM/HAM8",
    .init =		ham_init,
    .set_visual =	pseudocolor_set_visual,
    .match_color =	pseudocolor_match_color,
};

