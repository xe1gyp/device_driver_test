/*********************************************************************
 *                
 * Filename:      ircard_cs.c
 * Version:       0.8
 * Description:   IrDA FIR device driver for the IRDATA IRCARD which is
 *                using the IBM 31T1502 controller chip
 * Status:        Experimental.
 * Author:        Dag Brattli <dagb@cs.uit.no>
 * Created at:    Fri Apr  2 00:01:11 1999
 * Modified at:   Fri Aug 27 12:30:00 1999
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

#include <pcmcia/config.h>
#include <pcmcia/k_compat.h>

#include <linux/autoconf.h> /* Very important! */
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/ptrace.h>
#include <linux/malloc.h>
#include <linux/string.h>
#include <linux/timer.h>
#include <linux/delay.h>
#include <linux/ioport.h>
#include <asm/io.h>
#include <asm/system.h>

#include <net/irda/irda.h>
#include <net/irda/irmod.h>
#include <net/irda/irda_device.h>
#include <net/irda/irport.h>
#include <net/irda/wrapper.h>

#include <pcmcia/version.h>
#include <pcmcia/cs_types.h>
#include <pcmcia/cs.h>
#include <pcmcia/cistpl.h>
#include <pcmcia/cisreg.h>
#include <pcmcia/ds.h>
#include <pcmcia/mem_op.h>

#include "ircard_cs.h"

#if LINUX_VERSION_CODE < VERSION(2,3,14)
#define net_device device
#endif

static char *version = "ircard_cs.c 0.1 Fri Apr 2 11:21:27 1999 (Dag Brattli)";

/* Parameters that can be set with 'insmod' */

/* The old way: bit map of interrupts to choose from */
/* This means pick from 15, 14, 12, 11, 10, 9, 7, 5, 4, and 3 */
static u_int irq_mask = 0xdeb8;
/* Newer, simpler way of listing specific interrupts */
static int irq_list[4] = { -1 };

/* The IBM 31T1100A tranceiver needs at least 120 us */
static int qos_mtt_bits = 0x0f; /* We have to set it to 500 us */

MODULE_PARM(irq_mask, "i");
MODULE_PARM(irq_list, "1-4i");
MODULE_PARM(qos_mtt_bits, "i");

static void ircard_config(dev_link_t *link);
static void ircard_release(u_long arg);
static int  ircard_event(event_t event, int priority, 
			 event_callback_args_t *args);
static dev_link_t *ircard_attach(void);
static void ircard_detach(dev_link_t *);

static int  ircard_probe(struct irda_device *idev);
static void ircard_change_speed(struct irda_device *idev, __u32 speed);
static int  ircard_hard_xmit(struct sk_buff *skb, struct net_device *dev);
static void ircard_dma_write(struct irda_device *idev, int iobase);
static int  ircard_receive(struct irda_device *idev, int iobase);
static void ircard_interrupt(int irq, void *dev_id, struct pt_regs *regs);
static int  ircard_is_receiving(struct irda_device *idev);
static void ircard_wait_until_sent(struct irda_device *idev);
static int  ircard_net_close(struct net_device *dev);
static int  ircard_net_open(struct net_device *dev);
static int  ircard_net_init(struct net_device *dev);

static dev_info_t dev_info = "ircard_cs";
static dev_link_t *dev_list = NULL;

static void cs_error(client_handle_t handle, int func, int ret)
{
	error_info_t err = { func, ret };
	CardServices(ReportError, handle, &err);
}

/*
 * Function ircard_attach ()
 *
 *    creates an "instance" of the driver, allocating local data structures
 *    for one device. The device is registered with Card Services.
 * 
 */
static dev_link_t *ircard_attach(void)
{
        client_reg_t client_reg;
	dev_link_t *link;
	struct ircard_cb *self;
	int ret, i;
	
	DEBUG(0, __FUNCTION__ "()\n");
	
	/* Initialize the dev_link_t structure */
	link = kmalloc(sizeof(struct dev_link_t), GFP_KERNEL);
	memset(link, 0, sizeof(struct dev_link_t));
	link->release.function = &ircard_release;
	link->release.data = (u_long)link;
	
	/* Interrupt setup */
	link->irq.Attributes = IRQ_TYPE_EXCLUSIVE|IRQ_HANDLE_PRESENT;
	link->irq.IRQInfo1 = IRQ_INFO2_VALID|IRQ_LEVEL_ID;
	if (irq_list[0] == -1)
	     link->irq.IRQInfo2 = irq_mask;
	else
	     for (i = 0; i < 4; i++)
		  link->irq.IRQInfo2 |= 1 << irq_list[i];
	link->irq.Handler = &ircard_interrupt;
	
	/* 
	 * General socket configuration defaults can go here.  In this
	 * client, we assume very little, and rely on the CIS for almost
	 * everything.  In most clients, many details (i.e., number, sizes,
	 * and attributes of IO windows) are fixed by the nature of the
	 * device, and can be hard-wired here.  
	 */
	link->conf.Attributes = 0;
	link->conf.Vcc = 50;
	link->conf.IntType = INT_MEMORY_AND_IO;
	
	/* Allocate space for private device-specific data */
	self = kmalloc(sizeof(struct ircard_cb), GFP_KERNEL);
	memset(self, 0, sizeof(struct ircard_cb));
	self->magic = IRCARD_MAGIC;
	link->priv = self;
	link->irq.Instance = &self->idev;

	/* Register with Card Services */
	link->next = dev_list;
	dev_list = link;
	client_reg.dev_info = &dev_info;
	client_reg.Attributes = INFO_IO_CLIENT | INFO_CARD_SHARE;
	client_reg.EventMask =
	     CS_EVENT_CARD_INSERTION | CS_EVENT_CARD_REMOVAL |
	     CS_EVENT_RESET_PHYSICAL | CS_EVENT_CARD_RESET |
	     CS_EVENT_PM_SUSPEND | CS_EVENT_PM_RESUME;
	client_reg.event_handler = &ircard_event;
	client_reg.Version = 0x0210;
	client_reg.event_callback_args.client_data = link;
	ret = CardServices(RegisterClient, &link->handle, &client_reg);
	if (ret != 0) {
		cs_error(link->handle, RegisterClient, ret);
		ircard_detach(link);
		return NULL;
	}
	
	return link;
}

