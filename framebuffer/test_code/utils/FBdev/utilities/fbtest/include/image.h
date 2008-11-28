
/*
 *  Image manipulation
 *
 *  (C) Copyright 2001-2003 Geert Uytterhoeven
 *
 *  This file is subject to the terms and conditions of the GNU General Public
 *  License. See the file COPYING in the main directory of this archive for
 *  more details.
 */


    /*
     *   Image
     */

enum image_type { IMAGE_BW, IMAGE_GREY256, IMAGE_CLUT256, IMAGE_RGB888 };

struct image {
    unsigned int width, height;
    enum image_type type;
    const unsigned char *data;	/* pixel data stream */
    /* IMAGE_CLUT256 only */
    unsigned int clut_len;	/* number of CLUT elements (max. 256) */
    const unsigned char *clut;	/* CLUT RGB stream */
};


    /*
     *  Builtin images
     */

extern const struct image penguin;

