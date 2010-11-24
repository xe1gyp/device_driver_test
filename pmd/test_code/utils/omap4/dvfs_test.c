/*
 * OMAP4 DVFS test file
 *
 * Copyright (C) 2009-2010 Texas Instruments, Inc.
 * Lesly A M <leslyam@ti.com>
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
 * 2010-07-13: Teerth reddy     Initial code
 *
 * 2010-07-30: Lesly A M	Added changes for testing the VDD1/VDD2/VDD3 OPP change
 *				in random orsder with separte threads.
 *				Added changes to test all combination of VDD1/VVD2/VDD3 OPP.
 */


#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/kthread.h>
#include <linux/delay.h>
#include <linux/slab.h>

#include <plat/omap_device.h>
#include <plat/opp.h>
#include <plat/common.h>

#define SCALABLE_VDD	3
#define TEST_VDD1	0x1
#define TEST_VDD2	0x2
#define TEST_VDD3	0x4
#define TEST_VDD_ALL	0x7

#define MPU_VOLT_DOMAIN		0x0
#define DSP_VOLT_DOMAIN		0x1
#define L3_VOLT_DOMAIN		0x2

#define SEC_IN_12HRS	43200
#define FREQ_IN_MHZ	1000000

#define pr_debug(fmt, ...) \
	printk(KERN_DEBUG pr_fmt(fmt), ##__VA_ARGS__)

static uint test_id;
static uint test_vdd;
static uint delay;
static uint debug;

module_param(test_id, int, S_IRUGO|S_IWUSR);
module_param(test_vdd, int, S_IRUGO|S_IWUSR);
module_param(delay, int, S_IRUGO|S_IWUSR);
module_param(debug, int, S_IRUGO|S_IWUSR);

static unsigned long rnd;

static inline unsigned char random_gen(void)
{
	rnd = rnd * 1664525L + 1013904223L;
	return (unsigned char)(rnd >> 24);
}

struct dvfs_data {
	bool enable_test;
	bool test_passed;
	int vdd_num;
	int opp_cnt;
	unsigned long rate_table[8];
	struct completion comp;
	struct device *dev_ptr;
};

static struct dvfs_data dvfs_data_vdd[SCALABLE_VDD];

struct opp_tupple {
	int vdd1_oppnum;
	int vdd2_oppnum;
	int vdd3_oppnum;
	int valid_opp;
}opp_tupple;

struct opp_tupple *opp_tupple_data;

int update_dvfs_data(unsigned int vdd_num)
{
	struct omap_opp *opp;
	unsigned long freq = 0;
	int opp_cnt = 0;
	int indx;

	switch (vdd_num) {
	case 1:
		indx = MPU_VOLT_DOMAIN;
		dvfs_data_vdd[indx].vdd_num = 1;
		dvfs_data_vdd[indx].dev_ptr = omap2_get_mpuss_device();
		break;
	case 2:
		indx = DSP_VOLT_DOMAIN;
		dvfs_data_vdd[indx].vdd_num = 2;
		dvfs_data_vdd[indx].dev_ptr = omap2_get_iva_device();
		break;
	case 4:
		indx = L3_VOLT_DOMAIN;
		dvfs_data_vdd[indx].vdd_num = 3;
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

int update_opp_tuple(void)
{
	int i, j, k;
	int opp_tupple_count = 0;

	int vdd1_opp_cnt = dvfs_data_vdd[0].opp_cnt;
	int vdd2_opp_cnt = dvfs_data_vdd[1].opp_cnt;
	int vdd3_opp_cnt = dvfs_data_vdd[2].opp_cnt;

	struct opp_tupple *tmp_opp_tupple;


	opp_tupple_count = vdd1_opp_cnt * vdd2_opp_cnt * vdd3_opp_cnt;

	opp_tupple_data = (struct opp_tupple *)kmalloc((opp_tupple_count * sizeof(struct opp_tupple)), GFP_KERNEL);

	for (i = 0; i < vdd1_opp_cnt; i++) {
		for (j = 0; j < vdd2_opp_cnt; j++) {
			for (k = 0; k < vdd3_opp_cnt; k++) {

				tmp_opp_tupple = (((opp_tupple_data +
					(i * (vdd2_opp_cnt + vdd3_opp_cnt))) +
					(j * vdd3_opp_cnt)) + k);

				tmp_opp_tupple->vdd1_oppnum = i;
				tmp_opp_tupple->vdd2_oppnum = j;
				tmp_opp_tupple->vdd3_oppnum = k;
				tmp_opp_tupple->valid_opp = (!(i || j) || k);
			}
		}
	}

	pr_info("\nOPP TUPPLE\n");
	pr_info("----------\n");
	pr_info("opp_tupple[i]: < vdd1_opp, vdd2_opp, vdd3_opp > [valid]\n\n");

	for (i = 0; i < opp_tupple_count; i++) {
		tmp_opp_tupple = opp_tupple_data + i;
		pr_info("opp_tupple[%d]: < %d, %d, %d > [%d]\n", i,
					tmp_opp_tupple->vdd1_oppnum + 1,
					tmp_opp_tupple->vdd2_oppnum + 1,
					tmp_opp_tupple->vdd3_oppnum + 1,
					tmp_opp_tupple->valid_opp);
	}
	pr_info("====\n\n");

	return 0;
}



int do_opp_change(struct dvfs_data *vdd_dvfs_data, int freq)
{
	int ret;
	int cur_freq, req_freq;

	req_freq = freq / FREQ_IN_MHZ;

	ret = omap_device_set_rate(vdd_dvfs_data->dev_ptr,
				vdd_dvfs_data->dev_ptr, freq);
	if (ret) {
		if (debug)
			pr_debug("\n Unable to set VDD%d @ %d\n",
					vdd_dvfs_data->vdd_num, req_freq);
		goto skip;
	}

	cur_freq = omap_device_get_rate(vdd_dvfs_data->dev_ptr) / FREQ_IN_MHZ;
	if (cur_freq != req_freq) {
		if (debug) {
			pr_debug("\n * cur_freq %d, req_freq %d ",
					cur_freq, req_freq);
			pr_debug(" Unable to set VDD%d @ %d\n",
					vdd_dvfs_data->vdd_num, req_freq);
		}

		if (vdd_dvfs_data->vdd_num == 3)
			ret = 0;
		else
			ret = -1;
	} else {
		if (debug)
			pr_debug("\n Successfully set VDD%d @ %d\n",
					vdd_dvfs_data->vdd_num, req_freq);
		ret = 0;
	}

skip:
	return ret;
}

int opp_change_tupple(int src, int dst)
{
	int ret = 0;
	struct opp_tupple *tmp_opp_tupple;

	tmp_opp_tupple = opp_tupple_data + dst;

	ret = do_opp_change(&dvfs_data_vdd[0],
		dvfs_data_vdd[0].rate_table[tmp_opp_tupple->vdd1_oppnum]);
	if (ret)
		goto opp_change_fail;
	ret = do_opp_change(&dvfs_data_vdd[1],
		dvfs_data_vdd[1].rate_table[tmp_opp_tupple->vdd2_oppnum]);
	if (ret)
		goto opp_change_fail;
	ret = do_opp_change(&dvfs_data_vdd[2],
		dvfs_data_vdd[2].rate_table[tmp_opp_tupple->vdd3_oppnum]);
	if (ret)
		goto opp_change_fail;

	tmp_opp_tupple = opp_tupple_data + src;

	ret = do_opp_change(&dvfs_data_vdd[0],
		dvfs_data_vdd[0].rate_table[tmp_opp_tupple->vdd1_oppnum]);
	if (ret)
		goto opp_change_fail;
	ret = do_opp_change(&dvfs_data_vdd[1],
		dvfs_data_vdd[1].rate_table[tmp_opp_tupple->vdd2_oppnum]);
	if (ret)
		goto opp_change_fail;
	ret = do_opp_change(&dvfs_data_vdd[2],
		dvfs_data_vdd[2].rate_table[tmp_opp_tupple->vdd3_oppnum]);
	if (ret)
		goto opp_change_fail;

opp_change_fail:
	return ret;
}

int start_dvfs_test1(void)
{
	int i, j, k = 0;
	int ret = 0;
	int opp_tupple_count = 0;
	int vdd1_opp_cnt = dvfs_data_vdd[0].opp_cnt;
	int vdd2_opp_cnt = dvfs_data_vdd[1].opp_cnt;
	int vdd3_opp_cnt = dvfs_data_vdd[2].opp_cnt;
	struct opp_tupple *tmp_opp_tupple;

	pr_info("%s: ...\n", __FUNCTION__);

	opp_tupple_count = vdd1_opp_cnt * vdd2_opp_cnt * vdd3_opp_cnt;

	for (i = 0; i < opp_tupple_count; i++) {

		tmp_opp_tupple = opp_tupple_data + i;
		if (!tmp_opp_tupple->valid_opp)
			continue;

		for (j = (i + 1); j < opp_tupple_count; j++) {

			tmp_opp_tupple = opp_tupple_data + j;
			if (!tmp_opp_tupple->valid_opp)
				continue;

			if (debug)
				pr_info("%s: i [%d] -> j [%d]\n", __FUNCTION__, i, j);
			ret = opp_change_tupple(i, j);
			k++;
		}
	}

	pr_info("dvfs_test1 completed ... [%d]\n", k);
	return ret;
}

int dvfs_stress_test(void *data)
{
	int opp_num;
	int ret = 0;
	struct timespec start_time;
	int time_delta;
	int freq, cur_freq, req_freq;
	unsigned int rnd_number = 0;
	unsigned int sleep_time = 0;

	struct dvfs_data *vdd_dvfs_data = (struct dvfs_data *)data;

	start_time = current_kernel_time();

	while (1) {

		rnd_number = (unsigned int)random_gen();
		opp_num = rnd_number % vdd_dvfs_data->opp_cnt;
		freq = vdd_dvfs_data->rate_table[opp_num];
		req_freq = freq / FREQ_IN_MHZ;

		ret = omap_device_set_rate(vdd_dvfs_data->dev_ptr, vdd_dvfs_data->dev_ptr, freq);
		if (ret) {
			if (debug)
				pr_debug("\n Unable to set VDD%d @ %d\n",
					vdd_dvfs_data->vdd_num, req_freq);
			break;
		}

		cur_freq = omap_device_get_rate(vdd_dvfs_data->dev_ptr) / FREQ_IN_MHZ;

		if (cur_freq != req_freq) {
			if (debug) {
				pr_warning(" * get_rate %d, req_freq %d \n", cur_freq, req_freq);
				pr_debug("\n Unable to set VDD%d @ %d\n",
					vdd_dvfs_data->vdd_num, req_freq);
			}

			if (vdd_dvfs_data->vdd_num == 3) {
				ret = 0;
			} else {
				ret = -1;
				break;
			}

		} else {
			if (debug)
				pr_debug("\n Successfully set VDD%d @ %lu\n", vdd_dvfs_data->vdd_num,
					vdd_dvfs_data->rate_table[opp_num] / FREQ_IN_MHZ);

			if (delay)
				msleep_interruptible(delay);
			else {
				sleep_time = 50 * (rnd_number % 10);
				msleep_interruptible(sleep_time);
			}

			ret = 0;
		}


		time_delta = (current_kernel_time().tv_sec - start_time.tv_sec);
		//if (ret || (time_delta > SEC_IN_12HRS))
		if (ret || (time_delta > 60))
			break;
	}

	if (ret)
		vdd_dvfs_data->test_passed = false;
	else
		vdd_dvfs_data->test_passed = true;

	if (test_vdd == TEST_VDD_ALL)
		complete(&vdd_dvfs_data->comp);

	pr_debug("\n\nVDD%d: dvfs test completed (%d) ...\n", vdd_dvfs_data->vdd_num, ret);
	return ret;
}

int start_dvfs_test2(void)
{
	int i, ret = 0;
	bool test_pass = 0;
	struct task_struct *p;

	pr_info("%s: ...\n", __FUNCTION__);

	switch (test_vdd) {
	case 1:
		ret = dvfs_stress_test(&dvfs_data_vdd[MPU_VOLT_DOMAIN]);
		break;
	case 2:
		ret = dvfs_stress_test(&dvfs_data_vdd[DSP_VOLT_DOMAIN]);
		break;
	case 4:
		ret = dvfs_stress_test(&dvfs_data_vdd[L3_VOLT_DOMAIN]);
		break;
	case 7:
		for (i = MPU_VOLT_DOMAIN; i < SCALABLE_VDD; i++) {
			if (dvfs_data_vdd[i].enable_test) {
				init_completion(&dvfs_data_vdd[i].comp);
	        		p = kthread_create(dvfs_stress_test, &dvfs_data_vdd[i], "dvfs_test/%d", i);
				if (IS_ERR(p))
					return PTR_ERR(p);

				/* bind all threads to CPU0 */
			        kthread_bind(p, 0);

				if (!wake_up_process(p)) {
					pr_err("Could not start the thread.\n");
					return -1;
				}
			}
		}

		for (i = MPU_VOLT_DOMAIN; i < SCALABLE_VDD; i++)
			if (dvfs_data_vdd[i].enable_test)
				wait_for_completion(&dvfs_data_vdd[i].comp);

		test_pass = dvfs_data_vdd[MPU_VOLT_DOMAIN].test_passed;
		test_pass |= dvfs_data_vdd[DSP_VOLT_DOMAIN].test_passed;
		test_pass |= dvfs_data_vdd[L3_VOLT_DOMAIN].test_passed;

		if (test_pass)
			ret = 0;
		else
			ret = -1;

		break;
	default:
		pr_info("Invalid test_vdd option ...\n\n");
	}

	return ret;
}

int start_dvfs_test3(void)
{
	int opp_num, opp_cnt;
	int indx, ret = 0;

	pr_info("%s: ...\n", __FUNCTION__);

	for (indx = 0; indx < SCALABLE_VDD; indx++) {
		if (dvfs_data_vdd[indx].enable_test) {

			opp_cnt = dvfs_data_vdd[indx].opp_cnt;
			for (opp_num = opp_cnt - 1; opp_num >= 0; opp_num--) {

				ret = do_opp_change(&dvfs_data_vdd[indx], dvfs_data_vdd[indx].rate_table[opp_num]);
				if (ret)
					break;
				else
					mdelay(delay);
			}
		}
		if (ret)
			break;
	}

	pr_info("dvfs_test3 completed ... \n");
	return ret;
}

int __init dvfs_test_init(void)
{
	int ret = 0;

	pr_info("\n*************************************************************\n");
	pr_info("\t\tDVFS STRESS TEST MODULE HELP\n");
	pr_info("*************************************************************\n");
	pr_info("\nUsage: insmod dvfs_test.ko test_id=<1,2,3> \n\n");

	pr_info("test_id:[1] do all possible combination of VDD1/VDD2/VDD3 OPP change\n\n");

	pr_info("test_id:[2] do the OPP change in random order\n");
	pr_info("\tUse hex value:- test_vdd\n");
	pr_info("\t0x1\t- VDD1 dvfs test\n");
	pr_info("\t0x2\t- VDD2 dvfs test\n");
	pr_info("\t0x4\t- VDD3 dvfs test\n");
	pr_info("\t0x7\t- VDD1/VDD2/VDD3 dvfs test\n");
	pr_info("\tdelay: delay in mSec between OPP change\n\n");

	pr_info("test_id:[3] do the OPP change in sequential order\n\n");

	pr_info("debug: enable debug prints\n");
	pr_info("*************************************************************\n\n");

	ret = update_dvfs_data(TEST_VDD1);
	ret |= update_dvfs_data(TEST_VDD2);
	ret |= update_dvfs_data(TEST_VDD3);

	if (ret)
		goto end;

	switch (test_id) {
	case 1:
		ret |= update_opp_tuple();
		if (ret)
			goto end;
		else
			ret = start_dvfs_test1();
		break;
	case 2:
		if (test_vdd == 0) { 
			pr_info("* Please specify the test_vdd=<option>\n\n");
			goto end;
		} else {
			ret = start_dvfs_test2();
		}
		break;
	case 3:
		ret = start_dvfs_test3();
		break;
	default:
		pr_info("Usage: insmod dvfs_test.ko test_id=<1,2,3>n\n");
		ret = -1;
	}

	if (ret)
		pr_alert("\nTEST FAILED\n");
	else
		pr_alert("\nTEST PASSED\n");

end:
	return ret;
}

void __exit dvfs_test_exit(void)
{
	kfree(opp_tupple_data);

	pr_info("\nExiting DVFS test ...\n");
}

module_init(dvfs_test_init);
module_exit(dvfs_test_exit);

MODULE_AUTHOR("Texas Instruments");
MODULE_LICENSE("GPL");
