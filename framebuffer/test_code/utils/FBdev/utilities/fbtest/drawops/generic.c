/*
 *  Generic drawing operations
 *
 *  (C) Copyright 2001-2002 Geert Uytterhoeven
 *
 *  This file is subject to the terms and conditions of the GNU General Public
 *  License. See the file COPYING in the main directory of this archive for
 *  more details.
 */

#include "types.h"
#include "drawops.h"
#include "fb.h"
#include "util.h"


    /*
     *  Draw a horizontal line
     */

void generic_draw_hline(u32 x, u32 y, u32 length, pixel_t pixel)
{
    while (length--)
	set_pixel(x++, y, pixel);
}


    /*
     *  Draw a vertical line
     */

void generic_draw_vline(u32 x, u32 y, u32 length, pixel_t pixel)
{
    while (length--)
	set_pixel(x, y++, pixel);
}


    /*
     *  Draw a rectangle
     */

void generic_draw_rect(u32 x, u32 y, u32 width, u32 height, pixel_t pixel)
{
    draw_hline(x, y, width, pixel);
    if (height >= 1) {
	if (height >= 2) {
	    draw_vline(x, y+1, height-2, pixel);
	    if (width >= 1)
		draw_vline(x+width-1, y+1, height-2, pixel);
	}
	draw_hline(x, y+height-1, width, pixel);
    }
}


    /*
     *  Draw a filled rectangle
     */

void generic_fill_rect(u32 x, u32 y, u32 width, u32 height, pixel_t pixel)
{
    while (height--)
	draw_hline(x, y++, width, pixel);
}


    /*
     *  Draw a line using the Bresenham algorithm for lines
     */

void generic_draw_line(u32 x1, u32 y1, u32 x2, u32 y2, pixel_t pixel)
{
    int dx, dy, sx, sy, e;

    dx = x2-x1;
    dy = y2-y1;
    if (dy == 0) {
	if (dx < 0) {
	    dx = -dx;
	    x1 = x2;
	}
	draw_hline(x1, y1, dx+1, pixel);
    } else if (dx == 0) {
	if (dy < 0) {
	    dy = -dy;
	    y1 = y2;
	}
	draw_vline(x1, y1, dy+1, pixel);
    } else {
	if (dy < 0) {
	    dy = -dy;
	    sy = -1;
	} else {
	    sy = 1;
	}
	if (dx < 0) {
	    dx = -dx;
	    sx = -1;
	} else {
	    sx = 1;
	}
	if (dx > dy) {
	    e = -dx/2;
	    set_pixel(x1, y1, pixel);
	    while (x1 != x2) {
		e += dy;
		if (e >= 0) {
		    y1 += sy;
		    e -= dx;
		}
		x1 += sx;
		set_pixel(x1, y1, pixel);
	    }
	} else {
	    e = -dy/2;
	    set_pixel(x1, y1, pixel);
	    while (y1 != y2) {
		e += dx;
		if (e >= 0) {
		    x1 += sx;
		    e -= dy;
		}
		y1 += sy;
		set_pixel(x1, y1, pixel);
	    }
	}
    }
}


    /*
     *  Monochrome bitmap expansion
     */

void generic_expand_bitmap(u32 x, u32 y, u32 width, u32 height, const u8 *data,
			   u32 pitch, pixel_t pixel0, pixel_t pixel1)
{
    u32 w, x0;
    const u8 *data0;
    int i;
    u8 bits;

    while (height--) {
	w = width;
	x0 = x;
	data0 = data;
	while (w > 7) {
	    bits = *data0++;
	    for (i = 0; i < 8; i++, bits <<= 1)
		set_pixel(x0++, y, bits & 0x80 ? pixel1 : pixel0);
	    w -= 8;
	}
	if (w > 0) {
	    bits = *data0++;
	    while (w-- > 0) {
		set_pixel(x0++, y, bits & 0x80 ? pixel1 : pixel0);
		bits <<= 1;
	    }
	}
	y++;
	data += pitch;
    }
}


    /*
     *  Draw a pixmap
     */

void generic_draw_pixmap(u32 x, u32 y, u32 width, u32 height,
			 const pixel_t *pixmap)
{
    u32 w, x0;

    while (height--) {
	for (w = width, x0 = x; w > 0; w--)
	    set_pixel(x0++, y, *pixmap++);
	y++;
    }
}


    /*
     *  Draw a circle using the differential version of the midpoint algorithm
     *
     *  Cfr. Computer Graphics: Principles and Practices, Second Edition in C,
     *  Foley et al., 1997, p. 87
     */

