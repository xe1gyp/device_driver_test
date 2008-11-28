
/*
 *  Test010
 *
 *  (C) Copyright 2002-2003 Geert Uytterhoeven
 *
 *  This file is subject to the terms and conditions of the GNU General Public
 *  License. See the file COPYING in the main directory of this archive for
 *  more details.
 */

#include <unistd.h>

#include "types.h"
#include "fb.h"
#include "font.h"
#include "console.h"
#include "visual.h"
#include "test.h"
#include "util.h"

#define FONT	sun12x22


static enum test_res test010_func(void)
{
    int i, j;

    con_init(&FONT);
    con_puts("Hello, world!\n");
    for (i = 0; i < fb_var.yres/FONT.height+10; i++) {
	for (j = 0; j < i; j++)
	    con_putc(' ');
	con_printf("The quick brown fox jumps over the lazy dog\n");
    }
    for (i = 32; i < 256; i++)
	con_putc(i);
    wait_for_key(10);
    return TEST_OK;
}

const struct test test010 = {
    .name =		"test010",
    .desc =		"Hello world",
    .visual =		VISUAL_PSEUDOCOLOR,
    .reqs =		REQF_num_colors,
    .num_colors =	16,
    .func =		test010_func,
};

