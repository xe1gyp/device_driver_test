/*
 * DMA Test module - Chained transfers
 *
 * The following testcode checks that the dynamic chain DMA mode
 * works correctly. One transfer is registered in a new dynamic
 * chain. When the transfer finishes another chain is linked until
 * the transfer count is reached.
 *
 * History:
 * 28-01-2009	Gustavo Diaz	Initial version of the testcode
 *
 * Copyright (C) 2007-2009 Texas Instruments, Inc
 *
 * This package is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 */

#include <linux/sched.h>
#include <linux/kthread.h>
#include <linux/syscalls.h>
#include "dma_chain_transfer.h"


#define MAX_THREADS		2
#define STRESS_COUNT		100
#define TRANSFER_COUNT		14
#define TRANSFER_POLL_COUNT	60
#define TRANSFER_POLL_TIME	1500

struct chain_transfer {
	struct dma_transfer transfers[TRANSFER_COUNT];
	struct dma_chain chain;
	int current_transfer;
	int transfer_end;
};

struct chain_transfer cht[MAX_THREADS];
int cht1phy;
static volatile int chained_id = 0;
static int count;
static int thread_count;

/*
 * Checks that the destination buffers were written correctly
 */
static void check_test_passed(struct dma_transfer *transfers)
{
	int i;
	int error = 0;

	/* Check that all the transfers finished */
	for (i = 0; i < TRANSFER_COUNT; i++) {
		if (!transfers[i].data_correct) {
			error = 1;
			printk(KERN_INFO "Chain id %d failed for transfer %d\n",
				transfers[i].chain_id, i);
			break;
		}
	}

	if (!error)
		set_test_passed_chain(1);
	else
		set_test_passed_chain(0);
}

static int dynamic_chain_transfer(struct chain_transfer *ct)
{
	int error;
	struct dma_chain *chain_params = &(ct->chain);
	struct dma_transfer *transfer = (struct dma_transfer *)&ct->transfers;
	transfer->finished = 0;
	transfer->data_correct = 0;
	transfer->frame_count = 1;
	transfer->request_success = 0;

	switch (chain_params->data_type) {
	case OMAP_DMA_DATA_TYPE_S8:
		transfer->elements_in_frame = transfer->buffers.buf_size;
		break;
	case OMAP_DMA_DATA_TYPE_S16:
		transfer->elements_in_frame = transfer->buffers.buf_size / 2;
		break;
	case OMAP_DMA_DATA_TYPE_S32:
		transfer->elements_in_frame = transfer->buffers.buf_size / 4;
		break;
	default:
		printk(KERN_ERR "Invalid transfer data type\n");
		return 0;
	}

	map_to_phys_buffers(&transfer->buffers);
	printk(KERN_DEBUG "Chaining a transfer to chain id %d\n", chain_params->chain_id);
	transfer->chained_id = chained_id++;
	transfer->chain_id = chain_params->chain_id;
	error = omap_dma_chain_a_transfer(chain_params->chain_id,
					transfer->buffers.src_buf_phys,
					transfer->buffers.dest_buf_phys,
					transfer->elements_in_frame,
					transfer->frame_count, (void *)transfer);
	if (error) {
		printk(KERN_ERR "Error chaining transfer to chain id %d\n",
				chain_params->chain_id);
		return 1;
	}
	transfer->request_success = 1;
	printk(KERN_DEBUG " Chained transfer id is %d\n", transfer->chained_id);
	return 0;
}

/*
 * Callback function that dma framework will invoke after transfer is done
 */