static void draw_circle_points(u32 cx, u32 cy, u32 x, u32 y, pixel_t pixel)
{
    if (x == 0) {
	set_pixel(cx, cy-y, pixel);
	set_pixel(cx-y, cy, pixel);
	set_pixel(cx+y, cy, pixel);
	set_pixel(cx, cy+y, pixel);
    } else {
	set_pixel(cx-x, cy-y, pixel);
	set_pixel(cx+x, cy-y, pixel);
	set_pixel(cx-x, cy+y, pixel);
	set_pixel(cx+x, cy+y, pixel);
	if (x < y) {
	    set_pixel(cx-y, cy-x, pixel);
	    set_pixel(cx+y, cy-x, pixel);
	    set_pixel(cx-y, cy+x, pixel);
	    set_pixel(cx+y, cy+x, pixel);
	}
    }
}

void generic_draw_circle(u32 x, u32 y, u32 r, pixel_t pixel)
{
    u32 x1 = 0;
    u32 y1 = r;
    int d = 1-r;
    int de = 3;
    int dse = -2*r+5;

    do {
	draw_circle_points(x, y, x1, y1, pixel);
	if (d < 0) {	// Select E
	    d += de;
	    de += 2;
	    dse += 2;
	} else {	// Select SE
	    d += dse;
	    de += 2;
	    dse += 4;
	    y1--;
	}
	x1++;
    } while (x1 <= y1);
}


    /*
     *  Draw a filled circle
     */

static void fill_circle_points_x(u32 cx, u32 cy, u32 x, u32 y, pixel_t pixel)
{
    if (x == 0) {
	set_pixel(cx, cy-y, pixel);
	set_pixel(cx, cy+y, pixel);
    } else {
	draw_hline(cx-x, cy-y, 2*x+1, pixel);
	draw_hline(cx-x, cy+y, 2*x+1, pixel);
    }
}

static void fill_circle_points_y(u32 cx, u32 cy, u32 x, u32 y, pixel_t pixel)
{
    if (x == 0)
	draw_hline(cx-y, cy, 2*y+1, pixel);
    else {
	draw_hline(cx-y, cy-x, 2*y+1, pixel);
	draw_hline(cx-y, cy+x, 2*y+1, pixel);
    }
}

void generic_fill_circle(u32 x, u32 y, u32 r, pixel_t pixel)
{
    u32 x1 = 0;
    u32 y1 = r;
    int d = 1-r;
    int de = 3;
    int dse = -2*r+5;

    do {
	fill_circle_points_y(x, y, x1, y1, pixel);
	if (d < 0) {	// Select E
	    d += de;
	    de += 2;
	    dse += 2;
	} else {	// Select SE
	    d += dse;
	    de += 2;
	    dse += 4;
	    if (x1 != y1)
		fill_circle_points_x(x, y, x1, y1, pixel);
	    y1--;
	}
	x1++;
    } while (x1 <= y1);
}


    /*
     *  Draw an ellipse using a differential version of the Bresenham algorithm
     *  for ellipses
     */

static void draw_ellipse_points(u32 cx, u32 cy, u32 x, u32 y, pixel_t pixel)
{
    if (x == 0) {
	set_pixel(cx, cy-y, pixel);
	set_pixel(cx, cy+y, pixel);
    } else if (0 == y) {
	set_pixel(cx-x, cy, pixel);
	set_pixel(cx+x, cy, pixel);
    } else {
	set_pixel(cx-x, cy-y, pixel);
	set_pixel(cx+x, cy-y, pixel);
	set_pixel(cx-x, cy+y, pixel);
	set_pixel(cx+x, cy+y, pixel);
    }
}

void generic_draw_ellipse(u32 x, u32 y, u32 a, u32 b, pixel_t pixel)
{
    if (a == b)
	draw_circle(x, y, a, pixel);
    else {
	u32 a2 = a*a;
	u32 b2 = b*b;
	if (a <= b) {
	    u32 x1 = 0;
	    u32 y1 = b;
	    int S = a2*(1-2*b)+2*b2;
	    int T = b2-2*a2*(2*b-1);
	    int dT1 = 4*b2;
	    int dS1 = dT1+2*b2;
	    int dS2 = -4*a2*(b-1);
	    int dT2 = dS2+2*a2;

	    draw_ellipse_points(x, y, x1, y1, pixel);
	    do {
		if (S < 0) {
		    S += dS1;
		    T += dT1;
		    dS1 += 4*b2;
		    dT1 += 4*b2;
		    x1++;
		} else if (T < 0) {
		    S += dS1+dS2;
		    T += dT1+dT2;
		    dS1 += 4*b2;
		    dT1 += 4*b2;
		    dS2 += 4*a2;
		    dT2 += 4*a2;
		    x1++;
		    y1--;
		} else {
		    S += dS2;
		    T += dT2;
		    dS2 += 4*a2;
		    dT2 += 4*a2;
		    y1--;
		}
		draw_ellipse_points(x, y, x1, y1, pixel);
	    } while (y1 > 0);
	} else {
	    u32 x1 = a;
	    u32 y1 = 0;
            int S = b2*(1-2*a)+2*a2;
            int T = a2-2*b2*(2*a-1);
            int dT1 = 4*a2;
            int dS1 = dT1+2*a2;
            int dS2 = -4*b2*(a-1);
            int dT2 = dS2+2*b2;

	    draw_ellipse_points(x, y, x1, y1, pixel);
	    do {
		if (S < 0) {
		    S += dS1;
		    T += dT1;
		    dS1 += 4*a2;
		    dT1 += 4*a2;
		    y1++;
		} else if (T < 0) {
		    S += dS1+dS2;
		    T += dT1+dT2;
		    dS1 += 4*a2;
		    dT1 += 4*a2;
		    dS2 += 4*b2;
		    dT2 += 4*b2;
		    x1--;
		    y1++;
		} else {
		    S += dS2;
		    T += dT2;
		    dS2 += 4*b2;
		    dT2 += 4*b2;
		    x1--;
		}
		draw_ellipse_points(x, y, x1, y1, pixel);
	    } while (x1 > 0);
	}
    }
}


    /*
     *  Draw a filled ellipse
     */

