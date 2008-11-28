/*
 * /vobs/wtbu/OMAPSW_L/linux/test/device_driver_test/dma/test_code/dma_buf_logic.c
 *
 * Dma Test Module- Buffer Logic - to be included in dma_test.c
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

#include "dmatest.h"

#include <linux/version.h>

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,00))
#include <linux/dma-mapping.h>
#endif

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,27))
#include <mach/dma.h>
#else
#include <asm/arch/dma.h>
#endif

/* Defines */
#define DEFAULT_BUFFER_SIZE PAGE_SIZE

#define PS_S 54132
#define PS_A 1103515245
#define PS_C 12345
#define PS_M 0x80000000
/* Private structures */
struct dma_buf_s {
	unsigned int src_buf, dest_buf;
	unsigned int src_buf_phys, dest_buf_phys;
};

struct dma_buf_id_s {
	int channels;
	char init_string[MAX_INIT_STRING];
	struct dma_buf_s *dma_buf;
};

/* Seed */
static unsigned int Xn = PS_S;

int debug = DEFAULT_DEBUG_LVL;
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,00))
MODULE_PARM(debug, "i");
#else
module_param(debug, int, 0444);
#endif

/** 
 * @brief buf_verify verify the src and dest buffers are same
 * 
 * @param idx idx to check
 * @param size size to test
 * 
 * @return 0 if ok, else 1
 */
int buf_verify(struct dma_buf_id_s *buf_id, int idx, int size)
{
	unsigned int i;
	struct dma_buf_s *dma_buf = buf_id->dma_buf;
	unsigned int *s = (unsigned int *)dma_buf[idx].src_buf;
	unsigned int *d = (unsigned int *)dma_buf[idx].dest_buf;
	FN_IN();

	for (i = 0; i < size / 4; i++) {
		if (*s != *d) {
			ERR("DMA[%d] verify failed at offset 0x%x.  "
			    "Expected 0x%08x, got 0x%08x", idx,
			    (unsigned int)s -
			    (unsigned int)dma_buf[idx].src_buf, *s, *d);
			FN_OUT(1);
			return 1;
		}
		s++;
		d++;
	}
	FN_OUT(0);
	return 0;
}

/** 
 * @brief pseudo_generate - a silly pseudo random num generator
 * 
 * (BSD rand() is defined as 
 *  Xn+1 = (A*Xn + C ) mod M
 *  http://en.wikipedia.org/wiki/List_of_pseudorandom_number_generators
 * 
 * @param idx - not used
 * 
 * @return unsigned int random number
 */
static unsigned int pseudo_generate(int idx)
{
	unsigned int Xn_1 = ((PS_A * (Xn + idx) + PS_C) % PS_M);
	Xn = Xn_1;
	return Xn;
}

/** 
 * @brief buf_get_src_dst_phy get the physical address of the buffers..
 * 
 * @param idx idx for which u want the address
 * @param src  returns the src phy address
 * @param dest returns the dest phy address
 * 
 * @return 0 for hte time bein
 */
int buf_get_src_dst_phy(struct dma_buf_id_s *buf_id, int idx, unsigned int *src,
			unsigned int *dest)
{
	struct dma_buf_s *dma_buf = buf_id->dma_buf;
	*src = dma_buf[idx].src_buf_phys;
	*dest = dma_buf[idx].dest_buf_phys;
	return 0;
}

/** 
 * @brief buf_fill - reset the buffers and fill up the buffer with random data
 * 
 * @param idx - the idx to fill up
 * @param size  - size of the buffers
 * 
 * @return - 0 if ok, else fail
 */
int buf_fill(struct dma_buf_id_s *buf_id, int idx, int dma_lch, int size)
{
	unsigned int i;
	struct dma_buf_s *dma_buf = buf_id->dma_buf;
	unsigned int *s = (unsigned int *)dma_buf[idx].src_buf;
	unsigned int *d = (unsigned int *)dma_buf[idx].dest_buf;
	FN_IN();
	memset(d, 0, size);

	for (i = 0; i < size / 4; i++) {
		s[i] = pseudo_generate(idx);
	}
	FN_OUT(0);
	return 0;
}

/** 
 * @brief buf_free - free all allocated buffers
 * 
 * @param channels -total num of channels
 * @param size - buffer size
 * 
 * @return 0 if success, else fail
 */