void dma_callback_chain(int transfer_id, u16 transfer_status, void *data)
{
	struct chain_transfer *ct = (struct chain_transfer *)data;
	struct dma_transfer *transfer = (struct dma_transfer *)&ct->transfers;

	int error = 1;
	transfer->data_correct = 1;
	transfer->finished = 1;

	if (ct->current_transfer == TRANSFER_COUNT) {
		/* Stop the chain */
		ct->transfer_end = 1;
		omap_stop_dma_chain_transfers(transfer->chain_id);
		printk(KERN_DEBUG  "Chain id %d stopped\n", transfer->chain_id);
		return;
	}

	unmap_phys_buffers(&transfer->buffers);
	printk(KERN_DEBUG  "Transfer complete in chain %d-%d, checking dest buffer\n",
	   transfer->chain_id, transfer->chained_id);
	/* Check if the transfer numbers are equal */
	if (transfer->chain_id != transfer_id) {
		printk(KERN_DEBUG "Transfer chain id %d differs from the"
			"one received in callback (%d)\n", transfer->chain_id,
			transfer_id);
	}
	unmap_phys_buffers(&transfer->buffers);

	/* Check the transfer status is acceptable */
	if ((transfer_status & OMAP_DMA_BLOCK_IRQ) || (transfer_status == 0)) {
		/* Verify the contents of the buffer are equal */
		error = verify_buffers_chain(&(transfer->buffers));
	} else {
	   printk(KERN_ERR "Verification failed, transfer id %d-%d status"
			"is not acceptable\n", transfer->chain_id,
			transfer->chained_id);
	   ct->transfer_end = 1;
	   return;
	}

	if (error) {
		printk(KERN_ERR "Verification failed, transfer %d-%d source"
				"and dest buffers differ\n", transfer->chain_id,
				transfer->chained_id);
		ct->transfer_end = 1;
		return;
	} else
		printk(KERN_INFO "Transfer and Verification success for chain %d-%d\n",
			transfer->chain_id, transfer->chained_id);

	transfer[ct->current_transfer].data_correct = 1;
	ct->current_transfer++;

	/* Create the buffers for a new transfer */
	ct->transfers[ct->current_transfer].buffers.buf_size = (1024 * 1);
	error = create_transfer_buffers_chain(
			&(ct->transfers[ct->current_transfer].buffers));
	if (error) {
		ct->transfer_end = 1;
		return;
	}
	fill_source_buffer_chain(
		&(ct->transfers[ct->current_transfer].buffers));

	/* Chain a transfer to the current dynamic chain */
	printk(KERN_DEBUG  "Linking transfer %d to chain id %d\n",
		    transfer->chained_id, transfer->chain_id);
	error = dynamic_chain_transfer(ct);
	if (error) {
		ct->transfer_end = 1;
		return;
	}

	if (error) {
		printk(KERN_ERR "Error chaining transfer to chain id %d\n",
			transfer->chain_id);
		transfer->data_correct = 0;
		return;
	}
	printk(KERN_DEBUG " Linking succeed\n");
}

/*
 * Requests a dma transfer
 */
int request_dma_chain(struct dma_chain *chain_params)
{
	int error;
	chain_params->finished = 0;
	/* Configure the dma channel parameters for the chain*/
	chain_params->channel_params.data_type = chain_params->data_type;
	chain_params->channel_params.elem_count = 0;
	chain_params->channel_params.frame_count = 1;
	chain_params->channel_params.src_amode = chain_params->addressing_mode;
	chain_params->channel_params.src_start = 0;
	chain_params->channel_params.src_ei = 0;
	chain_params->channel_params.src_fi = 0;
	chain_params->channel_params.dst_amode = chain_params->addressing_mode;
	chain_params->channel_params.dst_start = 0;
	chain_params->channel_params.dst_ei = 0;
	chain_params->channel_params.dst_fi = 0;
	chain_params->channel_params.trigger = chain_params->device_id;
	chain_params->channel_params.sync_mode = chain_params->sync_mode;
	chain_params->channel_params.ie = 0;
	chain_params->channel_params.burst_mode = chain_params->data_burst;

	/* Request the chain */
	printk(KERN_DEBUG  "Requesting OMAP DMA chain transfer\n");

	error = omap_request_dma_chain(chain_params->device_id,
				"dma_test", dma_callback_chain,
				&(chain_params->chain_id),
				chain_params->channel_count,
				chain_params->chain_type,
				chain_params->channel_params);
	if (error) {
		printk(KERN_ERR "Request failed\n");
		chain_params->request_success = 0;
		return 1;
	}
	chain_params->request_success = 1;
	return 0;
}

