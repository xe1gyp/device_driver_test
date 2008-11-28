/*
 * drivers/ssi/omap-hdq.c
 *
 * Copyright (C) 2006 Texas Instruments, Inc.
 *
 * This file is licensed under the terms of the GNU General Public License 
 * version 2. This program is licensed "as is" without any warranty of any 
 * kind, whether express or implied.
 *
 */

#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/errno.h>
#include <linux/delay.h>
#include <linux/types.h>
#include <linux/module.h>
#include <asm/system.h>
#include <asm/hardware.h>
#include <linux/interrupt.h>
#include <asm/io.h>
#include <asm/arch/hdq.h>


static int __init omap_hdq_test_init(void)
{
	u8 reg, val, val2;
	int ret; 
	
	printk("***** Starting TEST 1 ... ******\n");
	if ((ret = omap_hdq_get())) {
		printk("TEST 1: omap_hdq_get failed, ret=%d\n", ret);
		omap_hdq_reg_dump();
		return -ENODEV;
	}
	printk("TEST 1: omap_hdq_get passed\n");

	printk("***** Starting TEST 2 ... ******\n");
	if ((ret= omap_hdq_break())) {
		printk("TEST 2: omap_hdq_break failed, ret=%d\n", ret);
		omap_hdq_reg_dump();
		omap_hdq_reset();
	}
	else printk("TEST 2: omap_hdq_break passed\n");

	printk("***** Starting TEST 3 ... ******\n");
	for (reg = 0x0; reg < 0x10; reg++){
		if ((ret = omap_hdq_read(reg, &val))){
			printk("TEST 3: omap_hdq_read failed, ret=%d\n", ret);
			omap_hdq_reg_dump();
			omap_hdq_reset();
			break;
		}
		else  printk("BQ27000 reg%x=%x\n",reg, val);
	}
	if (reg == 0x10)
		printk("TEST 3: omap_hdq_read passed\n");

	printk("***** Starting TEST 4 ... ******\n");
	reg = 0x1;
	if ((ret = omap_hdq_read(reg, &val))){
		printk("TEST 4-1: omap_hdq_read failed, ret=%d\n", ret);
		omap_hdq_reg_dump();
		omap_hdq_reset();
	}
	else {
		printk("Before write: BQ27000 reg%x=%x\n", reg, val); 
		val = ~val;
		if ((ret = omap_hdq_write(reg, val))){
			printk("TEST 4-2: omap_hdq_write failed, ret=%d\n", ret);
			omap_hdq_reg_dump();
			omap_hdq_reset();
		}
		else {
			if ((ret = omap_hdq_read(reg, &val2))){
				printk("TEST 4-3: omap_hdq_read failed, ret=%d\n", ret);
				omap_hdq_reg_dump();
				omap_hdq_reset();
			}
			else {
				printk("After write %x: BQ27000 reg %x = %x\n", val, reg, val2);
				if (val != val2)
					printk("TEST 4: omap_hdq_write verification failed\n");
				else	printk("TEST 4: omap_hdq_write verification passed\n");
			}
		}
	}

	omap_hdq_put();
	return -ENODEV;
}

static void __exit omap_hdq_test_exit(void)
{
	return;
}

module_init(omap_hdq_test_init);
module_exit(omap_hdq_test_exit);


MODULE_AUTHOR("Texas Instruments");
MODULE_DESCRIPTION("Simple HDQ driver test using BQ27000");
MODULE_LICENSE("GPL");