/*
 * Function ircard_detach (link)
 *
 *    This deletes a driver "instance". The device is de-registered with
 *    Card Services. If it has been released, all local data structures are
 *    freed.  Otherwise, the structures will be freed when the device is
 *    released.
 * 
 */
static void ircard_detach(dev_link_t *link)
{
	   dev_link_t **linkp;
	   struct ircard_cb *self;
	   struct irda_device *idev;
	
	   DEBUG(0, __FUNCTION__ "(0x%p)\n", link);

	   self = (struct ircard_cb *) link->priv;
	   idev = &self->idev;
	   
	   /* Locate device structure */
	   for (linkp = &dev_list; *linkp; linkp = &(*linkp)->next)
		   if (*linkp == link) break;
	   if (*linkp == NULL)
		   return;
	   
	   /*
	    * If the device is currently configured and active, we won't
	    * actually delete it yet.  Instead, it is marked so that when
	    * the release() function is called, that will trigger a proper
	    * detach().
	    */
	   if (link->state & DEV_CONFIG) {
#ifdef PCMCIA_DEBUG
		   printk(KERN_DEBUG "ircard_cs: detach postponed, '%s' "
			  "still locked\n", link->dev->dev_name);
#endif
		   link->state |= DEV_STALE_LINK;
		   return;
	   }
	   
	   /* Break the link with Card Services */
	   if (link->handle)
		   CardServices(DeregisterClient, link->handle);
	   
	   /* Unlink device structure, free pieces */
	   *linkp = link->next;
	   if (self) {		   
		   /* Close irport */
		   irport_stop(idev, idev->io.iobase2);
		   
		   irda_device_close(idev);
		   
		   kfree_s(self, sizeof(struct ircard_cb));
	   }
	   kfree_s(link, sizeof(struct dev_link_t));   
}

#define CS_CHECK(fn, args...) \
while ((last_ret=CardServices(last_fn=(fn),args))!=0) goto cs_failed

#define CFG_CHECK(fn, args...) \
if (CardServices(fn, args) != 0) goto next_entry

/*
 * Function ircard_config (link)
 *
 *    Configure the card and prepare for IO
 *
 */
