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
#include <linux/io.h>

#include "dma_single_channel.h"

/* Base Addresses for all omap2+'s */
#define OMAP24XX_DMA4_BASE		(L4_24XX_BASE + 0x56000)
#define OMAP34XX_DMA4_BASE		(L4_34XX_BASE + 0x56000)
#define OMAP44XX_DMA4_BASE		(L4_44XX_BASE + 0x56000)

#define dma_read(reg)							\
({									\
	u32 __val;							\
	__val = __raw_readl(omap_dma_base + OMAP_DMA4_##reg);		\
	__val;								\
})

#define dma_write(val, reg)						\
({									\
	__raw_writel((val), omap_dma_base + OMAP_DMA4_##reg);		\
})

static void __iomem *omap_dma_base;

void dma_channel_registers_dump(int lch, u32 *reg_values)
{
	u32 base, dma_stride, i;
	u8 count = 0;
	u32 reg_dump[CH_END * 4];
	if (cpu_is_omap24xx())
		base = OMAP24XX_DMA4_BASE;
	else if (cpu_is_omap34xx())
		base = OMAP34XX_DMA4_BASE;
	else if (cpu_is_omap44xx())
		base = OMAP44XX_DMA4_BASE;
	else {
		pr_err("DMA Base address not found\n");
		return;
	}

	omap_dma_base = ioremap(base, SZ_4K);
	printk("OMAP2+ Channel Registers Dump...\n");
	for (i = CCR; i < CH_END; i += 1) {
		dma_stride = (i * 4) + 0x80;
		dma_stride += lch * 0x60;	
		reg_dump[count] = __raw_readl(omap_dma_base + dma_stride);
		printk("%d. offset : %X Value: %X\n", count, dma_stride, reg_dump[count]);
		count += 1;
	}
	memcpy(reg_values, reg_dump, sizeof(reg_dump));
}
EXPORT_SYMBOL(dma_channel_registers_dump);

static int __init dma_dump_init(void) 
{
       /* Create the proc entry */
	return 0;
}

static void __exit dma_dump_exit(void)
{

}

module_init(dma_dump_init);
module_exit(dma_dump_exit);

MODULE_AUTHOR("Texas Instruments");
MODULE_LICENSE("GPL");
