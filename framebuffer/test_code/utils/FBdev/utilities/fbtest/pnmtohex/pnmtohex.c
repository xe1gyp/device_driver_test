
/*
 *  Convert a PNM image to a hexadecimal stream suitable for inclusion in
 *  C-source
 *
 *  (C) Copyright 2001-2002 Geert Uytterhoeven
 *
 *  This file is subject to the terms and conditions of the GNU General Public
 *  License. See the file COPYING in the main directory of this archive for
 *  more details.
 */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pnm.h>

#include "image.h"


static pixel clut[256];

static void normalize_ppm(xel **pnm, int cols, int rows, int maxval)
{
    int i, j;

    for (i = 0 ; i < rows; i++)
	for (j = 0; j < cols; j++)
	    PPM_DEPTH(pnm[i][j], pnm[i][j], maxval, 255);
}

static int fill_clut(xel **pnm, int cols, int rows)
{
    int clut_len = 0;
    int i, j, k;

    for (i = 0 ; i < rows; i++)
	for (j = 0; j < cols; j++) {
	    for (k = 0; k < clut_len; k++)
		if (PPM_EQUAL(pnm[i][j], clut[k]))
		    break;
	    if (k == clut_len) {
		if (clut_len == 256)
		    return 257;
		PPM_ASSIGN(clut[clut_len], PPM_GETR(pnm[i][j]),
			   PPM_GETG(pnm[i][j]), PPM_GETB(pnm[i][j]));
		clut_len++;
	    }
	}
    return clut_len;
}

static void print_clut256_data(xel **pnm, int cols, int rows, int clut_len)
{
    int i, j, k, l;

    for (i = 0, k = 0; i < rows; i++)
	for (j = 0; j < cols; j++, k = (k+1) % 12) {
	    if (k == 0)
		printf("   ");
	    for (l = 0; l < clut_len; l++)
		if (PPM_EQUAL(pnm[i][j], clut[l]))
		    break;
	    printf(" 0x%02x,", l);
	    if (k == 11)
		putchar('\n');
	}
    if (k != 0)
	putchar('\n');
}

static void print_rgb888_data(xel **pnm, int cols, int rows)
{
    int i, j, k;

    for (i = 0, k = 0; i < rows; i++)
	for (j = 0; j < cols; j++, k = (k+1) % 4) {
	    if (k == 0)
		printf("   ");
	    printf(" 0x%02x, 0x%02x, 0x%02x,", PPM_GETR(pnm[i][j]),
		   PPM_GETG(pnm[i][j]), PPM_GETB(pnm[i][j]));
	    if (k == 3)
		putchar('\n');
	}
    if (k != 0)
	putchar('\n');
}

static void print_grey256_data(xel **pnm, int cols, int rows, int maxval)
{
    int i, j, k;

    for (i = 0, k = 0; i < rows; i++)
	for (j = 0; j < cols; j++, k = (k+1) % 12) {
	    if (k == 0)
		printf("   ");
	    printf(" 0x%02x,", PNM_GET1(pnm[i][j])*(255+maxval/2)/maxval);
	    if (k == 11)
		putchar('\n');
	}
    if (k != 0)
	putchar('\n');
}

static const unsigned char bitmask[8] = {
    0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01
};

static void print_bw_data(xel **pnm, int cols, int rows)
{
    int i, j, k, l;
    unsigned char data;

    for (i = 0, k = 0, l = 0, data = 0; i < rows; i++) {
	for (j = 0; j < cols; j++, l = (l+1) % 8) {
	    if (PNM_GET1(pnm[i][j]))
		data |= bitmask[l];
	    if (l == 7) {
		if (k == 0)
		    printf("   ");
		printf(" 0x%02x, ", data);
		if (k == 11)
		    putchar('\n');
		k = (k+1) % 12;
		data = 0;
	    }
	}
	if (l != 0) {
	    if (k == 0)
		printf("   ");
	    printf(" 0x%02x, ", data);
	    if (k == 11)
		putchar('\n');
	    k = (k+1) % 12;
	    data = 0;
	}
    }
    if (k != 0)
	putchar('\n');
}

