/*
 * DMA Test module - Querying information from a transfer
 *
 * The following testcode queries the status of a transfer before starting,
 * when it is on going and when it is finished.
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
#define TIME_AFTER_START 100
#define PROC_FILE "driver/dma_transfer_query"

static struct dma_transfer transfers[TRANSFER_COUNT];
static struct dma_query queries[TRANSFER_COUNT];

/*
 * Determines if the transfers have finished
 */
static int get_transfers_finished(void){
       if(!transfers[0].finished){
           return 0;
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
       /* Create the transfer for the test */
       transfers[0].device_id = OMAP_DMA_NO_DEVICE;
       transfers[0].sync_mode = OMAP_DMA_SYNC_ELEMENT;
       transfers[0].data_burst = OMAP_DMA_DATA_BURST_DIS;
       transfers[0].data_type = OMAP_DMA_DATA_TYPE_S8;
       transfers[0].endian_type = DMA_TEST_LITTLE_ENDIAN;
       transfers[0].addressing_mode = OMAP_DMA_AMODE_POST_INC;
       transfers[0].priority = DMA_CH_PRIO_LOW;
       /* Use a big buffer so we have time to stop the transfer */
       transfers[0].buffers.buf_size = 1024 * 1 * 1;

       /* Request a dma transfer */
       error = request_dma(&transfers[0]);
       if( error ){
           set_test_passed(0);
           return 1;
       }

       /* Request 2 buffer for the transfer and fill them */
       error = create_transfer_buffers(&(transfers[0].buffers));
       if( error ){
           set_test_passed(0);
           return 1;
       }
       fill_source_buffer(&(transfers[0].buffers));

       /* Setup the dma transfer parameters */
       setup_dma_transfer(&transfers[0]);

       /* Query data before the transfer starts */
       printk("Query data for transfer id %d before it starts\n",
              transfers[0].transfer_id);
       error = dma_channel_query(&transfers[0], &queries[0]);
       if( error ){
           set_test_passed(0);
           return 1;
       }

       /* Start the transfer */
       start_dma_transfer(&transfers[0]);

       /* Wait a very short time then stop the transfer */
       printk("Waiting %dms before querying data again for transfer"
              " id %d\n", TIME_AFTER_START, transfers[0].transfer_id);
       mdelay(TIME_AFTER_START);

       /* Query transfer state while it is on going */
       error = dma_channel_query(&transfers[0], &queries[0]);
       if( error ){
           set_test_passed(0);
           return 1;
       }

       /* Poll if the transfer has finished */
       for(i = 0; i < TRANSFER_POLL_COUNT; i++){
            if(get_transfers_finished()){
               mdelay(TRANSFER_POLL_TIME);
               /* Query transfer state for the finished transfer */
               error = dma_channel_query(&transfers[0], &queries[0]);
               if( error || !transfers[0].data_correct){
                   set_test_passed(0);
                   return 1;
               }
               break;
            }else{
               mdelay(TRANSFER_POLL_TIME);
            }
       }

       /* At this point the transfer data was requested 3 times, this is ok */
       set_test_passed(1);

       return 0;
}

/*
 * Function called when the module is removed
 */
static void __exit dma_module_exit(void) {
       stop_dma_transfer(&transfers[0]);
       remove_dma_proc(PROC_FILE);
}

module_init(dma_module_init);
module_exit(dma_module_exit);

MODULE_AUTHOR("Texas Instruments");
MODULE_LICENSE("GPL");
