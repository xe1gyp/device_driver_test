/*
 * DMA Test module - Burst mode
 *
 * The following testcode checks that the burst mode of 4 bytes works for
 * data transfer types 8, 16 ands 32 bits.
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

#define TRANSFER_COUNT 3
#define TRANSFER_POLL_COUNT 60
#define TRANSFER_POLL_TIME 1500
#define PROC_FILE "driver/dma_burst_mode4"

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
 * Function called when the module is initialized
 */
static int __init dma_module_init(void) {
       int error;
       int i = 0;
       /* Create the proc entry */
       create_dma_proc(PROC_FILE);
       transfers[0].data_type = OMAP_DMA_DATA_TYPE_S8;
       transfers[1].data_type = OMAP_DMA_DATA_TYPE_S16;
       transfers[2].data_type = OMAP_DMA_DATA_TYPE_S32;

       for(i = 0; i < TRANSFER_COUNT; i++){

           /* Create the transfer for the test */
           transfers[i].device_id = OMAP_DMA_NO_DEVICE;
           transfers[i].sync_mode = OMAP_DMA_SYNC_ELEMENT;
           transfers[i].data_burst = OMAP_DMA_DATA_BURST_4;
           transfers[i].endian_type = DMA_TEST_LITTLE_ENDIAN;
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
