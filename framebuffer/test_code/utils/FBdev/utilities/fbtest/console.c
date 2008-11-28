
/*
 *  Simple text console
 *
 *  (C) Copyright 2002 Geert Uytterhoeven
 *
 *  This file is subject to the terms and conditions of the GNU General Public
 *  License. See the file COPYING in the main directory of this archive for
 *  more details.
 */

#include <stdarg.h>
#include <stdio.h>

#include "types.h"
#include "font.h"
#include "console.h"
#include "fb.h"
#include "drawops.h"
#include "visual.h"
#include "clut.h"
#include "util.h"


#define PRINTF_BUFFER_SIZE	1024
#define BITMAP_SIZE		1024

static char printf_buffer[PRINTF_BUFFER_SIZE];

static const struct font *con_font;
static unsigned int con_pitch, con_charsize;
static unsigned int con_cols, con_rows;
static unsigned int con_x, con_y;
static pixel_t con_fgcolor, con_bgcolor;

static unsigned char bitmap[BITMAP_SIZE];
static unsigned int bitmap_width, bitmap_pitch, bitmap_max_width;
static unsigned int bitmap_x;

static void con_flush(void)
{
    if (bitmap_width) {
	expand_bitmap(bitmap_x*con_font->width, con_y*con_font->height,
		      bitmap_width, con_font->height, (const u8 *)bitmap,
		      bitmap_pitch, con_bgcolor, con_fgcolor);
	bitmap_width = 0;
    }
}

static void con_clear(void)
{
    con_flush();
    fill_rect(0, 0, con_cols*con_font->width, con_rows*con_font->height,
	      con_bgcolor);
}

static void con_reset(void)
{
    con_flush();

    con_x = 0;
    con_y = 0;
    con_fgcolor = idx_pixel[7];
    con_bgcolor = idx_pixel[0];

    con_clear();
}


static void con_scrollup(void)
{
    con_flush();
    copy_rect(0, 0, con_cols*con_font->width, (con_rows-1)*con_font->height,
	      0, con_font->height);
    fill_rect(0, (con_rows-1)*con_font->height, con_cols*con_font->width,
	      con_font->height, con_bgcolor);
}


void con_init(const struct font *font)
{
    int i;

    fill_rect(0, 0, fb_var.xres, fb_var.yres, 0);

    con_font = font;
    con_pitch = (font->width+7)/8;
    con_charsize = con_pitch*font->height;

    con_cols = fb_var.xres/font->width;
    con_rows = fb_var.yres/font->height;

    for (i = 0; i < 16; i++)
	clut[i] = clut_console[i];
    clut_update();

    bitmap_pitch = (sizeof(bitmap)/(font->height*sizeof(unsigned long)))*
		   sizeof(unsigned long);
    bitmap_max_width = bitmap_pitch*8;
    bitmap_width = 0;
    bitmap_x = 0;

    con_reset();
}

static void con_newline(void)
{
    con_flush();
    con_x = 0;
    con_y++;
    if (con_y == con_rows) {
	con_y--;
	con_scrollup();
    }
}

static void con_store_char(unsigned char c)
{
    const unsigned char *src;
    unsigned char *dst;
    int y;

    if (bitmap_width+con_font->width > bitmap_max_width)
	con_flush();
    if (!bitmap_width)
	bitmap_x = con_x;
    src = con_font->data+c*con_charsize;
    switch (con_font->width) {
	case 4:
	    dst = bitmap+bitmap_width/8;
	    if (bitmap_width & 4) {
		for (y = 0; y < con_font->height; y++) {
		    *dst |= *src++ & 0x0f;
		    dst += bitmap_pitch;
		}
	    } else {
		for (y = 0; y < con_font->height; y++) {
		    *dst = *src++ & 0xf0;
		    dst += bitmap_pitch;
		}
	    }
	    break;

	case 8:
	    dst = bitmap+bitmap_width/8;
	    for (y = 0; y < con_font->height; y++) {
		*dst = *src++;
		dst += bitmap_pitch;
	    }
	    break;

	case 12:
	    dst = bitmap+bitmap_width/8;
	    if (bitmap_width & 4) {
		for (y = 0; y < con_font->height; y++) {
		    unsigned char d = *src++;
		    dst[0] |= d >> 4;
		    dst[1] = d << 4 | *src++ >> 4;
		    dst += bitmap_pitch;
		}
	    } else {
		for (y = 0; y < con_font->height; y++) {
		    dst[0] = *src++;
		    dst[1] = *src++ & 0xf0;
		    dst += bitmap_pitch;
		}
	    }
	    break;

	case 16:
	    dst = bitmap+bitmap_width/8;
	    for (y = 0; y < con_font->height; y++) {
		dst[0] = *src++;
		dst[1] = *src++;
		dst += bitmap_pitch;
	    }
	    break;

	default:
	    Fatal("Fontwidth %d is not yet supported\n", con_font->width);
    }
    bitmap_width += con_font->width;
}

static void con_do_putc(unsigned char c)
{
    switch (c) {
	case '\n':
	    con_newline();
	    break;

	default:
	    con_store_char(c);
	    con_x++;
	    if (con_x == con_cols)
		con_newline();
	    break;
    }
}

void con_putc(char c)
{
    con_do_putc(c);
    con_flush();
}

void con_puts(const char *s)
{
    unsigned char c;

    while ((c = *s++))
	con_do_putc(c);
    con_flush();
}


void con_printf(const char *fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    vsnprintf(printf_buffer, sizeof(printf_buffer), fmt, ap);
    va_end(ap);
    con_puts(printf_buffer);
}

