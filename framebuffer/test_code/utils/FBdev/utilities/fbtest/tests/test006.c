
/*
 *  Test006
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

#define Y_BLOCKS	16

static enum test_res test006_func(void)
{
    int i, j;
    pixel_t pixels[2];
    u32 x0, x1, y0, y1;

    for (i = 1, x0 = 0; i <= gray_len; i++, x0 = x1) {
	pixels[0] = gray_pixel[i-1];
	pixels[1] = gray_pixel[gray_len-i];
	x1 = i*fb_var.xres/gray_len;
	for (j = 1, y0 = 0; j <= Y_BLOCKS; j++, y0 = y1) {
	    y1 = j*fb_var.yres/Y_BLOCKS;
	    fill_rect(x0, y0, x1-x0, y1-y0, pixels[j & 1]);
	}
    }
    wait_for_key(10);
    return TEST_OK;
}

const struct test test006 = {
    .name =	"test006",
    .desc =	"Draw grayscale bands",
    .visual =	VISUAL_GRAYSCALE,
    .func =	test006_func,
};

