/*
 * DMA Test module - Stopping an ongoing transfer
 *
 * The following testcode checks that stopping an already started
 * transfer is possible.
 *
 * History:
 * 28-01-2009	Gustavo Diaz	Initial version of the testcode
 *
 * Copyright (C) 2004-2009 Texas Instruments, Inc
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
#define TIME_BEFORE_STOP 50
#define PROC_FILE "driver/dma_stop_transfer_twice"

static struct dma_transfer transfers[TRANSFER_COUNT];
static struct dma_query queries[TRANSFER_COUNT];

/*
 * Checks the transfer did not complete.
 */
static void check_test_passed(void){
     int error = 0;

     if(transfers[0].finished){
         printk("The transfer id %d was not supposed to be completed\n",
               transfers[0].transfer_id);
         error = 1;
     }else{
         printk("The transfer id %d is not completed\n",
               transfers[0].transfer_id);
     }

     if(!error){
         set_test_passed(1);
     }else{
         set_test_passed(0);
     }
}

/*
 * Function called when the module is initialized
 */
static int __init dma_module_init(void) {
       int error;
       /* Create the proc entry */
       create_dma_proc(PROC_FILE);
       /* Create the transfer for the test */
       transfers[0].device_id = OMAP_DMA_NO_DEVICE;
       transfers[0].sync_mode = OMAP_DMA_SYNC_ELEMENT;
       transfers[0].data_burst = OMAP_DMA_DATA_BURST_DIS;
       transfers[0].data_type = OMAP_DMA_DATA_TYPE_S8;
       transfers[0].endian_type = DMA_TEST_LITTLE_ENDIAN;
       transfers[0].addressing_mode = OMAP_DMA_AMODE_POST_INC;
       transfers[0].dst_addressing_mode = OMAP_DMA_AMODE_POST_INC;
       transfers[0].priority = DMA_CH_PRIO_LOW;
       /* Use a big buffer so we have time to stop the transfer */
       transfers[0].buffers.buf_size = 1024 * 1024 * 2;

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
       /* Start the transfer */
       start_dma_transfer(&transfers[0]);

       /* Wait a very short time then stop the transfer */
       printk("Waiting %dms before stopping transfer id %d\n",
               TIME_BEFORE_STOP, transfers[0].transfer_id);
       mdelay(TIME_BEFORE_STOP);
       printk("Stopping transfer id %d before completion\n",
               transfers[0].transfer_id);
       stop_dma_transfer(&transfers[0]);
       stop_dma_transfer(&transfers[0]);	
       check_test_passed();

       return 0;
}

/*
 * Function called when the module is removed
 */
static void __exit dma_module_exit(void) {
	/* Dummy exit */
}

module_init(dma_module_init);
module_exit(dma_module_exit);

MODULE_AUTHOR("Texas Instruments");
MODULE_LICENSE("GPL");