static void fill_ellipse_points(u32 cx, u32 cy, u32 x, u32 y, pixel_t pixel)
{
    if (x == 0) {
	set_pixel(cx, cy-y, pixel);
	set_pixel(cx, cy+y, pixel);
    } else if (y == 0) {
	draw_hline(cx-x, cy, 2*x+1, pixel);
    } else {
	draw_hline(cx-x, cy-y, 2*x+1, pixel);
	draw_hline(cx-x, cy+y, 2*x+1, pixel);
    }
}

void generic_fill_ellipse(u32 x, u32 y, u32 a, u32 b, pixel_t pixel)
{
    if (a == b)
	fill_circle(x, y, a, pixel);
    else {
	u32 a2 = a*a;
	u32 b2 = b*b;
	if (a <= b) {
	    u32 x1 = 0;
	    u32 y1 = b;
	    int S = a2*(1-2*b)+2*b2;
	    int T = b2-2*a2*(2*b-1);
	    int dT1 = 4*b2;
	    int dS1 = dT1+2*b2;
	    int dS2 = -4*a2*(b-1);
	    int dT2 = dS2+2*a2;

	    while (1) {
		if (S < 0) {
		    S += dS1;
		    T += dT1;
		    dS1 += 4*b2;
		    dT1 += 4*b2;
		    x1++;
		} else if (T < 0) {
		    fill_ellipse_points(x, y, x1, y1, pixel);
		    if (y1 == 0)
			break;
		    S += dS1+dS2;
		    T += dT1+dT2;
		    dS1 += 4*b2;
		    dT1 += 4*b2;
		    dS2 += 4*a2;
		    dT2 += 4*a2;
		    x1++;
		    y1--;
		} else {
		    fill_ellipse_points(x, y, x1, y1, pixel);
		    if (y1 == 0)
			break;
		    S += dS2;
		    T += dT2;
		    dS2 += 4*a2;
		    dT2 += 4*a2;
		    y1--;
		}
	    }
	} else {
	    u32 x1 = a;
	    u32 y1 = 0;
            int S = b2*(1-2*a)+2*a2;
            int T = a2-2*b2*(2*a-1);
            int dT1 = 4*a2;
            int dS1 = dT1+2*a2;
            int dS2 = -4*b2*(a-1);
            int dT2 = dS2+2*b2;

	    fill_ellipse_points(x, y, x1, y1, pixel);
	    do {
		if (S < 0) {
		    S += dS1;
		    T += dT1;
		    dS1 += 4*a2;
		    dT1 += 4*a2;
		    y1++;
		    fill_ellipse_points(x, y, x1, y1, pixel);
		} else if (T < 0) {
		    S += dS1+dS2;
		    T += dT1+dT2;
		    dS1 += 4*a2;
		    dT1 += 4*a2;
		    dS2 += 4*b2;
		    dT2 += 4*b2;
		    x1--;
		    if (x1 < 0)
			break;
		    y1++;
		    fill_ellipse_points(x, y, x1, y1, pixel);
		} else {
		    S += dS2;
		    T += dT2;
		    dS2 += 4*b2;
		    dT2 += 4*b2;
		    x1--;
		}
	    } while (x1 > 0);
	}
    }
}


    /*
     *  Copy a rectangular area
     */

void generic_copy_rect(u32 dx, u32 dy, u32 width, u32 height, u32 sx, u32 sy)
{
    u32 w, dx0, sx0;

    if (dy > sy || (dy == sy && dx > sx)) {
	dx += width;
	dy += height;
	sx += width;
	sy += height;
	while (height--) {
	    dy--;
	    sy--;
	    for (w = width, dx0 = dx, sx0 = sx; w > 0; w--) {
		dx0--;
		sx0--;
		set_pixel(dx0, dy, get_pixel(sx0, sy));
	    }
	}
    } else {
	while (height--) {
	    for (w = width, dx0 = dx, sx0 = sx; w > 0; w--) {
		set_pixel(dx0, dy, get_pixel(sx0, sy));
		dx0++;
		sx0++;
	    }
	    dy++;
	    sy++;
	}
    }
}

