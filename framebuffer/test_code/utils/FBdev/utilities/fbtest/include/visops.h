
/*
 *  Visual operations
 *
 *  These operations control the interpretation of pixel values
 *
 *  (C) Copyright 2001-2002 Geert Uytterhoeven
 *
 *  This file is subject to the terms and conditions of the GNU General Public
 *  License. See the file COPYING in the main directory of this archive for
 *  more details.
 */


    /*
     *  Visual operations
     */

struct visops {
    const char *name;
    int (*init)(void);
    void (*cleanup)(void);
    int (*set_visual)(enum visual_id id);
    /* For PseudoColor and DirectColor */
    void (*update_cmap)(void);
    /* Generic mode */
    pixel_t (*match_color)(const rgba_t *color, rgba_t *error);
};

extern struct visops visops;


    /*
     *  Fbdev visuals
     */

extern const struct visops mono_visops;
extern const struct visops grayscale_visops;
extern const struct visops ham_visops;
extern const struct visops pseudocolor_visops;
extern const struct visops truecolor_visops;
extern const struct visops directcolor_visops;


    /*
     *  Initialization
     */

extern void visops_init(void);


    /*
     *  Internal routines
     */

extern pixel_t *create_component_table(u32 size, u32 offset, int msb_right,
				       u32 bpp);
extern void pseudocolor_create_tables(u32 bpp);
extern pixel_t pseudocolor_match_color(const rgba_t *color, rgba_t *error);
extern int pseudocolor_set_visual(enum visual_id id);
extern void truecolor_create_tables(void);
extern pixel_t truecolor_match_color(const rgba_t *color, rgba_t *error);

