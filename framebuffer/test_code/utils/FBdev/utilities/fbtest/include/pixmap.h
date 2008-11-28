
/*
 *  Pixmap manipulation
 *
 *  (C) Copyright 2001 Geert Uytterhoeven
 *
 *  This file is subject to the terms and conditions of the GNU General Public
 *  License. See the file COPYING in the main directory of this archive for
 *  more details.
 */


    /*
     *  Convert an image to a pixmap
     */

extern pixel_t *create_pixmap(const struct image *image);


    /*
     *  Free a pixmap
     */

#define free_pixmap(pixmap)	free(pixmap)

