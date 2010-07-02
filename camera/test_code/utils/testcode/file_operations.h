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

#ifndef _APP_OMAP3_FILE_OPERATIONS_H
#define _APP_OMAP3_FILE_OPERATIONS_H

/* Load file */
long Load_Image(const char *fname, char *buffer, size_t fSize);

/* Save file */
long Save_Image(const char *fname, char *buffer, size_t fSize);

#endif
