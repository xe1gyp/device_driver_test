/*
 * DMA Test module - Trying to unlink the channels which are not linked together.
 *
 * The following testcode checks that the constant addressing mode
 * works correctly for different buffer sizes (even and odd sizes).
 *
 * History:
 * 28-02-2010	Shilpa Maddi	Initial version of the testcode
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

#define TRANSFER_COUNT 2
#define TRANSFER_POLL_COUNT 60
#define TRANSFER_POLL_TIME 1500
#define PROC_FILE "driver/unlink_nonlinked"

static struct dma_transfer transfers[TRANSFER_COUNT];

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
             printk("Transfer id %d failed\n", transfers[i].transfer_id);
         }
     }

     if(!error){
         set_test_passed(1);
     }else{
         set_test_passed(0);
     }
}

/*
 * Function used to verify the source an destination buffers of a dma transfer
 * are equal in content
 */
int verify_buffers(struct dma_buffers_info *buffers) {
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
        /*
         * Increment the pointer to the next data, only the destination
         * since we are using the constant addressing
         */
        dest_address++;
    }
    return 0;
}

/*
 * Callback function that dma framework will invoke after transfer is done
 */
void dma_callback(int transfer_id, u16 transfer_status, void *data) {
       struct dma_transfer *transfer = (struct dma_transfer *) data;
       int error = 1;
       transfer->data_correct = 0;
       transfer->finished = 1;
       stop_dma_selflink_transfer(transfer->transfer_id);
       printk("\n The transfer for link for %d is stopped", transfer->transfer_id);
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
		return;
      }
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
 * Function called when the module is initialized
 */
static int __init dma_module_init(void) {
       int error;
       int i = 0;
       /* Create the proc entry */
       create_dma_proc(PROC_FILE);

       for(i = 0; i < TRANSFER_COUNT; i++){

           /* Create the transfer for the test */
           transfers[i].device_id = OMAP_DMA_NO_DEVICE;
           transfers[i].sync_mode = OMAP_DMA_SYNC_ELEMENT;
           transfers[i].data_burst = OMAP_DMA_DATA_BURST_DIS;
           transfers[i].data_type = OMAP_DMA_DATA_TYPE_S8;
           transfers[i].endian_type = DMA_TEST_LITTLE_ENDIAN;
           transfers[i].addressing_mode = OMAP_DMA_AMODE_CONSTANT;
           transfers[i].priority = DMA_CH_PRIO_HIGH;
           transfers[i].buffers.buf_size = (10240 * (i+1)*(i+1)) + i % 2;
	   /* Request a dma transfer */
           error = request_dma(&transfers[i]);
           if( error ){
               set_test_passed(0);
               return 1;
           }
		//printk("\n Linking transfer ID %d with itself \n", transfers[i].transfer_id);

		//dma_link_lch(&transfers[i]);

           /* Request 2 buffer for the transfer and fill them */
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
	printk("\n In the exit DMA module \n");
              }
       remove_dma_proc(PROC_FILE);
}

module_init(dma_module_init);
module_exit(dma_module_exit);

MODULE_AUTHOR("Texas Instruments");
MODULE_LICENSE("GPL");
