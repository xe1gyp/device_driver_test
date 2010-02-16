/*
 * DMA Single channel transfer common functions
 *
 * The following code has the implementation for the common functions
 * used in the single channel transfer tests.
 *
 * History:
 * 20-01-2009	Gustavo Diaz	Initial version of the testcode
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


#include "dma_single_channel.h"

#define TEST_FINISHED_POLL_TIME 1000

static int test_passed = -1;

/*
 * The read proc entry returns passed or failed according to the stored value.
 * It will not return until the test sets a value different than -1
 */
static int dma_read_proc(char *buf, char **start, off_t offset,
				int count, int *eof, void *data)
{
	int len;

    while (test_passed == -1)
        mdelay(TEST_FINISHED_POLL_TIME);

    if (test_passed) {
        len = sprintf(buf,"Test PASSED\n");
    }else{
        len = sprintf(buf,"Test FAILED\n");
    }

    return len;
}

/*
 * Set if the test passed or not
 */
void set_test_passed(int passed)
{
     if (passed) {
        printk("\nTest PASSED\n");
     }else{
        printk("\nTest FAILED\n");
     }
     test_passed = passed;
}
EXPORT_SYMBOL(set_test_passed);

/*
 * Checks that the destination buffers were written correctly
 */
int check_dma_transfer_complete(struct dma_transfer *transfers,
				int num_transfer)
{
	int i;
	/* Check that all the transfers finished */
	for (i = 0; i < num_transfer; i++) {
		if (!transfers[i].data_correct) {
			printk(KERN_INFO "Transfer id %d failed\n",
					transfers[i].transfer_id);
			return -(i);
		}
	}

	return 0;
}
EXPORT_SYMBOL(check_dma_transfer_complete);

/*
 * Creates a read proc entry in the procfs
 */
void create_dma_proc(char *proc_name)
{
	create_proc_read_entry(proc_name, 0, NULL, dma_read_proc, NULL);
}
EXPORT_SYMBOL(create_dma_proc);

/*
 * Removes a proc entry from the procfs
 */
void remove_dma_proc(char *proc_name)
{
    remove_proc_entry(proc_name, NULL);
}
EXPORT_SYMBOL(remove_dma_proc);


/*
 * Function used to verify the source an destination buffers of a dma transfer
 * are equal in content
 */
int verify_buffers(struct dma_buffers_info *buffers)
{
    int i;
    u8 *src_address = (u8*) buffers->src_buf;
    u8 *dest_address = (u8*) buffers->dest_buf;

    /* Iterate through the source and destination buffers byte per byte */
    for (i = 0; i < buffers->buf_size; i++) {
        /* Compare the data in the source and destination */
        if (*src_address != *dest_address) {
            printk("Source buffer at 0x%x = %d , Destination buffer at 0x%x"
                   " = %d\n", buffers->src_buf, *src_address, buffers->dest_buf,
                   *dest_address);
            return 1; /* error, buffers differ */
        }
        /* Increment the pointer to the next data */
        src_address++;
        dest_address++;
    }
    return 0;
}
EXPORT_SYMBOL(verify_buffers);

/*
 * Callback function that dma framework will invoke after transfer is done
 */
void dma_callback(int transfer_id, u16 transfer_status, void *data)
{
	struct dma_transfer *transfer = (struct dma_transfer *) data;
	int error = 1;
	transfer->data_correct = 0;
	transfer->finished = 1;
	printk("\nTransfer complete for id %d, checking destination buffer\n",
	transfer->transfer_id);
	/* Check if the transfer numbers are equal */
	if (transfer->transfer_id != transfer_id) {
		printk(KERN_WARNING "WARN:Transfer id %d differs from the one"
		" received in callback (%d)\n", transfer->transfer_id,
		transfer_id);
	}
	unmap_phys_buffers(&transfer->buffers);

       /* Check the transfer status is acceptable */
	if ((transfer_status & OMAP_DMA_BLOCK_IRQ) || (transfer_status == 0)) {
		/* Verify the contents of the buffer are equal */
		error = verify_buffers(&(transfer->buffers));
	} else {
		printk(KERN_ERR " Verification failed, trfr id %d status is not"
		"acceptable\n", transfer->transfer_id);
		return;
	}

	if (error) {
		printk(KERN_ERR "Verification failed, transfer id %d source"
		"and destination buffers differ\n", transfer->transfer_id);
	} else {
		printk(KERN_INFO " Verification succeeded for transfer id %d\n",
		transfer->transfer_id);
		transfer->data_correct = 1;
	}
}
EXPORT_SYMBOL(dma_callback);

