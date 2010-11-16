/*
 *  DMA OSWR feature testing module
 *
 *  Copyright (C) 2010 Texas Instruments Incorporated - http://www.ti.com/
 *          - G, Manjunath Kondaiah <manjugk@ti.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 */

#include "dma_single_channel.h"

extern u32 reg_dump_after_suspend[CH_END*4];
extern u8 wait_oswr_trigger;

static int __init oswr_trigger_init(void)
{
	int i;
	while(wait_oswr_trigger < 0 ) {
		mdelay(1000);
		cpu_relax();
	}
	printk("Register Dump after suspend:\n");
	printk("%s: wait_oswr_trigger : %d\n", __func__, wait_oswr_trigger);

	dma_channel_registers_dump(wait_oswr_trigger, reg_dump_after_suspend);
	return 0;
}
static void __exit oswr_trigger_exit(void)
{

	/* Dummy */
}

module_init(oswr_trigger_init);
module_exit(oswr_trigger_exit);

MODULE_AUTHOR("Texas Instruments");
MODULE_LICENSE("GPL");
