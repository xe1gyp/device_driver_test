
/*
 *  Simple text console
 *
 *  (C) Copyright 2002 Geert Uytterhoeven
 *
 *  This file is subject to the terms and conditions of the GNU General Public
 *  License. See the file COPYING in the main directory of this archive for
 *  more details.
 */


extern void con_init(const struct font *font);
extern void con_putc(char c);
extern void con_puts(const char *s);
extern void con_printf(const char *fmt, ...)
    __attribute__ ((format (printf, 1, 2)));

