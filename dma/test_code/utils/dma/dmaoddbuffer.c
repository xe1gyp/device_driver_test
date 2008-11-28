/*
 * /vobs/wtbu/OMAPSW_L/linux/test/device_driver_test/dma/test_code/dmaoddbuffer.c
 *
 * Dma Test Module for odd buffer sizes
 *
 * Copyright (C) 2004-2008 Texas Instruments, Inc.
 *
 * This package is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * THIS PACKAGE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 * History:
 * -------
 * Original based on Tony L's code(see documentation for additional info)
 * Modified by Kevin Hilman for OMAP1
 * Modified by Nishanth Menon for OMAP2
 * Modified by Anand Gadiyar for OMAP3 and to align with Open Source
 * Modified by Gustavo Diaz Prado to verify DMA supports buffers with odd size
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/proc_fs.h>

#include <asm/io.h>

#include <linux/version.h>

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,00))
#include <linux/dma-mapping.h>
#endif

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,27))
#include <mach/dma.h>
#else
#include <asm/arch/dma.h>
#endif

/* Debug Levels 0 - 4 - 0 prints more information and 4 the least */
#define DEBUG_MESSAGE(VAL, ARGS...) if (VAL>=debug){ printk( "%s[%d]:",__FUNCTION__,__LINE__);printk (ARGS);printk ("\n");}
#define DEBUG_FUNCTION_IN() DEBUG_MESSAGE(2,"Entry");
#define DEBUG_FUNCTION_OUT(RET) DEBUG_MESSAGE(2,"Exit(%d)",RET);

/* Debug Default debug level */
#define DEFAULT_DEBUG_LVL 5

/* Test using the page APIs  make this 0 to use coherent allocation */
#define PAGE_TEST 0

/* PROC File location */
#define PROC_FILE "driver/dmatest"

/* load-time options */
int debug = DEFAULT_DEBUG_LVL;
int startup = 1; /* whether to start DMA channels during module init */
int channels = 8;
int maximum_transfers = 5; /* max transfers per channel */
int nochain = 0; /* During Linking - -1= dont chain till channel linking, or 1=dont chain after channel linking */
#if defined(CONFIG_ARCH_OMAP24XX) || defined(CONFIG_ARCH_OMAP34XX)
int channel_priority = -1; /* Set high priority for which channel? */
int query_idx = 0; /* Set high priority for which channel? */
#endif

#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,00))
MODULE_PARM(debug, "i");
MODULE_PARM(startup, "i");
MODULE_PARM(channels, "i");
MODULE_PARM(maximum_transfers, "i");
#if defined(CONFIG_ARCH_OMAP24XX) || defined(CONFIG_ARCH_OMAP34XX)
MODULE_PARM(channel_priority, "i");
MODULE_PARM(query_idx, "i");
#endif
#else
module_param(debug, int, 0444);
module_param(startup, int, 0444);
module_param(channels, int, 0444);
module_param(maximum_transfers, int, 0444);
#if defined(CONFIG_ARCH_OMAP24XX) || defined(CONFIG_ARCH_OMAP34XX)
module_param(channel_priority, int, 0444);
module_param(query_idx, int, 0444);
#endif
#endif

#define MAX_CHANNELS	32

struct dma_test_s {
    int dev_id;
    int dma_ch, next_ch;
    unsigned int src_buf, dest_buf;
    unsigned int src_buf_phys, dest_buf_phys;
    unsigned int count, good;
    unsigned int max_transfers;
    unsigned int buffer_size;
};
static struct dma_test_s dma_test[MAX_CHANNELS];

/* Function used to verify the source an destination buffers of a dma transfer
   are equal in content */
