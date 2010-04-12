/*
 * utils/omap4/pd_test.c
 * OMAP4 power domain test file
 *
 * Copyright (C) 2009-2010 Texas Instruments, Inc.
 * Abhijit Pagare <abhijitpagare@ti.com>
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
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/file.h>
#include <asm/uaccess.h>
#include <linux/mm.h>

MODULE_LICENSE("GPL");

#define PWRDM_MAX_MEM_BANKS	5
#define PWRDM_MAX_CLKDMS	9
#define PWRDM_MAX_PWRSTS	4

/* Powerdomain basic power states */
#define PWRDM_POWER_OFF		0x0
#define PWRDM_POWER_RET		0x1
#define PWRDM_POWER_INACTIVE	0x2
#define PWRDM_POWER_ON		0x3

struct omap_chip_id {
	u16 oc;
	u8 type;
};

struct clockdomain {
	const char *name;
	union {
		const char *name;
		struct powerdomain *ptr;
	} pwrdm;
	void __iomem *clkstctrl_reg;
	const u16 clktrctrl_mask;
	const u8 flags;
	const u8 dep_bit;
	struct clkdm_dep *wkdep_srcs;
	struct clkdm_dep *sleepdep_srcs;
	const struct omap_chip_id omap_chip;
	atomic_t usecount;
	struct list_head node;
};

struct powerdomain {
	const char *name;
	const struct omap_chip_id omap_chip;
	const s16 prcm_offs;
	const u8 pwrsts;
	const u8 pwrsts_logic_ret;
	const u8 flags;
	const u8 banks;
	const u8 pwrsts_mem_ret[PWRDM_MAX_MEM_BANKS];
	const u8 pwrsts_mem_on[PWRDM_MAX_MEM_BANKS];
	struct clockdomain *pwrdm_clkdms[PWRDM_MAX_CLKDMS];
	struct list_head node;
	int state;
	unsigned state_counter[PWRDM_MAX_PWRSTS];
};

extern struct powerdomain *pwrdm_lookup(const char *name);

extern int pwrdm_for_each(int (*fn)(struct powerdomain *pwrdm, void *user),
                        void *user);

extern int pwrdm_add_clkdm(struct powerdomain *pwrdm, struct clockdomain *clkdm);
extern int pwrdm_del_clkdm(struct powerdomain *pwrdm, struct clockdomain *clkdm);
extern int pwrdm_for_each_clkdm(struct powerdomain *pwrdm,
                         int (*fn)(struct powerdomain *pwrdm,
                                   struct clockdomain *clkdm));

extern int pwrdm_get_mem_bank_count(struct powerdomain *pwrdm);

extern int pwrdm_set_next_pwrst(struct powerdomain *pwrdm, u8 pwrst);
extern int pwrdm_read_next_pwrst(struct powerdomain *pwrdm);
extern int pwrdm_read_pwrst(struct powerdomain *pwrdm);
extern int pwrdm_read_prev_pwrst(struct powerdomain *pwrdm);
extern int pwrdm_clear_all_prev_pwrst(struct powerdomain *pwrdm);

extern int pwrdm_set_logic_retst(struct powerdomain *pwrdm, u8 pwrst);
extern int pwrdm_set_mem_onst(struct powerdomain *pwrdm, u8 bank, u8 pwrst);
extern int pwrdm_set_mem_retst(struct powerdomain *pwrdm, u8 bank, u8 pwrst);

extern int pwrdm_read_logic_pwrst(struct powerdomain *pwrdm);
extern int pwrdm_read_prev_logic_pwrst(struct powerdomain *pwrdm);
extern int pwrdm_read_mem_pwrst(struct powerdomain *pwrdm, u8 bank);
extern int pwrdm_read_prev_mem_pwrst(struct powerdomain *pwrdm, u8 bank);

extern int pwrdm_enable_hdwr_sar(struct powerdomain *pwrdm);
extern int pwrdm_disable_hdwr_sar(struct powerdomain *pwrdm);
extern bool pwrdm_has_hdwr_sar(struct powerdomain *pwrdm);

extern int pwrdm_wait_transition(struct powerdomain *pwrdm);