int buf_free(struct dma_buf_id_s *buf_id, int channels, int size)
{
	int ret = 0;
	int i;
	struct dma_buf_s *dma_buf = NULL;
	FN_IN();
	if (buf_id) {
		dma_buf = buf_id->dma_buf;
		remove_proc_entry(buf_id->init_string, NULL);
		kfree(buf_id);
		buf_id = NULL;
	}
	if (dma_buf) {
		for (i = 0; i < channels; i++) {

			if (dma_buf[i].src_buf) {
				D(4, "Free src- %p",
				  (void *)dma_buf[i].src_buf);
#if (LINUX_VERSION_CODE <= KERNEL_VERSION(2,6,00))
				consistent_free((void *)dma_buf[i].src_buf,
						size, dma_buf[i].src_buf_phys);
#else
				dma_free_coherent(NULL, size,
						  (void *)dma_buf[i].src_buf,
						  dma_buf[i].src_buf_phys);
#endif

			}
			if (dma_buf[i].dest_buf) {
				D(4, "Free dst- %p",
				  (void *)dma_buf[i].dest_buf);
#if (LINUX_VERSION_CODE <= KERNEL_VERSION(2,6,00))
				consistent_free((void *)dma_buf[i].dest_buf,
						size, dma_buf[i].dest_buf_phys);
#else
				dma_free_coherent(NULL, size,
						  (void *)dma_buf[i].dest_buf,
						  dma_buf[i].dest_buf_phys);
#endif

			}
		}
		kfree(dma_buf);
	}
	FN_OUT(0);
	FN_OUT(ret);
	return ret;
}

/** 
 * @brief buf_allocate - allocate the buffers
 * 
 * @param channels -total num of channels
 * @param size - buffer size
 * 
 * @return 0 if success, else fail
 */
int buf_allocate(int channels, int size, struct dma_buf_id_s **buf_id_copy)
{
	int i;
	int ret = 0;
	struct dma_buf_id_s *buf_id = NULL;
	struct dma_buf_s *dma_buf = NULL;
	FN_IN();

	if (likely(!buf_id_copy)) {
		ERR("buf_id [%p] is null!!", buf_id);
		return -EPERM;
	}

	/* Allocate memory */
	buf_id = kmalloc(sizeof(struct dma_buf_id_s), GFP_KERNEL | GFP_ATOMIC);
	if (!buf_id) {
		ERR("Unable to allocate buf_id\n");
		ret = -ENOMEM;
		goto cleanup;
	}
	memset(buf_id, 0, sizeof(struct dma_buf_id_s));
	buf_id->channels = channels;
	dma_buf =
	    kmalloc(sizeof(struct dma_buf_s) * channels,
		    GFP_KERNEL | GFP_ATOMIC);
	if (!dma_buf) {
		ERR("Unable to allocate dma_buf\n");
		ret = -ENOMEM;
		goto cleanup;
	}
	memset(dma_buf, 0, sizeof(struct dma_buf_s) * channels);
	buf_id->dma_buf = dma_buf;

	/* Alloc DMA-able buffers */
	for (i = 0; i < channels; i++) {
#if (LINUX_VERSION_CODE <= KERNEL_VERSION(2,6,00))
		dma_buf[i].src_buf =
		    (unsigned int)consistent_alloc(GFP_KERNEL | GFP_DMA,
						   size,
						   &(dma_buf[i].src_buf_phys));
#else
		dma_buf[i].src_buf =
		    (unsigned int)dma_alloc_coherent(NULL,
						     size,
						     &(dma_buf[i].
						       src_buf_phys), 0);
#endif
		if (!dma_buf[i].src_buf) {
			ERR("src buff consistent_alloc(%d) failed.\n", i);
			ret = -ENOMEM;
			goto cleanup;
		}
#if (LINUX_VERSION_CODE <= KERNEL_VERSION(2,6,00))
		dma_buf[i].dest_buf =
		    (unsigned int)consistent_alloc(GFP_KERNEL | GFP_DMA,
						   size,
						   &(dma_buf[i].dest_buf_phys));
#else
		dma_buf[i].dest_buf =
		    (unsigned int)dma_alloc_coherent(NULL,
						     size,
						     &(dma_buf[i].
						       dest_buf_phys), 0);
#endif
		if (!dma_buf[i].dest_buf) {
			ERR("dst buff consistent_alloc(%d) failed.\n", i);
#if (LINUX_VERSION_CODE <= KERNEL_VERSION(2,6,00))
			consistent_free((void *)dma_buf[i].src_buf, size,
					dma_buf[i].src_buf_phys);
#else
			dma_free_coherent(NULL, size,
					  (void *)dma_buf[i].src_buf,
					  dma_buf[i].src_buf_phys);
#endif
			dma_buf[i].src_buf = 0;

			ret = -ENOMEM;
			goto cleanup;
		}
		/* allocated.. now.. print?? */
		D(4, "alloc dst- v=%p p=%p", (void *)dma_buf[i].dest_buf,
		  (void *)dma_buf[i].dest_buf_phys);
		D(4, "alloc src- v=%p p=%p", (void *)dma_buf[i].src_buf,
		  (void *)dma_buf[i].src_buf_phys);
	}
      cleanup:
	if (ret) {
		/* clean up all other allocated channels */
		(void)buf_free(buf_id, channels, size);
	} else {
		*buf_id_copy = buf_id;
	}
	FN_OUT(ret);
	return ret;
}

EXPORT_SYMBOL(buf_verify);
EXPORT_SYMBOL(buf_fill);
EXPORT_SYMBOL(buf_free);
EXPORT_SYMBOL(buf_allocate);
EXPORT_SYMBOL(buf_get_src_dst_phy);
MODULE_AUTHOR("Texas Instruments");
MODULE_LICENSE("GPL");
