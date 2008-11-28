
/*
 *  Frame buffer device test suite
 *
 *  (C) Copyright 2001-2003 Geert Uytterhoeven
 *
 *  This file is subject to the terms and conditions of the GNU General Public
 *  License. See the file COPYING in the main directory of this archive for
 *  more details.
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "types.h"
#include "util.h"
#include "fb.h"
#include "drawops.h"
#include "visual.h"
#include "visops.h"
#include "test.h"

#define DEFAULT_FBDEV	"/dev/fb0"


const char *ProgramName;

const char *Opt_Fbdev = DEFAULT_FBDEV;
int Opt_Debug = 0;
int Opt_List = 0;
int Opt_Quiet = 0;
int Opt_Verbose = 0;


    /*
     *  Print the usage template
     */

static void Usage(void) __attribute__ ((noreturn));

static void Usage(void)
{
    printf("%s: [options] [test ...]\n\n"
	   "Valid options are:\n"
	   "    -h, --help       Display this usage information\n"
	   "    -f, --fbdev dev  Specify frame buffer device (default: %s)\n"
	   "    -d, --debug      Enable debug mode\n"
	   "    -l, --list       List tests only, don't run them\n"
	   "    -q, --quiet      Suppress messages\n"
	   "    -v, --verbose    Enable verbose mode\n"
	   "\n",
	   ProgramName, DEFAULT_FBDEV);
    exit(1);
}


    /*
     *  Signal handler
     */

static void SigHandler(int signo)
{
    signal(signo, SIG_IGN);
    Fatal("Caught signal %d. Exiting\n", signo);
}


    /*
     *  Main routine
     */

int main(int argc, char *argv[])
{
    ProgramName = argv[0];

    while (argc > 1 && argv[1][0] == '-') {
	if (!strcmp(argv[1], "-h") || !strcmp(argv[1], "--help"))
	    Usage();
	else if (!strcmp(argv[1], "-f") || !strcmp(argv[1], "--fbdev")) {
	    if (argc <= 2)
		Usage();
	    else {
		Opt_Fbdev = argv[2];
		argv += 2;
		argc -= 2;
	    }
	} else if (!strcmp(argv[1], "-d") || !strcmp(argv[1], "--debug")) {
	    Opt_Debug = 1;
	    argv++;
	    argc--;
	} else if (!strcmp(argv[1], "-l") || !strcmp(argv[1], "--list")) {
	    Opt_List = 1;
	    argv++;
	    argc--;
	} else if(!strcmp(argv[1], "-q") || !strcmp(argv[1], "--quiet")) {
	    Opt_Quiet = 1;
	    argv++;
	    argc--;
	} else if (!strcmp(argv[1], "-v") || !strcmp(argv[1], "--verbose")) {
	    Opt_Verbose = 1;
	    argv++;
	    argc--;
	} else
	    Usage();
    }

    /*
     *  Install signal handlers
     */
    signal(SIGINT, SigHandler);
    signal(SIGQUIT, SigHandler);
    signal(SIGILL, SigHandler);
    signal(SIGFPE, SigHandler);
    signal(SIGSEGV, SigHandler);
    signal(SIGTERM, SigHandler);

    if (Opt_List) {
	if (argc < 2) {
	    Message("Listing all tests\n");
	    test_list(NULL, Opt_Verbose);
	} else
	    while (argc > 1) {
		test_list(argv[1], Opt_Verbose);
		argc--;
		argv++;
	    }
    } else {
	fb_init();
        drawops_init();
	visops_init();

	if (argc < 2) {
	    Message("Running all tests\n");
	    test_run(NULL);
	} else
	    while (argc > 1) {
		test_run(argv[1]);
		argc--;
		argv++;
	    }
	fb_cleanup();
    }
    exit(0);
}

