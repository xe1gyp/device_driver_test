/*
 * GPADC Test module - SMP thread access check
 *
 * Copyright (C) 2009 Texas Instruments, Inc
 *
 * History
 *   Initial Version: Keerthy J <j-keerthy@ti.com>
 * This package is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 */

#include <linux/sched.h>
#include <linux/kthread.h>
#include <linux/syscalls.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/init.h>
#include <linux/errno.h>
#include <linux/i2c/twl6030-gpadc.h>
#include <linux/delay.h>

static int channels = 0x1FFFF; /* Read All 17 Channels */
int wait_funccb;
static int iterations;
module_param(iterations, int, 0);

void omap_gpadc_func_cb1(int len, int channels, int *buf)
{
	int index;

	printk(KERN_INFO "Conversion req from cpu1: result for %d Channels\n",
		 len);
	for (index = 0; index < TWL6030_GPADC_MAX_CHANNELS; index++) {
		if (channels & (1 << index))
			printk(KERN_INFO "Channel No = %d\t Value = %d\n",
			 index, buf[index]);
	}
	wait_funccb = 0;

	return;
}

void omap_gpadc_func_cb2(int len, int channels, int *buf)
{
	int index;

	printk(KERN_INFO "Conversion request from cpu2:  result for %d Channels\n",
		len);
	for (index = 0; index < TWL6030_GPADC_MAX_CHANNELS; index++) {
		if (channels & (1 << index))
			printk(KERN_INFO "Channel No = %d\t Value = %d\n",
				index, buf[index]);
	}
	wait_funccb = 0;

	return;
}
static int smp_test1()
{
	struct twl6030_gpadc_request req;
	int ret, i;

	req.channels = channels;
	req.do_avg = 0;
	req.method = TWL6030_GPADC_SW2;
	req.type = TWL6030_GPADC_IRQ_ONESHOT;
	req.active = 0;
	req.result_pending = 0;
	req.func_cb = omap_gpadc_func_cb1;
	for (i = 0; i < iterations; i++) {
		while (wait_funccb == 1)
			udelay(10);
		wait_funccb = 1;
		ret = twl6030_gpadc_conversion(&req);
		if (ret < 0)
			printk(KERN_INFO "FAIL, return value = %d\n", ret);
		else
		printk(KERN_INFO "OMAP GPADC TEST driver installed successfully\n");
	} /*end of for */
	return ret;
}

static int smp_test2()
{
	struct twl6030_gpadc_request req;
	int ret, i;

	req.channels = channels;
	req.do_avg = 0;
	req.method = TWL6030_GPADC_SW2;
	req.type = TWL6030_GPADC_IRQ_ONESHOT;
	req.active = 0;
	req.result_pending = 0;
	req.func_cb = omap_gpadc_func_cb2;
	for (i = 0; i < iterations; i++) {
		while (wait_funccb == 1)
			udelay(10);
		wait_funccb = 1;
		ret = twl6030_gpadc_conversion(&req);

		while (wait_funccb == 0)
			udelay(10);

		if (ret < 0)
			printk(KERN_INFO "FAIL, return value = %d\n", ret);
			else
			printk(KERN_INFO "OMAP GPADC TEST driver installed successfully\n");
	} /*end of for*/
	return ret;
}

/*
 * Function called when the module is initialized
 */
static int __init gpadc_smp_module_init(void)
{
	struct task_struct *p1, *p2;
	int x;

	p1 = kthread_create(smp_test1, NULL, "gpadctest/0");
	p2 = kthread_create(smp_test2, NULL, "gpadctest/1");
	kthread_bind(p1, 0);
	kthread_bind(p2, 1);
	x = wake_up_process(p1);
	x = wake_up_process(p2);
	return 0;
}

/*
 * Function called when the module is removed
 */
static void __exit gpadc_smp_module_exit(void)
{
	printk(KERN_INFO "\nOMAP GPADC TEST driver un-installed successfully\n");
	return;
}

module_init(gpadc_smp_module_init);
module_exit(gpadc_smp_module_exit);

MODULE_AUTHOR("Texas Instruments");
MODULE_DESCRIPTION("GPADC Test Driver");
MODULE_LICENSE("GPL");
