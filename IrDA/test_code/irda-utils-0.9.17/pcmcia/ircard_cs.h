/*********************************************************************
 *                
 * Filename:      ircard_cs.h
 * Version:       
 * Description:   
 * Status:        Experimental.
 * Author:        Dag Brattli <dagb@cs.uit.no>
 * Created at:    Fri Apr  2 15:59:20 1999
 * Modified at:   Wed Jun 30 01:00:45 1999
 * Modified by:   Dag Brattli <dagb@cs.uit.no>
 * 
 *     Copyright (c) 1999 Dag Brattli, All Rights Reserved.
 *     
 *     This program is free software; you can redistribute it and/or 
 *     modify it under the terms of the GNU General Public License as 
 *     published by the Free Software Foundation; either version 2 of 
 *     the License, or (at your option) any later version.
 * 
 *     This program is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *     GNU General Public License for more details.
 * 
 *     You should have received a copy of the GNU General Public License 
 *     along with this program; if not, write to the Free Software 
 *     Foundation, Inc., 59 Temple Place, Suite 330, Boston, 
 *     MA 02111-1307 USA
 *     
 ********************************************************************/

#ifndef IRCARD_CS_H
#define IRCARD_CS_H

#include <linux/time.h>

/*#define PIO_THRESHOLD       4000000*/
#define PIO_THRESHOLD       1152000

/* 
 * FIR Control Registers 
 */

#define MCR                  0x00 /* Master Control Register */
#define  MCR_INTR_EN         0x80 /* FIR interrupt enable */
#define  MCR_TX_EN           0x40 /* Transmit enable */
#define  MCR_RX_EN           0x20 /* Receive enable */
#define  MCR_BANK_MASK       0x1f

/* Bank 0 */
#define MSR                    0x01 /* Master Status Register */
#define  MSR_TCV_I             0x80 /* Tranceiver interrupt */
#define  MSR_TMR_I             0x40 /* Timer interrupt */
#define  MSR_TX_I              0x20 /* Transmit interrupt */
#define  MSR_RX_I              0x10 /* Receive interrupt */
#define   MSR_I_MASK           0xf0
#define  MSR_RX_COND_ID        0x08 /* Rx special condition */
#define  MSR_RX_DATA_ID        0x0a /* Rx data available */
#define  MSR_TX_EMPTY_ID       0x0c /* Tx buffer empty */
#define  MSR_TX_COND_ID        0x0e /* Tx special condition */
#define   MSR_ID_MASK          0x0e

#define  MISC                  0x01 /* Miscellaneous Control Register */
#define   MISC_XMIT            0x80 /* Use channel 2 for send */
#define   MISC_RECV            0x40 /* Use channel 1 for receive */

#define RX_FIFO                0x02
#define TX_FIFO                0x02

#define TX_CTRL1               0x03 /* Transmit control register 1 */
#define  TX_CTRL1_RTS          0x80 /* Transmit request to send (RTS) */
#define  TX_CTRL1_FIFO_RDY_I   0x40 /* Transmit FIFO ready interrupt enable */
#define  TX_CTRL1_UNDR_EOM_I   0x20 /* Transmit underrun/eom int. enable */
#define  TX_CTRL1_FIFO_LVL     0x10 /* Transmit FIFO level */
#define  TX_CTRL1_AUTO_RST_RTS 0x08 /* Auto reset RTS */
#define  TX_CTRL1_AUTO_RST_EOM 0x04 /* Auto reset EOM */
#define  TX_CTRL1_TX_IDLE      0x02 /* TX idle */
#define  TX_CTRL1_UNDR_ABORT   0x01 /* Transmit underrun abort */

#define TX_CTRL2               0x04 /* Transmit control register 2 */
#define  TX_CTRL2_EN_TX_CRC    0x40 /* Enable transmit CRC */
#define  TX_CTRL2_EARLY_EOM_16  0x01
#define  TX_CTRL2_EARLY_EOM_32  0x02
#define  TX_CTRL2_EARLY_EOM_64  0x03
#define  TX_CTRL2_EARLY_EOM_128 0x04
#define  TX_CTRL2_EARLY_EOM_256 0x05
#define  TX_CTRL2_EARLY_EOM_512 0x06
#define  TX_CTRL2_EARLY_EOM_1K  0x07

#define TX_STAT                0x05 /* Transmit status register */
#define  TX_STAT_FIFO_UNDR     0x08 /* Transmit FIFO underrun */
#define  TX_STAT_EOM           0x04 /* Transmit end of message */
#define  TX_STAT_FIFO_RDY      0x02 /* Transmit FIFO ready */
#define  TX_STAT_EARLY_EOM     0x01 /* Transmit early end of message */

#define RX_CTRL                0x06 /* Receive control register */
#define  RX_CTRL_COND_I        0x01 /* Rx special condition interrupt */
#define  RX_CTRL_FIFO_READY    0x02 /* Rx FIFO ready */
#define  RX_CTRL_EN_CRC        0x40 /* Enable checking of CRC */

