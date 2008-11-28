
/*
 *  Test005
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
#include "test.h"
#include "clut.h"
#include "util.h"

static enum test_res test005_func(void)
{
    int i, j;
    pixel_t pixel;
    u32 x0, y0, x1, y1;
    int x_bits, y_bits, x_blocks, y_blocks;

    clut_init_nice();
    y_bits = idx_bits/2;
    x_bits = idx_bits-y_bits;
    x_blocks = 1<<x_bits;
    y_blocks = 1<<y_bits;
    for (i = 1, y0 = 0; i <= y_blocks; i++, y0 = y1) {
	y1 = i*fb_var.yres/y_blocks;
	for (j = 1, x0 = 0; j <= x_blocks; j++, x0 = x1) {
	    pixel = idx_pixel[(i-1)*x_blocks+(j-1)];
	    x1 = j*fb_var.xres/x_blocks;
	    fill_rect(x0, y0, x1-x0, y1-y0, pixel);
	}
    }
    wait_for_key(10);
    return TEST_OK;
}

const struct test test005 = {
    .name =	"test005",
    .desc =	"Draw the default color palette",
    .visual =	VISUAL_PSEUDOCOLOR,
    .func =	test005_func,
};

