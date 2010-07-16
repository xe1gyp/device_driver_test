/*
 * DMA Test module - SMP thread access check
 *
 * Copyright (C) 2009 Texas Instruments, Inc
 *
 * History
 *   Initial Version: Venkatraman S <svenkatr@ti.com>
 * This package is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 */


#include "dma_single_channel.h"
#include <linux/sched.h>
#include <linux/kthread.h>
#include <linux/syscalls.h>


#define TRANSFER_COUNT 13
#define TRANSFER_POLL_COUNT 60
#define TRANSFER_POLL_TIME 1500
#define PROC_FILE "driver/dma_thread_access"

static struct dma_transfer gtransfers[TRANSFER_COUNT];
static struct dma_transfer gtransfers2[TRANSFER_COUNT];

/*
 * Determines if the transfers have finished
 */
static int get_transfers_finished(struct dma_transfer *transfers)
{
	int i = 0;
	for (i = 0; i < TRANSFER_COUNT; i++) {
		if (!transfers[i].finished)
			return 0;
       }
       return 1;
}

static int dma_test_entry(void *info)
{
	int i = 0;
	int error;
	struct dma_transfer *transfers = (struct dma_transfer *)info;

	for (i = 0; i < TRANSFER_COUNT; i++) {

		/* Create the transfer for the test */
		transfers[i].device_id = OMAP_DMA_NO_DEVICE;
		transfers[i].sync_mode = OMAP_DMA_SYNC_ELEMENT;
		transfers[i].data_burst = OMAP_DMA_DATA_BURST_DIS;
		transfers[i].data_type = OMAP_DMA_DATA_TYPE_S8;
		transfers[i].endian_type = DMA_TEST_LITTLE_ENDIAN;
		transfers[i].addressing_mode = OMAP_DMA_AMODE_POST_INC;
		transfers[i].dst_addressing_mode = OMAP_DMA_AMODE_POST_INC;
		transfers[i].priority = DMA_CH_PRIO_HIGH;
		transfers[i].buffers.buf_size = (512 * (i+1)*(i+1)) + i % 2;

		/* Request a dma transfer */
		error = request_dma(&transfers[i]);
		if (error) {
			set_test_passed(0);
		return 1;
		}

		/* Request 2 buffer for the transfer and fill them */
		error = create_transfer_buffers(&(transfers[i].buffers));
		if (error) {
			set_test_passed(0);
			return 1;
		}
		fill_source_buffer(&(transfers[i].buffers));

		/* Setup the dma transfer parameters */
		setup_dma_transfer(&transfers[i]);
	}

	for (i = 0; i < TRANSFER_COUNT; i++) {
		/* Start the transfers */
		start_dma_transfer(&transfers[i]);
	}

	/* Poll if the all the transfers have finished */
	for (i = 0; i < TRANSFER_POLL_COUNT; i++) {
		if (get_transfers_finished(transfers)) {
			mdelay(TRANSFER_POLL_TIME);
			error = check_dma_transfer_complete(transfers,
					TRANSFER_COUNT);
			break;
		} else {
			mdelay(TRANSFER_POLL_TIME);
		}
	}

	/* This will happen if the poll retries have been reached*/
	if (error)
		set_test_passed(0);
	else
		set_test_passed(1);

	return 0;
}

/*
 * Function called when the module is initialized
 */
static int __init dma_module_init(void)
{
	struct task_struct *p1, *p2;
	int x;

	/* Create the proc entry */
	create_dma_proc(PROC_FILE);
	p1 = kthread_create(dma_test_entry, gtransfers, "dmatest/0");
	p2 = kthread_create(dma_test_entry, gtransfers2, "dmatest/1");
	kthread_bind(p1, 0);
	kthread_bind(p2, 1);
	x = wake_up_process(p1);
	x = wake_up_process(p2);
	return 0;
}

/*
 * Function called when the module is removed
 */
static void __exit dma_module_exit(void)
{
	int i, ret = 0;
	for (i = 0; i < TRANSFER_COUNT; i++) {
		stop_dma_transfer(&gtransfers[i]);
	}
}

module_init(dma_module_init);
module_exit(dma_module_exit);

MODULE_AUTHOR("Texas Instruments");
MODULE_LICENSE("GPL");
