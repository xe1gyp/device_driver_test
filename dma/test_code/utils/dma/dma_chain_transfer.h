/*
 * DMA Chained transfer common functions
 *
 * The following code has the headers for the common functions
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


#include <linux/dma-mapping.h>
#include <linux/proc_fs.h>
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/delay.h>
#include <linux/init.h>
#include <asm/io.h>
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,32))
 #include <plat/dma.h>
#elif (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,27))
 #include <mach/dma.h>
#else
 #include <asm/arch/dma.h>
#endif

#ifndef _DMA_CHAINED_TRANSFER_H
#define _DMA_CHAINED_TRANSFER_H

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,27))
    #define DMA_TEST_LITTLE_ENDIAN OMAP_DMA_LITTLE_ENDIAN
    #define DMA_TEST_BIG_ENDIAN OMAP_DMA_BIG_ENDIAN
#else
    #define DMA_TEST_LITTLE_ENDIAN LITTLE_ENDIAN
    #define DMA_TEST_BIG_ENDIAN BIG_ENDIAN
#endif

/*
 * An struct that represents the buffers used for the transfer
 */
struct dma_buffers_info{
       unsigned int src_buf; /* virtual source buffer address */
       unsigned int src_buf_phys; /* physycal source buffer address */
       unsigned int dest_buf; /* virtual destination buffer address */
       unsigned int dest_buf_phys; /* physycal destination buffer address */
       unsigned int buf_size; /* buffer size in bytes */
};

/*
 * An struct that represents a dma transfer
 */
struct dma_transfer{
       int request_success; /* whether or not the dma request succeeded */
       int chain_id;
       int chained_id; /* Generated id for each chained transfer */
       int rounds;
       int elements_in_frame;
       int frame_count;
       int finished;
       int data_correct;
       struct dma_buffers_info buffers;
};

/*
 * An struct that represents a dma chain
 */
struct dma_chain{
       int channel_count; /* number of channels to be chained */
       int device_id;
       int chain_type;
       int chain_id;
       int request_success; /* whether or not the dma request succeeded */
       int finished; /* whether or not the transfer is finished */
       int data_type;
       int sync_mode; /* synchronization mode */
       int addressing_mode;
       int data_burst;
       struct omap_dma_channel_params channel_params;
};

/*
 * Set if the test passed or not
 */
void set_test_passed_chain(int passed);

/*
 * Creates a read proc entry in the procfs
 */
void create_dma_proc_chain(char *proc_name);

/*
 * Removes a proc entry from the procfs
 */
void remove_dma_proc_chain(char *proc_name);

/*
 * Function used to verify the source an destination buffers of a dma transfer
 * are equal in content
 */
int verify_buffers_chain(struct dma_buffers_info *buffers);

/*
 * Callback function that dma framework will invoke after transfer is done
 */
void dma_callback_chain(int transfer_id, u16 transfer_status, void *data);

/*
 * This function allocates 2 dma buffers with the same size for the source
 * and destination.
 */
int create_transfer_buffers_chain( struct dma_buffers_info *buffers);

/*
 * Fill the source buffer with bytes using a pseudo-random value generator
 */
void fill_source_buffer_chain(struct dma_buffers_info *buffers);

/*
 * Requests a dma chain transfer
 */
int request_dma_chain(struct dma_chain *chain_params);

/*
 * Configures the global parameters for dma transfers
 */
void setup_dma_chain(struct dma_chain *chain_param);

/*
 * Starts a dma chain transfer
 */
int start_dma_chain(struct dma_chain *chain_params);

void map_to_phys_buffers(struct dma_buffers_info *buffers);

void unmap_phys_buffers(struct dma_buffers_info *buffers);

/*
 * Chains a transfer to a pre-created chain
 */
int chain_transfer(struct dma_chain *chain_params,
       struct dma_transfer *transfer);

/*
 * Stops a dma transfer and frees used resources
 */
void stop_dma_transfer_chain(struct dma_transfer *transfer);

/*
 * Stops a dma chain and frees used resources
 */
void stop_dma_chain(struct dma_chain *chain);

/*
 * Specifies the maximum rounds when using static chaining
 * Example: 3 transfers x 2 rounds = 6 transfers total
 */
void set_max_rounds(int rounds);

#endif