static int verify_buffer(unsigned int *src_buf, unsigned int *dest_buf, unsigned int buffer_size) {
    unsigned int i;
    u8 *src_data = (u8*)src_buf, *dest_data = (u8*) dest_buf;

    DEBUG_FUNCTION_IN();
    /* Iterate through the data from the source and destination buffers */
    DEBUG_MESSAGE(5, " Verifying source buffer at = %p, destination = %p\n with size %d",
     src_buf, dest_buf, buffer_size);
    for (i = 0; i < buffer_size; i++) {
        DEBUG_MESSAGE(1, "src data = %x, dest data = %x\n", *src_data, *dest_data);
        /* Compare the data in the source and destination */
        if (*src_data != *dest_data) {
            DEBUG_MESSAGE(5, "!!!! DMA verify failed at offset 0x%x.  "
                    "Expected %x, got %x",
                    (unsigned int) src_data - (unsigned) src_buf, *src_data, *dest_data);
            DEBUG_FUNCTION_OUT(1);
            return 1;
        }
        /* Increment the pointer to the next data */
        src_data++;
        dest_data++;
    }
    DEBUG_MESSAGE(5, " Verification for source buffer at = %p, with size %d OK!",src_buf,buffer_size);
    DEBUG_FUNCTION_OUT(0);
    return 0;
}

/* This is the callback function the dma framework will call after a dma transfer is done */
static void dma_callback(int logical_channel, u16 channel_status, void *data) {
    /* In data we receive the dma_test array pointer to the dma_test_s structs */
    struct dma_test_s *tests = (struct dma_test_s *) data;
    DEBUG_FUNCTION_IN();

    if (logical_channel == tests->dma_ch) {
        if (((channel_status & OMAP_DMA_BLOCK_IRQ) || (channel_status == 0)) &&
                (verify_buffer
                ((unsigned int *) tests->src_buf,
                (unsigned int *) tests->dest_buf,                
                tests->buffer_size) == 0)) {
            tests->good++;
        } else {
            DEBUG_MESSAGE(3, "dma_callback(lch=%d, ch_status=0x%04x)",
                    logical_channel, channel_status);
        }

        tests->count++;
        DEBUG_MESSAGE(3, "mx=%d cnt=%d ", tests->max_transfers, tests->count);

        if (tests->max_transfers && (tests->count >= tests->max_transfers)) {
            DEBUG_MESSAGE(3, "Stopping %d", tests->dma_ch);
            omap_stop_dma(tests->dma_ch);
        }
    } else {
        DEBUG_MESSAGE(4, "dma_callback(): Unexpected event on channel %d", logical_channel);
    }
    DEBUG_FUNCTION_OUT(0);
}

/* Function called when reading the proc entry. */
static int dmatest_read_procmem(char *buf, char **start, off_t offset,
        int count, int *eof, void *data) {
    int i, length = 0;
    DEBUG_FUNCTION_IN();

    for (i = 0; i < channels; i++) {
        length += sprintf(buf + length, " %2d %4d %4d %4d\n",
                dma_test[i].dma_ch,
                dma_test[i].count,
                dma_test[i].good, dma_test[i].max_transfers);
    }
    DEBUG_FUNCTION_OUT(length);
    return length;
}

/* Function called when the module is being unloaded to make a cleanup */
static void __exit dmatest_cleanup(void) {
    int i;

    DEBUG_FUNCTION_IN();
    /* Free the resources used by the channels involved in the transfer */
    for (i = 0; i < channels; i++) {
        if (dma_test[i].dma_ch >= 0) {
            omap_stop_dma(dma_test[i].dma_ch);
            omap_free_dma(dma_test[i].dma_ch);

            dma_test[i].dma_ch = -1;
        }

        dma_test[i].src_buf_phys = 0;
        if (dma_test[i].src_buf)
#if PAGE_TEST
            free_page(dma_test[i].src_buf);
#else
            DEBUG_MESSAGE(5, "Free src- %p", (void *) dma_test[i].src_buf);
#if (LINUX_VERSION_CODE <= KERNEL_VERSION(2,6,00))
        consistent_free((void *) dma_test[i].src_buf, dma_test[i].buffer_size,
                dma_test[i].src_buf_phys);
#else
        dma_free_coherent(NULL, dma_test[i].buffer_size, (void *) dma_test[i].src_buf,
                dma_test[i].src_buf_phys);
#endif

#endif        
        dma_test[i].dest_buf_phys = 0;
        if (dma_test[i].dest_buf)
#if PAGE_TEST
            free_page(dma_test[i].dest_buf);
#else
            DEBUG_MESSAGE(5, "Free dst- %p", (void *) dma_test[i].dest_buf);
#if (LINUX_VERSION_CODE <= KERNEL_VERSION(2,6,00))
        consistent_free((void *) dma_test[i].dest_buf, dma_test[i].buffer_size,
                dma_test[i].dest_buf_phys);
#else
        dma_free_coherent(NULL, dma_test[i].buffer_size, (void *) dma_test[i].dest_buf,
                dma_test[i].dest_buf_phys);
#endif

#endif
        dma_test[i].buffer_size = 0;
    }
    
    remove_proc_entry(PROC_FILE, NULL);
    DEBUG_FUNCTION_OUT(0);
}

