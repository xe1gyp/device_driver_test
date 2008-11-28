
/*
 *  Visual operations
 *
 *  (C) Copyright 2001 Geert Uytterhoeven
 *
 *  This file is subject to the terms and conditions of the GNU General Public
 *  License. See the file COPYING in the main directory of this archive for
 *  more details.
 */

#include "types.h"
#include "visual.h"
#include "visops.h"
#include "util.h"


    /*
     *  Supported fbdev visuals
     */

static const struct visops *all_visops[] = {
    &mono_visops,
    &grayscale_visops,
    &ham_visops,
    &pseudocolor_visops,
    &truecolor_visops,
    &directcolor_visops,
    NULL
};


    /*
     *  Current fbdev visual
     */

struct visops visops;


    /*
     *  Initialization
     */

void visops_init(void)
{
    int i;

    for (i = 0; all_visops[i]; i++)
	if (all_visops[i]->init()) {
	    visops = *all_visops[i];
	    Message("Using visops %s\n", visops.name);
	    return;
	}

    Fatal("No visual operations available for this frame buffer mode\n");
}

