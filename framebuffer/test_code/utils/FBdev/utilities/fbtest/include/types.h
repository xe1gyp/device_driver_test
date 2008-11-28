
/*
 *  Type declarations
 *
 *  (C) Copyright 2001-2002 Geert Uytterhoeven
 *
 *  This file is subject to the terms and conditions of the GNU General Public
 *  License. See the file COPYING in the main directory of this archive for
 *  more details.
 */


    /*
     *  Fixed size quantities
     */

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;

#if defined(__LP64__) || defined(__alpha__) || defined(__ia64__) || \
    defined(__mips64__) || defined(__powerpc64__) || defined(__s390x__) || \
    defined(__sparc64__) || defined(__x86_64__)
#define BITS_PER_LONG	64
#define BYTES_PER_LONG	8
#define SHIFT_PER_LONG	6
#elif defined(__LP32__) || defined(__arm__) || defined(__cris__) || \
      defined(__i386__) || defined(__mc68000__) || defined(__mips__) || \
      defined(__parisc__) || defined(__powerpc__) || defined(__s390__) || \
      defined(__sh__) || defined(__sparc__)
#define BITS_PER_LONG	32
#define BYTES_PER_LONG	4
#define SHIFT_PER_LONG	5
#else
#error Please add your architecture
#endif


    /*
     *  Pixel value (dependent on the visual)
     */

typedef u32 pixel_t;


    /*
     *  RGBA color quartet
     */

typedef struct {
    int r;
    int g;
    int b;
    int a;
} rgba_t;


    /*
     *  YUVA color quartet
     */

typedef struct {
    int y;
    int u;
    int v;
    int a;
} yuva_t;


    /*
     *  NULL pointer
     */

#define NULL	((void *)0)

