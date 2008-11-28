
/*
 *  CLUT manipulation
 *
 *  (C) Copyright 2001-2002 Geert Uytterhoeven
 *
 *  This file is subject to the terms and conditions of the GNU General Public
 *  License. See the file COPYING in the main directory of this archive for
 *  more details.
 */

#include <string.h>

#include "types.h"
#include "visual.h"
#include "util.h"


    /*
     *  Monochrome
     */

const rgba_t clut_mono[2] = {
    { 0x0000, 0x0000, 0x0000, 0xffff },
    { 0xffff, 0xffff, 0xffff, 0xffff }
};


    /*
     *  Linux console palette
     */

const rgba_t clut_console[16] = {
    { 0x0000, 0x0000, 0x0000, 0xffff },
    { 0x0000, 0x0000, 0xaaaa, 0xffff },
    { 0x0000, 0xaaaa, 0x0000, 0xffff },
    { 0x0000, 0xaaaa, 0xaaaa, 0xffff },
    { 0xaaaa, 0x0000, 0x0000, 0xffff },
    { 0xaaaa, 0x0000, 0xaaaa, 0xffff },
    { 0xaaaa, 0x5555, 0x0000, 0xffff },
    { 0xaaaa, 0xaaaa, 0xaaaa, 0xffff },
    { 0x5555, 0x5555, 0x5555, 0xffff },
    { 0x5555, 0x5555, 0xffff, 0xffff },
    { 0x5555, 0xffff, 0x5555, 0xffff },
    { 0x5555, 0xffff, 0xffff, 0xffff },
    { 0xffff, 0x5555, 0x5555, 0xffff },
    { 0xffff, 0x5555, 0xffff, 0xffff },
    { 0xffff, 0xffff, 0x5555, 0xffff },
    { 0xffff, 0xffff, 0xffff, 0xffff }
};


    /*
     *  VGA palette
     */

const rgba_t clut_vga[16] = {
    { 0x0000, 0x0000, 0x0000, 0xffff },
    { 0x8080, 0x0000, 0x0000, 0xffff },
    { 0x0000, 0x8080, 0x0000, 0xffff },
    { 0x8080, 0x8080, 0x0000, 0xffff },
    { 0x0000, 0x0000, 0x8080, 0xffff },
    { 0x8080, 0x0000, 0x8080, 0xffff },
    { 0x0000, 0x8080, 0x8080, 0xffff },
    { 0xc0c0, 0xc0c0, 0xc0c0, 0xffff },
    { 0x8080, 0x8080, 0x8080, 0xffff },
    { 0xffff, 0x0000, 0x0000, 0xffff },
    { 0x0000, 0xffff, 0x0000, 0xffff },
    { 0xffff, 0xffff, 0x0000, 0xffff },
    { 0x0000, 0x0000, 0xffff, 0xffff },
    { 0xffff, 0x0000, 0xffff, 0xffff },
    { 0x0000, 0xffff, 0xffff, 0xffff },
    { 0xffff, 0xffff, 0xffff, 0xffff },
};


    /*
     *  4 Extra Windows colors
     */

const rgba_t clut_windows[4] = {
    { 0xc0c0, 0xdcdc, 0xc0c0, 0xffff },
    { 0xa6a6, 0xcaca, 0xf0f0, 0xffff },
    { 0xffff, 0xfbfb, 0xf0f0, 0xffff },
    { 0xa0a0, 0xa0a0, 0xa4a4, 0xffff },
};


    /*
     *  Create a color cube
     */

void clut_create_rgbcube(rgba_t *clut, u32 rlen, u32 glen, u32 blen)
{
    u32 r, g, b;
    u16 red, green, blue;

    Debug("clut_create_rgbcube(): RGB %dx%dx%d\n", rlen, glen, blen);
    for (r = 0; r < rlen; r++) {
	red = EXPAND_TO_16BIT(r, rlen-1);
	for (g = 0; g < glen; g++) {
	    green = EXPAND_TO_16BIT(g, glen-1);
	    for (b = 0; b < blen; b++) {
		blue = EXPAND_TO_16BIT(b, blen-1);
		clut->r = red;
		clut->g = green;
		clut->b = blue;
		clut->a = 0xffff;
		clut++;
	    }
	}
    }
}


    /*
     *  Create a linear ramp
     */

void clut_create_linear(rgba_t *clut, u32 len)
{
    u32 i;
    u16 gray;

    Debug("clut_create_linear(): GRAY %d\n", len);
    for (i = 0; i < len; i++) {
	gray = EXPAND_TO_16BIT(i, len-1);
	clut->r = gray;
	clut->g = gray;
	clut->b = gray;
	clut->a = 0xffff;
	clut++;
    }
}


    /*
     *  Create a `useful' palette
     */

void clut_init_nice(void)
{
    if (idx_len >= 512) {
	clut_create_rgbcube(clut, 8, 8, 8);
    } else if (idx_len >= 256) {
	memcpy(clut, clut_console, sizeof(clut_console));
	/* FIXME: skip colors that are already present */
	clut_create_rgbcube(clut+16, 6, 6, 6);
	/* FIXME: skip colors that are already present */
	clut_create_linear(clut+232, 24);
    } else if (idx_len >= 128) {
	memcpy(clut, clut_console, sizeof(clut_console));
	/* FIXME: skip colors that are already present */
	clut_create_rgbcube(clut+16, 4, 4, 4);
	/* FIXME: skip colors that are already present */
	clut_create_linear(clut+80, 48);
    } else if (idx_len >= 64) {
	clut_create_rgbcube(clut, 4, 4, 4);
    } else if (idx_len >= 32) {
	clut_create_rgbcube(clut, 3, 3, 3);
	/* FIXME: still 3 entries left */
    } else if (idx_len >= 16) {
	memcpy(clut, clut_console, sizeof(clut_console));
    } else if (idx_len >= 8) {
	memcpy(clut, clut_console, 8*sizeof(rgba_t));
    } else if (idx_len >= 4) {
	clut_create_linear(clut, 4);
    } else if (idx_len >= 2) {
	memcpy(clut, clut_mono, sizeof(clut_mono));
    }

    clut_update();
}
