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
int query_idx = 0;
int buf_ver = 0;
int burst = 0;
static struct dma_stat_id_s *stat_id;
static struct dma_buf_id_s *buf_id;

#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,00))
MODULE_PARM(debug, "i");
MODULE_PARM(channels, "i");
MODULE_PARM(maximum_transfers, "i");
MODULE_PARM(buf_size, "i");
MODULE_PARM(buf_ver, "i");
#if defined(CONFIG_ARCH_OMAP24XX) || defined(CONFIG_ARCH_OMAP34XX)
MODULE_PARM(prio, "i");
MODULE_PARM(query_idx, "i");
MODULE_PARM(burst, "i");
#endif
#else
module_param(debug, int, 0444);
module_param(channels, int, 0444);
module_param(maximum_transfers, int, 0444);
module_param(buf_size, int, 0444);
module_param(buf_ver, int, 0444);
#if defined(CONFIG_ARCH_OMAP24XX) || defined(CONFIG_ARCH_OMAP34XX)
module_param(prio, int, 0444);
module_param(query_idx, int, 0444);
module_param(burst, int, 0444);
#endif

#endif

struct dma_test_s {
	int dev_id;
	int dma_ch;		/* Storing the idx */
};
static struct dma_test_s dma_test[MAX_CHANNELS];

static struct omap_dma_channel_params transfer_params = {
	.data_type = OMAP_DMA_DATA_TYPE_S32,	/* data type 8,16,32 */
	/* IGNORE */
	.elem_count = 0,	/* number of elements in a frame */
	.frame_count = 1,	/* number of frames in a element */

	.src_amode = OMAP_DMA_AMODE_POST_INC,	/* constant , post increment, indexed , double indexed */
	.src_start = 0,		/* source address : physical */
	.src_ei = 0,		/* source element index */
	.src_fi = 0,		/* source frame index */

	.dst_amode = OMAP_DMA_AMODE_POST_INC,	/* constant , post increment, indexed , double indexed */
	.dst_start = 0,		/* source address : physical */
	.dst_ei = 0,		/* source element index */
	.dst_fi = 0,		/* source frame index */

	.trigger = OMAP_DMA_NO_DEVICE,	/* trigger attached if the channel is synchronized */
	.sync_mode = OMAP_DMA_SYNC_ELEMENT,	/* sycn on element, frame , block or packet */
	.src_or_dst_synch = 0x0,	/* source synch(1) or destination synch(0) */

	.ie = 0,		/* interrupt enabled */
	.burst_mode = OMAP_DMA_DATA_BURST_DIS,
	//.b_loc = LC_SRC,
};

/* Store the chain ID */
static int chain_id;

/********************** TEST LOGIC *****************/

static void dma_callback(int chainid, u16 ch_status, void *data)
{
	struct dma_test_s *t = (struct dma_test_s *)data;
	int idx = t->dma_ch;
	int status = 1;
	FN_IN();
	D(1, "data=%p", data);

	printk("%d: ", idx);
	if (chain_id == chainid) {
		ch_status &= ~(OMAP_DMA_BLOCK_IRQ);
		if (!ch_status) {
			status = 0;
			if (buf_ver && buf_verify(buf_id, idx, buf_size)) {
				status = 1;
			}
		}
		if (status) {
			printk("F\n");
		} else {
			printk("P\n");
		}
		/* Stop the chain?? stop if returns 1 */
		if (stat_update(stat_id, idx, status)) {
			omap_stop_dma_chain_transfers(chain_id);
			printk("Chain Stopped\n");
		} else {
			int elem_count, frame_count;
			unsigned int src, dest;
			int ret = 0;

			elem_count = buf_size / 4;
			frame_count = 1;
			D(2, "   pre-filling src buf %d\n", idx);
			if (buf_ver) {
				(void)buf_fill(buf_id, idx, idx, buf_size);	/* dont expect this to die .. yet */
			}
			/* grab the physical pointers */
			(void)buf_get_src_dst_phy(buf_id, idx, &src, &dest);
			D(2, "physical address src=0x%08x 0x%08x\n", src, dest);
			/* Chain the transfer */
			D(1, "omap_dma_chain_a_transfer %d 0x%08X 0x%08X %d %p",
			  chain_id, src, dest, elem_count, &dma_test[idx]);
			ret =
			    omap_dma_chain_a_transfer(chain_id, src, dest,
						      elem_count, frame_count,
						      (void *)&dma_test[idx]);
			D(2, "return =%d", ret);
			if (ret) {
				ERR(" Failed to Chain transfer idx=%d, ret=%d",
				    idx, ret);
				omap_stop_dma_chain_transfers(chain_id);
			}
		}

	} else {
		ERR("Unexpected event on chain %d expected %d\n", chainid,
		    chain_id);
	}

	FN_OUT(0);
}

