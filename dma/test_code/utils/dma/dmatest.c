/*
 * /vobs/wtbu/OMAPSW_L/linux/test/device_driver_test/dma/test_code/dmatest.c
 *
 * Dma Test Module
 *
 * Copyright (C) 2004-2007 Texas Instruments, Inc.
 *
 * This package is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * THIS PACKAGE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 * History:
 * -------
 * Original based on Tony L's code(see documentation for additional info)
 * Modified by Kevin Hilman for OMAP1
 * Modified by Nishanth Menon for OMAP2
 * Modified by Anand Gadiyar for OMAP3 and to align with Open Source
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/proc_fs.h>

#include <asm/io.h>

#include <linux/version.h>

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,00))
 #include <linux/dma-mapping.h>
#endif

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,27))
 #include <mach/dma.h>
#else
 #include <asm/arch/dma.h>
#endif

/* Debug Levels 0 - 4 - 0 prints more information and 4 the least */
#define D(VAL, ARGS...) if (VAL>=debug){ printk( "%s[%d]:",__FUNCTION__,__LINE__);printk (ARGS);printk ("\n");}
#define FN_IN() D(2,"Entry");
#define FN_OUT(RET) D(2,"Exit(%d)",RET);

/* Debug Default debug level */
#define DEFAULT_DEBUG_LVL 5

/* Test using the page APIs  make this 0 to use coherent allocation */
#define PAGE_TEST 0

/* PROC File location */
#define PROC_FILE "driver/dmatest"

/* load-time options */
int debug = DEFAULT_DEBUG_LVL;
int startup = 1;		/* whether to start DMA channels during module init */
int channels = 2;
int maximum_transfers = 5;	/* max transfers per channel */
int nochain = 0;		/* During Linking - -1= dont chain till channel linking, or 1=dont chain after channel linking */
#if defined(CONFIG_ARCH_OMAP24XX) || defined(CONFIG_ARCH_OMAP34XX)
int prio = -1;			/* Set high priority for which channel? */
int query_idx = 0;			/* Set high priority for which channel? */
#endif

#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,00))
MODULE_PARM(debug, "i");
MODULE_PARM(startup, "i");
MODULE_PARM(channels, "i");
MODULE_PARM(maximum_transfers, "i");
#if defined(CONFIG_ARCH_OMAP24XX) || defined(CONFIG_ARCH_OMAP34XX)
MODULE_PARM(prio, "i");
MODULE_PARM(query_idx, "i");
#endif
#else
module_param(debug, int, 0444);
module_param(startup, int, 0444);
module_param(channels, int, 0444);
module_param(maximum_transfers, int, 0444);
#if defined(CONFIG_ARCH_OMAP24XX) || defined(CONFIG_ARCH_OMAP34XX)
module_param(prio, int, 0444);
module_param(query_idx, int, 0444);
#endif
#endif

#define BUF_SIZE PAGE_SIZE
#define MAX_CHANNELS 32

struct dma_test_s {
	int dev_id;
	int dma_ch, next_ch;
	unsigned int src_buf, dest_buf;
	unsigned int src_buf_phys, dest_buf_phys;
	unsigned int count, good;
	unsigned int max_transfers;
};
static struct dma_test_s dma_test[MAX_CHANNELS];

#define lswap32(x) \
({ \
         u32 __x = (x); \
	         ((u32)( \
		  (((u32)(__x) & (u32)0x0000ffffUL) << 16) | \
		  (((u32)(__x) & (u32)0xffff0000UL) >> 16) )); \
 })

static int verify_buffer(unsigned int *src, unsigned int *dest)
{
	unsigned int i;
	unsigned int *s = src, *d = dest;

	FN_IN();
	for (i = 0; i < BUF_SIZE / 4; i++) {
		D(3," src = %x, dest = %x\n", *s, lswap32(*d));
		if (*s != lswap32(*d)) {
			printk("F\n");
			D(3, "!!!! DMA verify failed at offset 0x%x.  "
			  "Expected 0x%08x, got 0x%08x",
			  (unsigned int)s - (unsigned)src, *s, *d);
			FN_OUT(1);
			return 1;
		}
		s++;
		d++;
	}
	printk("P\n");
	FN_OUT(0);
	return 0;
}

