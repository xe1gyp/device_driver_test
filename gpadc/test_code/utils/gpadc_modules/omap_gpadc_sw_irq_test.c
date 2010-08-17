/*
 * omap4_gpadc_sw_irq_test.c
 *
 * Test Driver for OMAP4 GPADC driver
 *
 * Copyright (C) 2010 Texas Instruments, Inc.
 *
 * This file is licensed under the terms of the GNU General Public License
 * version 2. This program is licensed "as is" without any warranty of any
 * kind, whether express or implied.
 *
 * Author : Keerthy J
 * Date   : 17 Aug 2010
 */

#include <linux/module.h>
#include <linux/types.h>
#include <linux/init.h>
#include <linux/errno.h>
#include <linux/i2c/twl6030-gpadc.h>
#include <linux/delay.h>


static int channels = 0x1FFFF; /* Read All 17 Channels */
module_param(channels, int, 0);
int wait_funccb;

void omap_gpadc_func_cb(int len, int channels, int *buf)
{
	int index;

	printk(KERN_INFO "GPADC conversion result for %d Channels\n", len);
	for (index = 0; index < TWL6030_GPADC_MAX_CHANNELS; index++) {
		if (channels & (1 << index))
			printk(KERN_INFO "Channel No = %d\t Value = %d\n", index, buf[index]);
	}
	wait_funccb = 1;

	return;
}

static int __init omap_gpadc_sw_irq_init(void)
{
	struct twl6030_gpadc_request req;
	int ret;

	req.channels = channels;
	req.do_avg = 0;
	req.method = TWL6030_GPADC_SW2;
	req.type = TWL6030_GPADC_IRQ_ONESHOT;
	req.active = 0;
	req.result_pending = 0;
	req.func_cb = omap_gpadc_func_cb;
	ret = twl6030_gpadc_conversion(&req);

	while (wait_funccb == 0)
		udelay(10);

	if (ret < 0) {
		printk(KERN_INFO "FAIL, return value = %d\n", ret);
		return 1;
	} else {
		printk(KERN_INFO "OMAP GPADC TEST driver installed successfully\n");
		return 0;
	}
}

static void __exit omap_gpadc_sw_irq_exit(void)
{
	printk(KERN_INFO "OMAP GPADC TEST driver un-installed successfully\n");
	return;
}

module_init(omap_gpadc_sw_irq_init);
module_exit(omap_gpadc_sw_irq_exit);

MODULE_AUTHOR("Texas Instruments");
MODULE_DESCRIPTION("GPADC Test Driver");
MODULE_LICENSE("GPL");
