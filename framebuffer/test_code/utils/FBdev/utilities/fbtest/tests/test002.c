
/*
 *  Test002
 *
 *  (C) Copyright 2001-2003 Geert Uytterhoeven
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

static enum test_res test002_func(void)
{
    int i;
    u32 a, b, x1, x2, y1, y2;

    fill_rect(0, 0, fb_var.xres, fb_var.yres, black_pixel);
    for (i = 0; i <= Y_BLOCKS; i++)
	draw_hline(0, i*(fb_var.yres-1)/Y_BLOCKS, fb_var.xres, white_pixel);
    for (i = 0; i <= X_BLOCKS; i++)
	draw_vline(i*(fb_var.xres-1)/X_BLOCKS, 0, fb_var.yres, white_pixel);
    draw_ellipse(fb_var.xres/2, fb_var.yres/2, 3*fb_var.xres/8,
		 fb_var.yres/2-1, white_pixel);
    a = (fb_var.xres-1)/X_BLOCKS;
    b = (fb_var.yres-1)/Y_BLOCKS;
    x1 = (fb_var.xres-1)/X_BLOCKS;
    y1 = (fb_var.yres-1)/Y_BLOCKS;
    x2 = (X_BLOCKS-1)*(fb_var.xres-1)/X_BLOCKS;
    y2 = (Y_BLOCKS-1)*(fb_var.yres-1)/Y_BLOCKS;
    draw_ellipse(x1, y1, a, b, white_pixel);
    draw_ellipse(x2, y1, a, b, white_pixel);
    draw_ellipse(x1, y2, a, b, white_pixel);
    draw_ellipse(x2, y2, a, b, white_pixel);
    wait_for_key(10);
    return TEST_OK;
}

const struct test test002 = {
    .name =	"test002",
    .desc =	"Draw a grid and some circles",
    .visual =	VISUAL_MONO,
    .func =	test002_func,
};