extern int pwrdm_state_switch(struct powerdomain *pwrdm);
extern int pwrdm_clkdm_state_switch(struct clockdomain *clkdm);
extern int pwrdm_pre_transition(void);
extern int pwrdm_post_transition(void);

void power_domain_test(void);
static int power_test_1(void);
static int power_test_2(void);
static int power_test_3(void);
static int power_test_4(void);

const char *powerdomains_name[] = {
"core_pwrdm",
"gfx_pwrdm",
"abe_pwrdm",
"dss_pwrdm",
"tesla_pwrdm",
"wkup_pwrdm",
"cpu0_pwrdm",
"cpu1_pwrdm",
"emu_pwrdm",
"mpu_pwrdm",
"ivahd_pwrdm",
"cam_pwrdm",
"l3init_pwrdm",
"l4per_pwrdm",
"always_on_core_pwrdm",
"cefuse_pwrdm",
NULL
};

static struct powerdomain *pwrdm_omap;

static struct clockdomain dummy_clkdm = {
	.name = "dummy_cd1",
};

static uint test_case;
module_param(test_case, int, S_IRUGO|S_IWUSR);

/*
 * power_test_1 - Test some basic power domain api's
 *
 * This set tests the API's for the link between the power domains and
 * the clock domains by make use of a dummy clock domain and some other
 * preliminary api's.
 */
static int power_test_1()
{
	int val = -EINVAL, ret = 0;
	static struct powerdomain *p;

	printk("\n\nEntered Power Test 1.....\n\n");
	val = pwrdm_add_clkdm(p = pwrdm_omap, &dummy_clkdm);
	if (val == 0)
		printk(KERN_INFO "Clock Domain Registered for %s\n", p->name);
	else if (val == -EINVAL) {
		printk(KERN_ERR "Failed for %s\n", p->name);
		ret = -1;
	}

	val = pwrdm_del_clkdm(p = pwrdm_omap, &dummy_clkdm);
	if (val == 0)
		printk(KERN_INFO "Clock Domain Unregistered for %s\n", p->name);
	else if (val == -EINVAL) {
		printk(KERN_ERR "Failed for %s\n", p->name);
		ret = -1;
	}

	val = pwrdm_get_mem_bank_count(p = pwrdm_omap);
	printk(KERN_INFO "Bank Count for %s = %d\n", p->name, val);
	return ret;

}

/*
 * power_test_2 - Test the power transitions
 *
 * This set tests the API's for testing the power state transitions of the
 * power domains and also validates the status after the transitions.
 */
static int power_test_2()
{
	int val = -EINVAL, ret = 0;
	int ret1 = 0;
	static struct powerdomain *p;

	printk("\n\nEntered Power Test 2.....\n\n");
	ret1 = pwrdm_read_pwrst(p = pwrdm_omap);
	printk(KERN_INFO "Default Power State of %s = %d\n", p->name, ret1);

	val = pwrdm_set_next_pwrst(p = pwrdm_omap, PWRDM_POWER_ON);
	if (val == 0)
		printk(KERN_INFO "Setting Next PWRST for %s to ON\n", p->name);
	else if (val == -EINVAL) {
		printk(KERN_INFO "ON not supported for %s\n", p->name);
		goto loc1;
	} else {
		printk(KERN_ERR "Failed with value %d\n", val);
		ret = -1;
		goto loc1;
	}

	ret1 = pwrdm_read_next_pwrst(p);
	if (ret1 == PWRDM_POWER_ON)
		printk(KERN_INFO "Successfully set ON for %s\n", p->name);
	else {
		printk(KERN_INFO "Unsuccessful, Please Validate!!!\n");
		ret = -1;
	}

loc1:
	val = pwrdm_set_next_pwrst(p = pwrdm_omap, PWRDM_POWER_RET);
	if (val == 0)
		printk(KERN_INFO "Setting Next PWRST for %s to RET\n", p->name);
	else if (val == -EINVAL) {
		printk(KERN_INFO "RET not supported for %s\n", p->name);
		goto loc2;
	} else {
		printk(KERN_ERR "Failed with value %d\n", val);
		ret = -1;
		goto loc2;
	}

	ret1 = pwrdm_read_next_pwrst(p);
	if (ret1 == PWRDM_POWER_RET)
		printk(KERN_INFO "Successfully set RET for %s\n", p->name);
	else {
		printk(KERN_INFO "Unsuccessful, Please Validate!!!\n");
		ret = -1;
	}

loc2:
	val = pwrdm_set_next_pwrst(p = pwrdm_omap, PWRDM_POWER_OFF);
	if (val == 0)
		printk(KERN_INFO "Setting Next PWRST for %s to OFF\n", p->name);
	else if (val == -EINVAL) {
		printk(KERN_INFO "OFF not supported for %s\n", p->name);
		goto loc3;
	} else {
		printk(KERN_ERR "Failed with value %d\n", val);
		ret = -1;
		goto loc3;
	}

	ret1 = pwrdm_read_next_pwrst(p);
	if (ret1 == PWRDM_POWER_OFF)
		printk(KERN_INFO "Successfully set OFF for %s\n", p->name);
	else {
		printk(KERN_INFO "Unsuccessful, Please Validate!!!\n");
		ret = -1;
	}

loc3:
	return ret;
}

