/*
 * /vobs/wtbu/OMAPSW_L/linux/test/device_driver_test/dma/test_code/dmatest.h
 *
 * Dma Config Params
 *
 * Copyright (C) 2004-2006 Texas Instruments, Inc.
 *
 * This package is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * THIS PACKAGE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */

#ifndef __DMA_TEST_H
#define __DMA_TEST_H

/********************** KERNEL SPECIFIC STUFF *****************/
#include <linux/delay.h>
#include <asm/uaccess.h>
#include <linux/version.h>

#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,00))
/* Borrowed from 2.6 kernel -include/linux/jiffies.h 2.6.13*/
#define MAX_JIFFY_OFFSET ((~0UL >> 1)-1)

static inline unsigned int jiffies_to_usecs(const unsigned long j)
{
#if HZ <= 1000000 && !(1000000 % HZ)
	return (1000000 / HZ) * j;
#elif HZ > 1000000 && !(HZ % 1000000)
	return (j + (HZ / 1000000) - 1) / (HZ / 1000000);
#else
	return (j * 1000000) / HZ;
#endif
}
static inline unsigned long usecs_to_jiffies(const unsigned int u)
{
	if (u > jiffies_to_usecs(MAX_JIFFY_OFFSET))
		return MAX_JIFFY_OFFSET;
#if HZ <= 1000000 && !(1000000 % HZ)
	return (u + (1000000 / HZ) - 1) / (1000000 / HZ);
#elif HZ > 1000000 && !(HZ % 1000000)
	return u * (HZ / 1000000);
#else
	return (u * HZ + 999999) / 1000000;
#endif
}

#endif				/* kernel specs */

/********************** DEBUG STUFF *****************/
/* Debug Levels 0 - 4 - 0 prints more information and 4 the least */
#define D(VAL,format,...) if (VAL>=debug) printk (KERN_INFO "%s[%d]" format "\n", __FUNCTION__, __LINE__, ## __VA_ARGS__);
#define ERR(format,...) printk (KERN_ERR "ERROR:%s[%d]" format "\n", __FUNCTION__, __LINE__, ## __VA_ARGS__);
#define FN_IN() D(2,"Entry");
#define FN_OUT(RET) D(2,"Exit(%d)",RET);

/********************** GENERAL Data Structures *********/
struct dma_stat_id_s;
struct dma_buf_id_s;
/********************** GENERAL DEFINES *****************/
/* Debug Default debug level */
#define DEFAULT_DEBUG_LVL 5
#define MAX_INIT_STRING   100
#define MAX_CHANNELS      32

/* General Functions */
/* BUFFER LOGICS */
int buf_verify(struct dma_buf_id_s *buf_id, int idx, int size);
int buf_fill(struct dma_buf_id_s *buf_id, int idx, int dma_lch, int size);
int buf_free(struct dma_buf_id_s *buf_id, int channels, int size);
int buf_allocate(int channels, int size, struct dma_buf_id_s **buf_id_copy);
int buf_get_src_dst_phy(struct dma_buf_id_s *buf_id, int idx, unsigned int *src,
			unsigned int *dest);

/* STATISTICS */
int stat_update(struct dma_stat_id_s *stat_id, int idx, int status);
int stat_set_transfer(struct dma_stat_id_s *stat_id, int idx, int dma_lch,
		      int max_transfers);
int stat_init(char *s, int max_channels, struct dma_stat_id_s **id);
int stat_exit(struct dma_stat_id_s *stat_id);

#endif				/* __DMA_TEST_H */
