/**
 * Copyright (C) 2010 MM-SOL ltd. All Rights Reserved.
 *
 * Command line options parser.
 *
 * Authors:
 * 	 Atanas Filipov <afilipov@mm-sol.com>
 *
 * Use of this software is controlled by the terms and conditions found
 * in the license agreement under which this software has been supplied.
 */

#ifndef _APP_OMAP3_CMD_OPTIONS_H
#define _APP_OMAP3_CMD_OPTIONS_H

#include "types.h"

/* Command line parameter type */
enum options_e{
	e_BYTE, e_WORD, e_LONG, e_CHAR
};

/* Command line parameter value */
union default_u {
    char  v_byte;
    int   v_word;
    long  v_long;
    char *v_char;
    void *v_null;
};

/* Command line parameter structure */
struct options {
	char		*o_symb; /* Command line options string */
	enum options_e	 o_type; /* Command line options type of parameter */
	union default_u	 o_dflt; /* Command line options default value */
	char		*o_help; /* Command line options description */
	int		entered; /* Command line options if it's entered flag */
};

int parse_prepare(int argc, char *argv[], struct options *array, \
							unsigned count);

void parse_release(struct options *array, unsigned count);

int get_option_index(char *option, struct options *array, unsigned count);

void print_options(struct options *array, unsigned count);

#endif
