
/*
 *  Test001
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
#include "util.h"

#define X_BLOCKS	16
#define Y_BLOCKS	12

static enum test_res test001_func(void)
{
    int i, j;
    pixel_t pixel;
    u32 x0, y0, x1, y1;

    for (i = 1, y0 = 0; i <= Y_BLOCKS; i++, y0 = y1) {
	y1 = i*fb_var.yres/Y_BLOCKS;
	for (j = 1, x0 = 0; j <= X_BLOCKS; j++, x0 = x1) {
	    pixel = (i+j) & 1 ? white_pixel : black_pixel;
	    x1 = j*fb_var.xres/X_BLOCKS;
	    fill_rect(x0, y0, x1-x0, y1-y0, pixel);
	}
    }
    wait_for_key(10);
    return TEST_OK;
}

const struct test test001 = {
    .name =	"test001",
    .desc =	"Draw a 16x12 checkerboard pattern",
    .visual =	VISUAL_MONO,
    .func =	test001_func,
};