static void ircard_config(dev_link_t *link)
{
	struct irda_device *idev;
	struct ircard_cb *self;
	client_handle_t handle;
	tuple_t tuple;
	cisparse_t parse;
	int last_fn, last_ret;
	u_char buf[64];
	win_req_t req;
	memreq_t map;
	
	handle = link->handle;
	self = (struct ircard_cb *) link->priv;

	ASSERT(self != NULL, return;);
	ASSERT(self->magic == IRCARD_MAGIC, return;);
	
	DEBUG(0, __FUNCTION__ "(0x%p)\n", link);
	
	/*
	 * This reads the card's CONFIG tuple to find its configuration
	 * registers.
	 */
	tuple.DesiredTuple = CISTPL_CONFIG;
	tuple.Attributes = 0;
	tuple.TupleData = buf;
	tuple.TupleDataMax = sizeof(buf);
	tuple.TupleOffset = 0;
	CS_CHECK(GetFirstTuple, handle, &tuple);
	CS_CHECK(GetTupleData, handle, &tuple);
	CS_CHECK(ParseTuple, handle, &tuple, &parse);
	link->conf.ConfigBase = parse.config.base;
	link->conf.Present = parse.config.rmask[0];
	DEBUG(0, __FUNCTION__ "() present=%x\n", link->conf.Present);
	/* Configure card */
	link->state |= DEV_CONFIG;
	
	/* 
	 * In this loop, we scan the CIS for configuration table entries,
	 * each of which describes a valid card configuration, including
	 * voltage, IO window, memory window, and interrupt settings.
	 * 
	 * We make no assumptions about the card to be configured: we use
	 * just the information available in the CIS.  In an ideal world,
	 * this would work for any PCMCIA card, but it requires a complete
	 * and accurate CIS.  In practice, a driver usually "knows" most of
	 * these things without consulting the CIS, and most client drivers
	 * will only use the CIS to fill in implementation-defined details.
	 * 
	 */
	tuple.DesiredTuple = CISTPL_CFTABLE_ENTRY;
	CS_CHECK(GetFirstTuple, handle, &tuple);
	while (1) {
		cistpl_cftable_entry_t dflt = { 0 };
		cistpl_cftable_entry_t *cfg = &(parse.cftable_entry);
		CFG_CHECK(GetTupleData, handle, &tuple);
		CFG_CHECK(ParseTuple, handle, &tuple, &parse);
		
		if (cfg->flags & CISTPL_CFTABLE_DEFAULT) dflt = *cfg;
		if (cfg->index == 0) goto next_entry;
		link->conf.ConfigIndex = cfg->index;
		
		/* Does this card need audio output? */
		if (cfg->flags & CISTPL_CFTABLE_AUDIO) {
			link->conf.Attributes |= CONF_ENABLE_SPKR;
			link->conf.Status = CCSR_AUDIO_ENA;
		}
		/* 
		 * Use power settings for Vcc and Vpp if present 
		 *
		 * Note that the CIS values need to be rescaled 
		 */
		if (cfg->vcc.present & (1<<CISTPL_POWER_VNOM))
			link->conf.Vcc = cfg->vcc.param[CISTPL_POWER_VNOM]/10000;
		else if (dflt.vcc.present & (1<<CISTPL_POWER_VNOM))
			link->conf.Vcc = dflt.vcc.param[CISTPL_POWER_VNOM]/10000;
		
		if (cfg->vpp1.present & (1<<CISTPL_POWER_VNOM))
			link->conf.Vpp1 = link->conf.Vpp2 =
				cfg->vpp1.param[CISTPL_POWER_VNOM]/10000;
		else if (dflt.vpp1.present & (1<<CISTPL_POWER_VNOM))
			link->conf.Vpp1 = link->conf.Vpp2 =
				dflt.vpp1.param[CISTPL_POWER_VNOM]/10000;
		
		/* Do we need to allocate an interrupt? */
		if (cfg->irq.IRQInfo1 || dflt.irq.IRQInfo1)
			link->conf.Attributes |= CONF_ENABLE_IRQ;
		
		/* IO window settings */
		link->io.NumPorts1 = link->io.NumPorts2 = 0;
		if ((cfg->io.nwin > 0) || (dflt.io.nwin > 0)) {
			cistpl_io_t *io = (cfg->io.nwin) ? &cfg->io : &dflt.io;

			link->io.Attributes1 = IO_DATA_PATH_WIDTH_AUTO;
			if (!(io->flags & CISTPL_IO_8BIT))
				link->io.Attributes1 = IO_DATA_PATH_WIDTH_16;
			if (!(io->flags & CISTPL_IO_16BIT))
				link->io.Attributes1 = IO_DATA_PATH_WIDTH_8;
			link->io.BasePort1 = io->win[0].base;
			link->io.NumPorts1 = io->win[0].len;
			if (io->nwin > 1) {
				link->io.Attributes2 = link->io.Attributes1;
				link->io.BasePort2 = io->win[1].base;
				link->io.NumPorts2 = io->win[1].len;
			}
		}
		
		/* This reserves IO space but doesn't actually enable it */
		CFG_CHECK(RequestIO, link->handle, &link->io);
		
		/*  
		 * Now set up a common memory window, if needed. There is
		 * room in the dev_link_t structure for one memory window
		 * handle, but if the base addresses need to be saved, or
		 * if multiple windows are needed, the info should go in
		 * the private data structure for this device.
		 * 
		 * Note that the memory window base is a physical address,
		 * and needs to be mapped to virtual space with ioremap()
		 * before it is used.  
		 */
		if ((cfg->mem.nwin > 0) || (dflt.mem.nwin > 0)) {
			cistpl_mem_t *mem =
				(cfg->mem.nwin) ? &cfg->mem : &dflt.mem;

			req.Attributes = WIN_DATA_WIDTH_16|WIN_MEMORY_TYPE_CM|
				WIN_ENABLE/*|WIN_USE_WAIT*/|WIN_MAP_BELOW_1MB;
			req.Base = 0; /*mem->win[0].host_addr;*/
			req.Size = mem->win[0].len;
			req.Size = 0x8000;
			req.AccessSpeed = 0;
			link->win = (window_handle_t)link->handle;
			CFG_CHECK(RequestWindow, &link->win, &req);
			map.Page = 0; map.CardOffset = mem->win[0].card_addr;
			CFG_CHECK(MapMemPage, link->win, &map);
		}
		/* If we got this far, we're cool! */
		break;
		
	next_entry:
		CS_CHECK(GetNextTuple, handle, &tuple);
	}
	
	/*
	 * Allocate an interrupt line.  Note that this does not assign a
	 * handler to the interrupt, unless the 'Handler' member of the
	 * irq structure is initialized.
	 */
	if (link->conf.Attributes & CONF_ENABLE_IRQ)
		CS_CHECK(RequestIRQ, link->handle, &link->irq);
	
	/*
	 * This actually configures the PCMCIA socket -- setting up
	 * the I/O windows and the interrupt mapping, and putting the
	 * card and host interface into "Memory and IO" mode.
	 */
	CS_CHECK(RequestConfiguration, link->handle, &link->conf);
	
	/*
	 * At this point, the dev_node_t structure(s) need to be
	 * initialized and arranged in a linked list at link->dev.
	 */
	sprintf(self->node.dev_name, "ircard0");
	self->node.major = self->node.minor = 0;
	link->dev = &self->node;
	
	/* Finally, report what we've done */
	printk(KERN_INFO "%s: index 0x%02x: Vcc %d.%d",
	       self->node.dev_name, link->conf.ConfigIndex,
	       link->conf.Vcc/10, link->conf.Vcc%10);
	if (link->conf.Vpp1)
		printk(", Vpp %d.%d", link->conf.Vpp1/10, link->conf.Vpp1%10);
	if (link->conf.Attributes & CONF_ENABLE_IRQ)
		printk(", irq %d", link->irq.AssignedIRQ);
	if (link->io.NumPorts1)
		printk(", io 0x%04x-0x%04x", link->io.BasePort1,
		       link->io.BasePort1+link->io.NumPorts1-1);
	if (link->io.NumPorts2)
		printk(" & 0x%04x-0x%04x", link->io.BasePort2,
		       link->io.BasePort2+link->io.NumPorts2-1);
	if (link->win)
		printk(", mem 0x%06lx-0x%06lx", req.Base, req.Base+req.Size-1);
	printk("\n");

	/* Look up the irda device */
	idev = &self->idev;

	/* Initialize IO */
	idev->io.iobase    = link->io.BasePort1+8;
        idev->io.iobase2   = link->io.BasePort1; /* Used by irport */
        idev->io.irq       = link->irq.AssignedIRQ;
        idev->io.io_ext    = 8;
        idev->io.io_ext2   = 8;       /* Used by irport */
	idev->io.membase   = (int) ioremap(req.Base, req.Size);

        idev->io.fifo_size = 16;
	idev->io.baudrate  = 9600;

	DEBUG(0, __FUNCTION__ "(), membase=%08x\n", idev->io.membase);

	/* Reset Tx queue info */
	self->tx_q.len = self->tx_q.ptr = self->tx_q.free = 0;
	self->tx_q.tail = SHMEM_TX_START + idev->io.membase;

	ircard_probe(idev);

	/* Initialize QoS for this device */
	irda_init_max_qos_capabilies(&idev->qos);
	
	/* The only value we must override it the baudrate */
	idev->qos.baud_rate.bits = IR_9600|IR_19200|IR_38400|IR_57600|
		IR_115200|IR_576000|IR_1152000 |(IR_4000000 << 8);

	idev->qos.min_turn_time.bits = qos_mtt_bits;
	irda_qos_bits_to_value(&idev->qos);

	idev->flags = IFF_FIR|IFF_MIR|IFF_SIR|IFF_SHM|IFF_PIO;

	/* Specify which buffer allocation policy we need */
	idev->rx_buff.flags = GFP_KERNEL;
	idev->tx_buff.flags = GFP_KERNEL;

	/* We still need the SIR buffer since we don't use shmem for that */
	idev->rx_buff.truesize = 4000; 
	idev->tx_buff.truesize = 4000;
		
	/* Initialize callbacks */
	idev->change_speed           = &ircard_change_speed;
	idev->wait_until_sent        = &ircard_wait_until_sent;
	idev->is_receiving           = &ircard_is_receiving;
     
	/* Override the network functions we need to use */
	idev->netdev.init            = &ircard_net_init;
	idev->netdev.hard_start_xmit = &irport_hard_xmit; /* used for SIR */
	idev->netdev.open            = &ircard_net_open;
	idev->netdev.stop            = &ircard_net_close;

	/* Open the IrDA device */
	irda_device_open(idev, (char *) dev_info, self);
	/*irda_device_open(idev, dev_name, self);*/

	link->state &= ~DEV_CONFIG_PENDING;

	return;
	
 cs_failed:
	cs_error(link->handle, last_fn, last_ret);
	ircard_release((u_long)link);
}

