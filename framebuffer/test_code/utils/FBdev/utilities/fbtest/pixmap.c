
/*
 *  Pixmap manipulation
 *
 *  (C) Copyright 2001-2003 Geert Uytterhoeven
 *
 *  This file is subject to the terms and conditions of the GNU General Public
 *  License. See the file COPYING in the main directory of this archive for
 *  more details.
 */

#include <stdlib.h>

#include "types.h"
#include "clut.h"
#include "image.h"
#include "pixmap.h"
#include "visual.h"
#include "visops.h"
#include "util.h"


static void image_bw_to_pixmap(const struct image *image, pixel_t *pixmap);
static void image_lut256_to_pixmap(const struct image *image, pixel_t *pixmap);
static void image_rgb888_to_pixmap(const struct image *image, pixel_t *pixmap);


    /*
     *  Convert an image to a pixmap
     */

pixel_t *create_pixmap(const struct image *image)
{
    pixel_t *pixmap;

    pixmap = malloc(image->width*image->height*sizeof(pixel_t));
    if (!pixmap)
	Fatal("Not enough memory\n");
    switch (image->type) {
	case IMAGE_BW:
	    image_bw_to_pixmap(image, pixmap);
	    break;

	case IMAGE_GREY256:
	case IMAGE_CLUT256:
	    image_lut256_to_pixmap(image, pixmap);
	    break;

	case IMAGE_RGB888:
	    image_rgb888_to_pixmap(image, pixmap);
	    break;

	default:
	    Fatal("Unknown image type %d\n", image->type);
	    break;
    }
    return pixmap;
}


    /*
     *  Convert a black-and-white image to a pixmap
     */

static const unsigned char bitmask[8] = {
    0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01
};

static void image_bw_to_pixmap(const struct image *image, pixel_t *pixmap)
{
    pixel_t black, white;
    const unsigned char *src;
    pixel_t *dst;
    int i, j, k;

    black = match_color(&clut_mono[0]);
    white = match_color(&clut_mono[1]);

    src = image->data;
    dst = pixmap;
    for (i = 0; i < image->height; i++) {
	for (j = 0, k = 0; j < image->width; j++, k = (k+1) & 7) {
	    *dst++ = (*src & bitmask[k]) ? white : black;
	    if (k == 7)
		src++;
	}
	if (k)
	    src++;
    }
}


    /*
     *  Convert a GREY256/CLUT256 image to a pixmap
     */

static pixel_t lut[256];

static void image_lut256_to_pixmap(const struct image *image, pixel_t *pixmap)
{
    rgba_t color;
    const unsigned char *src;
    pixel_t *dst;
    int i;

    color.a = 65535;
    if (image->type == IMAGE_GREY256) {
	for (i = 0; i < 256; i++) {
	    color.r = color.g = color.b = EXPAND_TO_16BIT(i, 255);
	    lut[i] = match_color(&color);
	}
    } else {
	src = image->clut;
	for (i = 0; i < image->clut_len; i++) {
	    color.r = EXPAND_TO_16BIT(*src++, 255);
	    color.g = EXPAND_TO_16BIT(*src++, 255);
	    color.b = EXPAND_TO_16BIT(*src++, 255);
	    lut[i] = match_color(&color);
	}
    }

    src = image->data;
    dst = pixmap;
    for (i = 0; i < image->width*image->height; i++)
	*dst++ = lut[*src++];
}


    /*
     *  Convert an RGB888 image to a pixmap
     */

static void image_rgb888_to_pixmap(const struct image *image, pixel_t *pixmap)
{
    const unsigned char *src;
    pixel_t *dst;
    rgba_t color;
    int i;

    src = image->data;
    dst = pixmap;
    color.a = 65535;
    for (i = 0; i < image->width*image->height; i++) {
	color.r = EXPAND_TO_16BIT(*src++, 255);
	color.g = EXPAND_TO_16BIT(*src++, 255);
	color.b = EXPAND_TO_16BIT(*src++, 255);
	*dst++ = match_color(&color);
    }
}