static void print_image_clut(int clut_len)
{
    int i, j;

    for (i = 0, j = 0; i < clut_len; i++, j = (j+1) % 4) {
	if (j == 0)
	    printf("   ");
	printf(" 0x%02x, 0x%02x, 0x%02x,", PPM_GETR(clut[i]),
	       PPM_GETG(clut[i]), PPM_GETB(clut[i]));
	if (j == 3)
	    putchar('\n');
    }
    if (j != 0)
	putchar('\n');
}

static void convert_image(const char *filename, const char *name)
{
    FILE *fp;
    xel **pnm;
    xelval maxval;
    int cols, rows, fmt, clut_len = 0, size;
    const char *type;

    // Load the image
    if (!strcmp(filename, "-"))
	fp = stdin;
    else {
	fp = fopen(filename, "r");
	if (!fp) {
	    fprintf(stderr, "Cannot open file %s: %s\n", filename,
		    strerror(errno));
	    exit(1);
	}
    }
    pnm = pnm_readpnm(fp, &cols, &rows, &maxval, &fmt);
    if (!pnm) {
	fprintf(stderr, "Error reading PNM file: %s\n", strerror(errno));
	exit(1);
    }

    switch (PNM_FORMAT_TYPE(fmt)) {
	case PPM_TYPE:
	    normalize_ppm(pnm, cols, rows, maxval);
	    clut_len = fill_clut(pnm, cols, rows);
	    if (clut_len > 0 && clut_len <= 256) {
		type = "CLUT256";
		size = cols*rows;
	    } else {
		type = "RGB888";
		size = cols*rows*3;
	    }
	    break;

	case PGM_TYPE:
	    type = "GREY256";
	    size = cols*rows;
	    break;

	case PBM_TYPE:
	    type = "BW";
	    size = (cols+7)/8*rows;
	    break;

	default:
	    fprintf(stderr, "Unknown PNM format %d\n", PNM_FORMAT_TYPE(fmt));
	    exit(1);
    }

    // Print header
    printf("    /*\n");
    printf("     *  Image %s\n", name);
    printf("     */\n\n");
    printf("#include \"image.h\"\n\n");

    // Print forward declarations
    printf("static const unsigned char %s_data[];\n", name);
    if (clut_len > 0 && clut_len <= 256)
	printf("static const unsigned char %s_clut[];\n", name);
    printf("\n");

    // Print image structure
    printf("const struct image %s = {\n", name);
    printf("    width:\t%d,\n", cols);
    printf("    height:\t%d,\n", rows);
    printf("    type:\tIMAGE_%s,\n", type);
    printf("    data:\t%s_data,\n", name);
    if (clut_len > 0 && clut_len <= 256) {
	printf("    clut_len:\t%d,\n", clut_len);
	printf("    clut:\t%s_clut\n", name);
    }
    printf("};\n\n");

    // Print image data
    printf("static const unsigned char %s_data[%d] = {\n", name, size);
    switch (PNM_FORMAT_TYPE(fmt)) {
	case PPM_TYPE:
	    if (clut_len > 0 && clut_len <= 256)
		print_clut256_data(pnm, cols, rows, clut_len);
	    else
		print_rgb888_data(pnm, cols, rows);
	    break;

	case PGM_TYPE:
	    print_grey256_data(pnm, cols, rows, maxval);
	    break;

	case PBM_TYPE:
	    print_bw_data(pnm, cols, rows);
	    break;
    }
    printf("};\n\n");

    // Print image clut
    if (clut_len > 0 && clut_len <= 256) {
	printf("static const unsigned char %s_clut[%d] = {\n", name,
	       clut_len*3);
	print_image_clut(clut_len);
	printf("};\n\n");
    }

    // Free temporary data
    ppm_freearray(pnm, rows);
}


int main(int argc, char *argv[])
{
    pnm_init(&argc, argv);

    if (argc < 3 || !(argc % 2)) {
	fprintf(stderr, "Usage: %s <filename> <name> ...\n", argv[0]);
	exit(1);
    }

    while (argc >= 2) {
	convert_image(argv[1], argv[2]);
	argv += 2;
	argc -= 2;
    }

    exit(0);
}