/* Function called when initializing the module. Setup the dma transfers. */
static int __init dmatest_init(void) {
    int i, j; /* used for loop indexes */
    int return_value; /* used to hold the value returned by this function */
    int element_count, frame_count;
    unsigned int *source_buffer_ptr;
    u8 * source_buffer_byte;

    DEBUG_FUNCTION_IN();
    /* Check the requested channels do not pass the maximum allowed channels */
    if (channels > MAX_CHANNELS) {
        DEBUG_MESSAGE(5, "dmatest_init(): channels arg (%d) > MAX_CHANNELS (%d)\n",
                channels, MAX_CHANNELS);
        DEBUG_FUNCTION_OUT(-ENODEV);
        return -ENODEV;
    }

    /* Create /proc entry */
    create_proc_read_entry(PROC_FILE, 0 /* default mode */,
            NULL /* parent dir */,
            dmatest_read_procmem, NULL /* client data */);

    /* Allocate DMA-able buffers */
    for (i = 0; i < channels; i++) {
        DEBUG_MESSAGE(5, "Creating DMA test %d\n", i);
        dma_test[i].count = 0;
        dma_test[i].next_ch = -1;
        /* Set the buffer size for this channel, it is an odd number */
        dma_test[i].buffer_size = (1 << 2 * (i+1))-1;

#if PAGE_TEST
        /* Request dma pages that can be used for dma transfers */
        dma_test[i].src_buf = get_zeroed_page(GFP_KERNEL | GFP_DMA);
        if (!dma_test[i].src_buf) {
            DEBUG_MESSAGE(5, "dmatest_init(): get_zeroed_page() failed.\n");
            return_value = -ENOMEM;
            goto cleanup;
        }
        dma_test[i].dest_buf = get_zeroed_page(GFP_KERNEL | GFP_DMA);
        if (!dma_test[i].dest_buf) {
            DEBUG_MESSAGE(5, "dmatest_init(): get_zeroed_page() failed.\n");
            return_value = -ENOMEM;
            goto cleanup;
        }
        /* Convert the virtual addresses from the buffers to the actual physical addresses */
        dma_test[i].src_buf_phys = virt_to_bus(dma_test[i].src_buf);
        dma_test[i].dest_buf_phys = virt_to_bus(dma_test[i].dest_buf);
#else
#if (LINUX_VERSION_CODE <= KERNEL_VERSION(2,6,00))
        dma_test[i].src_buf =
                (unsigned int) consistent_alloc(GFP_KERNEL | GFP_DMA,
                dma_test[i].buffer_size,
                &(dma_test[i].src_buf_phys));
#else
        dma_test[i].src_buf =
                (unsigned int) dma_alloc_coherent(NULL,
                dma_test[i].buffer_size,
                &(dma_test[i].
                src_buf_phys), 0);
#endif
        if (!dma_test[i].src_buf) {
            DEBUG_MESSAGE(5, "dmatest_init(): consistent_alloc() failed.\n");
            return_value = -ENOMEM;
            goto cleanup;
        }
#if (LINUX_VERSION_CODE <= KERNEL_VERSION(2,6,00))
        dma_test[i].dest_buf =
                (unsigned int) consistent_alloc(GFP_KERNEL | GFP_DMA,
                dma_test[i].buffer_size,
                &(dma_test[i].
                dest_buf_phys));
#else
        dma_test[i].dest_buf =
                (unsigned int) dma_alloc_coherent(NULL,
                dma_test[i].buffer_size,
                &(dma_test[i].
                dest_buf_phys), 0);
#endif
        if (!dma_test[i].dest_buf) {
            DEBUG_MESSAGE(5, "dmatest_init(): consistent_alloc() failed.\n");
#if (LINUX_VERSION_CODE <= KERNEL_VERSION(2,6,00))
            consistent_free((void *) dma_test[i].src_buf, dma_test[i].buffer_size,
                    dma_test[i].src_buf_phys);
#else
            dma_free_coherent(NULL, dma_test[i].buffer_size,
                    (void *) dma_test[i].src_buf,
                    dma_test[i].src_buf_phys);
#endif

            return_value = -ENOMEM;
            goto cleanup;
        }
#endif
        DEBUG_MESSAGE(5, "Allocated buffers src=0x%08x/0x%08x, dest=0x%08x/0x%08x with size %d\n",
                dma_test[i].src_buf,
                dma_test[i].src_buf_phys,
                dma_test[i].dest_buf, dma_test[i].dest_buf_phys,
                dma_test[i].buffer_size);

        /* Setup DMA transfer */
        dma_test[i].dev_id = OMAP_DMA_NO_DEVICE;
        dma_test[i].dma_ch = -1;
        dma_test[i].max_transfers = 0;
        return_value = omap_request_dma(dma_test[i].dev_id, "DMA Test",
                dma_callback, (void *) & dma_test[i],
                &dma_test[i].dma_ch);
        
        /* Check if the dma request failed */
        if (return_value) {
            DEBUG_MESSAGE(5, "dmatest_init(): request_dma() failed: %d\n", return_value);
            dma_test[i].dev_id = 0;
            DEBUG_MESSAGE(5, "WARNING: Only go %d/%d channels.\n", i, channels);
            channels = i;
            break;
        }

        /* Fills the source buffer with some data for the transfer */
        DEBUG_MESSAGE(5, "Pre-filling source buffer %d with %d bytes\n", i, dma_test[i].buffer_size);
        source_buffer_ptr = (unsigned int *) dma_test[i].src_buf;        
        source_buffer_byte = (u8*) source_buffer_ptr;
        for (j = 0; j < dma_test[i].buffer_size; j++) {                        
            source_buffer_byte[j] = (~j << 7) | (dma_test[i].dma_ch << 3) | j;
            DEBUG_MESSAGE(1, "Wrote %p with %x\n", &source_buffer_byte[j], source_buffer_byte[j] );
        }
        DEBUG_MESSAGE(5, "Source buffer for DMA test %d filled\n", i);
        /* Set how many data elements will be transfered from the source buffer
           to the destination buffer. Let's send the complete source buffer. */
        element_count = dma_test[i].buffer_size; //Number of elements in a frame        
        frame_count = 1; //How many frames are in a transfer block
#if defined(CONFIG_ARCH_OMAP24XX) || defined(CONFIG_ARCH_OMAP34XX)
        /* 24XX and 34XX parameters */
        /* Setting S8 data type for the transfer will allow us to use an odd buffer size */
        omap_set_dma_transfer_params(dma_test[i].dma_ch, /* int lch */
                OMAP_DMA_DATA_TYPE_S8, /* int data_type */
                element_count, /* int elem_count */
                frame_count, /* int frame_count */
                OMAP_DMA_SYNC_ELEMENT, /*  int sync_mode */
                dma_test[i].dev_id, /* int dma_trigger */
                0x0); /* int src_or_dst_synch */
        omap_set_dma_src_params(dma_test[i].dma_ch, /* int lch */
                0, /* src_port */
                OMAP_DMA_AMODE_POST_INC, /* post increment int src_amode */
                dma_test[i].src_buf_phys, /*  int src_start */
                0x0, /*  int src_ei */
                0x0); /* int src_fi */
        
        #if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,27))
					omap_set_dma_src_endian_type(dma_test[i].dma_ch, OMAP_DMA_LITTLE_ENDIAN);
				#else
					omap_set_dma_src_endian_type(dma_test[i].dma_ch, LITTLE_ENDIAN);
				#endif
        
        omap_set_dma_src_burst_mode(dma_test[i].dma_ch, OMAP_DMA_DATA_BURST_4 );
        omap_set_dma_dest_params(dma_test[i].dma_ch, /* int lch */
                0, /* dest_port */
                OMAP_DMA_AMODE_POST_INC, /* post increment int dst_amode */
                dma_test[i].dest_buf_phys, /*  int dst_start */
                0x0, /*  int dst_ei */
                0x0); /* int dst_fi */
        omap_set_dma_dest_burst_mode(dma_test[i].dma_ch, OMAP_DMA_DATA_BURST_4 );
#else
        /* 16XX params */
        omap_set_dma_transfer_params(dma_test[i].dma_ch,
                OMAP_DMA_DATA_TYPE_S8,
                element_count,
                frame_count,
                OMAP_DMA_SYNC_ELEMENT);
        omap_set_dma_src_params(dma_test[i].dma_ch,
                OMAP_DMA_PORT_EMIFF,
                OMAP_DMA_AMODE_POST_INC,
                dma_test[i].src_buf_phys);
        omap_set_dma_dest_params(dma_test[i].dma_ch,
                OMAP_DMA_PORT_EMIFF,
                OMAP_DMA_AMODE_POST_INC,
                dma_test[i].dest_buf_phys);
#endif

    }

#if defined(CONFIG_ARCH_OMAP24XX) || defined(CONFIG_ARCH_OMAP34XX)
    /* Set the priority settings for the transfer. 
       Setting the priority of a single channel alone now */
    if (channel_priority >= 0) {
        DEBUG_MESSAGE(5, "Priority to %d 25%% channel reserved ", channel_priority);
        omap_dma_set_global_params(DMA_DEFAULT_ARB_RATE,
                DMA_DEFAULT_FIFO_DEPTH,
                0);
        omap_dma_set_prio_lch(dma_test[channel_priority].dma_ch, DMA_CH_PRIO_HIGH, DMA_CH_PRIO_HIGH);
    }
#endif
    
    /* Determine if the dma transfer should be done during the module initialization */
    if (startup) {
        for (i = 0; i < channels; i++) {
            DEBUG_MESSAGE(5, "Start DMA test %d channel %d\n",
                    i, dma_test[i].dma_ch);
            /* Start the dma transfer */
            omap_start_dma(dma_test[i].dma_ch);
        }
    }

    /* Print information about the channels used in the dma transfer */
    if (query_idx) {
        int lch = 0;
        int src[MAX_CHANNELS];
        int dst[MAX_CHANNELS];
        int ei[MAX_CHANNELS];
        int fi[MAX_CHANNELS];
        for (lch = 0; lch < channels; lch++) {
            src[lch] = omap_get_dma_src_pos(lch);
            dst[lch] = omap_get_dma_dst_pos(lch);
            if (unlikely(omap_get_dma_index(lch, &(ei[lch]), &(fi[lch])))) {
                printk("lch-%d failed src indx\n", lch);
            }
        }
        printk("lch->[src] [dst] [ei/fi] \n");
        for (lch = 0; lch < channels; lch++) {
            printk("%d->[0x%x] [0x%x] [0x%x/0x%x]\n", lch, src[lch], dst[lch], ei[lch], fi[lch]);
        }
    }
    DEBUG_FUNCTION_OUT(0);
    return 0;

cleanup:
    dmatest_cleanup();
    DEBUG_FUNCTION_OUT(return_value);
    return return_value;
}

module_init(dmatest_init);
module_exit(dmatest_cleanup);

MODULE_AUTHOR("Texas Instruments");
MODULE_LICENSE("GPL");
