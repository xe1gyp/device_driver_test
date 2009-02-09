/*
 * DMA Test module - Addressing mode
 *
 * The following testcode checks that the single index addressing mode
 * works correctly for different buffer sizes (even and odd sizes).
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

#define TRANSFER_COUNT 13
#define TRANSFER_POLL_COUNT 60
#define TRANSFER_POLL_TIME 1500
#define PROC_FILE "driver/dma_addrmode_singleidx"

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
void setup_dma_transfer(struct dma_transfer *transfer){

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
                0x1, /* Element Index (EI) set to 1 for single idx */
                0x0);

        omap_set_dma_src_endian_type(
                transfer->transfer_id,
                transfer->endian_type);

        omap_set_dma_src_burst_mode(
                transfer->transfer_id,
                transfer->data_burst);

        /* Configure the destination parameters */
        omap_set_dma_dest_params(
                transfer->transfer_id,
                0,
                OMAP_DMA_AMODE_POST_INC, /* To check the buffer sequentially */
                transfer->buffers.dest_buf_phys,
                0x0,
                0x0);

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

        printk(" Transfer with id %d is ready\n", transfer->transfer_id);
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
           transfers[i].addressing_mode = OMAP_DMA_AMODE_SINGLE_IDX;
           transfers[i].priority = DMA_CH_PRIO_HIGH;
           transfers[i].buffers.buf_size = (10240 * (i+1)*(i+1)) + i % 2;

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
