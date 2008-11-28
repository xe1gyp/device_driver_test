
/*
 *  Colormap manipulation
 *
 *  (C) Copyright 2001-2002 Geert Uytterhoeven
 *
 *  This file is subject to the terms and conditions of the GNU General Public
 *  License. See the file COPYING in the main directory of this archive for
 *  more details.
 */

#include <stdlib.h>
#include <string.h>

#include "types.h"
#include "color.h"
#include "colormap.h"
#include "fb.h"


    /*
     *  Clear the colormap
     */

#define CLEAR_COMPONENT(name)					\
    do {							\
	memset(fb_cmap.name, 0, fb_cmap.len*sizeof(u16));	\
    } while (0)

void cmap_clear(void)
{
    CLEAR_COMPONENT(red);
    CLEAR_COMPONENT(green);
    CLEAR_COMPONENT(blue);
    if (fb_cmap.transp)
	CLEAR_COMPONENT(transp);
}

#undef CLEAR_COMPONENT


    /*
     *  Initialization
     */

#define ALLOC_COMPONENT(name, len)		\
    do {					\
	fb_cmap.name = malloc(len*sizeof(u16));	\
    } while (0)

void cmap_init(u32 len)
{
    ALLOC_COMPONENT(red, len);
    ALLOC_COMPONENT(green, len);
    ALLOC_COMPONENT(blue, len);
    ALLOC_COMPONENT(transp, len);
    fb_cmap.start = 0;
    fb_cmap.len = len;
}

#undef ALLOC_COMPONENT


    /*
     *  Clean up
     */

#define FREE_COMPONENT(name)	\
    do {			\
	free(fb_cmap.name);	\
	fb_cmap.name = NULL;	\
    } while (0)

void cmap_cleanup(void)
{
    FREE_COMPONENT(red);
    FREE_COMPONENT(green);
    FREE_COMPONENT(blue);
    FREE_COMPONENT(transp);
    fb_cmap.start = 0;
    fb_cmap.len = 0;
}

#undef FREE_COMPONENT