/*
 * power_test_3 - Test the power transitions
 *
 * This set tests the API's for testing the power state transitions of the
 * logic and memory components and also validates the status after the transitions.
 */
static int power_test_3()
{
	int bank;
	int val = -EINVAL, ret = 0;
	int ret1 = 0;
	static struct powerdomain *p;

	printk("\n\nEntered Power Test 3.....\n\n");
	val = pwrdm_read_logic_pwrst(p = pwrdm_omap);
	printk(KERN_INFO "Default Logic Power State of %s = %d\n",
							 p->name, val);

	val = pwrdm_set_logic_retst(p = pwrdm_omap,
					 PWRDM_POWER_RET);
	if (val == 0)
		printk(KERN_INFO "Setting Logic-Ret State RET for %s\n", p->name);
	else if (val == -EINVAL) {
		printk(KERN_INFO "Logic-ret RET State Not supported"
					" for %s\n", p->name);
		goto dest1;
	} else {
		printk(KERN_ERR "Failed with value %d\n", val);
		ret = -1;
		goto dest1;
	}

	ret1 = pwrdm_read_logic_pwrst(p = pwrdm_omap);
	if (ret1 == PWRDM_POWER_RET)
		printk(KERN_INFO "Successfully set Logic-ret RET for"
						" %s\n", p->name);
	else {
		printk(KERN_INFO "Unsuccessful, Please Validate!!!\n");
		ret = -1;
	}

dest1:
	val = pwrdm_set_logic_retst(p = pwrdm_omap,
					 PWRDM_POWER_OFF);
	if (val == 0)
		printk(KERN_INFO "Setting Logic-Ret State OFF for %s\n",
						 p->name);
	else if (val == -EINVAL) {
		printk(KERN_INFO "OFF State Not supported for %s\n", p->name);
		goto dest2;
	} else {
		printk(KERN_ERR "Failed with value %d\n", val);
		ret = -1;
		goto dest2;
	}

	ret1 = pwrdm_read_logic_pwrst(p = pwrdm_omap);
	if (ret1 == PWRDM_POWER_OFF)
		printk(KERN_INFO "Successfully set Logic-ret OFF for"
						 " %s\n", p->name);
	else {
		printk(KERN_INFO "Unsuccessful, Please Validate!!!\n");
		ret = -1;
	}

dest2:
	for (bank = 0; bank < PWRDM_MAX_MEM_BANKS; bank++) {
		val = pwrdm_set_mem_onst(p = pwrdm_omap, bank, PWRDM_POWER_RET);
		if (val == 0)
			printk(KERN_INFO "Setting Memory-on State RET for %s"
					 " bank%d\n", p->name, bank);
		else if (val == -EINVAL) {
			printk(KERN_INFO "Mem-on RET State Not supported for"
					 " %s bank%d\n", p->name, bank);
			goto dest3;
		} else if (val == -EEXIST)
			break;
		else {
			printk(KERN_ERR "Failed with value %d\n", val);
			ret = -1;
			goto dest3;
		}

		ret1 = pwrdm_read_mem_pwrst(p = pwrdm_omap, bank);
		if (ret1 == PWRDM_POWER_RET)
			printk(KERN_INFO "Successfully set Mem-on RET"
					 " state for bank %d of %s = %d\n",
					 bank, p->name, ret1);
		else {
			printk(KERN_INFO "Unsuccessful, Please Validate!!!\n");
			ret = -1;
		}
	}

dest3:
	for (bank = 0; bank < PWRDM_MAX_MEM_BANKS; bank++) {
		val = pwrdm_set_mem_onst(p = pwrdm_omap, bank, PWRDM_POWER_OFF);
		if (val == 0)
			printk(KERN_INFO "Setting Memory-on State OFF for %s"
					 " bank%d\n", p->name, bank);
		else if (val == -EINVAL) {
			printk(KERN_INFO "Mem-on OFF State Not supported for"
					 " %s bank%d\n", p->name, bank);
			goto dest4;
		} else if (val == -EEXIST)
			break;
		else {
			printk(KERN_ERR "Failed with value %d\n", val);
			ret = -1;
			goto dest4;
		}

		ret1 = pwrdm_read_mem_pwrst(p = pwrdm_omap, bank);
		if (ret1 == PWRDM_POWER_OFF)
			printk(KERN_INFO "Successfully set Mem-on OFF"
			" state for bank %d of %s = %d\n", bank, p->name, ret1);
		else {
			printk(KERN_INFO "Unsuccessful, Please Validate!!!\n");
			ret = -1;
		}
	}

dest4:
	for (bank = 0; bank < PWRDM_MAX_MEM_BANKS; bank++) {
		val = pwrdm_set_mem_retst(p = pwrdm_omap, bank, PWRDM_POWER_RET);
		if (val == 0)
			printk(KERN_INFO "Setting Memory-ret State RET for %s"
					 " bank%d\n", p->name, bank);
		else if (val == -EINVAL) {
			printk(KERN_INFO "Mem-ret RET State Not supported for"
					 " %s bank%d\n", p->name, bank);
			goto dest5;
		} else if (val == -EEXIST)
			break;
		else {
			printk(KERN_ERR "Failed with value %d\n", val);
			ret = -1;
			goto dest5;
		}

		ret1 = pwrdm_read_mem_pwrst(p = pwrdm_omap, bank);
		if (ret1 == PWRDM_POWER_OFF)
			printk(KERN_INFO "Successfully set Mem-ret RET"
			" state for bank %d of %s = %d\n",
					 bank, p->name, ret1);
		else {
			printk(KERN_INFO "Unsuccessful, Please Validate!!!\n");
			ret = -1;
		}
	}

dest5:
	for (bank = 0; bank < PWRDM_MAX_MEM_BANKS; bank++) {
		val = pwrdm_set_mem_retst(p = pwrdm_omap,
					 bank, PWRDM_POWER_OFF);
		if (val == 0)
			printk(KERN_INFO "Setting Memory-ret State OFF for %s"
					 " bank%d\n", p->name, bank);
		else if (val == -EINVAL) {
			printk(KERN_INFO "Mem-ret OFF State Not supported for"
					 " %s bank%d\n", p->name, bank);
			goto dest6;
		} else if (val == -EEXIST)
			break;
		else {
			printk(KERN_ERR "Failed with value %d\n", val);
			ret = -1;
			goto dest6;
		}

		ret1 = pwrdm_read_mem_pwrst(p = pwrdm_omap, bank);
		if (ret1 == PWRDM_POWER_OFF)
			printk(KERN_INFO "Successfully set Mem-ret OFF"
			" state for bank %d of %s = %d\n", bank, p->name, ret1);
		else {
			printk(KERN_INFO "Unsuccessful, Please Validate!!!\n");
			ret = -1;
		}
	}

dest6:
	return ret;

}

