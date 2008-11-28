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

/********************** KERNEL SPECIFIC STUFF *****************/
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,00))
 #include <linux/dma-mapping.h>
#endif				/* kernel specs */

#include "dmatest.h"

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,27))
 #include <mach/dma.h>
#else
 #include <asm/arch/dma.h>
#endif

/********************** GENERAL VARS *****************/
/* load-time options */
int debug = DEFAULT_DEBUG_LVL;
int channels = 2;
int maximum_transfers = 5;	/* max transfers per channel */
int buf_size = PAGE_SIZE;	/* Buffer size for each channel */
int prio = -1;
int burst = -1;
int query_idx = 0;

static struct dma_stat_id_s *stat_id;
static struct dma_buf_id_s *buf_id;

#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,00))
MODULE_PARM(debug, "i");
MODULE_PARM(channels, "i");
MODULE_PARM(maximum_transfers, "i");
MODULE_PARM(buf_size, "i");
#if defined(CONFIG_ARCH_OMAP24XX) || defined(CONFIG_ARCH_OMAP34XX)
MODULE_PARM(prio, "i");
MODULE_PARM(query_idx, "i");
MODULE_PARM(burst, "i");
#endif  // CONFIG_ARCH_OMAP24XX
#else
module_param(debug, int, 0444);
module_param(channels, int, 0444);
module_param(maximum_transfers, int, 0444);
module_param(buf_size, int, 0444);
#if defined(CONFIG_ARCH_OMAP24XX) || defined(CONFIG_ARCH_OMAP34XX)
module_param(prio, int, 0444);
module_param(burst, int, 0444);
module_param(query_idx, int, 0444);
#endif
#endif

struct dma_test_s {
	int dev_id;
	int dma_ch;
};
static struct dma_test_s dma_test[MAX_CHANNELS];

/********************** TEST LOGIC *****************/

static int get_lch(int channel)
{
	int i = 0;
	for (i = 0; i < MAX_CHANNELS; i++) {
		if (dma_test[i].dma_ch == channel)
			break;
	}
	if (i == MAX_CHANNELS)
		return -EPERM;
	return i;
}
static void dma_callback(int lch, u16 ch_status, void *data)
{
	struct dma_test_s *t = (struct dma_test_s *)data;
	int idx = get_lch(lch);
	int status = 1;
	FN_IN();

	printk("%d: ", lch);
	if (lch == t->dma_ch) {
		ch_status &= ~(OMAP_DMA_BLOCK_IRQ);
		if (!ch_status && !buf_verify(buf_id, idx, buf_size)) {
			printk("P\n");
			status = 0;
		} else {
			printk("F\n");
		}
		/* Dont care abt the continuing the transfer */
		(void)stat_update(stat_id, idx, status);

	} else {
		D(4, "dma_callback(): Unexpected event on channel %d", lch);
	}

#ifdef DMA_DUMP
	{
		int i;
		/* Dump all the dma registers for all channels */
		for (i = 0; i < channels; i++) {
			printk("CH %d\n", i);
			omap_dump_lch_reg(dma_test[i].dma_ch);
		}
	}
#endif
	FN_OUT(0);
}

static void __exit dmatest_cleanup(void)
{
	int i;

	FN_IN();
	for (i = 0; i < channels; i++) {
		if (dma_test[i].dma_ch >= 0) {
			omap_stop_dma(dma_test[i].dma_ch);
			omap_free_dma(dma_test[i].dma_ch);

			dma_test[i].dma_ch = -1;
		}
	}
	buf_free(buf_id, channels, buf_size);
	stat_exit(stat_id);
	FN_OUT(0);
}

