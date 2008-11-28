/*
 * /vobs/wtbu/OMAPSW_L/linux/test/device_driver_test/dma/test_code/dmatest.c
 *
 * Dma Test Module
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

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/proc_fs.h>
#include <linux/sched.h>

#include <asm/io.h>

#include <linux/version.h>

#include <asm/string.h>

#include "dmatest.h"

/* PROC File location */
#define PROC_FILE "driver/dmatest"

/* load-time options */
static int debug = DEFAULT_DEBUG_LVL;
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,00))
MODULE_PARM(debug, "i");
#else
module_param(debug, int, 0444);
#endif

struct dma_stat_s {
	int dma_ch;
	unsigned int count, good;
	unsigned int max_transfers;
};
struct dma_stat_id_s {
	int channels;
	char init_string[MAX_INIT_STRING];
	struct dma_stat_s *dma_stat;
};

/** 
 * @brief stat_update - update the statistics of transfers
 * 
 * @param idx - idx number
 * @param status - 0 - pass, 1 - fail
 * 
 * @return 0 - more transfers to do on this idx
 *         1 - max transfers reached on this idx
 */
int stat_update(struct dma_stat_id_s *stat_id, int idx, int status)
{
	struct dma_stat_s *dma_stat = stat_id->dma_stat;
	struct dma_stat_s *t = dma_stat + idx;
	int ret = 0;
	FN_IN();

	if (!status)
		t->good++;

	t->count++;
	D(3, "mx=%d cnt=%d good=%d ", t->max_transfers, t->count, t->good);

	if (t->max_transfers && (t->count >= t->max_transfers))
		ret = 1;

	FN_OUT(ret);
	return ret;
}

/** 
 * @brief stat_read_procmem - Proc entry read function
 * 
 * @param buf 
 * @param *start 
 * @param offset 
 * @param count 
 * @param eof 
 * @param data 
 * 
 * @return 
 */
static int stat_read_procmem(char *buf, char **start, off_t offset,
			     int count, int *eof, void *data)
{
	struct dma_stat_id_s *stat_id = (struct dma_stat_id_s *)data;
	struct dma_stat_s *dma_stat = stat_id->dma_stat;
	int i, len = 0;
	FN_IN();

//	len += sprintf(buf + len, "OMAP DMA test\n" "-------------\n" "FILE: ");
//	len += sprintf(buf + len, stat_id->init_string);

//	len += sprintf(buf + len, "\n Ch# xfers good max"
//		       "\n ----------------------\n");
	for (i = 0; i < stat_id->channels; i++) {
		len += sprintf(buf + len, " %2d  %4d %4d %4d\n",
			       dma_stat[i].dma_ch,
			       dma_stat[i].count,
			       dma_stat[i].good, dma_stat[i].max_transfers);
	}
//	len += sprintf(buf + len, "\n ----------------------\n");
	FN_OUT(len);
	return len;
}

/** 
 * @brief stat_set_transfer - selectively select how many transfers
 *                                for a specific idx
 * @param idx 
 * @param max_transfers 
 * 
 * @return 
 */
int stat_set_transfer(struct dma_stat_id_s *stat_id, int idx, int dma_lch,
		      int max_transfers)
{
	struct dma_stat_s *dma_stat = stat_id->dma_stat;
	struct dma_stat_s *t = dma_stat + idx;
	dma_stat = stat_id->dma_stat;
	FN_IN();
	D(3, "idx=%d,lch=%d mx_=%d\n", idx, dma_lch, max_transfers);
	t->dma_ch = dma_lch;
	t->max_transfers = max_transfers;
	FN_OUT(0);
	return 0;
}

/** 
 * @brief stat_init
 * 
 * @return 
 */
int stat_init(char *s, int max_channels, struct dma_stat_id_s **stat_id_copy)
{
	int ret = 0;
	struct dma_stat_id_s *stat_id = NULL;
	struct dma_stat_s *dma_stat = NULL;
	FN_IN();

	if (likely(!stat_id_copy || !s)) {
		ERR("stat_id [%p] or s [%p] is null!!", stat_id, s);
		return -EPERM;
	}

	/* Allocate memory */
	stat_id =
	    kmalloc(sizeof(struct dma_stat_id_s), GFP_KERNEL | GFP_ATOMIC);
	if (!stat_id) {
		ERR("Unable to allocate stat_id\n");
		ret = -ENOMEM;
		goto cleanup;
	}
	memset(stat_id, 0, sizeof(struct dma_stat_id_s));
	snprintf(stat_id->init_string, MAX_INIT_STRING, "%s%s", PROC_FILE, s);
	D(3, "StatFileName=%s", stat_id->init_string);
	stat_id->channels = max_channels;
	dma_stat =
	    kmalloc(sizeof(struct dma_stat_s) * max_channels,
		    GFP_KERNEL | GFP_ATOMIC);
	if (!dma_stat) {
		ERR("Unable to allocate dma_stat\n");
		ret = -ENOMEM;
		goto cleanup;
	}
	memset(dma_stat, 0, sizeof(struct dma_stat_s) * max_channels);
	stat_id->dma_stat = dma_stat;
	/* Create /proc entry */
	ret =
	    (int)create_proc_read_entry(stat_id->init_string,
					0 /* default mode */ ,
					NULL /* parent dir */ ,
					stat_read_procmem,
					stat_id /* client data */ );
	if (!ret) {
		ret = -EPERM;
		goto cleanup;
	}
	*stat_id_copy = stat_id;
	FN_OUT(0);
	return 0;
      cleanup:
	if (stat_id) {
		kfree(stat_id);
		stat_id = NULL;
	}
	if (dma_stat) {
		kfree(dma_stat);
	}
	FN_OUT(ret);
	return ret;
}

/** 
 * @brief stat_exit
 * 
 * @return 
 */
int stat_exit(struct dma_stat_id_s *stat_id)
{
	struct dma_stat_s *dma_stat = NULL;
	FN_IN();
	if (stat_id) {
		dma_stat = stat_id->dma_stat;
		remove_proc_entry(stat_id->init_string, NULL);
		kfree(stat_id);
		stat_id = NULL;
	}
	if (dma_stat) {
		kfree(dma_stat);
	}
	FN_OUT(0);
	return 0;
}

EXPORT_SYMBOL(stat_update);
EXPORT_SYMBOL(stat_set_transfer);
EXPORT_SYMBOL(stat_init);
EXPORT_SYMBOL(stat_exit);

MODULE_AUTHOR("Texas Instruments");
MODULE_LICENSE("GPL");