#define RX_STAT                0x07 /* Receive status register */
#define  RX_STAT_DATA_AVAIL    0x08 /* Receive data available */
#define  RX_STAT_EOF           0x10 /* Receive end of frame */
#define  RX_STAT_FIFO_OVRN     0x20 /* FIFO overrun interrupt enable */
#define  RX_STAT_FRAME_ERROR   0x40 /* Rx Frame error */
#define  RX_STAT_ABORT         0x80 /* Rx abort sequence detected */
#define   RX_STAT_MASK         0xfc

#define RESET                  0x07 /* Reset command register */
#define  RESET_ENTER_HUNT_MODE 0x10
#define  RESET_RX_FIFO_PTR     0x20
#define  RESET_RX_SPEC_BITS    0x30
#define  RESET_RX_RFP          0x40
#define  RESET_TX_SPEC_BITS    0x50
#define  RESET_TX_FIFO_PTR     0x60
#define  RESET_HARDWARE        0x70

/* Bank 1 */
#define FRAME_ADDR            0x01 /* Frame address register */
#define RX_BYTE_COUNT_LOW     0x02 /* Receive byte count low byte */
#define RX_BYTE_COUNT_HIGH    0x03 /* Receive byte count high byte */
#define RX_RING_FRM_PTR_LOW   0x04 /* Receive ring frame pointer low byte */
#define RX_RING_FRM_PTR_HIGH  0x05 /* Receive ring frame pointer high byte */
#define TX_BYTE_COUNT_LOW     0x06 /* Transmit byte count low byte */
#define TX_BYTE_COUNT_HIGH    0x07 /* Transmit byte count high byte */

/* Bank 2 */
#define IR_CONF               0x01 /* Infrared configuration register 1 */
#define  IR_CONF_SIR          0x00
#define  IR_CONF_MIR          0x02
#define  IR_CONF_FIR          0x04
#define  IR_CONF_MIR_1152     0x00
#define  IR_CONF_MIR_576      0x10

#define IR_TRANS_CTRL          0x02 /* Infrared tranceiver control register */
#define  IR_TRANS_CTRL_TXD_FRC 0x02
#define  IR_TRANS_CTRL_XCVROFF 0x10

#define IR_CONF2              0x03 /* Infrared configuration register 2 */
#define  IR_CONF2_IRQ_DEF     0x00 /* Default IRQ merging */
#define  IR_CONF2_IRQ_UIRQ    0x01 /* Merge IRQs at UART pin */

#define TMR                   0x04 /* General purpose timer register */

/* Bank 3 */
#define SH_MEM_PAGE           0x01 /* Shared memory page register */

#define TX_DMA_ADDR_LOW       0x02 /* Tx DMA start address (low byte) */
#define TX_DMA_ADDR_HIGH      0x03 /* Tx DMA start address (high byte) */
#define  PAGE_DISABLE         0x40 /* Disable shared memory paging */

#define REVISION_ID           0x07 /* Revision ID */

/* Offset to transmit part of shared memory */
/* #define SHMEM_TX_START 0x1000 */
#define SHMEM_TX_START 0x4000
#define MAX_WINDOW 8

/* Just to make sure we are sane */
#define IRCARD_MAGIC 92612

struct frame_cb {
	int start; /* Start of frame in sh-mem */
	int len;   /* Lenght of frame in sh-mem */
};

struct shm_queue_t {
	struct frame_cb queue[MAX_WINDOW]; /* Info about frames in queue */
	int             ptr;               /* Currently being sent */
	int             len;               /* Lenght of queue */
	int             free;              /* Next free slot */
	int             tail;              /* Next free start in shmem */
};

struct ircard_cb {
	int magic;
 	struct irda_device  idev;
	dev_node_t	    node;
	int		    stop;

	struct shm_queue_t  tx_q;    /* Transmit queue info */
	struct shm_queue_t  rx_q;    /* Receive queue info */
	int                 data;    /* Ptr to last frame received */
	struct timeval      stamp;
	struct timeval      now;
};

/*
 * Function ircard_set_mcr(iobase, itr, bank)
 *
 *    Sets the value of the MCR register. Itr is the interrupt, transmit
 *    and receive bits (7-5) of MCR. Bank is bank bits (4-0) of MCR.
 */
static inline void ircard_set_mcr(int iobase, __u8 itr, __u8 bank)
{
	outb(itr|bank, iobase+MCR);
}

/*
 * Function __switch_bank (iobase, bank)
 *
 *    Switches to requested bank, but does not preserve the previous contents
 *    of MCR. Returns nothing.
 */
static inline void __switch_bank(int iobase, __u8 bank)
{
	outb(bank, iobase+MCR);
}

/*
 * Function switch_bank (iobase, bank)
 *
 *    Switches to requested bank. The original contents of the interrups, 
 *    transmit, and receive bits of MCR is preserved. Returns the the original
 *    bank used before the call.
 */
static inline __u8 switch_bank(int iobase, __u8 bank)
{
	__u8 old_bank;
	__u8 mcr;

	/* Read contents of MCR */
	mcr = inb(iobase+MCR);

	/* Extract original bank */
	old_bank = mcr & MCR_BANK_MASK;

	ircard_set_mcr(iobase, mcr & ~MCR_BANK_MASK, bank);

	return old_bank;
}

/* Makes the code more readable */
#define restore_bank switch_bank

#endif

