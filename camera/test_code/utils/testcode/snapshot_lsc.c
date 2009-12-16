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
#include <linux/ioctl.h>
#include <sys/mman.h>
#include <linux/videodev2.h>
#include <linux/errno.h>
#include <errno.h>
#include <string.h>
#include <sys/time.h>
#include <mach/isp_user.h>

#define MAX_IMAGE_WIDTH		3280
#define MAX_IMAGE_HEIGHT	2464
#define LSC_GAIN_MODE_M		6
#define LSC_GAIN_MODE_N		6

static __u8 *lsc_tbl;
static int lsc_tbl_size;		/* Table size in bytes */
static int lsc_tbl_width;		/* Width of table in paxels */
static int lsc_tbl_height;		/* Height of table in paxels */


/** lsc_cleanup - Free memory used for LSC table
 *
 * Return: void
 **/
void lsc_cleanup(void)
{
	if (lsc_tbl)
		free(lsc_tbl);
	lsc_tbl = NULL;
	lsc_tbl_size = 0;
	return;
}

/** lsc_init_table - Allocate & build LSC table
 *
 * Return: 0 - Success,  -1 - Memory alloc failed.
 **/
int lsc_init_table(void)
{
	int x, y;
	int paxel_width, paxel_height;
	__u8 *ptr;
	__u8 gain;

	if (lsc_tbl)
		lsc_cleanup();

	/* Calculate LSC table width and height.
	 * Table is calculated for a maximum 8MP image.
	 * e.g For width:
	 *    (3280+64-1)/64 = 52.23 (round up to 53)
	 */
	paxel_width  = 1 << LSC_GAIN_MODE_M;
	paxel_height = 1 << LSC_GAIN_MODE_N;
	lsc_tbl_width  = ((MAX_IMAGE_WIDTH + paxel_width - 1)
			/ paxel_width) + 1;
	lsc_tbl_height = ((MAX_IMAGE_HEIGHT + paxel_height - 1)
			/ paxel_height) + 1;
	lsc_tbl_size = lsc_tbl_width * lsc_tbl_height * 4;

	printf("LSC paxel width:  %i\n", paxel_width);
	printf("LSC paxel height: %i\n", paxel_height);
	printf("LSC table width in paxels:  %i\n", lsc_tbl_width);
	printf("LSC table height in paxels: %i\n", lsc_tbl_height);

	lsc_tbl = malloc(lsc_tbl_size);
	if (!lsc_tbl) {
		perror("Failed to allocate LSC table!\n");
		return -1;
	}

	ptr = lsc_tbl;

	/* Create a gain table (grid of 0,1,0):
	 *   0 1 0 1 ..
	 *   1 0 1 0 ..
	 *   0 1 0 1 ..
	 */
	for (y = 0; y < lsc_tbl_height; y++) {
		for (x = 0; x < lsc_tbl_width; x++) {
			/* Setup paxel gain */
			gain = (((y%2)+x)%2) ? 0x00 : 0x40;

			/* Set gain for each color component */
			*(ptr++) = gain;
			*(ptr++) = gain;
			*(ptr++) = gain;
			*(ptr++) = gain;
		}
	}

	printf("LSC table initialized (%d bytes).\n", lsc_tbl_size);
	return 0;
}

/** lsc_update_table - Update driver with current table
 * @cfd: camera file descriptor
 * @enable: enable(1) or disable(0) LSC
 *
 * Return: 0 - Success,  -ve - Ioctl error
 **/
int lsc_update_table(int cfd, int enable)
{
	struct ispccdc_lsc_config lsc_config_t;
	struct ispccdc_update_config ccdc_t;
	int ret = 0;

	memset(&lsc_config_t, 0x00, sizeof(lsc_config_t));
	memset(&ccdc_t, 0x00, sizeof(ccdc_t));

	lsc_config_t.offset = lsc_tbl_width * 4;
	lsc_config_t.gain_mode_n = LSC_GAIN_MODE_N;
	lsc_config_t.gain_mode_m = LSC_GAIN_MODE_M;
	lsc_config_t.gain_format = 4;  /* 2-bit integer|6-bit fraction */
	lsc_config_t.fmtsph = 0;
	lsc_config_t.fmtlnh = 0;
	lsc_config_t.fmtslv = 0;
	lsc_config_t.fmtlnv = 0;
	lsc_config_t.initial_x = 0;
	lsc_config_t.initial_y = 0;
	lsc_config_t.size = lsc_tbl_size;

	ccdc_t.lsc_cfg = &lsc_config_t;
	ccdc_t.lsc = (void *)lsc_tbl;
	ccdc_t.update = ISP_ABS_CCDC_CONFIG_LSC | ISP_ABS_TBL_LSC;
	ccdc_t.flag = (enable) ? ISP_ABS_CCDC_CONFIG_LSC : 0;

	ret = ioctl(cfd, VIDIOC_PRIVATE_ISP_CCDC_CFG, &ccdc_t);
	if (ret < 0)
		perror("Failed to update lsc table\n");

	return ret;
}