static void __exit dmatest_cleanup(void)
{
	FN_IN();
	omap_stop_dma_chain_transfers(chain_id);
	omap_free_dma_chain(chain_id);
	chain_id = -1;
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
	if ((ret = stat_init("dmastatic_chain", channels, &stat_id))) {
		ERR(" Unable to proc init %d\n", ret);
		buf_free(buf_id, channels, buf_size);
		return ret;
	}
	/* request chain */
#if defined(CONFIG_ARCH_OMAP24XX) || defined(CONFIG_ARCH_OMAP34XX)
	/* set the priority settings  priority of a single channel alone now.. */
	if (burst > 0) {
		transfer_params.burst_mode = OMAP_DMA_DATA_BURST_8;
		//transfer_params.b_loc = BOTH;
	}
#endif
	D(1, "omap_request_dma_chain %d %d %d", OMAP_DMA_NO_DEVICE, channels,
	  OMAP_DMA_STATIC_CHAIN);
	if ((ret =
	     omap_request_dma_chain(OMAP_DMA_NO_DEVICE,
				    "Memory to memory transfer", dma_callback,
				    &chain_id, channels, OMAP_DMA_STATIC_CHAIN,
				    transfer_params))) {
		ERR(" Unable to allocate chain %d\n", ret);
		buf_free(buf_id, channels, buf_size);
		stat_exit(stat_id);
		return ret;
	}
	D(1, "Chainid=%d", chain_id);

	for (i = 0; i < channels; i++) {
		D(3, "DMA test %d\n", i);

		/* Setup DMA transfer */
		dma_test[i].dev_id = OMAP_DMA_NO_DEVICE;
		dma_test[i].dma_ch = i;
		elem_count = buf_size / 4;
		frame_count = 1;
		/* Set stat initialization */
		stat_set_transfer(stat_id, i, i, maximum_transfers);
		/* src buf init */
		D(2, "   pre-filling src buf %d\n", i);
		(void)buf_fill(buf_id, i, i, buf_size);	/* dont expect this to die .. yet */
		/* grab the physical pointers */
		(void)buf_get_src_dst_phy(buf_id, i, &src, &dest);
		D(2, "physical address src=0x%08x 0x%08x\n", src, dest);
		/* Chain the transfer */
		D(1, "omap_dma_chain_a_transfer %d 0x%08X 0x%08X %d %p",
		  chain_id, src, dest, elem_count, &dma_test[i]);
		ret =
		    omap_dma_chain_a_transfer(chain_id, src, dest, elem_count, frame_count,
					      (void *)&dma_test[i]);
		if (ret) {
			ERR("omap_dma_chain_a_transfer() failed: %d\n", ret);
			dma_test[i].dev_id = 0;
			D(4, "WARNING: Only go %d/%d channels.\n", i, channels);
			channels = i;
			break;
		}

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
	D(1, " omap_start_dma_chain_transfers %d", chain_id);
	/* Start the chain.. */
	ret = omap_start_dma_chain_transfers(chain_id);
	if (ret) {
		ERR(" Unable to start the DMA chain[%d]!!\n", ret);
		dmatest_cleanup();
		return ret;
	}

	udelay(10);

	if (query_idx) {
		int lh = 0;
		int lch = 0;
		int src[MAX_CHANNELS];
		int dst[MAX_CHANNELS];
		int ei[MAX_CHANNELS];
		int fi[MAX_CHANNELS];
		for (lh = 0; lh < channels; lh++) {
			lch = dma_test[lh].dma_ch;
			src[lch] = omap_get_dma_src_pos(lch);
			dst[lch] = omap_get_dma_dst_pos(lch);
			/*if (unlikely
			    (omap_get_dma_index(lch, &(ei[lch]), &(fi[lch])))) {
				printk("lch-%d fialed src indx\n", lch);
			}*/
		}
		printk("lch->[src] [dst] [ei/fi] \n");
		for (lch = 0; lch < channels; lch++) {
			printk("%d->[0x%x] [0x%x] [0x%x/0x%x]\n", lch, src[lch],
			       dst[lch], ei[lch], fi[lch]);
		}
	}
	FN_OUT(0);
	return 0;
}

module_init(dmatest_init);
module_exit(dmatest_cleanup);

MODULE_AUTHOR("Texas Instruments");
MODULE_LICENSE("GPL");
