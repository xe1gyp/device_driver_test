/*
 * DMA Chained transfer common functions
 *
 * The following code has the implementation for the common functions
 * used in the chained transfer tests.
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


#include "dma_chain_transfer.h"

#define TEST_FINISHED_POLL_TIME 1000

static int test_passed = -1;
static volatile int chained_id = 0;
static int max_rounds = 1;

/*
 * The read proc entry returns passed or failed according to the stored value.
 * It will not return until the test sets a value different than -1
 */
static int dma_read_proc(char *buf, char **start, off_t offset,
				int count, int *eof, void *data){
	int len;

    while(test_passed == -1){
        mdelay(TEST_FINISHED_POLL_TIME);
    }

    if(test_passed){
        len = sprintf(buf,"Test PASSED\n");
    }else{
        len = sprintf(buf,"Test FAILED\n");
    }

    return len;
}

/*
 * Set if the test passed or not
 */
void set_test_passed_chain(int passed){
     if(passed){
        printk("\nTest PASSED\n");
     }else{
        printk("\nTest FAILED\n");
     }
     test_passed = passed;
}
EXPORT_SYMBOL(set_test_passed_chain);

/*
 * Creates a read proc entry in the procfs
 */
void create_dma_proc_chain(char *proc_name){
	create_proc_read_entry(proc_name, 0, NULL, dma_read_proc, NULL);
}
EXPORT_SYMBOL(create_dma_proc_chain);

/*
 * Removes a proc entry from the procfs
 */
void remove_dma_proc_chain(char *proc_name){
    remove_proc_entry(proc_name, NULL);
}
EXPORT_SYMBOL(remove_dma_proc_chain);

/*
 * Function used to verify the source an destination buffers of a dma transfer
 * are equal in content
 */
