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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/param.h>

#include "cmd_options.h"

int parse_prepare(int argc, char *argv[], struct options *array, unsigned count)
{
	int ai;
	unsigned oi;
	char *pos_ptr;

	for (ai = 1; ai < argc; ai++) {
		if (argv[ai] != NULL) {
			if (strchr(argv[ai], '-') != NULL) {
				for (oi = 0; oi < count; oi++) {
					if (strcmp(argv[ai],
						   array[oi].o_symb) == 0) {
						array[oi].entered = 1;
						switch (array[oi].o_type) {
						case e_BYTE:
						array[oi].o_dflt.v_byte =
							atoi(argv[ai+1]);
							break;
						case e_WORD:
						array[oi].o_dflt.v_word =
							atoi(argv[ai+1]);
							break;
						case e_LONG:
						array[oi].o_dflt.v_long =
							atoi(argv[ai]);
							break;
						case e_CHAR:
						array[oi].o_dflt.v_char =
							argv[ai+1];
							break;
						}
					}
				}
			}
		}
	}
	return 0;
}

void parse_release(struct options *array, unsigned count)
{
	unsigned oi;
	for (oi = 0; oi < count; oi++) {
		if (array[oi].o_dflt.v_char == NULL)
			free(array[oi].o_dflt.v_char);
	}
}

int get_option_index(char *option, struct options *array, unsigned count)
{
	unsigned oi;
	for (oi = 0; oi < count; oi++) {
		if (strcmp(option, array[oi].o_symb) == 0)
			return oi;
	}
	return -1;
}

void print_options(struct options *array, unsigned count)
{
	unsigned oi;
	for (oi = 0; oi < count; oi++) {
		switch (array[oi].o_type) {
		case e_BYTE:
			break;
		case e_WORD:
			break;
		case e_LONG:
			break;
		case e_CHAR:
			break;
		}
	}
	fputc('\n', stderr);
}