static void dma_callback(int lch, u16 ch_status, void *data)
{
	struct dma_test_s *t = (struct dma_test_s *)data;
	FN_IN();

	printk("%d: ", lch);
	if (lch == t->dma_ch) {
		if (((ch_status & OMAP_DMA_BLOCK_IRQ) || (ch_status == 0)) &&
		    (verify_buffer
		     ((unsigned int *)t->src_buf,
		      (unsigned int *)t->dest_buf) == 0)) {
			t->good++;
		} else {
			D(3, "dma_callback(lch=%d, ch_status=0x%04x)",
			  lch, ch_status);
		}

		t->count++;
		D(3, "mx=%d cnt=%d ", t->max_transfers, t->count);

		if (t->max_transfers && (t->count >= t->max_transfers)) {
			D(3, "Stopping %d", t->dma_ch);
			omap_stop_dma(t->dma_ch);
		}
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

static int dmatest_read_procmem(char *buf, char **start, off_t offset,
				int count, int *eof, void *data)
{
	int i, len = 0;
	FN_IN();

//	len += sprintf(buf + len, "OMAP DMA test\n");

//	len += sprintf(buf + len, " Ch# Nxt xfers good max\n");
	for (i = 0; i < channels; i++) {
//		len += sprintf(buf + len, " %2d%s%2d %4d %4d %4d\n",
		len += sprintf(buf + len, " %2d %4d %4d %4d\n",
			       dma_test[i].dma_ch,
//			       dma_test[i].next_ch != -1 ? "->" : "  ",
//			       dma_test[i].next_ch,
			       dma_test[i].count,
			       dma_test[i].good, dma_test[i].max_transfers);
	}
	FN_OUT(len);
	return len;
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

		dma_test[i].src_buf_phys = 0;
		if (dma_test[i].src_buf)
#if PAGE_TEST
			free_page(dma_test[i].src_buf);
#else
			D(4, "Free src- %p", (void *)dma_test[i].src_buf);
#if (LINUX_VERSION_CODE <= KERNEL_VERSION(2,6,00))
		consistent_free((void *)dma_test[i].src_buf, BUF_SIZE,
				dma_test[i].src_buf_phys);
#else
		dma_free_coherent(NULL, BUF_SIZE, (void *)dma_test[i].src_buf,
				  dma_test[i].src_buf_phys);
#endif

#endif

		dma_test[i].dest_buf_phys = 0;
		if (dma_test[i].dest_buf)
#if PAGE_TEST
			free_page(dma_test[i].dest_buf);
#else
			D(4, "Free dst- %p", (void *)dma_test[i].dest_buf);
#if (LINUX_VERSION_CODE <= KERNEL_VERSION(2,6,00))
		consistent_free((void *)dma_test[i].dest_buf, BUF_SIZE,
				dma_test[i].dest_buf_phys);
#else
		dma_free_coherent(NULL, BUF_SIZE, (void *)dma_test[i].dest_buf,
				  dma_test[i].dest_buf_phys);
#endif

#endif
	}

	remove_proc_entry(PROC_FILE, NULL);
	FN_OUT(0);
}

static int __init dmatest_init(void)
{
	int i, j, r;
	int elem_count, frame_count;
	unsigned int *p;

	FN_IN();
	if (channels > MAX_CHANNELS) {
		D(4, "dmatest_init(): channels arg (%d) > MAX_CHANNELS (%d)\n",
		  channels, MAX_CHANNELS);
		FN_OUT(-ENODEV);
		return -ENODEV;
	}

	/* Crate /proc entry */
	create_proc_read_entry(PROC_FILE, 0 /* default mode */ ,
			       NULL /* parent dir */ ,
			       dmatest_read_procmem, NULL /* client data */ );

	/* Alloc DMA-able buffers */
	for (i = 0; i < channels; i++) {
		D(3, "DMA test %d\n", i);
		dma_test[i].count = 0;
		dma_test[i].next_ch = -1;

#if PAGE_TEST
		dma_test[i].src_buf = get_zeroed_page(GFP_KERNEL | GFP_DMA);
		if (!dma_test[i].src_buf) {
			D(4, "dmatest_init(): get_zeroed_page() failed.\n");
			r = -ENOMEM;
			goto cleanup;
		}
		dma_test[i].dest_buf = get_zeroed_page(GFP_KERNEL | GFP_DMA);
		if (!dma_test[i].dest_buf) {
			D(4, "dmatest_init(): get_zeroed_page() failed.\n");
			r = -ENOMEM;
			goto cleanup;
		}
		dma_test[i].src_buf_phys = virt_to_bus(dma_test[i].src_buf);
		dma_test[i].dest_buf_phys = virt_to_bus(dma_test[i].dest_buf);
#else
#if (LINUX_VERSION_CODE <= KERNEL_VERSION(2,6,00))
		dma_test[i].src_buf =
		    (unsigned int)consistent_alloc(GFP_KERNEL | GFP_DMA,
						   BUF_SIZE,
						   &(dma_test[i].src_buf_phys));
#else
		dma_test[i].src_buf =
		    (unsigned int)dma_alloc_coherent(NULL,
						     BUF_SIZE,
						     &(dma_test[i].
						       src_buf_phys), 0);
#endif
		if (!dma_test[i].src_buf) {
			D(4, "dmatest_init(): consistent_alloc() failed.\n");
			r = -ENOMEM;
			goto cleanup;
		}
#if (LINUX_VERSION_CODE <= KERNEL_VERSION(2,6,00))
		dma_test[i].dest_buf =
		    (unsigned int)consistent_alloc(GFP_KERNEL | GFP_DMA,
						   BUF_SIZE,
						   &(dma_test[i].
						     dest_buf_phys));
#else
		dma_test[i].dest_buf =
		    (unsigned int)dma_alloc_coherent(NULL,
						     BUF_SIZE,
						     &(dma_test[i].
						       dest_buf_phys), 0);
#endif
		if (!dma_test[i].dest_buf) {
			D(4, "dmatest_init(): consistent_alloc() failed.\n");
#if (LINUX_VERSION_CODE <= KERNEL_VERSION(2,6,00))
			consistent_free((void *)dma_test[i].src_buf, BUF_SIZE,
					dma_test[i].src_buf_phys);
#else
			dma_free_coherent(NULL, BUF_SIZE,
					  (void *)dma_test[i].src_buf,
					  dma_test[i].src_buf_phys);
#endif

			r = -ENOMEM;
			goto cleanup;
		}
#endif
		D(4, "Alloc src=0x%08x/0x%08x, dest=0x%08x/0x%08x\n",
		  dma_test[i].src_buf,
		  dma_test[i].src_buf_phys,
		  dma_test[i].dest_buf, dma_test[i].dest_buf_phys);

		/* Setup DMA transfer */
		dma_test[i].dev_id = OMAP_DMA_NO_DEVICE;
		dma_test[i].dma_ch = -1;
		dma_test[i].max_transfers = 0;
		r = omap_request_dma(dma_test[i].dev_id, "DMA Test",
				     dma_callback, (void *)&dma_test[i],
				     &dma_test[i].dma_ch);
		if (r) {
			D(4, "dmatest_init(): request_dma() failed: %d\n", r);
			dma_test[i].dev_id = 0;
			D(4, "WARNING: Only go %d/%d channels.\n", i, channels);
			channels = i;
			break;
		}

		/* src buf init */
		D(2, "   pre-filling src buf %d\n", i);
		p = (unsigned int *)dma_test[i].src_buf;
		for (j = 0; j < BUF_SIZE / 4; j++) {
			p[j] = (~j << 24) | (dma_test[i].dma_ch << 16) | j;
		}

		elem_count = BUF_SIZE / 2;
		frame_count = 1;
#if defined(CONFIG_ARCH_OMAP24XX) || defined(CONFIG_ARCH_OMAP34XX)
		/* 24XX params */
		omap_set_dma_transfer_params(dma_test[i].dma_ch,	/* int lch */
					     OMAP_DMA_DATA_TYPE_S16,	/* int data_type */
					     elem_count,	/* int elem_count */
					     frame_count,	/* int frame_count */
					     OMAP_DMA_SYNC_ELEMENT,	/*  int sync_mode */
					     dma_test[i].dev_id,	/* int dma_trigger */
					     0x0);	/* int src_or_dst_synch */
		omap_set_dma_src_params(dma_test[i].dma_ch,	/* int lch */
					0,		/* src_port */
					OMAP_DMA_AMODE_POST_INC,	/* post increment int src_amode */
					dma_test[i].src_buf_phys,	/*  int src_start */
					0x0,	/*  int src_ei */
					0x0);	/* int src_fi */

		#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,27))
			omap_set_dma_src_endian_type(dma_test[i].dma_ch,OMAP_DMA_BIG_ENDIAN);
		#else
			omap_set_dma_src_endian_type(dma_test[i].dma_ch,BIG_ENDIAN);
		#endif
		
		omap_set_dma_src_burst_mode(dma_test[i].dma_ch, OMAP_DMA_DATA_BURST_4);
		omap_set_dma_dest_params(dma_test[i].dma_ch,	/* int lch */
					 0,		/* dest_port */
					 OMAP_DMA_AMODE_POST_INC,	/* post increment int dst_amode */
					 dma_test[i].dest_buf_phys,	/*  int dst_start */
					 0x0,	/*  int dst_ei */
					 0x0);	/* int dst_fi */
		omap_set_dma_dest_burst_mode(dma_test[i].dma_ch, OMAP_DMA_DATA_BURST_4);
#else
		/* 16XX params */
		omap_set_dma_transfer_params(dma_test[i].dma_ch,
					     OMAP_DMA_DATA_TYPE_S32,
					     elem_count,
					     frame_count,
					     OMAP_DMA_SYNC_ELEMENT);
		omap_set_dma_src_params(dma_test[i].dma_ch, OMAP_DMA_PORT_EMIFF,
					OMAP_DMA_AMODE_POST_INC,
					dma_test[i].src_buf_phys);
		omap_set_dma_dest_params(dma_test[i].dma_ch,
					 OMAP_DMA_PORT_EMIFF,
					 OMAP_DMA_AMODE_POST_INC,
					 dma_test[i].dest_buf_phys);
#endif

	}

#if defined(CONFIG_ARCH_OMAP24XX) || defined(CONFIG_ARCH_OMAP34XX)
	/* set the priority settings  priority of a single channel alone now.. */
	if (prio >= 0) {
		D(2,"Priority to %d 25%% channel reserved ",prio);
		omap_dma_set_global_params(DMA_DEFAULT_ARB_RATE,
					  DMA_DEFAULT_FIFO_DEPTH,
					  DMA_THREAD_RESERVE_ONET |
					  DMA_THREAD_FIFO_25);
		omap_dma_set_prio_lch(dma_test[prio].dma_ch, DMA_CH_PRIO_HIGH, DMA_CH_PRIO_HIGH);
	}
#endif
	if (startup) {
		for (i = 0; i < channels; i++) {
			D(3, "   2Start DMA channel %d\n",
			  dma_test[i].dma_ch);
			omap_start_dma(dma_test[i].dma_ch);
		}
	}

	if (query_idx) {
		int lch =0;
		int src[MAX_CHANNELS];
		int dst[MAX_CHANNELS];
		int ei[MAX_CHANNELS];
		int fi[MAX_CHANNELS];
		for (lch = 0; lch < channels; lch++) {
			src[lch]= omap_get_dma_src_pos(lch);
			dst[lch]= omap_get_dma_dst_pos(lch);
			if (unlikely(omap_get_dma_index(lch,&(ei[lch]),&(fi[lch])))) {
				printk("lch-%d fialed src indx\n",lch);
			}
		}
		printk("lch->[src] [dst] [ei/fi] \n");
		for (lch = 0; lch < channels; lch++) {
			printk("%d->[0x%x] [0x%x] [0x%x/0x%x]\n",lch,src[lch],dst[lch], ei[lch],fi[lch]);
		}
	}
	FN_OUT(0);
	return 0;

      cleanup:
	dmatest_cleanup();
	FN_OUT(r);
	return r;
}

module_init(dmatest_init);
module_exit(dmatest_cleanup);

MODULE_AUTHOR("Texas Instruments");
MODULE_LICENSE("GPL");
