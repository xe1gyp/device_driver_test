
/*
 *  Test003
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

static enum test_res test003_func(void)
{
    int i;
    pixel_t pixel;
    u32 x0, x1;

    for (i = 0; i < 16; i++)
	clut[i] = clut_console[i];
    clut_update();
    for (i = 1, x0 = 0; i <= 16; i++, x0 = x1) {
	pixel = idx_pixel[i];
	x1 = i*fb_var.xres/16;
	fill_rect(x0, 0, x1-x0, fb_var.yres, pixel);
    }
    wait_for_key(10);
    return TEST_OK;
}

const struct test test003 = {
    .name =		"test003",
    .desc =		"Draw the 16 Linux console colors",
    .visual =		VISUAL_PSEUDOCOLOR,
    .reqs =		REQF_num_colors,
    .num_colors =	16,
    .func =		test003_func,
};

