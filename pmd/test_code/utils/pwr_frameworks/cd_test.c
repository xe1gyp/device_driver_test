/*
 * utils/omap4/cd_test.c
 * OMAP4 clock domain test file
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

struct clk {
	struct list_head	node;
	const struct clkops	*ops;
	const char		*name;
	int			id;
	struct clk		*parent;
	struct list_head	children;
	struct list_head	sibling;	/* node for children */
	unsigned long		rate;
	__u32			flags;
	void __iomem		*enable_reg;
	unsigned long		(*recalc)(struct clk *);
	int			(*set_rate)(struct clk *, unsigned long);
	long			(*round_rate)(struct clk *, unsigned long);
	void			(*init)(struct clk *);
	__u8			enable_bit;
	__s8			usecount;
	u8			fixed_div;
	void __iomem		*clksel_reg;
	u32			clksel_mask;
	const struct clksel	*clksel;
	struct dpll_data	*dpll_data;
	const char		*clkdm_name;
	struct clockdomain	*clkdm;
};

/* FIXME: Remove once dependency testing is supported
struct clkdm_autodep {
        union {
		const char *name;
		struct clockdomain *ptr;
        } clkdm;
        const struct omap_chip_id omap_chip;
};
 */

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

extern struct clockdomain *clkdm_lookup(const char *name);

extern int clkdm_for_each(int (*fn)(struct clockdomain *clkdm, void *user),
		        void *user);
extern struct powerdomain *clkdm_get_pwrdm(struct clockdomain *clkdm);

/* FIXME: Remove once dependency testing is supported
extern int clkdm_add_wkdep(struct clockdomain *clkdm1, struct clockdomain *clkdm2);
extern int clkdm_del_wkdep(struct clockdomain *clkdm1, struct clockdomain *clkdm2);
extern int clkdm_read_wkdep(struct clockdomain *clkdm1, struct clockdomain *clkdm2);
extern int clkdm_clear_all_wkdeps(struct clockdomain *clkdm);
extern int clkdm_add_sleepdep(struct clockdomain *clkdm1, struct clockdomain *clkdm2);
extern int clkdm_del_sleepdep(struct clockdomain *clkdm1, struct clockdomain *clkdm2);
extern int clkdm_read_sleepdep(struct clockdomain *clkdm1, struct clockdomain *clkdm2);
extern int clkdm_clear_all_sleepdeps(struct clockdomain *clkdm);
 */

extern void omap2_clkdm_allow_idle(struct clockdomain *clkdm);
extern void omap2_clkdm_deny_idle(struct clockdomain *clkdm);

extern int omap2_clkdm_wakeup(struct clockdomain *clkdm);
extern int omap2_clkdm_sleep(struct clockdomain *clkdm);

extern int omap2_clkdm_clk_enable(struct clockdomain *clkdm, struct clk *clk);
extern int omap2_clkdm_clk_disable(struct clockdomain *clkdm, struct clk *clk);

void clock_domain_test(void);
static int clock_dm_test_1(void);
static int clock_dm_test_2(void);
static int clock_dm_test_3(void);

const char *clockdomains_name[] = {
"l4_cefuse_clkdm",
"l4_cfg_clkdm",
"tesla_clkdm",
"l3_gfx_clkdm",
"ivahd_clkdm",
"l4_secure_clkdm",
"l4_per_clkdm",
"abe_clkdm",
"l3_instr_clkdm",
"l3_init_clkdm",
"mpuss_clkdm",
"mpu0_clkdm",
"mpu1_clkdm",
"l3_emif_clkdm",
"l4_ao_clkdm",
"ducati_clkdm",
"l3_2_clkdm",
"l3_1_clkdm",
"l3_d2d_clkdm",
"iss_clkdm",
"l3_dss_clkdm",
"l4_wkup_clkdm",
"emu_sys_clkdm",
"l3_dma_clkdm",
NULL
};

static struct clockdomain *clkdm_omap;

static struct clk dummy_clk1 = {
	.name = "clk1",
};

static uint test_case;
module_param(test_case, int, S_IRUGO|S_IWUSR);

/*
 * clock_dm_test_1 - Test some basic clock domain api's
 *
 * This set tests the API's for the link between the clock domains and
 * the clocks by make use of a dummy clock structure and some other
 * preliminary api's.
 */