static int __init dmatest_init(void)
{
	int i;
	int elem_count, frame_count;
	unsigned int src, dest;
	int ret = 0;

	FN_IN();
	if (channels > MAX_CHANNELS) {
		ERR("channels arg (%d) > MAX_CHANNELS (%d)\n",
		    channels, MAX_CHANNELS);
		FN_OUT(-ENODEV);
		return -ENODEV;
	}

	/* Alloc DMA-able buffers */
	if ((ret = buf_allocate(channels, buf_size, &buf_id))) {
		ERR(" Unable to find memory%d\n", ret);
		return ret;
	}
	/* Init statitistics */
	if ((ret = stat_init("dmaunlnk", channels, &stat_id))) {
		ERR(" Unable to proc init %d\n", ret);
		buf_free(buf_id, channels, buf_size);
		return ret;
	}

	for (i = 0; i < channels; i++) {
		D(3, "DMA test %d\n", i);

		/* Setup DMA transfer */
		dma_test[i].dev_id = OMAP_DMA_NO_DEVICE;
		dma_test[i].dma_ch = -1;
		ret = omap_request_dma(dma_test[i].dev_id, "DMA Test",
				       dma_callback, (void *)&dma_test[i],
				       &dma_test[i].dma_ch);
		if (ret) {
			ERR("request_dma() failed: %d\n", ret);
			dma_test[i].dev_id = 0;
			D(4, "WARNING: Only go %d/%d channels.\n", i, channels);
			channels = i;
			break;
		}

		/* Set stat initialization */
		stat_set_transfer(stat_id, i, dma_test[i].dma_ch, 0);
		/* src buf init */
		D(2, "   pre-filling src buf %d\n", i);
		(void)buf_fill(buf_id, i, dma_test[i].dma_ch, buf_size);	/* dont expect this to die .. yet */
		/* grab the physical pointers */
		(void)buf_get_src_dst_phy(buf_id, i, &src, &dest);
		D(2, "physical address src=0x%08x 0x%08x\n", src, dest);

		elem_count = buf_size / 4;
		frame_count = 1;
#if defined(CONFIG_ARCH_OMAP24XX) || defined(CONFIG_ARCH_OMAP34XX)
		/* 24XX params */
		omap_set_dma_transfer_params(dma_test[i].dma_ch,	/* int lch */
					     OMAP_DMA_DATA_TYPE_S32,	/* int data_type */
					     elem_count,	/* int elem_count */
					     frame_count,	/* int frame_count */
					     OMAP_DMA_SYNC_ELEMENT,	/*  int sync_mode */
					     dma_test[i].dev_id,	/* int dma_trigger */
					     0x0);	/* int src_or_dst_synch */
		omap_set_dma_src_params(dma_test[i].dma_ch,	/* int lch */
					0, /* Port only for OMAP1 */
					OMAP_DMA_AMODE_POST_INC,	/* post increment int src_amode */
					src,	/*  int src_start */
					0x0,	/*  int src_ei */
					0x0);	/* int src_fi */
		omap_set_dma_dest_params(dma_test[i].dma_ch,	/* int lch */
					0, /* Port only for OMAP1 */
					 OMAP_DMA_AMODE_POST_INC,	/* post increment int dst_amode */
					 dest,	/*  int dst_start */
					 0x0,	/*  int dst_ei */
					 0x0);	/* int dst_fi */
		if (burst >= 0) {
			omap_set_dma_src_burst_mode(dma_test[i].dma_ch,
							OMAP_DMA_DATA_BURST_4);
			omap_set_dma_dest_burst_mode(dma_test[i].dma_ch,
							OMAP_DMA_DATA_BURST_4);
		}
#else
		/* 16XX params */
		omap_set_dma_transfer_params(dma_test[i].dma_ch,
					     OMAP_DMA_DATA_TYPE_S32,
					     elem_count,
					     frame_count,
					     OMAP_DMA_SYNC_ELEMENT);
		omap_set_dma_src_params(dma_test[i].dma_ch, OMAP_DMA_PORT_EMIFF,
					OMAP_DMA_AMODE_POST_INC, src);
		omap_set_dma_dest_params(dma_test[i].dma_ch,
					 OMAP_DMA_PORT_EMIFF,
					 OMAP_DMA_AMODE_POST_INC, dest);
#endif

	}

#if defined(CONFIG_ARCH_OMAP24XX) || defined(CONFIG_ARCH_OMAP34XX)
	/* set the priority settings  priority of a single channel alone now.. */
	if (prio >= 0) {
		D(2, "Priority to %d 25%% channel reserved ", prio);
		omap_dma_set_global_params(DMA_DEFAULT_ARB_RATE,
					  DMA_DEFAULT_FIFO_DEPTH,
					  DMA_THREAD_RESERVE_ONET |
					  DMA_THREAD_FIFO_25);
		omap_dma_set_prio_lch(dma_test[prio].dma_ch, DMA_CH_PRIO_HIGH,
				      DMA_CH_PRIO_HIGH);
	}
#endif
	for (i = 0; i < channels; i++) {
		D(3, "   2Start DMA channel %d\n", dma_test[i].dma_ch);
		omap_start_dma(dma_test[i].dma_ch);
	}
	udelay(10);

	if (query_idx) {
		int lh = 0;
		int lch = 0;
		int src[MAX_CHANNELS];
		int dst[MAX_CHANNELS];
	//	int ei[MAX_CHANNELS];
	//	int fi[MAX_CHANNELS];
		for (lh = 0; lh < channels; lh++) {
			lch = dma_test[lh].dma_ch;
			src[lh] = omap_get_dma_src_pos(lch);
			dst[lh] = omap_get_dma_dst_pos(lch);
	//		if (unlikely
	//		    (omap_get_dma_index(lch, &(ei[lh]), &(fi[lh])))) {
	//			printk("lch-%d fialed src indx\n", lch);
	//		}
		}
		printk("idx[lch]-> [src]        [dst]        [ei/fi] \n");
		for (lch = 0; lch < channels; lch++) {
			printk
			    ("%02d [%02d] -> [0x%08X] [0x%08X] \n",		//[0x%08X/0x%08X]\n",
			     lch, dma_test[lch].dma_ch, src[lch], dst[lch]);//
	//		     ,ei[lch], fi[lch]);
		}
	}
	FN_OUT(0);
	return 0;
}

module_init(dmatest_init);
module_exit(dmatest_cleanup);

MODULE_AUTHOR("Texas Instruments");
MODULE_LICENSE("GPL");
