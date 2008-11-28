
/*
 *  Test008
 *
 *  (C) Copyright 2001-2002 Geert Uytterhoeven
 *
 *  This file is subject to the terms and conditions of the GNU General Public
 *  License. See the file COPYING in the main directory of this archive for
 *  more details.
 */

#include <unistd.h>

#include "types.h"
#include "fb.h"
#include "drawops.h"
#include "visual.h"
#include "visops.h"
#include "color.h"
#include "test.h"
#include "util.h"

#define X_BLOCKS	256
#define Y_BLOCKS	256

static enum test_res test008_func(void)
{
    int i, j;
    yuva_t yuva;
    rgba_t rgba;
    pixel_t pixel;
    u32 x0, y0, x1, y1;

    yuva.y = 0xa000;
    yuva.a = 0xffff;
    for (i = 1, y0 = 0; i <= Y_BLOCKS; i++, y0 = y1) {
	y1 = i*fb_var.yres/Y_BLOCKS;
	for (j = 1, x0 = 0; j <= X_BLOCKS; j++, x0 = x1) {
	    yuva.u = EXPAND_TO_16BIT((i-1), Y_BLOCKS-1);
	    yuva.v = EXPAND_TO_16BIT((j-1), X_BLOCKS-1);
	    yuva_to_rgba(&yuva, &rgba);
	    pixel = match_color(&rgba);
	    x1 = j*fb_var.xres/X_BLOCKS;
	    fill_rect(x0, y0, x1-x0, y1-y0, pixel);
	}
    }
    wait_for_key(10);
    return TEST_OK;
}

const struct test test008 = {
    .name =	"test008",
    .desc =	"Draw the UV color space",
    .visual =	VISUAL_TRUECOLOR,
    .func =	test008_func,
};

