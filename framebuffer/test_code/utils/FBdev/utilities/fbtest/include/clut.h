
/*
 *  CLUT manipulation
 *
 *  (C) Copyright 2001 Geert Uytterhoeven
 *
 *  This file is subject to the terms and conditions of the GNU General Public
 *  License. See the file COPYING in the main directory of this archive for
 *  more details.
 */


extern const rgba_t clut_mono[2];
extern const rgba_t clut_console[16];
extern const rgba_t clut_vga[16];
extern const rgba_t clut_windows[4];

extern void clut_create_rgbcube(rgba_t *clut, u32 rlen, u32 glen, u32 blen);
extern void clut_create_linear(rgba_t *clut, u32 len);
extern void clut_init_nice(void);

