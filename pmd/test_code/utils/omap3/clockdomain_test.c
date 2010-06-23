/*
 * SRF/cd_test.c
 * OMAP3 Clock Domain test
 *
 * Copyright (C) 2009-2010 Texas Instruments, Inc.
 * Teerth Reddy <teerth@ti.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * THIS PACKAGE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 * History:
 *
 * 2009-10-13: Teerth Reddy        Ported from 23x
 *
 */


#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/file.h>
#include <asm/uaccess.h>
#include <linux/mm.h>

MODULE_LICENSE("GPL");

extern void clock_domain_test(void);

static int __init cd_test_init(void)
{
	clock_domain_test();
	printk("End of Test...\n");
	return 0;
}

static void __exit cd_test_exit(void)
{
	printk(" Exiting Module \n");
	printk(" Done \n");
}

module_init(cd_test_init);
module_exit(cd_test_exit);