/*
 * This function allocates 2 dma buffers with the same size for the source
 * and destination.
 */
int create_transfer_buffers(struct dma_buffers_info *buffers)
{
	printk(KERN_INFO "Allocating non-cacheable source and destination buffers\n");

	/* Allocate source buffer */
	buffers->src_buf = 0;
	buffers->src_buf = (unsigned int)kmalloc(buffers->buf_size, GFP_DMA);

	/* Allocate destination buffer */
	buffers->dest_buf = 0;
	buffers->dest_buf = (unsigned int)kmalloc(buffers->buf_size, GFP_DMA);

       /* Check the buffers have been allocated correctly */
       if (!buffers->src_buf) {
		printk(KERN_ERR "Unable to allocate %d bytes for the transfer"
		" buffer\n", buffers->buf_size);
		return 1;
       } else if (!buffers->dest_buf) {
		printk(KERN_ERR "Unable to allocate %d bytes for the transfer"
		" buffer\n", buffers->buf_size);
		return 1;
       } else {
		printk(KERN_INFO "Buffers allocated (%d bytes per buffer)\n",
			buffers->buf_size);
		printk(KERN_INFO "Source buffer on address 0x%x\n",
			buffers->src_buf);
		printk(KERN_INFO "Destination buffer on address 0x%x\n",
			buffers->dest_buf);
       }
       return 0;
}
EXPORT_SYMBOL(create_transfer_buffers);

/*
 * Fill the source buffer with bytes using a pseudo-random value generator
 */
void fill_source_buffer(struct dma_buffers_info *buffers)
{
       int i;
       u8 *src_buf_byte;
       printk("Filling source buffer 0x%x with %d bytes...", buffers->src_buf,
           buffers->buf_size);
       src_buf_byte = (u8*) buffers->src_buf;
       for (i = 0; i < buffers->buf_size; i++) {
           src_buf_byte[i] = (~i << 7) | (buffers->buf_size << 3) | i;
       }
       printk("done\n");
}
EXPORT_SYMBOL(fill_source_buffer);

/*
 * Requests a dma transfer
 */
int request_dma(struct dma_transfer *transfer)
{
       int success;
       transfer->finished = 0;
       printk("\nRequesting OMAP DMA transfer\n");
       success = omap_request_dma(
               transfer->device_id,
               "dma_test",
               dma_callback,
               (void *) transfer,
               &(transfer->transfer_id));
       if (success) {
          printk(" Request failed\n");
          transfer->request_success = 0;
          return 1;
       }else{
          printk(" Request succeeded, transfer id is %d\n",
               transfer->transfer_id);
          transfer->request_success = 1;
       }
       return 0;
}
EXPORT_SYMBOL(request_dma);

void map_to_phys_buffers(struct dma_buffers_info *buffers)
{
	buffers->src_buf_phys =
		dma_map_single(NULL, buffers->src_buf,
			buffers->buf_size, DMA_BIDIRECTIONAL);
	buffers->dest_buf_phys =
		dma_map_single(NULL, buffers->dest_buf,
			buffers->buf_size, DMA_BIDIRECTIONAL);

	if (buffers->src_buf_phys == 0 ||
		buffers->dest_buf_phys == 0)
		printk(KERN_ERR "DMA Mapping failed\n");

}

void unmap_phys_buffers(struct dma_buffers_info *buffers)
{
	dma_unmap_single(NULL, buffers->src_buf_phys,
		buffers->buf_size, DMA_BIDIRECTIONAL);
	dma_unmap_single(NULL, buffers->dest_buf_phys,
		buffers->buf_size, DMA_BIDIRECTIONAL);

}
/*
 * Setup the source, destination and global transfer parameters
 */
