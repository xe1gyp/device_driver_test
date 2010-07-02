/**
 * Copyright (C) 2010 MM-SOL ltd. All Rights Reserved.
 *
 * File operations.
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
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

#include "types.h"
#include "file_operations.h"

/* Load file */
long Load_Image(const char *fname, char *buffer, size_t fSize)
{
	size_t size;
	if (fname == NULL && strlen(fname) < 1) {
		fprintf(stderr, "\nInvalid input file name !");
		return -1;
	}
	if (buffer == NULL && fSize < 1) {
		fprintf(stderr, "\nInvalid buffer or size !");
		return -1;
	}

	FILE *fhdl = fopen(fname, "r");
	if (fhdl == NULL) {
		fprintf(stderr, "\nOpen error !");
		return -1;
	} else {
		/* seek to end of file */
		if (fseek(fhdl, 0L, SEEK_END) != 0L) {
			fprintf(stderr, "\nSeek error !");
			return -1;
		}
		/* get file size */
		size = min((long)fSize, ftell(fhdl));
		/* seek to begin of file */
		fseek(fhdl, 0L, SEEK_SET);
		/* load source image */
		if (fread(buffer, 1, size, fhdl) == 0) {
			fprintf(stderr, "\nRead error !");
			return -1;
		}
	}
	fclose(fhdl);

	return 0;
}

/* Save file */
long Save_Image(const char *fname, char *buffer, size_t fSize)
{
	if (fname == NULL && strlen(fname) < 1) {
		fprintf(stderr, "\nInvalid output file name !");
		return -1;
	}

	FILE *fhdl = fopen(fname, "w+");
	if (fhdl == NULL) {
		printf("\n\rOpen error !");
		return -1;
	} else {
		/* save destination image */
		if (fwrite(buffer, 1, fSize, fhdl) != fSize) {
			printf("\nWrite error !");
			return -1;
		}
		fprintf(stderr, "\nImage Saved Len: %d\n\r", fSize);
	}
	fclose(fhdl);

	return 0;
}
