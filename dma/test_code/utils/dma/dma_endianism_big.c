/*
 * DMA Test module - Endianism
 *
 * The following testcode checks that changing the source transfer buffer
 * endianism to big endian works correctly.
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

#define TRANSFER_COUNT 1
#define TRANSFER_POLL_COUNT 60
#define TRANSFER_POLL_TIME 1500
#define PROC_FILE "driver/dma_endianism_big"

/* This macro swaps the endianness value */
#define to_big_endian(x) \
({ \
         u32 __x = (x); \
	         ((u32)( \
		  (((u32)(__x) & (u32)0x0000ffffUL) << 16) | \
		  (((u32)(__x) & (u32)0xffff0000UL) >> 16) )); \
 })

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
    u32 *src_address = (u32*) buffers->src_buf;
    u32 *dest_address = (u32*) buffers->dest_buf;

    /* Iterate through the source and destination buffers */
    for (i = 0; i < buffers->buf_size / 4; i++) {
        /* Compare the data in the src and dest, src is big endian */
        if (*src_address != to_big_endian(*dest_address)) {
            printk("Source buffer at 0x%x = %d , destination buffer at 0x%x = "
                   "%d, big endian value for destination should be = %d\n",
                   buffers->src_buf, *src_address,
                   buffers->dest_buf, *dest_address,
                   to_big_endian(*dest_address));
            return 1; /* error, buffers differ */
        }
        src_address++;
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
       printk("\nTransfer complete for id %d, checking destination buffer\n",
           transfer->transfer_id);
       /* Check if the transfer numbers are equal */
       if(transfer->transfer_id != transfer_id){
           printk(" WARNING: Transfer id %d differs from the one"
                " received in callback (%d)\n", transfer->transfer_id,
                transfer_id);
       }
	unmap_phys_buffers(&transfer->buffers);

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
 * Setup the source, destination and global transfer parameters
 */
void setup_dma_transfer_(struct dma_transfer *transfer){

       /* Determine the elements present in a frame */
       printk("Setting up transfer id %d\n", transfer->transfer_id);
       transfer->frame_count = 1;
       switch(transfer->data_type){
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
       }

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
                0x0,
                0x0);

        /*omap_set_dma_src_endian_type(
                transfer->transfer_id,
                DMA_TEST_BIG_ENDIAN);*/

        omap_set_dma_src_burst_mode(
                transfer->transfer_id,
                transfer->data_burst);

        /* Configure the destination parameters */
        omap_set_dma_dest_params(
                transfer->transfer_id,
                0,
                transfer->addressing_mode,
                transfer->buffers.dest_buf_phys,
                0x0,
                0x0);

        /*omap_set_dma_dst_endian_type(
                transfer->transfer_id,
                DMA_TEST_LITTLE_ENDIAN);*/

        omap_set_dma_dest_burst_mode(
                transfer->transfer_id,
                transfer->data_burst);


        /* Global dma configuration parameters */
        omap_dma_set_global_params(
                0x3,
                0x40,
                0);

        /* Transfer priority */
        omap_dma_set_prio_lch(
                transfer->transfer_id,
                transfer->priority,
                transfer->priority);

        printk(" Transfer with id %d is ready\n", transfer->transfer_id);
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
           transfers[i].data_type = OMAP_DMA_DATA_TYPE_S16;
           transfers[i].addressing_mode = OMAP_DMA_AMODE_POST_INC;
           transfers[i].priority = DMA_CH_PRIO_HIGH;
           transfers[i].buffers.buf_size = (1024 * 1024);

           /* Request a dma transfer */
           error = request_dma(&transfers[i]);
           if( error ){
               set_test_passed(0);
               return 1;
           }

           /* Request 2 buffer for the transfer and fill them */
           error = create_transfer_buffers(&(transfers[i].buffers));
           if( error ){
               set_test_passed(0);
               return 1;
           }
           fill_source_buffer(&(transfers[i].buffers));

           /* Setup the dma transfer parameters */
           setup_dma_transfer_(&transfers[i]);
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