void setup_dma_transfer(struct dma_transfer *transfer)
{
	/* Determine the elements present in a frame */
	printk(KERN_INFO "Setting up transfer id %d\n", transfer->transfer_id);
	transfer->frame_count = 1;
	switch (transfer->data_type) {
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
	}

	map_to_phys_buffers(&transfer->buffers);
	/* Set dma transfer parameters */
	omap_set_dma_transfer_params(
		transfer->transfer_id,
		transfer->data_type,
		transfer->elements_in_frame,
		transfer->frame_count,
		transfer->sync_mode,
		transfer->device_id,
		0x0);

	/* Configure the source parameters */
	omap_set_dma_src_params(
		transfer->transfer_id,
		0,
		transfer->addressing_mode,
		transfer->buffers.src_buf_phys,
		transfer->src_ei, transfer->src_fi);

	omap_set_dma_src_burst_mode(
		transfer->transfer_id,
		transfer->data_burst);

	/* Configure the destination parameters */
	omap_set_dma_dest_params(
		transfer->transfer_id,
		0, transfer->dst_addressing_mode,
		transfer->buffers.dest_buf_phys,
		transfer->dest_ei, transfer->dest_fi);

	omap_set_dma_dest_burst_mode(
		transfer->transfer_id,
		transfer->data_burst);

	/* Global dma configuration parameters */
	omap_dma_set_global_params(
		0x3,
		DMA_DEFAULT_FIFO_DEPTH,
		0);

	/* Transfer priority */
	omap_dma_set_prio_lch(
		transfer->transfer_id,
		transfer->priority,
		transfer->priority);

	printk(KERN_INFO "Transfer with id %d is ready\n",
			transfer->transfer_id);
}
EXPORT_SYMBOL(setup_dma_transfer);

/*
 * Starts a dma transfer
 */
void start_dma_transfer(struct dma_transfer *transfer)
{
	char *priority;
	if (transfer->priority == DMA_CH_PRIO_LOW)
		priority = "low priority";
	else
		priority = "high priority";

	printk(KERN_INFO "Starting dma transfer for id %d with %s\n",
	transfer->transfer_id, priority);
	omap_start_dma(transfer->transfer_id);
}
EXPORT_SYMBOL(start_dma_transfer);

/*
 * Stops a dma transfer and free used resources
 */
void stop_dma_transfer(struct dma_transfer *transfer)
{
	/* Stop the dma transfer */
	if (transfer->request_success) {
		omap_stop_dma(transfer->transfer_id);
		omap_free_dma(transfer->transfer_id);
	}

		kfree(transfer->buffers.src_buf);
		kfree(transfer->buffers.dest_buf);
}
EXPORT_SYMBOL(stop_dma_transfer);

/*
 * Queries for information about an on going dma transfer
 */
int dma_channel_query(struct dma_transfer *transfer,
     struct dma_query *query)
{
      int error = 0;
      /* Get element and frame index */
      /*error = omap_get_dma_index(transfer->transfer_id,
              &(query->element_index),
              &(query->frame_index));*/
      /* Get source and destination addresses positions */
      query->src_addr_counter = omap_get_dma_src_pos(transfer->transfer_id);
      query->dest_addr_counter = omap_get_dma_dst_pos(transfer->transfer_id);

	if (error) {
		printk(KERN_INFO "Unable to query index data from transfer id %d\n",
                transfer->transfer_id);
           return 1;
      }

      printk("Transfer id %d query\n source counter 0x%x\n"
             " destination counter 0x%x\n element index %d\n"
             " frame index %d\n",
             transfer->transfer_id,
             query->src_addr_counter,
             query->dest_addr_counter,
             query->element_index,
             query->frame_index);

      return 0;
}
EXPORT_SYMBOL(dma_channel_query);

MODULE_AUTHOR("Texas Instruments");
MODULE_LICENSE("GPL");
