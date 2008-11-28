
/*
 *  Font manipulation
 *
 *  (C) Copyright 2002 Geert Uytterhoeven
 *
 *  This file is subject to the terms and conditions of the GNU General Public
 *  License. See the file COPYING in the main directory of this archive for
 *  more details.
 */


    /*
     *   Font
     */

struct font {
    unsigned int width, height;
    const unsigned char *data;	/* font data stream */
};


    /*
     *  Builtin fonts
     */

extern const struct font pearl8x8;
extern const struct font sun12x22;
extern const struct font vga8x16;

