
/*
 *  Utility definitions and routines
 *
 *  (C) Copyright 2001-2003 Geert Uytterhoeven
 *
 *  This file is subject to the terms and conditions of the GNU General Public
 *  License. See the file COPYING in the main directory of this archive for
 *  more details.
 */


    /*
     *  ANSI text control
     */

#define TXT_BLACK	"\e[30m"
#define TXT_RED		"\e[31m"
#define TXT_GREEN	"\e[32m"
#define TXT_YELLOW	"\e[33m"
#define TXT_BLUE	"\e[33m"
#define TXT_PURPLE	"\e[35m"
#define TXT_CYAN	"\e[36m"
#define TXT_WHITE	"\e[37m"

#define TXT_NORMAL	"\e[0m"
#define TXT_BOLD	"\e[1m"
#define TXT_UNDERLINE	"\e[4m"
#define TXT_INVERSE	"\e[7m"


    /*
     *  Messages
     */

extern void Message(const char *fmt, ...)
    __attribute__ ((format (printf, 1, 2)));
extern void Warning(const char *fmt, ...)
    __attribute__ ((format (printf, 1, 2)));
extern void Error(const char *fmt, ...)
    __attribute__ ((format (printf, 1, 2)));
extern void Fatal(const char *fmt, ...)
    __attribute__ ((noreturn)) __attribute ((format (printf, 1, 2)));
extern void Debug(const char *fmt, ...)
    __attribute__ ((format (printf, 1, 2)));


    /*
     *  Minimum and maximum
     */

#undef min
#define min(a, b)		\
    ( {				\
        typeof(a) _a = (a);	\
	typeof(a) _b = (b);	\
	_a < _b ? _a : _b;	\
    } )

#undef max
#define max(a, b)		\
    ( {				\
        typeof(a) _a = (a);	\
	typeof(a) _b = (b);	\
	_a > _b ? _a : _b;	\
    } )


    /*
     *  Wait
     */

extern void wait_for_key(int timeout);
extern void wait_ms(int ms);


    /*
     *  Number range conversion
     */

#define CONVERT_RANGE(n, oldmaxval, newmaxval)	\
    (((n)*(newmaxval)+((oldmaxval)/2))/(oldmaxval))

#define EXPAND_TO_16BIT(n, maxval)	CONVERT_RANGE(n, maxval, 65535)
#define COMPRESS_FROM_16BIT(n, maxval)	CONVERT_RANGE(n, 65535, maxval)


    /*
     *   Command line options
     */

extern const char *Opt_Fbdev;
extern int Opt_Debug;
extern int Opt_List;
extern int Opt_Quiet;
extern int Opt_Verbose;

