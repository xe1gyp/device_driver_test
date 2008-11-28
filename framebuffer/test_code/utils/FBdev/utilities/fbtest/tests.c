
/*
 *  Run some tests
 *
 *  (C) Copyright 2001-2003 Geert Uytterhoeven
 *
 *  This file is subject to the terms and conditions of the GNU General Public
 *  License. See the file COPYING in the main directory of this archive for
 *  more details.
 */

#include <stdio.h>
#include <string.h>

#include "types.h"
#include "fb.h"
#include "visual.h"
#include "test.h"
#include "util.h"


static const struct test *all_tests[] = {
    &test001,
    &test002,
    &test003,
    &test004,
    &test005,
    &test006,
    &test007,
    &test008,
    &test009,
    &test010,
    NULL
};


    /*
     *  Run one test
     */

#define TEST_REQ_MIN(reqname, varname)				\
    if (test->reqs & REQF_ ## reqname) {			\
	if (varname < test->reqname) {				\
	    Debug("Requirement " #reqname " >= %d not met\n",	\
		  test->reqname);				\
	    return;						\
	}							\
    }

static void run_one_test(const struct test *test)
{
    enum test_res res;

    Debug("Running test %s\n", test->name);

    if (test->visual != VISUAL_NONE && !visual_set(test->visual)) {
	Debug("Visual %d not supported\n", test->visual);
	return;
    }

    TEST_REQ_MIN(bits_per_pixel, fb_var.bits_per_pixel);
    TEST_REQ_MIN(num_colors, idx_len);
    TEST_REQ_MIN(red_length, fb_var.red.length);
    TEST_REQ_MIN(green_length, fb_var.green.length);
    TEST_REQ_MIN(blue_length, fb_var.blue.length);
    TEST_REQ_MIN(transp_length, fb_var.transp.length);
    TEST_REQ_MIN(xres, fb_var.xres);
    TEST_REQ_MIN(yres, fb_var.yres);

    res = test->func();
    switch (res) {
	case TEST_OK:
	    Message("%s: PASSED\n", test->name);
	    break;

	case TEST_FAIL:
	    Error("%s: FAILED\n", test->name);
	    break;

	case TEST_NA:
	    Debug("Not applicable\n");
	    break;

	default:
	    Fatal("%s returned unknown code %d\n", test->name, res);
	    break;
    }
}

#undef TEST_REQ_MIN


    /*
     *  Test run
     */

void test_run(const char *name)
{
    int i;

    for (i = 0; all_tests[i]; i++)
	if (!name || !strcmp(all_tests[i]->name, name))
	    run_one_test(all_tests[i]);
}


    /*
     *  List all tests
     */

void test_list(const char *name, int verbose)
{
    int i;
    const struct test *test;

    for (i = 0; all_tests[i]; i++)
	if (!name || !strcmp(all_tests[i]->name, name)) {
	    test = all_tests[i];
	    printf("%s: %s\n", test->name, test->desc);
	}
}

