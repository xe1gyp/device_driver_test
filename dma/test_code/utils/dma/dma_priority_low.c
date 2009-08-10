/*
 * DMA Test module - Transfer Priority
 *
 * The following testcode checks that the transfer priority works
 * correctly. Four single channel transfers are created with the same
 * buffer size, the first two have low priority and the other two
 * high priority. The transfers are started at the same time from
 * the first to the last one, so the low priority transfers are
 * started first then the high priority ones. The expected result is
 * the first low priority transfer should not be the first to finish.
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


#include "dma_single_channel.h"

#define TRANSFER_COUNT 4
#define TRANSFER_POLL_COUNT 60
#define TRANSFER_POLL_TIME 1500
#define PROC_FILE "driver/dma_priority_low"

static struct dma_transfer transfers[TRANSFER_COUNT];
static int transfer_finished_order[TRANSFER_COUNT];
static volatile int transfer_finished_count = 0;

/*
 * Checks that the first transfer to finish is actually not the first in the
 * transfers array with the lowest priority.
 */
static void check_test_passed(void){
     int i;
     int error = 0;
     /* Check that all the transfers finished */
     for(i = 0; i < TRANSFER_COUNT; i++){
         if(!transfers[i].data_correct){
             error = 1;
             printk("Transfer id %d failed\n", transfers[i].transfer_id);
         }
     }

     /* Check the first transfer is not the first one to finish */
     if(!error){
         int first_transfer = transfer_finished_order[0];
         printk("The transfer id %d was the first to finish\n", first_transfer);
         printk("Expected transfer id %d not to be the first one to finish\n",
              transfers[0].transfer_id);
         if(first_transfer == transfers[0].transfer_id){
              /* The low priority transfer finished first */
              error = 1;
         }
     }

     if(!error){
         set_test_passed(1);
     }else{
         set_test_passed(0);
     }
}

/*
 * Callback function that dma framework will invoke after transfer is done
 */
void dma_callback(int transfer_id, u16 transfer_status, void *data) {
       int error;
       struct dma_transfer *transfer = (struct dma_transfer *) data;
       int finished_index = transfer_finished_count++;
       transfer_finished_order[finished_index] = transfer->transfer_id;
       transfer->data_correct = 0;
       transfer->finished = 1;
       printk("\nTransfer complete for id %d, checking destination buffer\n",
           transfer->transfer_id);

       /* Check if the transfer numbers are equal */
       if(transfer->transfer_id != transfer_id){
           printk(" WARNING: Transfer id %d differs from the one"
                " received in callback (%d)\n", transfer->transfer_id,
                transfer_id);
       }

       /* Check the transfer status is acceptable */
       if((transfer_status & OMAP_DMA_BLOCK_IRQ) || (transfer_status == 0)){
           /* Verify the contents of the buffer are equal */
           error = verify_buffers(&(transfer->buffers));
       }else{
           printk(" Verification failed, transfer id %d status is not "
                "acceptable\n", transfer->transfer_id);
           return;
       }

       if(error){
           printk(" Verification failed, transfer id %d source and destination"
                " buffers differ\n", transfer->transfer_id);
       }else{
           printk(" Verification succeeded for transfer id %d\n",
                transfer->transfer_id);
           transfer->data_correct = 1;
       }
}

/*
 * Requests a dma transfer
 */
int request_dma(struct dma_transfer *transfer){
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

/*
 * Determines if the transfers have finished
 */
static int get_transfers_finished(void){
       int i = 0;
       for(i = 0; i < TRANSFER_COUNT; i++){
            if(!transfers[i].finished){
                return 0;
            }
       }
       return 1;
}

/*
 * Function called when the module is initialized
 */
static int __init dma_module_init(void) {
       int error;
       int i;
       /* Create the proc entry */
       create_dma_proc(PROC_FILE);

       /* Create the transfers for the test */
       for(i = 0; i < TRANSFER_COUNT; i++){
           transfers[i].device_id = OMAP_DMA_NO_DEVICE;
           transfers[i].sync_mode = OMAP_DMA_SYNC_ELEMENT;
           transfers[i].data_burst = OMAP_DMA_DATA_BURST_DIS;
           transfers[i].data_type = OMAP_DMA_DATA_TYPE_S8;
           transfers[i].endian_type = DMA_TEST_LITTLE_ENDIAN;
           transfers[i].addressing_mode = OMAP_DMA_AMODE_POST_INC;

           if(i <= 1){
               /* Set the first transfer to low priority */
               transfers[i].priority = DMA_CH_PRIO_LOW;
           }else{
               /* Set the other transfers to high priority */
               transfers[i].priority = DMA_CH_PRIO_HIGH;
           }
           transfers[i].buffers.buf_size = 1024 * 1;

           /* Request a dma transfer */
           error = request_dma(&transfers[i]);
           if( error ){
              set_test_passed(0);
              return 1;
           }

           /* Request 2 buffers for each transfer and fill them */
           error = create_transfer_buffers(&(transfers[i].buffers));
           if( error ){
              set_test_passed(0);
              return 1;
           }
           fill_source_buffer(&(transfers[i].buffers));

           /* Setup the dma transfer parameters */
           setup_dma_transfer(&transfers[i]);
       }

       for(i = 0; i < TRANSFER_COUNT; i++){
           /* Start the transfers */
           start_dma_transfer(&transfers[i]);
       }

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
           set_test_passed(0);
           return 1;
       }

       return 0;
}

/*
 * Function called when the module is removed
 */
static void __exit dma_module_exit(void) {
       int i;
       for(i = 0; i < TRANSFER_COUNT; i++){
               stop_dma_transfer(&transfers[i]);
       }
       remove_dma_proc(PROC_FILE);
}

module_init(dma_module_init);
module_exit(dma_module_exit);

MODULE_AUTHOR("Texas Instruments");
MODULE_LICENSE("GPL");