static int clock_dm_test_1()
{
	int val = -EINVAL, ret = 0;
	static struct clockdomain *cd;
	static struct powerdomain *pd;

	printk("\n\nEntered Clock Domain Test 1.....\n\n");

	val = omap2_clkdm_clk_enable(cd = clkdm_omap, &dummy_clk1);
	if (val == 0)
		printk(KERN_INFO "Enabled Clock for %s\n", cd->name);
	else if (val == -EINVAL)
		printk(KERN_ERR "Invalid Clock-domain or no control allowed\n");
	else {
		printk("Failed to enable clock for %s", cd->name);
		ret = -1;
	}

	val = omap2_clkdm_clk_disable(cd = clkdm_omap, &dummy_clk1);
	if (val == 0)
		printk(KERN_INFO "Disabled Clock for %s\n", cd->name);
	else if (val == -EINVAL)
		printk(KERN_ERR "Invalid Clock-domain or no control allowed\n");
	else {
		printk("Failed to enable clock for %s", cd->name);
		ret = -1;
	}

	pd = clkdm_get_pwrdm(cd = clkdm_omap);
	if (pd)
		printk(KERN_INFO "%s is PWRDM for %s\n", pd->name,
						 cd->name);
	else
		printk(KERN_ERR "Invalid Clock Domain\n");

	return ret;
}

/*
 * clock_dm_test_2 - Test the power transitions
 *
 * This set tests the API's for testing the power state transitions of the
 * clock domains and also validates the status after the transitions.
 */
static int clock_dm_test_2()
{
	int val = -EINVAL, ret = 0;
	static struct clockdomain *cd;

	printk("\n\nEntered Clock Domain Test 2.....\n\n");

	omap2_clkdm_allow_idle(clkdm_omap);

	omap2_clkdm_deny_idle(clkdm_omap);

	val = omap2_clkdm_wakeup(cd = clkdm_omap);
	if (val == 0)
		printk(KERN_INFO "Forced %s to Wake up\n", cd->name);
	else if (val == -EINVAL)
		printk(KERN_ERR "Wake up not supported for %s\n", cd->name);
	else {
		printk(KERN_ERR "Failed to wake %s up\n", cd->name);
		ret = -1;
	}

	val = omap2_clkdm_sleep(cd = clkdm_omap);
	if (val == 0)
		printk(KERN_INFO "Forced %s Sleep\n", cd->name);
	else if (val == -EINVAL)
		printk(KERN_ERR "Sleep not supported for %s\n", cd->name);
	else {
		printk(KERN_ERR "Failed to force %s to sleep\n", cd->name);
		ret = -1;
	}

	return ret;
}

/*
 * clock_dm_test_3 - Test the Clock Domain Dependencies
 *
 * This set tests the API's for validating the clock domain dependencies
 * and related functionalities.
 */
static int clock_dm_test_3()
{
	int ret = 0;

	/* FIXME : Upgrade once OMAP4 dependency support is added */
	/* Not supported as of now....!!!! */
	//printk("\n\nEntered Clock Domain Test 3.....\n\n");

	return ret;
}

void clock_domain_test()
{
	int i, ret = -1;

	printk("Entered the Module....\n\n");

	switch (test_case) {

	case 0x1:
		for (i = 0; clockdomains_name[i]; i++) {
			clkdm_omap = clkdm_lookup(clockdomains_name[i]);
			ret = clock_dm_test_1();
			if (ret) {
				printk("\n\nTest Failed\n\n");
				break;
			}
		}
		if (!ret)
			printk("\n\nTest Passed\n\n");
		break;
	case 0x2:
		for (i = 0; clockdomains_name[i] != NULL; i++) {
			clkdm_omap = clkdm_lookup(clockdomains_name[i]);
			ret = clock_dm_test_2();
			if (ret) {
				printk("\n\nTest Failed\n\n");
				break;
			}
		}
		if (!ret)
			printk("\n\nTest Passed\n\n");
		break;
	case 0x3:
		for (i = 0; clockdomains_name[i] != NULL; i++) {
			clkdm_omap = clkdm_lookup(clockdomains_name[i]);
			ret = clock_dm_test_3();
		}
		/* FIXME: Remove this print once test case 3 is valid */
		printk("Test Case 3 not supported currently...!!!\n");
		/* Uncomment when test case 3 is valid */
		/*
		if (!ret)
			printk("\n\nTest Passed\n\n");
		else
			printk("\n\nTest Failed\n\n");
		 */
		break;
	default:
		printk (" INVALID PARAMETERS- \n");
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

static int __init cd_test_init(void)
{
	clock_domain_test();
	printk("\nEnd of Test...\n\n");
	return 0;
}

static void __exit cd_test_exit(void)
{
	printk(" Exiting Module \n");
	printk(" Done \n");
}

module_init(cd_test_init);
module_exit(cd_test_exit);