/*
 * Function ircard_release (arg)
 *
 *    After a card is removed, ircard_release() will unregister the net
 *    device, and release the PCMCIA configuration. If the device is still
 *    open, this will be postponed until it is closed.
 * 
 */
static void ircard_release(u_long arg)
{
	dev_link_t *link = (dev_link_t *) arg;
	struct ircard_cb *self;
	struct irda_device *idev;
	
	DEBUG(0, __FUNCTION__ "(0x%p)\n", link);

	self = (struct ircard_cb *) link->priv;
	idev = &self->idev;

	if (link->open) {
		DEBUG(1, "ircard_cs: release postponed, '%s' still open\n",
		      self->node.dev_name);
		link->state |= DEV_STALE_CONFIG;
		return;
	}
	
	CardServices(ReleaseWindow, link->win);
	iounmap((void *)idev->io.membase);
	CardServices(ReleaseConfiguration, link->handle);
	CardServices(ReleaseIO, link->handle, &link->io);
	CardServices(ReleaseIRQ, link->handle, &link->irq);
	
	link->state &= ~(DEV_CONFIG | DEV_RELEASE_PENDING);
}

/*
 * Function ircard_event (event, priority, args)
 *
 *    The card status event handler. Mostly, this schedules other stuff to
 *    run after an event is received. A CARD_REMOVAL event also sets some
 *    flags to discourage the net drivers from trying to talk to the card
 *    any more.
 * 
 */
static int ircard_event(event_t event, int priority,
			event_callback_args_t *args)
{
	dev_link_t *link = args->client_data;
	struct irda_device *idev;
	struct ircard_cb *self;
	
	DEBUG(1, __FUNCTION__ "(0x%06x)\n", event);
	
	self = (struct ircard_cb *) link->priv;
	idev = &self->idev;

	ASSERT(self != NULL, return -1;);
	ASSERT(self->magic == IRCARD_MAGIC, return -1;);

	switch (event) {
	case CS_EVENT_CARD_REMOVAL:
		link->state &= ~DEV_PRESENT;
		if (link->state & DEV_CONFIG) {
			self->stop = 1;
			link->release.expires = RUN_AT(HZ/20);
			add_timer(&link->release);
		}
		break;
	case CS_EVENT_CARD_INSERTION:
		link->state |= DEV_PRESENT | DEV_CONFIG_PENDING;
		ircard_config(link);
		break;
	case CS_EVENT_PM_SUSPEND:
		link->state |= DEV_SUSPEND;
		/* Fall through... */
	case CS_EVENT_RESET_PHYSICAL:
		/* Mark the device as stopped, to block IO until later */
		self->stop = 1;
		if (link->state & DEV_CONFIG)
			CardServices(ReleaseConfiguration, link->handle);
		break;
	case CS_EVENT_PM_RESUME:
		link->state &= ~DEV_SUSPEND;
		/* Fall through... */
	case CS_EVENT_CARD_RESET:
		if (link->state & DEV_CONFIG)
			CardServices(RequestConfiguration, link->handle, &link->conf);
		self->stop = 0;
		/*
		 * In a normal driver, additional code may go here to restore
		 * the device state and restart IO. 
		 */
		irport_start(idev, idev->io.iobase2);
		ircard_change_speed(idev, 9600);
		break;
	}
	return 0;
}

/*
 * Function ircard_probe (iobase, iobase2, irq, dma)
 *
 *    
 *
 */