int dma_chain_thread_entry(void *info)
{
	int error;
	int i = 0;
	struct chain_transfer *ct = (struct chain_transfer *)info;

	/* Request a chain */
	ct->chain.chain_type = OMAP_DMA_DYNAMIC_CHAIN;
	ct->chain.device_id = OMAP_DMA_NO_DEVICE;
	ct->chain.data_type = OMAP_DMA_DATA_TYPE_S8;
	ct->chain.addressing_mode = OMAP_DMA_AMODE_POST_INC;
	ct->chain.sync_mode = OMAP_DMA_SYNC_ELEMENT;
	ct->chain.data_burst = OMAP_DMA_DATA_BURST_DIS;
	ct->chain.channel_count = TRANSFER_COUNT;

	error = request_dma_chain(&(ct->chain));
	omap_set_dma_callback(ct->chain.chain_id, dma_callback_chain, ct);

	if (error) {
		set_test_passed_chain(0);
		return 1;
	}

	for (i = 0; i < 1/*chain.channel_count*/; i++) {
		/* Create the buffers for each transfer */
		ct->transfers[i].buffers.buf_size = (1024 * 1);
		error = create_transfer_buffers_chain(
					&(ct->transfers[i].buffers));

		if (error) {
			set_test_passed_chain(0);
			return 1;
		}
		fill_source_buffer_chain(&(ct->transfers[i].buffers));

		/* Chain the first transfer to the chain */
		error = chain_transfer(&(ct->chain), &(ct->transfers[i]));
		if (error) {
			set_test_passed_chain(0);
			return 1;
		}

	}
	ct->current_transfer = 0;

	/* Setup the global dma parameters */
	setup_dma_chain(&(ct->chain));

	/* Start the chain */
	start_dma_chain(&(ct->chain));

	/* Poll if the all the transfers have finished */
	for (i = 0; i < TRANSFER_POLL_COUNT; i++) {
		if (ct->transfer_end) {
			mdelay(TRANSFER_POLL_TIME);
			printk("Thread %d :", thread_count);
			check_test_passed(ct->transfers);
			break;
		} else {
			mdelay(TRANSFER_POLL_TIME);
		}
	}

	/* This will happen if the poll retries have been reached*/
	if (i == TRANSFER_POLL_COUNT) {
		set_test_passed_chain(0);
		return 1;
	}
	count += 1;
	thread_count += 1;
	return 0;
}

void create_dma_threads()
{
	struct task_struct *p[MAX_THREADS];
	int t_count;

	for (t_count = 0; t_count < MAX_THREADS; t_count++) {
		p[t_count] = kthread_create(dma_chain_thread_entry, &cht[t_count],
								"dma_chain_thread");
		kthread_bind(p[t_count], t_count);
	}

	/* Start all the threads at a time */
	for (t_count = 0; t_count < MAX_THREADS; t_count++) {
		wake_up_process(p[t_count]);
	}
	return;
}

/*
 * Function called when the module is initialized
 */
static int __init dma_module_init(void)
{
	int t_count, stress_count = 0;
	int i, ret;

	while ( stress_count++ < STRESS_COUNT) {
		create_dma_threads();

		printk(KERN_INFO "stress count : %d\n", stress_count);

		/* Wait till all the thread finish data transfer */
		while(count < MAX_THREADS)
			msleep(10);

		for (t_count = 0; t_count < MAX_THREADS; t_count++) {
			if (cht[t_count].chain.request_success) {
				ret = omap_stop_dma_chain_transfers(cht[t_count].
								chain.chain_id);
				if (ret) {
					printk("DMA stop chain failed\n");
					set_test_passed_chain(0);
				}
				ret = omap_free_dma_chain(cht[t_count].chain.chain_id);
				if (ret) {
					printk("DMA Chain Free failed for id : %d\n",
					cht[t_count].chain.chain_id);
					set_test_passed_chain(0);
				}
				for (i = 0; i < cht[t_count].current_transfer; i++)
					stop_dma_transfer_chain(&(cht[t_count].
									transfers[i]));
			}
		}
		count = 0;
	}
	return 0;
}

/*
 * Function called when the module is removed
 */
static void __exit dma_module_exit(void)
{

}

module_init(dma_module_init);
module_exit(dma_module_exit);

MODULE_AUTHOR("Texas Instruments");
MODULE_LICENSE("GPL");
