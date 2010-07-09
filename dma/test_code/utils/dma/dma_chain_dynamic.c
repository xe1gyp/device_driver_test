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


#include "dma_chain_transfer.h"

#define TRANSFER_COUNT 10
#define TRANSFER_POLL_COUNT 60
#define TRANSFER_POLL_TIME 1500
#define PROC_FILE "driver/dma_chain_dynamic"

static struct dma_transfer transfers[TRANSFER_COUNT];
static struct dma_chain chain;
static volatile int current_transfer = 0;
static int transfer_end = 0;

/*
 * Checks that the destination buffers were written correctly
 */
static void check_test_passed(void){
     int i;
     int error = 0;
     /* Check that all the transfers finished */
     for(i = 0; i < TRANSFER_COUNT; i++){
         if(!transfers[i].data_correct){
             error = 1;
             printk("Chain id %d failed\n", transfers[i].chain_id);
             break;
         }
     }

     if(!error){
         set_test_passed_chain(1);
     }else{
         set_test_passed_chain(0);
     }
}

/*
 * Determines if the transfers have finished
 */
static int get_transfers_finished(void){
       if(transfer_end){
           return 1;
       }
       return 0;
}

/*
 * Callback function that dma framework will invoke after transfer is done
 */
void dma_callback_chain(int transfer_id, u16 transfer_status, void *data) {
       struct dma_transfer *transfer = (struct dma_transfer *) data;
       int error = 1;
       if(current_transfer == TRANSFER_COUNT){
           transfer->data_correct = 1;
           transfer->finished = 1;
           transfer_end = 1;
           omap_stop_dma_chain_transfers(transfer->chain_id);
           printk("Chain id %d stopped\n", transfer->chain_id);
           return;
       }
       transfer->data_correct = 0;
       transfer->finished = 1;
	unmap_phys_buffers(&transfer->buffers);

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
           transfer_end = 1;
           return;
       }

       if(error){
           printk(" Verification failed, transfer id %d-%d source and "
                "destination buffers differ\n", transfer->chain_id,
                transfer->chained_id);
           transfer_end = 1;
           return;
       }else{
           printk(" Verification succeeded for transfer id %d-%d\n",
                transfer->chain_id, transfer->chained_id);
       }

       /* Create the buffers for a new transfer */
       transfers[current_transfer].buffers.buf_size = (1024 * 1);
       error = create_transfer_buffers_chain(
             &(transfers[current_transfer].buffers));
       if( error ){
           transfer_end = 1;
           return;
       }
       fill_source_buffer_chain(&(transfers[current_transfer].buffers));

       /* Chain a transfer to the current dynamic chain */
       printk(" Linking transfer %d to chain id %d\n",
                transfer->chained_id, transfer->chain_id);
       error = chain_transfer(&chain, &transfers[current_transfer]);
       if( error ){
           transfer_end = 1;
           return;
       }
       current_transfer++;
       printk(" Linking succeed\n");

       if(error){
           printk(" Error chaining transfer to chain id %d\n",
                    transfer->chain_id);
           transfer->data_correct = 0;
           return;
       }
       transfer->data_correct = 1;
}

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
      printk("\nRequesting OMAP DMA chain transfer\n");
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

/*
 * Function called when the module is initialized
 */
static int __init dma_module_init(void) {
       int error;
       int i = 0;

       /* Create the proc entry */
       create_dma_proc_chain(PROC_FILE);

       /* Request a chain */
       chain.chain_type = OMAP_DMA_DYNAMIC_CHAIN;
       chain.device_id = OMAP_DMA_NO_DEVICE;
       chain.data_type = OMAP_DMA_DATA_TYPE_S8;
       chain.addressing_mode = OMAP_DMA_AMODE_POST_INC;
       chain.sync_mode = OMAP_DMA_SYNC_ELEMENT;
       chain.data_burst = OMAP_DMA_DATA_BURST_DIS;
       chain.channel_count = TRANSFER_COUNT;

       error = request_dma_chain(&chain);
	omap_set_dma_callback(chain.chain_id, dma_callback_chain, &transfers);
       if(error){
            set_test_passed_chain(0);
            return 1;
       }

       for(i = 0; i < 1/*chain.channel_count*/; i++){

           /* Create the buffers for each transfer */
           transfers[i].buffers.buf_size = (1024 * 1);
           error = create_transfer_buffers_chain(&(transfers[i].buffers));
           if( error ){
               set_test_passed_chain(0);
               return 1;
           }
           fill_source_buffer_chain(&(transfers[i].buffers));

           /* Chain the first transfer to the chain */
           error = chain_transfer(&chain, &transfers[i]);
           if( error ){
               set_test_passed_chain(0);
               return 1;
           }

       }
       /* The first chained transfer has index 0, 1 points to the next one */
       current_transfer = 1;

       /* Setup the global dma parameters */
       setup_dma_chain(&chain);

       /* Start the chain */
       start_dma_chain(&chain);

       /* Poll if the all the transfers have finished */
       for(i = 0; i < TRANSFER_POLL_COUNT; i++){
            if(get_transfers_finished()){
               mdelay(TRANSFER_POLL_TIME);
               check_test_passed();
               break;
            }else{
               mdelay(TRANSFER_POLL_TIME);
            }
       }

       /* This will happen if the poll retries have been reached*/
       if(i == TRANSFER_POLL_COUNT){
           set_test_passed_chain(0);
           return 1;
       }

       return 0;
}

/*
 * Function called when the module is removed
 */
static void __exit dma_module_exit(void) {
       int i, ret = 0, status;
       u32 ch_stop_status = 0;

       if(chain.request_success){
	       ret = omap_stop_dma_chain_transfers(chain.chain_id);
	       if (ret) {
			printk("DMA stop chain failed\n");
			set_test_passed_chain(0);
	       }

	       ret = omap_free_dma_chain(chain.chain_id);
	       if (ret) {
			printk("DMA Chain Free failed for id : %d\n",
				chain.chain_id);
			set_test_passed_chain(0);
	       }
       }

       for(i = 0; i < current_transfer; i++)
           stop_dma_transfer_chain(&transfers[i]);
}

module_init(dma_module_init);
module_exit(dma_module_exit);

MODULE_AUTHOR("Texas Instruments");
MODULE_LICENSE("GPL");
