
/*
 *  Visual control
 *
 *  Available visuals
 *
 *  (C) Copyright 2001 Geert Uytterhoeven
 *
 *  This file is subject to the terms and conditions of the GNU General Public
 *  License. See the file COPYING in the main directory of this archive for
 *  more details.
 */


    /*
     *  Supported visuals
     */

enum visual_id {
    VISUAL_NONE = 0,
    VISUAL_GENERIC,
    VISUAL_MONO,
    VISUAL_PSEUDOCOLOR,
    VISUAL_GRAYSCALE,
    VISUAL_TRUECOLOR,
    VISUAL_DIRECTCOLOR
};


    /*
     *  Set the visual
     */

extern int visual_set(enum visual_id id);


    /*
     *  Generic mode
     */

#define match_color(color)	\
    visops.match_color((color), NULL)
#define match_color_error(color, error)	\
    visops.match_color((color), (error)


    /*
     *  Monochrome
     */

extern pixel_t black_pixel, white_pixel;


    /*
     *   Grayscale
     */

extern u32 gray_len, gray_bits;
extern const pixel_t *gray_pixel;


    /*
     *  Pseudocolor CLUT
     */

extern u32 idx_len, idx_bits;
extern const pixel_t *idx_pixel;
extern rgba_t *clut;

extern void clut_update(void);


    /*
     *   RGB(A) for Truecolor
     */

extern u32 red_len, green_len, blue_len, alpha_len;
extern u32 red_bits, green_bits, blue_bits, alpha_bits;
extern const pixel_t *red_pixel, *green_pixel, *blue_pixel, *alpha_pixel;

#define rgba_pixel(r, g, b, a)	\
    (red_pixel[(r)] | green_pixel[(g)] | blue_pixel[(b)] |	\
     (alpha_pixel ? alpha_pixel[(a)] : 0))
#define rgb_pixel(r, g, b)	\
    rgba_pixel((r), (g), (b), alpha_len-1)


    /*
     *   Directcolor
     *
     *   This mode uses a mix from Truecolor and Pseudocolor
     *     - red_len, green_len, blue_len, alpha_len
     *     - red_bits, green_bits, blue_bits, alpha_bits
     *     - red_pixel, green_pixel, blue_pixel, alpha_pixel
     *     - rgb_pixel
     *     - rgba_pixel
     *     - clut
     *     - clut_update
     */

