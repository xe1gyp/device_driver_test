/*
 * OMAP4 DVFS test file
 *
 * Copyright (C) 2009-2010 Texas Instruments, Inc.
 * Lesly A M <leslyam@ti.com>
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
 * 2010-07-30: Lesly A M	initial code
 *
 */


#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/delay.h>
#include <linux/platform_device.h>

#include <plat/omap_device.h>
#include <plat/opp.h>
#include <plat/common.h>

#include <plat/omap_hwmod.h>


#define SCALABLE_VDD	3
#define TEST_VDD1	0x1
#define TEST_VDD2	0x2
#define TEST_VDD3	0x4
#define TEST_VDD_ALL	0x7

#define SEC_IN_12HRS	43200
#define FREQ_IN_MHZ	1000000

static uint test_vdd;
static uint delay;

module_param(test_vdd, int, S_IRUGO|S_IWUSR);
module_param(delay, int, S_IRUGO|S_IWUSR);


struct dvfs_data {
		bool enable_test;
		int opp_cnt;
		unsigned long rate_table[8];
		struct device *dev_ptr; };

static struct dvfs_data dvfs_data_vdd[SCALABLE_VDD];

extern struct omap_hwmod *_lookup(const char *name);

int update_dvfs_data(unsigned int vdd_num)
{
	struct omap_opp *opp;
	unsigned long freq = 0;
	int opp_cnt = 0;
	int indx;

	switch (vdd_num) {
	case 1:
		indx = 0;
		dvfs_data_vdd[indx].dev_ptr = omap2_get_mpuss_device();
		break;
	case 2:
		indx = 1;
		dvfs_data_vdd[indx].dev_ptr = omap2_get_iva_device();
		break;
	case 4:
		indx = 2;
		dvfs_data_vdd[indx].dev_ptr = omap2_get_l3_device();
		break;
	default:
		pr_err("Invalid VDD num.\n");
		/* Invalid VDD number */
		return -1;
	}

	if (dvfs_data_vdd[indx].dev_ptr == NULL) {
		pr_err("dev_ptr could not be initialized.\n");
		return -1;
	}

	while (!IS_ERR(opp = opp_find_freq_ceil(dvfs_data_vdd[indx].dev_ptr, &freq))) {

		dvfs_data_vdd[indx].rate_table[opp_cnt] = freq;
		opp_cnt++;
		pr_info("%d. freq = %ld MHz\n", opp_cnt, (freq / FREQ_IN_MHZ));
		freq = freq + FREQ_IN_MHZ;
	}

	if (opp_cnt == 0) {
		pr_err("rate_table could not be initialized.\n");
		return -1;
	} else {
		pr_info("---\n\n");
	}

	dvfs_data_vdd[indx].opp_cnt = opp_cnt;
	dvfs_data_vdd[indx].enable_test = true;
	return 0;
}

struct device *find_dev_ptr(char *name)
{
	struct omap_hwmod *oh;
	struct omap_device *od_ptr;
	struct platform_device	*pdev_ptr;
	struct device *dev_ptr1;

	oh = _lookup(name);

	od_ptr = oh->od;
	pdev_ptr = (struct platform_device *)&od_ptr->pdev;
	dev_ptr1 = (struct device *)&pdev_ptr->dev;
/*	pr_info(" %s: %s_%d \n", __FUNCTION__, pdev_ptr->name, pdev_ptr->id); */

	return dev_ptr1;
}