static int ircard_probe(struct irda_device *idev)
{
	int revision;
	int iobase;

	iobase = idev->io.iobase;
	
	/* Read revision ID */
	switch_bank(iobase, 3);
	revision = inb(iobase+REVISION_ID); /* (bank 3) */

	printk("IBM 31T1502 controller revision %d\n", revision);

	/* Initialize shared memory page register */
	outb(0, iobase+SH_MEM_PAGE); /* (bank 3) */

	/* Enable Tx CRC just in case */
	switch_bank(iobase, 0);
	outb(TX_CTRL2_EN_TX_CRC, iobase+TX_CTRL2);

	switch_bank(iobase, 3);
	outb(0, iobase+SH_MEM_PAGE);         /* (bank 3) */
	outb(0, iobase+TX_DMA_ADDR_LOW);     /* (bank 3) */
	outb(PAGE_DISABLE, iobase+TX_DMA_ADDR_HIGH);    /* (bank 3) */

	return 0;
}

/*
 * Function ircard_sir_to_fir (iobase)
 *
 *    Some code to make the tranceiver switch from SIR to FIR mode
 *
 */
static void ircard_sir_to_fir(struct irda_device *idev, int iobase)
{
	DEBUG(4, __FUNCTION__ "()\n");

	__switch_bank(iobase, 2);
	
	/* Set XCRVOFF */
	outb(IR_TRANS_CTRL_XCVROFF, iobase+IR_TRANS_CTRL);
	
	/* Wait a minimum of 2 us */
	udelay(2);
	
	/* Set TXD_FORCE */
	outb(IR_TRANS_CTRL_XCVROFF|IR_TRANS_CTRL_TXD_FRC, 
	     iobase+IR_TRANS_CTRL);
	
	/* Wait a minimum of 2 us */
	udelay(2);
	
	/* Clear XCRVOFF */
	outb(IR_TRANS_CTRL_TXD_FRC, iobase+IR_TRANS_CTRL);
	
	/* Wait a minimum of 2 us */
	udelay(2);
	
	/* Clear TXD_FORCE */
	outb(0, iobase+IR_TRANS_CTRL);
}

/*
 * Function ircard_fir_to_sir (iobase)
 *
 *    Some code to make the tranceiver switch from FIR to SIR mode
 *
 */
static void ircard_fir_to_sir(struct irda_device *idev, int iobase)
{
	DEBUG(4, __FUNCTION__ "()\n");

	__switch_bank(iobase, 2);
	
	/* Set XCRVOFF */
	outb(IR_TRANS_CTRL_XCVROFF, iobase+IR_TRANS_CTRL);
	
	/* Wait a minimum of 2 us */
	udelay(2);
	
	/* Clear XCRVOFF */
	outb(0, iobase+IR_TRANS_CTRL);
}

/*
 * Function ircard_change_speed (idev, baud)
 *
 *    Change the speed of the device
 *
 */
static void ircard_change_speed(struct irda_device *idev, __u32 speed)
{
	struct ircard_cb *self;
	unsigned long flags;
	int iobase; 
	
	DEBUG(4, __FUNCTION__ "()\n");

	ASSERT(idev != NULL, return;);
	ASSERT(idev->magic == IRDA_DEVICE_MAGIC, return;);

	self = (struct ircard_cb *) idev->priv;
	iobase = idev->io.iobase;

	/* Serialize access to the driver */
	spin_lock_irqsave(&idev->lock, flags);

	/* Check if we are switching from FIR to SIR, or SIR to FIR */
	if ((idev->io.baudrate > 1152000) && (speed < 4000000))
		ircard_fir_to_sir(idev, iobase);
	else if ((idev->io.baudrate < 4000000) && (speed > 1152000))
		ircard_sir_to_fir(idev, iobase);
	     
	/* Update accounting for new speed */
	idev->io.baudrate = speed;
	
	if (speed > 115200)
		/* Install the FIR transmit handler */
		idev->netdev.hard_start_xmit = &ircard_hard_xmit;
	else		
		/* Install the SIR transmit handler */
		idev->netdev.hard_start_xmit = &irport_hard_xmit;
	
	/* Reset hardware */
	__switch_bank(iobase, 0);
	outb_p(0x07, iobase+7);
	
	/* Switch to bank2 and disable interrupts */
	__switch_bank(iobase, 2);
	
	switch (speed) {
	default:
		DEBUG(4, __FUNCTION__ "(), unknown baud rate of %d\n", speed);
		/* FALLTHROUGH */
	case 9600:   /* FALLTHROUGH */
	case 19200:  /* FALLTHROUGH */
	case 37600:  /* FALLTHROUGH */
	case 57600:  /* FALLTHROUGH */
	case 115200:
 		irport_start(idev, idev->io.iobase2);
		irport_change_speed(idev, speed);
		outb_p(IR_CONF_SIR, iobase+IR_CONF);
		break;
	case 576000:		
		DEBUG(0, __FUNCTION__ "(), handling baud of 576000\n");
		irport_stop(idev, idev->io.iobase2);
		outb_p(IR_CONF_MIR_576|IR_CONF_MIR, iobase+IR_CONF);
		break;
	case 1152000:
		DEBUG(0, __FUNCTION__ "(), handling baud of 1152000\n");
		irport_stop(idev, idev->io.iobase2);
		outb_p(IR_CONF_MIR_1152|IR_CONF_MIR, iobase+IR_CONF);
		break;
	case 4000000:
		DEBUG(4, __FUNCTION__ "(), handling baud of 4000000\n");
		irport_stop(idev, idev->io.iobase2);
		outb_p(IR_CONF_FIR, iobase+IR_CONF);
		break;
	}
	idev->netdev.tbusy = 0;
	
	/* Enable some interrupts so we can receive frames */
	if (speed > 115200) {
 		/* Enable receiving, interrupts and go to bank 0*/
 		ircard_receive(idev, iobase);
 	}
	spin_unlock_irqrestore(&idev->lock, flags);
}

/*
 * Function ircard_hard_xmit (skb, dev)
 *
 *    Transmit the frame!
 *
 */
