
/*
 *  Test009
 *
 *  (C) Copyright 2002 Geert Uytterhoeven
 *
 *  This file is subject to the terms and conditions of the GNU General Public
 *  License. See the file COPYING in the main directory of this archive for
 *  more details.
 */

#include <unistd.h>

#include "types.h"
#include "fb.h"
#include "color.h"
#include "drawops.h"
#include "image.h"
#include "pixmap.h"
#include "visual.h"
#include "visops.h"
#include "test.h"
#include "util.h"


static enum test_res test009_func(void)
{
    const struct image *image;
    pixel_t *pixmap;
    int x, y, width, height;

    image = &penguin;
    pixmap = create_pixmap(image);
    width = image->width;
    height = image->height;
    if (width > fb_var.xres || height > fb_var.yres) {
	Message("Screen size too small for this test\n");
	return TEST_NA;
    }

    fill_rect(0, 0, fb_var.xres, fb_var.yres, match_color(&c_black));
    draw_pixmap(0, 0, width, height, pixmap);
    wait_ms(1000);
    for (x = width; x < fb_var.xres; x += width) {
	copy_rect(x, 0, min(width, fb_var.xres-x), height, 0, 0);
	wait_ms(20);
    }
    wait_ms(1000);
    for (y = height; y < fb_var.yres; y += height) {
	copy_rect(0, y, fb_var.xres, min(height, fb_var.yres-y), 0, 0);
	wait_ms(20);
    }
    wait_for_key(10);
    return TEST_OK;
}

const struct test test009 = {
    .name =	"test009",
    .desc =	"Show the penguins using copy_rect",
    .visual =	VISUAL_GENERIC,
    .func =	test009_func,
};

