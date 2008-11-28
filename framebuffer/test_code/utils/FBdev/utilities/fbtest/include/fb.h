
/*
 *  (C) Copyright 2001 Geert Uytterhoeven
 *
 *  This file is subject to the terms and conditions of the GNU General Public
 *  License. See the file COPYING in the main directory of this archive for
 *  more details.
 */

#include <linux/fb.h>


    /*
     *  Frame buffer device kernel API
     */

extern struct fb_var_screeninfo fb_var;
extern struct fb_fix_screeninfo fb_fix;
extern struct fb_cmap fb_cmap;

extern void fb_open(void);
extern void fb_close(void);
extern int fb_get_fix(void);
extern int fb_get_var(void);
extern int fb_set_var(void);
extern int fb_get_cmap(void);
extern int fb_set_cmap(void);
extern void fb_map(void);
extern void fb_unmap(void);


    /*
     *  Initialization/Clean up
     */

extern void fb_init(void);
extern void fb_cleanup(void);


    /*
     *  Mapped frame buffer
     */

extern u8 *fb;