static int ircard_hard_xmit(struct sk_buff *skb, struct net_device *dev)
{
	struct irda_device *idev;
	struct ircard_cb *self;
	unsigned long flags;
	int mtt, diff;

	idev = (struct irda_device *) dev->priv;
	
	self = idev->priv;

	/* Lock transmit buffer */
	if (irda_lock((void *) &dev->tbusy) == FALSE)
		return -EBUSY;

	/* Serialize access to the driver */
	spin_lock_irqsave(&idev->lock, flags);

	switch_bank(idev->io.iobase, 0);

	/* Register this frame */
	self->tx_q.queue[self->tx_q.free].start = self->tx_q.tail;
	self->tx_q.queue[self->tx_q.free].len = skb->len;
	self->tx_q.tail += skb->len;

	/* Copy data to transmit part of shared memory */
	copy_to_pc((void *) self->tx_q.queue[self->tx_q.free].start, skb->data,
 		   skb->len);

	self->tx_q.len++;
	self->tx_q.free++;

	/* Start transmit only if there is currently no transmit going on */
	if (self->tx_q.len == 1) {
		/* Extract minimum turnaround time from skb */
		mtt = irda_get_mtt(skb);
		
		/* Currently we just use udelay for the MTT delay */
		if (mtt) {
			/* Check how much time we have used already */
			do_gettimeofday(&self->now);
			diff = self->now.tv_usec - self->stamp.tv_usec;
			if (diff < 0) 
				diff += 1000000;
			
			/* Check if the mtt is larger than the time we have
			 * already used by all the protocol processing
			 */
			if (mtt > diff) {
				DEBUG(4, __FUNCTION__ "(), delay=%d us\n", 
				      mtt-diff);
				/* Currently we just use udelay for the MTT
                                 * delay 
				 */
				udelay(mtt-diff);
			}
		}	
		ircard_dma_write(idev, idev->io.iobase);
	}
	spin_unlock_irqrestore(&idev->lock, flags);

	dev_kfree_skb(skb);

	/* Not busy transmitting anymore */
	dev->tbusy = 0;

	/* Return success */
	return 0;
}

/*
 * Function ircard_dma_write (idev, iobase)
 *
 *    Transmit data using shared memory (local DMA)
 *
 */
static void ircard_dma_write(struct irda_device *idev, int iobase)
{
	struct ircard_cb *self;
	int addr;
	int ptr;

	self = idev->priv;
	ptr = self->tx_q.ptr;

	/* Disable Rx DMA */
	switch_bank(iobase, 0);
 	outb(0, iobase+MISC);   /* (bank 0) */

	idev->io.direction = IO_XMIT;

	ircard_set_mcr(iobase, MCR_INTR_EN|MCR_TX_EN, 0);

	/* Set early EOM level */
	outb(TX_CTRL2_EN_TX_CRC|TX_CTRL2_EARLY_EOM_16, iobase+TX_CTRL2);

	/* Load the byte count to the transmit byte count register */
	switch_bank(iobase, 1);
	outb(self->tx_q.queue[ptr].len & 0xff, iobase+TX_BYTE_COUNT_LOW);
	outb((self->tx_q.queue[ptr].len >> 8) & 0x1f, 
	     iobase+TX_BYTE_COUNT_HIGH);

	/* Set the Tx DMA start address (0 for the first transfer) */
	switch_bank(iobase, 3);
	outb(0, iobase+SH_MEM_PAGE);         /* (bank 3) */

	/* Compute chip address */
	addr = self->tx_q.queue[ptr].start - idev->io.membase;
	outb(addr & 0xff, iobase+TX_DMA_ADDR_LOW);        /* (bank 3) */
	outb(PAGE_DISABLE|((addr >> 8) & 0x3f), iobase+TX_DMA_ADDR_HIGH);

	/* Set host DMA controller for transmission */
	switch_bank(iobase, 0);
 	outb(0xc0, iobase+MISC);   /* (bank 0) */

	/*udelay(6);*/

	/* Set RTS */
	switch_bank(iobase, 0);
	outb(TX_CTRL1_RTS|TX_CTRL1_UNDR_EOM_I/*|TX_CTRL1_AUTO_RST_EOM*/, 
	     iobase+TX_CTRL1); /* (bank 0) */
}

/*
 * Function ircard_xmit_complete (idev, underrun)
 *
 *    Finished transmitting a frame
 *
 */
static inline void ircard_xmit_complete(struct irda_device *idev)
{
	struct ircard_cb *self;
	int iobase;
	int stat;
	__u8 bank;
	int paranoid = 0;

	self   = idev->priv;
	iobase = idev->io.iobase;

	bank = switch_bank(iobase, 0);

	/* Read Tx status register and check for transmission completion */
	stat = inb(iobase+TX_STAT); /* (bank 0) */
	if (stat & TX_STAT_FIFO_UNDR) {
		DEBUG(0, __FUNCTION__ "() FIFO underrun!\n");
		idev->stats.tx_errors++;
		idev->stats.tx_fifo_errors++;	
	} else {
		/* Spin until EOM */
		while (!(stat & TX_STAT_EOM)) {
			if (paranoid++ > 1000) {
				DEBUG(0, __FUNCTION__ "(), breaking!\n");
				break;
			}
			stat = inb(iobase+TX_STAT);
		}
		idev->stats.tx_packets++;
		idev->stats.tx_bytes += idev->tx_buff.len;
	}
	
	/* This will reset transmit enable */
	ircard_set_mcr(iobase, MCR_INTR_EN, bank);

	/* Disable DMA */
	outb(0, iobase+MISC);

	/* Reset Tx special condition bits */
	outb(RESET_TX_SPEC_BITS, iobase+RESET);

	/* Unlock tx_buff and request another frame */
	idev->netdev.tbusy = 0; /* Unlock */
	idev->media_busy = FALSE;

	/* Finished with this frame, so prepare for next */
	self->tx_q.ptr++;
	self->tx_q.len--;

	/* Any frames to be sent back-to-back? */
	if (self->tx_q.len) {
		ircard_dma_write(idev, iobase);
	} else if (irda_device_txqueue_empty(idev)) {
		/* Reset EOM and RTS bits */
		outb(0, iobase+TX_CTRL1); /* (bank 0) */

		/* 
		 * No frames queued at card, and no frames in tx queue, so 
		 * now we can get ready for receiving 
		 */
		ircard_receive(idev, iobase);
	}

	/* Tell the network layer, that we can accept more frames */
	mark_bh(NET_BH);
	
	/* No need to restore original bank! */
}

