
/*
 *  Test routines
 *
 *  (C) Copyright 2001-2002 Geert Uytterhoeven
 *
 *  This file is subject to the terms and conditions of the GNU General Public
 *  License. See the file COPYING in the main directory of this archive for
 *  more details.
 */


enum test_reqflags {
    /* Bits per pixel */
    REQF_bits_per_pixel = (1<<0),
    /* Number of colors */
    REQF_num_colors = (1<<1),
    /* Color component sizes */
    REQF_red_length = (1<<2),
    REQF_green_length = (1<<3),
    REQF_blue_length = (1<<4),
    REQF_transp_length = (1<<5),
    /* Screen dimensions */
    REQF_xres = (1<<6),
    REQF_yres = (1<<7),
};

enum test_res {
    TEST_OK = 0,		/* OK */
    TEST_FAIL = 1,		/* Failed */
    TEST_NA = 2,		/* N/A */
};

struct test {
    const char *name;
    const char *desc;
    enum visual_id visual;
    enum test_reqflags reqs;	/* multiple requirements may be or'ed */
    u32 bits_per_pixel;		/* if (flags & REQF_bits_per_pixel) */
    u32 num_colors;		/* if (flags & REQF_num_colors) */
    u32 red_length;		/* if (flags & REQF_red_length) */
    u32 green_length;		/* if (flags & REQF_green_length) */
    u32 blue_length;		/* if (flags & REQF_blue_length) */
    u32 transp_length;		/* if (flags & REQF_transp_length) */
    u32 xres;			/* if (flags & REQF_xres) */
    u32 yres;			/* if (flags & REQF_yres) */
    enum test_res (*func)(void);
};


    /*
     *  Tests
     */

extern const struct test test001;
extern const struct test test002;
extern const struct test test003;
extern const struct test test004;
extern const struct test test005;
extern const struct test test006;
extern const struct test test007;
extern const struct test test008;
extern const struct test test009;
extern const struct test test010;


    /*
     *  Test run
     */

extern void test_run(const char *name);
extern void test_list(const char *name, int verbose);

