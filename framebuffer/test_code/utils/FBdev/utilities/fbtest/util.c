
/*
 *  Utility routines
 *
 *  (C) Copyright 2001-2003 Geert Uytterhoeven
 *
 *  This file is subject to the terms and conditions of the GNU General Public
 *  License. See the file COPYING in the main directory of this archive for
 *  more details.
 */

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include "types.h"
#include "fb.h"
#include "util.h"


#define TXT_MESSAGE	TXT_GREEN
#define TXT_WARNING	TXT_YELLOW
#define TXT_ERROR	TXT_RED
#define TXT_DEBUG	TXT_BLUE
#define TXT_FATAL	TXT_RED


    /*
     *  Print a prefixed message
     */

static void PrintMessage(const char *prefix, const char *fmt, va_list ap)
{
    fflush(stdout);
    fputs(prefix, stderr);
    vfprintf(stderr, fmt, ap);
    fputs(TXT_NORMAL, stderr);
}


    /*
     *  Print a message
     */

void Message(const char *fmt, ...)
{
    va_list ap;

    if (!Opt_Quiet) {
	va_start(ap, fmt);
	PrintMessage(TXT_MESSAGE, fmt, ap);
	va_end(ap);
    }
}


    /*
     *  Print a warning message
     */

void Warning(const char *fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    PrintMessage(TXT_WARNING, fmt, ap);
    va_end(ap);
}


    /*
     *  Print an error message
     */

void Error(const char *fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    PrintMessage(TXT_ERROR, fmt, ap);
    va_end(ap);
}


    /*
     *  Print a debug message
     */

void Debug(const char *fmt, ...)
{
    va_list ap;

    if (Opt_Debug) {
	va_start(ap, fmt);
	PrintMessage(TXT_DEBUG, fmt, ap);
	va_end(ap);
    }
}


    /*
     *  Print a fatal error message
     */

void Fatal(const char *fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    PrintMessage(TXT_FATAL, fmt, ap);
    va_end(ap);
    fb_cleanup();
    exit(1);
}


    /*
     *  Wait for a keypress
     */

void wait_for_key(int timeout)
{
    /* FIXME: no keypress handling yet */
    sleep(2);
}


    /*
     *  Wait some milliseconds
     */

void wait_ms(int ms)
{
    struct timespec req;

    req.tv_sec = ms/1000;
    req.tv_nsec = (ms % 1000)*1000000;
    nanosleep(&req, NULL);
}