/*
 * Function ircard_receive (idev)
 *
 *    Get ready for receiving a frame. The device will initiate a DMA
 *    if it starts to receive a frame.
 *
 */
static int ircard_receive(struct irda_device *idev, int iobase)
{
	struct ircard_cb *self;
	__u8 bank;

	self = idev->priv;

	bank = switch_bank(iobase, 0);

	/* Disable Rx, Tx */
	ircard_set_mcr(iobase, MCR_INTR_EN, bank);

	/* Reset Tx queue info */
	self->tx_q.len = self->tx_q.ptr = self->tx_q.free = 0;
	self->tx_q.tail = SHMEM_TX_START + idev->io.membase;;

	/* Reset Rx special condition bits */
	outb(RESET_RX_SPEC_BITS, iobase+RESET);

	/* Reset Rx receive frame pointer */
	outb(RESET_RX_RFP, iobase+RESET);

	/* Set receive and interrupt enable bits (but keep original bank) */
	ircard_set_mcr(iobase, MCR_RX_EN|MCR_INTR_EN, bank);

	/* Enable Rx ready interrupt */
	switch_bank(iobase, 0);

	/* Enable Rx interrupts */
	outb(RX_CTRL_EN_CRC|RX_CTRL_COND_I, iobase+RX_CTRL);
	
	/* Make sure DMA is set properly */
	outb(0xc0, iobase+MISC);

	idev->io.direction = IO_RECV;

	return 0;
}

/*
 * Function ircard_receive_complete (idev, iobase)
 *
 *    
 *
 */
static int ircard_receive_complete(struct irda_device *idev, int iobase)
{
	struct sk_buff *skb;
	struct ircard_cb *self;
	int len;
 	int ptr;
	int stat;
	int start;

	self = idev->priv;

	/* Read receive ring frame pointer  */
	switch_bank(iobase, 1);
	ptr = inb(iobase+RX_RING_FRM_PTR_LOW);
	ptr |= ((inb(iobase+RX_RING_FRM_PTR_HIGH) & 0x3f) << 8);
	if (!ptr)
		return FALSE;

	stat = readb(idev->io.membase+ptr);
	len = (stat & 0x1f) << 8;
	len |= readb(idev->io.membase+ptr-1);

	if (stat & 0x80) {
		DEBUG(0, __FUNCTION__ "(), frame abort!\n");
		
		/* switch_bank(iobase, 0); */
/* 		outb(RESET_RX_SPEC_BITS, iobase+RESET); */
/* 		outb(RESET_RX_FIFO_PTR, iobase+RESET); */
		
		/* Skip frame */
		idev->stats.rx_errors++;
		idev->stats.rx_frame_errors++;
		
		return FALSE;
	}
	if (stat & 0x40) {
		DEBUG(4, __FUNCTION__ "(), frame error!\n");
		DEBUG(4, __FUNCTION__ "(), len=%d\n", len);

		/* Skip frame */
		idev->stats.rx_errors++;
		idev->stats.rx_frame_errors++;

		return FALSE;
	}
	if (stat & 0x20) {
		DEBUG(4, __FUNCTION__ "(), overrun!\n");
		DEBUG(4, __FUNCTION__ "(), len=%d\n", len);
		
		switch_bank(iobase, 0);
		outb(RESET_RX_SPEC_BITS, iobase+RESET);

		/* Skip frame */
		idev->stats.rx_errors++;
		idev->stats.rx_fifo_errors++;

		return FALSE;
	}

	ASSERT(len>5, return FALSE;);

	/* Find start of frame */
	start = idev->io.membase+ptr-len-1;

	if (start > self->data) {
		DEBUG(0, __FUNCTION__ "() Received Back-to-Back frames!!\n");
	}

	/* Got a frame, so remember when */
	do_gettimeofday(&self->stamp);

	/* Save pointer to start of next frame */
	self->data = ptr+1;

	/* Remove CRC */
	len -= 4;

	/* Limit frame to max data size + LAP header */
	if (len > 2050)
		len = 2050;

	skb = dev_alloc_skb(len+1);
	if (skb == NULL)  {
		printk(KERN_INFO __FUNCTION__ 
		       "(), memory squeeze, dropping frame.\n");
		return FALSE;
	}
	
	/* Make sure IP header gets aligned */
	skb_reserve(skb, 1);
	
	skb_put(skb, len);
	copy_from_pc(skb->data, (void *) start, len);

	idev->stats.rx_packets++;
	
	skb->dev = &idev->netdev;
	skb->mac.raw  = skb->data;
	skb->protocol = htons(ETH_P_IRDA);
	netif_rx(skb);

	return TRUE;
}

/*
 * Function ircard_interrupt (irq, dev_id, regs)
 *
 *    An interrupt from the chip has arrived. Time to do some work
 *
 */
