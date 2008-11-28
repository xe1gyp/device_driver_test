
/*
 *  Test007
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

static void draw_grid(int x_blocks, int y_blocks, const pixel_t *x_table,
		      const pixel_t *y_table)
{
#if 1
    int i, j;
    pixel_t pixel;
    u32 x0, y0, x1, y1;

    for (i = 1, y0 = 0; i <= y_blocks; i++, y0 = y1) {
	y1 = i*fb_var.yres/y_blocks;
	for (j = 1, x0 = 0; j <= x_blocks; j++, x0 = x1) {
	    pixel = x_table[j-1] | y_table[i-1];
	    x1 = j*fb_var.xres/x_blocks;
	    fill_rect(x0, y0, x1-x0, y1-y0, pixel);
	}
    }
#endif
}

static void increase_level(int *component)
{
    int i;

    for (i = 0 ; i < 0x101000; i += 0x1010) {
	wait_ms(20);
	*component = i;
	clut_update();
    }
    wait_for_key(10);
    *component = 0;
    clut_update();
}

static enum test_res test007_func(void)
{
    int i;

    fill_rect(0, 0, fb_var.xres, fb_var.yres, black_pixel);
    for (i = 0; i < red_len; i++)
	clut[i].r = EXPAND_TO_16BIT(i, red_len-1);
    for (i = 0; i < green_len; i++)
	clut[i].g = EXPAND_TO_16BIT(i, green_len-1);
    for (i = 0; i < blue_len; i++)
	clut[i].b = EXPAND_TO_16BIT(i, blue_len-1);
    for (i = 0; i < alpha_len; i++)
	clut[i].a = 65535;
    clut_update();

    Message("Red and green, increasing blue level\n");
    draw_grid(red_len, green_len, red_pixel, green_pixel);
    increase_level(&clut[0].b);

    Message("Green and blue, increasing red level\n");
    draw_grid(green_len, blue_len, green_pixel, blue_pixel);
    increase_level(&clut[0].r);

    Message("Blue and red, increasing green level\n");
    draw_grid(blue_len, red_len, blue_pixel, red_pixel);
    increase_level(&clut[0].g);

    return TEST_OK;
}

const struct test test007 = {
    .name =	"test007",
    .desc =	"DirectColor test",
    .visual =	VISUAL_DIRECTCOLOR,
    .func =	test007_func,
};