/*
 * power_test_4 - Test the Harware SAR related Support
 *
 * This set tests API's for enabling and disabling the support for
 * Hardware SAR
 */
static int power_test_4()
{
	int ret = 0;

	/* FIXME : Upgrade once OMAP4 Hardware SAR is verified */
	/* Not supported as of now....!!!! */
	//printk("\n\nEntered Power Test 4.....\n\n");

	return ret;
}

void power_domain_test()
{
	int i,
	ret = -1;

	printk("Entered the Module....\n\n");

	switch (test_case) {

	case 0x1:
		for (i = 0; powerdomains_name[i]; i++) {
			pwrdm_omap = pwrdm_lookup(powerdomains_name[i]);
			ret = power_test_1();
			if (ret) {
				printk("\n\nTest Failed\n\n");
				break;
			}
		}
		if(!ret)
			printk("\n\nTest Passed\n\n");
		break;
	case 0x2:
		for (i = 0; powerdomains_name[i]; i++) {
			pwrdm_omap = pwrdm_lookup(powerdomains_name[i]);
			/* FIXME: Update once the prev_pwrst reg is in place */
			if (strcmp(powerdomains_name[i], "cpu0_pwrdm") &&
			strcmp(powerdomains_name[i], "cpu1_pwrdm") &&
			strcmp(powerdomains_name[i], "mpu_pwrdm") &&
			strcmp(powerdomains_name[i], "wkup_pwrdm") &&
			strcmp(powerdomains_name[i], "always_on_core_pwrdm") &&
			strcmp(powerdomains_name[i], "emu_pwrdm")) {
				ret = power_test_2();
				if (ret) {
					printk("\n\nTest Failed\n\n");
					break;
				}
			} else {
				printk("Test case 2 not supported for %s\n\n",
							powerdomains_name[i]);
				continue;
			}
		}
		if (!ret)
			printk("\n\nTest Passed\n\n");
		break;
	case 0x3:
		for (i = 0; powerdomains_name[i]; i++) {
			pwrdm_omap = pwrdm_lookup(powerdomains_name[i]);
			/* FIXME: Update once the prev_pwrst reg is in place */
			if (strcmp(powerdomains_name[i], "cpu0_pwrdm") &&
			strcmp(powerdomains_name[i], "cpu1_pwrdm") &&
			strcmp(powerdomains_name[i], "mpu_pwrdm") &&
			strcmp(powerdomains_name[i], "wkup_pwrdm") &&
			strcmp(powerdomains_name[i], "always_on_core_pwrdm") &&
			strcmp(powerdomains_name[i], "emu_pwrdm")) {
				ret = power_test_3();
				if (ret) {
					printk("\n\nTest Failed\n\n");
					break;
				}
			} else {
				printk("Test case 3 not supported for %s\n",
							powerdomains_name[i]);
				continue;
			}
		}
		if (!ret)
			printk("\n\nTest Passed\n\n");
		break;
	case 0x4:
		for (i = 0; powerdomains_name[i]; i++) {
			pwrdm_omap = pwrdm_lookup(powerdomains_name[i]);
			ret = power_test_4();
		}
		/* FIXME: Remove this print once test case 4 is valid */
		printk("Test Case 4 not supported currently...!!!\n");
		/* Uncomment when supported */
		/*
		if (!ret)
			printk("\n\nTest Passed\n\n");
		else
			printk("\n\nTest Failed\n\n");
		 */
		break;
	default :
		printk (" INVALID PARAMETERS- 			       \n");
		printk (" Please use below HELP MENU for testcase usage\n");
		printk (" ***************** HELP MENU **************** \n");
		printk (" ::::Use hex value - test function mapping::::\n");
		printk (" test_case=0x1          - power_test_1             \n");
		printk (" test_case=0x2          - power_test_2             \n");
		printk (" test_case=0x3          - power_test_3             \n");
		printk (" test_case=0x4          - power_test_4             \n");
		printk (" Usage: insmod pd_test.ko test_case=<option>       \n");
		printk (" ******************* HELP ******************* \n");
	}

}

static int __init pd_test_init(void)
{
	power_domain_test();
	printk("\nEnd of Test...\n\n");
	return 0;
}

static void __exit pd_test_exit(void)
{
	printk("Exiting Module...\n");
	printk("Done\n");
}

module_init(pd_test_init);
module_exit(pd_test_exit);