static void ircard_interrupt(int irq, void *dev_id, struct pt_regs *regs)
{
     	struct irda_device *idev;
	__u8 msr, bank;
	int iobase;
	int bogus = 0;
	
	idev = (struct irda_device *) dev_id;
	if (idev == NULL) {
		WARNING("%s: irq %d for unknown device.\n", (char *) dev_info,
			irq);
		return;
	}

	if (idev->io.baudrate <= 115200)
		return irport_interrupt(irq, dev_id, regs);

	spin_lock(&idev->lock);

	idev->netdev.interrupt = 1;

	iobase = idev->io.iobase;

	/* Save bank, so the original bank is restored when we return */
	bank = switch_bank(iobase, 0);

	/* Read interrupt status */
	msr = inb(iobase+MSR) & MSR_ID_MASK; 

	while (msr) {
	/* Check which interrupt this is */
	switch (msr) {
	case MSR_TX_COND_ID: /* Tx special condition */
		ircard_xmit_complete(idev);
		break;
	case MSR_RX_DATA_ID: /* Rx data available */
		DEBUG(0, __FUNCTION__ "() should not get here!\n");
		ircard_receive_complete(idev, iobase);
		break;
	case MSR_RX_COND_ID: /* Rx special condition */
		ircard_receive_complete(idev, iobase);
		break;
	case MSR_TX_EMPTY_ID: /* Tx buffer empty */
		DEBUG(0, __FUNCTION__ "() should not get here!\n");
		break;
	default:
		DEBUG(0, __FUNCTION__ "(), unknown interrupt status=%#x\n",
		      msr);
		break;	
	};
	if (bogus++ > 10) {
		WARNING(__FUNCTION__ "(), breaking!\n");
		break;
	}
	msr = inb(iobase+MSR) & MSR_ID_MASK; 
	}
	
	/* Restore bank */
	restore_bank(iobase, bank);

	idev->netdev.interrupt = 0;

	spin_unlock(&idev->lock);
}

static int ircard_net_init(struct net_device *dev)
{
	/* Setup to be a normal IrDA network device driver */
	irda_device_setup(dev);

	/* Insert overrides below this line! */

	return 0;
}

/*
 * Function ircard_net_open (dev)
 *
 *    
 *
 */
static int ircard_net_open(struct net_device *dev)
{
	struct irda_device *idev;
	int iobase;
	
	ASSERT(dev != NULL, return -1;);
	idev = (struct irda_device *) dev->priv;
	
	ASSERT(idev != NULL, return 0;);
	ASSERT(idev->magic == IRDA_DEVICE_MAGIC, return 0;);
	
	iobase = idev->io.iobase;
	
	/* Ready to play! */
#if LINUX_VERSION_CODE < VERSION(2,2,11)
	dev->tbusy = 0;
	dev->interrupt = 0;
	dev->start = 1;
#else
	irda_device_net_open(dev);
#endif
	/* Start SIR driver */
	irport_start(idev, idev->io.iobase2);

	MOD_INC_USE_COUNT;

	return 0;
}

/*
 * Function ircard_net_close (dev)
 *
 *    
 *
 */
static int ircard_net_close(struct net_device *dev)
{
	struct irda_device *idev;
	int iobase;
#if LINUX_VERSION_CODE < VERSION(2,2,11)
	/* Stop device */
	dev->tbusy = 1;
	dev->start = 0;
#else
	irda_device_net_close(dev);
#endif
	ASSERT(dev != NULL, return -1;);
	idev = (struct irda_device *) dev->priv;
	
	ASSERT(idev != NULL, return 0;);
	ASSERT(idev->magic == IRDA_DEVICE_MAGIC, return 0;);
	
	iobase = idev->io.iobase;

	/* Stop SIR driver */
	irport_stop(idev, idev->io.iobase2);

	MOD_DEC_USE_COUNT;

	return 0;
}

/*
 * Function uircc_wait_until_sent (idev)
 *
 *    This function should put the currently thread to sleep until all data 
 *    have been sent, so it is safe to change the speed.
 */
static void ircard_wait_until_sent(struct irda_device *idev)
{
	if (idev->io.baudrate > 115200) {
		/* Just delay 60 ms */
		current->state = TASK_INTERRUPTIBLE;
		schedule_timeout(MSECS_TO_JIFFIES(60));
	} else
		irport_wait_until_sent(idev);
}

/*
 * Function uircc_is_receiving (idev)
 *
 *    Return TRUE is we are currently receiving a frame
 *
 */
static int ircard_is_receiving(struct irda_device *idev)
{
	int status = FALSE;
	/* int iobase; */

	ASSERT(idev != NULL, return FALSE;);
	ASSERT(idev->magic == IRDA_DEVICE_MAGIC, return FALSE;);

	if (idev->io.baudrate > 115200) {
		/* Not impl yet */
	} else 
		status = (idev->rx_buff.state != OUTSIDE_FRAME);
	
	return status;
}

MODULE_AUTHOR("Dag Brattli <dagb@cs.uit.no>");
MODULE_DESCRIPTION("IBM 31T1502 IrDA PCMCIA device driver");

/*
 * Function init_module ()
 *
 *    
 *
 */
int init_module(void)
{
	servinfo_t serv;
	DEBUG(0, "%s\n", version);
	
	CardServices(GetCardServicesInfo, &serv);
	if (serv.Revision != CS_RELEASE_CODE) {
		printk(KERN_NOTICE "ircard_cs: Card Services release "
		       "does not match!\n");
		return -1;
	}
	register_pccard_driver(&dev_info, &ircard_attach, &ircard_detach);
	return 0;
}

/*
 * Function cleanup_module ()
 *
 *    
 *
 */
void cleanup_module(void)
{
	DEBUG(0, "ircard_cs: unloading\n");
	unregister_pccard_driver(&dev_info);

	while (dev_list != NULL) {
		if (dev_list->state & DEV_CONFIG)
			ircard_release((u_long)dev_list);
		ircard_detach(dev_list);
	}
}
