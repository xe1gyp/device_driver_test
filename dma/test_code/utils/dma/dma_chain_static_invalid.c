/*
 * DMA Test module - Chained transfers
 *
 * The following testcode checks that the static chain DMA mode
 * works correctly. A new static chain transfer is created and 3
 * transfers are chained at the beginning. The chain should not stop
 * until the rounds number is reached.
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

#define TRANSFER_ROUNDS 5
#define TRANSFER_COUNT 40
#define TRANSFER_POLL_COUNT 60
#define TRANSFER_POLL_TIME 1500
#define PROC_FILE "driver/dma_chain_static"

static struct dma_transfer transfers[TRANSFER_COUNT];
static struct dma_chain chain;

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
       int i = 0;
       for(i = 0; i < TRANSFER_COUNT; i++){
            if(transfers[i].rounds == TRANSFER_ROUNDS){
                return 1;
            }
       }
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
       chain.chain_type = OMAP_DMA_STATIC_CHAIN;
       chain.device_id = OMAP_DMA_NO_DEVICE;
       chain.data_type = OMAP_DMA_DATA_TYPE_S8;
       chain.addressing_mode = OMAP_DMA_AMODE_POST_INC;
       chain.sync_mode = OMAP_DMA_SYNC_ELEMENT;
       chain.data_burst = OMAP_DMA_DATA_BURST_DIS;
       chain.channel_count = TRANSFER_COUNT;
       set_max_rounds(TRANSFER_ROUNDS);

       error = request_dma_chain(&chain);

       if(error){
            set_test_passed_chain(0);
            return 1;
       }

       for(i = 0; i < chain.channel_count; i++){

           /* Create the buffers for each transfer */
           transfers[i].buffers.buf_size = (1024 * 1024);
           error = create_transfer_buffers_chain(&(transfers[i].buffers));
           if( error ){
               set_test_passed_chain(0);
               return 1;
           }
           fill_source_buffer_chain(&(transfers[i].buffers));

           /* Chain a transfer to the chain */
           error = chain_transfer(&chain, &transfers[i]);
           if( error ){
               set_test_passed_chain(0);
               return 1;
           }

       }

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
       int i;

       if(chain.request_success){
	       omap_stop_dma_chain_transfers(chain.chain_id);
	       omap_free_dma_chain(chain.chain_id);
       }

       for(i = 0; i < chain.channel_count; i++){
           stop_dma_transfer_chain(&transfers[i]);
       }

       remove_dma_proc_chain(PROC_FILE);
}

module_init(dma_module_init);
module_exit(dma_module_exit);

MODULE_AUTHOR("Texas Instruments");
MODULE_LICENSE("GPL");
