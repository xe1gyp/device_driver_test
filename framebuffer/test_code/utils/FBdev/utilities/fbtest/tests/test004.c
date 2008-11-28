
/*
 *  Test004
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
#include "color.h"
#include "drawops.h"
#include "image.h"
#include "pixmap.h"
#include "visual.h"
#include "visops.h"
#include "test.h"
#include "util.h"


static enum test_res test004_func(void)
{
    const struct image *image;
    pixel_t *pixmap;
    int x, y, width, height, i;

    image = &penguin;
    pixmap = create_pixmap(image);

    fill_rect(0, 0, fb_var.xres, fb_var.yres, match_color(&c_black));
    for (y = 0; y < fb_var.yres; y += image->height) {
	height = min(image->height, fb_var.yres-y);
	for (x = 0; x < fb_var.xres; x += image->width) {
	    width = min(image->width, fb_var.xres-x);
	    if (width == image->width)
		draw_pixmap(x, y, image->width, height, pixmap);
	    else
		for (i = 0; i < height; i++)
		    draw_pixmap(x, y+i, width, 1, pixmap+i*image->width);
	}
    }
    wait_for_key(10);
    return TEST_OK;
}

const struct test test004 = {
    .name =	"test004",
    .desc =	"Show the penguins",
    .visual =	VISUAL_GENERIC,
    .func =	test004_func,
};