int dvfs_functional_test_vdd3(void)
{
	int ret = 0;
	int indx, opp_num;
	int cur_freq, req_freq;
	struct device *mpu_dev_ptr;
	struct device *iva_dev_ptr;
	struct device *l3_dev_ptr;

	struct device *mmc1_devptr;
	struct device *mcspi1_devptr;

	mmc1_devptr = find_dev_ptr("mmc1");
	mcspi1_devptr = find_dev_ptr("mcspi1");

	/* chane MPU & IVA to 50% OPP */
	opp_num = 0;

	indx = 0;
	mpu_dev_ptr = dvfs_data_vdd[indx].dev_ptr;
	req_freq = dvfs_data_vdd[indx].rate_table[opp_num];
	ret = omap_device_set_rate(mpu_dev_ptr, mpu_dev_ptr, req_freq);
	if (ret) {
		printk("Unable to change the MPU to OPP_50\n");
		ret = -1;
		goto end1;
	}

	indx = 1;
	iva_dev_ptr = dvfs_data_vdd[indx].dev_ptr;
	req_freq = dvfs_data_vdd[indx].rate_table[opp_num];
	ret = omap_device_set_rate(iva_dev_ptr, iva_dev_ptr, req_freq);
	if (ret) {
		printk("Unable to change the IVA to OPP_50\n");
		ret = -1;
		goto end1;
	}


	/* CORE @ OPP 100% */
	indx = 2;
	opp_num = 1;
	l3_dev_ptr = dvfs_data_vdd[indx].dev_ptr;
	req_freq = dvfs_data_vdd[indx].rate_table[opp_num];
	ret = omap_device_set_rate(mmc1_devptr, l3_dev_ptr, req_freq);

	cur_freq = omap_device_get_rate(l3_dev_ptr) / FREQ_IN_MHZ;
	if (cur_freq != (req_freq / FREQ_IN_MHZ)) {
		printk("Unable to change the CORE to OPP_100\n");
		ret = -1;
		goto end1;
	}
	printk("1. CORE req_freq: %d, cur_freq: %d\n\n", req_freq, cur_freq);

	/* CORE @ OPP 100% */
	indx = 2;
	opp_num = 1;
	l3_dev_ptr = dvfs_data_vdd[indx].dev_ptr;
	req_freq = dvfs_data_vdd[indx].rate_table[opp_num];
	ret = omap_device_set_rate(mcspi1_devptr, l3_dev_ptr, req_freq);

	cur_freq = omap_device_get_rate(l3_dev_ptr) / FREQ_IN_MHZ;
	if (cur_freq != (req_freq / FREQ_IN_MHZ)) {
		printk("Unable to change the CORE to OPP_100\n");
		ret = -1;
		goto end1;
	}
	printk("2. CORE req_freq: %d, cur_freq: %d\n\n", req_freq, cur_freq);

	/* CORE @ OPP 50% */
	indx = 2;
	opp_num = 0;
	l3_dev_ptr = dvfs_data_vdd[indx].dev_ptr;
	req_freq = dvfs_data_vdd[indx].rate_table[opp_num];
	ret = omap_device_set_rate(mcspi1_devptr, l3_dev_ptr, req_freq);

	cur_freq = omap_device_get_rate(l3_dev_ptr) / FREQ_IN_MHZ;
	if (cur_freq == (req_freq / FREQ_IN_MHZ)) {
		printk("Change the CORE to OPP_50 (w OPP_100 req pending)\n");
		ret = -1;
		goto end1;
	}
	printk("3. CORE req_freq: %d, cur_freq: %d\n\n", req_freq, cur_freq);

	/* CORE @ OPP 50% */
	indx = 2;
	opp_num = 0;
	l3_dev_ptr = dvfs_data_vdd[indx].dev_ptr;
	req_freq = dvfs_data_vdd[indx].rate_table[opp_num];
	ret = omap_device_set_rate(mmc1_devptr, l3_dev_ptr, req_freq);

	cur_freq = omap_device_get_rate(l3_dev_ptr) / FREQ_IN_MHZ;
	if (cur_freq != (req_freq / FREQ_IN_MHZ)) {
		printk("Unable to change the CORE to OPP_50\n");
		ret = -1;
		goto end1;
	}
	printk("4. CORE req_freq: %d, cur_freq: %d\n\n", req_freq, cur_freq);

end1:

	return ret;
}


int __init dvfs_func_test_init(void)
{
	int ret = 0;

	pr_info("\n***************************************************\n");
	pr_info("\t\tDVFS TEST MODULE HELP\n");
	pr_info("***************************************************\n");
	pr_info("Usage: insmod dvfs_func_test.ko\n\n");
	pr_info("***************************************************\n\n");

	ret = update_dvfs_data(TEST_VDD1);
	ret = update_dvfs_data(TEST_VDD2);
	ret = update_dvfs_data(TEST_VDD3);

	if (ret)
		goto end;

	ret = dvfs_functional_test_vdd3();

end:
	if (ret)
		pr_info("\nTEST FAILED\n");
	else
		pr_info("\nTEST PASSED\n");

	return ret;
}

void __exit dvfs_func_test_exit(void)
{
	pr_info("\nExiting DVFS functional test ...\n");
}

module_init(dvfs_func_test_init);
module_exit(dvfs_func_test_exit);

MODULE_AUTHOR("Texas Instruments");
MODULE_LICENSE("GPL");
