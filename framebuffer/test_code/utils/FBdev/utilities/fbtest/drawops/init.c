
/*
 *  Low-level drawing operations initialization
 *
 *  (C) Copyright 2001-2003 Geert Uytterhoeven
 *
 *  This file is subject to the terms and conditions of the GNU General Public
 *  License. See the file COPYING in the main directory of this archive for
 *  more details.
 */

#include "types.h"
#include "drawops.h"
#include "util.h"


    /*
     *  Supported frame buffer formats
     */

static const struct drawops *all_drawops[] = {
    &cfb2_drawops,
    &cfb4_drawops,
    &cfb8_drawops,
    &cfb16_drawops,
    &cfb24_drawops,
    &cfb32_drawops,
    &planar_drawops,
    &iplan2_drawops,
    NULL
};


    /*
     *  Current drawing operations
     */

struct drawops drawops;


    /*
     *  Initialization
     */

#define PRESENT_OR_SET_GENERIC(op)		\
    if (!drawops.op)			\
	drawops.op = generic_ ## op;

void drawops_init(void)
{
    int i;

    for (i = 0; all_drawops[i]; i++)
	if (all_drawops[i]->init()) {
	    drawops = *all_drawops[i];
	    PRESENT_OR_SET_GENERIC(draw_hline);
	    PRESENT_OR_SET_GENERIC(draw_vline);
	    PRESENT_OR_SET_GENERIC(draw_rect);
	    PRESENT_OR_SET_GENERIC(fill_rect);
	    PRESENT_OR_SET_GENERIC(draw_line);
	    PRESENT_OR_SET_GENERIC(expand_bitmap);
	    PRESENT_OR_SET_GENERIC(draw_pixmap);
	    PRESENT_OR_SET_GENERIC(draw_circle);
	    PRESENT_OR_SET_GENERIC(fill_circle);
	    PRESENT_OR_SET_GENERIC(draw_ellipse);
	    PRESENT_OR_SET_GENERIC(fill_ellipse);
	    PRESENT_OR_SET_GENERIC(copy_rect);
	    Message("Using drawops %s\n", drawops.name);
	    return;
	}

    Fatal("No drawing operations available for this frame buffer mode\n");
}

#undef PRESENT_OR_SET_GENERIC