int verify_buffers_chain(struct dma_buffers_info *buffers) {
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
EXPORT_SYMBOL(verify_buffers_chain);

/*
 * Callback function that dma framework will invoke after transfer is done
 */
void dma_callback_chain(int transfer_id, u16 transfer_status, void *data) {
       struct dma_transfer *transfer = (struct dma_transfer *) data;
       int error = 1;
       if(transfer->rounds == max_rounds){
           omap_stop_dma_chain_transfers(transfer->chain_id);
           printk("Chain id %d stopped\n", transfer->chain_id);
           return;
       }
       (transfer->rounds)++;
       transfer->data_correct = 0;
       transfer->finished = 1;
       /* Stop the chain */
       printk("\nTransfer complete in chain %d-%d, checking destination buffer\n",
           transfer->chain_id, transfer->chained_id);
       /* Check if the transfer numbers are equal */
       if(transfer->chain_id != transfer_id){
           printk(" WARNING: Transfer chain id %d differs from the "
                "one received in callback (%d)\n", transfer->chain_id,
                transfer_id);
       }
       /* Check the transfer status is acceptable */
       if((transfer_status & OMAP_DMA_BLOCK_IRQ) || (transfer_status == 0)){
           /* Verify the contents of the buffer are equal */
           error = verify_buffers_chain(&(transfer->buffers));
       }else{
           printk(" Verification failed, transfer id %d-%d status is not "
                "acceptable\n", transfer->chain_id, transfer->chained_id);
           transfer->rounds = max_rounds;
           return;
       }

       if(error){
           printk(" Verification failed, transfer id %d-%d source and "
                "destination buffers differ\n", transfer->chain_id,
                transfer->chained_id);
           transfer->rounds = max_rounds;
       }else{
           printk(" Verification succeeded for transfer id %d-%d\n",
                transfer->chain_id, transfer->chained_id);
           transfer->data_correct = 1;
       }
}
EXPORT_SYMBOL(dma_callback_chain);

/*
 * This function allocates 2 dma buffers with the same size for the source
 * and destination.
 */
int create_transfer_buffers_chain( struct dma_buffers_info *buffers){
	int order;
	struct page *srcvirtpage, *dstvirtpage;
       printk("Allocating non-cacheable source and destination buffers\n");

       /* Allocate source buffer */
	buffers->src_buf = 0;

	order = get_order(buffers->buf_size);
	srcvirtpage = alloc_pages_exact(buffers->buf_size, GFP_DMA);
	buffers->src_buf = (unsigned int) srcvirtpage;
	buffers->src_buf_phys = dma_map_single(NULL, srcvirtpage, buffers->buf_size, DMA_BIDIRECTIONAL);

	printk("\n The buffers->src_buf is 0x%x", buffers->src_buf);
	printk("\n The  (buffers->src_buf_phys) After call is %x  ",  buffers->src_buf_phys);

	/* Allocate destination buffer */
	buffers->dest_buf = 0;
	order = get_order(buffers->buf_size);
	dstvirtpage = alloc_pages_exact(buffers->buf_size, GFP_DMA);
	buffers->dest_buf = (unsigned int) dstvirtpage;
	buffers->dest_buf_phys = dma_map_single(NULL, dstvirtpage, buffers->buf_size, DMA_BIDIRECTIONAL);

	printk("\n The buffers->dest_buf is 0x%x", buffers->dest_buf);
	printk("\n The  (buffers->dest_buf_phys) After call is %x  ",  buffers->dest_buf_phys);

       /* Check the buffers have been allocated correctly */
       if( !buffers->src_buf ){
           printk(" Unable to allocate %d bytes for the source transfer"
                " buffer\n", buffers->buf_size);
           return 1;
       }else if( !buffers->dest_buf ){
           printk(" Unable to allocate %d bytes for the destination transfer"
                " buffer\n", buffers->buf_size);
           return 1;
       }else{
           printk(" Buffers allocated successfully (%d bytes per buffer)\n",
                buffers->buf_size);
           printk(" Source buffer on address 0x%x\n",
                buffers->src_buf);
           printk(" Destination buffer on address 0x%x\n",
                buffers->dest_buf);
       }
       return 0;
}
EXPORT_SYMBOL(create_transfer_buffers_chain);

/*
 * Fill the source buffer with bytes using a pseudo-random value generator
 */
void fill_source_buffer_chain(struct dma_buffers_info *buffers){
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
EXPORT_SYMBOL(fill_source_buffer_chain);

/*
 * Requests a dma transfer
 */
int request_dma_chain(struct dma_chain *chain_params){
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
      printk("Requesting OMAP DMA chain transfer\n");
      error = omap_request_dma_chain(chain_params->device_id,
				    "dma_test", dma_callback_chain,
				    &(chain_params->chain_id),
                    chain_params->channel_count,
                    chain_params->chain_type,
				    chain_params->channel_params);
      if(error){
           printk(" Request failed\n");
           chain_params->request_success = 0;
           return 1;
      }
      chain_params->request_success = 1;
      printk(" Request succeeded, chain id is %d\n", chain_params->chain_id);
      return 0;
}
EXPORT_SYMBOL(request_dma_chain);

void setup_dma_chain(struct dma_chain *chain_param){

       /* Determine the elements present in a frame */
       printk("Setting up chain id %d\n", chain_param->chain_id);

       /* Global dma configuration parameters */
       omap_dma_set_global_params(
                0x3,
                DMA_DEFAULT_FIFO_DEPTH,
                0);
       printk(" Chain with id %d is ready\n", chain_param->chain_id);
}
EXPORT_SYMBOL(setup_dma_chain);

/*
 * Starts a dma transfer
 */
int start_dma_chain(struct dma_chain *chain_params){
       int error;
       printk("\nStarting dma chain for id %d", chain_params->chain_id);
       error = omap_start_dma_chain_transfers(chain_params->chain_id);
       if(error){
           printk("DMA chain id %d start failed\n", chain_params->chain_id);
           return 1;
       }
       printk("\n");
       return 0;
}
EXPORT_SYMBOL(start_dma_chain);

int chain_transfer(struct dma_chain *chain_params,
       struct dma_transfer *transfer){
       int error;
       transfer->finished = 0;
       transfer->data_correct = 0;
       transfer->frame_count = 1;
       transfer->request_success = 0;
       switch(chain_params->data_type){
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
               printk(" Invalid transfer data type\n");
               return 0;
       }

       printk("Chaining a transfer to chain id %d\n", chain_params->chain_id);
       transfer->chained_id = chained_id++;
       transfer->chain_id = chain_params->chain_id;
       error = omap_dma_chain_a_transfer(chain_params->chain_id,
             transfer->buffers.src_buf_phys,
             transfer->buffers.dest_buf_phys,
             transfer->elements_in_frame,
             transfer->frame_count, (void *)transfer);
       if(error){
           printk(" Error chaining transfer to chain id %d\n",
                    chain_params->chain_id);
           return 1;
       }
       transfer->request_success = 1;
       printk(" Chained transfer id is %d\n", transfer->chained_id);
       return 0;
}
EXPORT_SYMBOL(chain_transfer);

/*
 * Stops a dma transfer and free used resources
 */
void stop_dma_transfer_chain(struct dma_transfer *transfer){
       /* Stop the dma transfer */
       if(!transfer->request_success){
           return;
	}

/* Free the source and destination buffers*/
       if(transfer->buffers.src_buf){
	   free_pages_exact(transfer->buffers.src_buf, transfer->buffers.buf_size);
	}
       if(transfer->buffers.dest_buf){
	   free_pages_exact(transfer->buffers.dest_buf, transfer->buffers.buf_size);
	}
}
EXPORT_SYMBOL(stop_dma_transfer_chain);

/*
 * Stops a dma transfer and free used resources
 */
void stop_dma_chain(struct dma_chain *chain){
     if(chain->request_success){
	       omap_stop_dma_chain_transfers(chain->chain_id);
	       omap_free_dma_chain(chain->chain_id);
     }

}
EXPORT_SYMBOL(stop_dma_chain);

void set_max_rounds(int rounds){
     max_rounds = rounds;
}
EXPORT_SYMBOL(set_max_rounds);

MODULE_AUTHOR("Texas Instruments");
MODULE_LICENSE("GPL");
