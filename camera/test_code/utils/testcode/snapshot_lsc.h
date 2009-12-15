/* =============================================================================
*             Texas Instruments OMAP(TM) Platform Software
*  (c) Copyright Texas Instruments, Incorporated.  All Rights Reserved.
*
*  Use of this software is controlled by the terms and conditions found
*  in the license agreement under which this software has been supplied.
* ============================================================================*/

#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

#ifndef _SNAPSHOT_H_
#define _SNAPSHOT_H_

int  lsc_init_table(void);
int  lsc_update_table(int cfd);
void lsc_cleanup(void);

#endif



