/*
 * linux/arch/arm/mach-omap2/pwr_dm_test.c
 * OMAP3 power domain test file
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
 * 2009-09-16: Abhijit Pagare       Initial code
 * 2009-09-17: Charulatha V         Adapted for OMAP3
 */

#include <linux/module.h>
#include <mach/clockdomain.h>
extern struct powerdomain *powerdomains_omap[];

void power_domain_test(void);

static struct clockdomain dummy_clkdm = {
	.name = "dummy_cd1",
};

/**
 * power_domain_test - Test the power domain APIs
 *
 * Test the power domain APIs for all power domains
 *
 */
void power_domain_test()
{
	int bank, i;
	int val = -EINVAL;
	static struct powerdomain *p, *pwrdm;

	for (i = 0; powerdomains_omap[i] != NULL; i++)	{
		p = powerdomains_omap[i];
		pwrdm = pwrdm_lookup(p->name);
		if (pwrdm)
			printk(KERN_INFO "PWR DM No%d = %s\n", i, pwrdm->name);
		else
			printk(KERN_INFO "PWR DM %s not supported\n", p->name);
	}

	/* i starts from 1 as gfx_pwrdm not supported in ES3.1.1 */
	for (i = 1; powerdomains_omap[i] != NULL; i++)	{
		val = pwrdm_add_clkdm(p = powerdomains_omap[i], &dummy_clkdm);
		if (val == 0)
			printk(KERN_INFO "Clock Domain Registered for %s\n",
							 p->name);
		else if (val == -EINVAL)
			printk(KERN_ERR "Clock Domain Register FAILED!!! for"
				" %s\n", p->name);
	}

	for (i = 1; powerdomains_omap[i] != NULL; i++)	{
		val = pwrdm_del_clkdm(p = powerdomains_omap[i], &dummy_clkdm);
		if (val == 0)
			printk(KERN_INFO "Clock Domain Unregistered for %s\n",
							 p->name);
		else if (val == -EINVAL)
			printk(KERN_ERR "Clock Domain Unregister FAILED!!! for"
				" %s\n", p->name);
	}

	for (i = 1; powerdomains_omap[i] != NULL; i++) {
		val = pwrdm_get_mem_bank_count(p = powerdomains_omap[i]);
		printk(KERN_INFO "Bnk Cnt for %s = %d\n", p->name, val);
	}

	for (i = 1; powerdomains_omap[i] != NULL; i++) {
		val = pwrdm_read_logic_pwrst(p = powerdomains_omap[i]);
		printk(KERN_INFO "PwrState of %s = %d\n", p->name, val);
	}

	for (i = 1; powerdomains_omap[i] != NULL; i++) {
		val = pwrdm_set_logic_retst(p = powerdomains_omap[i],
						 PWRDM_POWER_OFF);
		if (val == 0)
			printk(KERN_INFO "Logic RET State OFF for %s Set\n",
							 p->name);
		else if (val == -EINVAL)
			printk(KERN_INFO "OFF State not supported for %s\n",
							 p->name);
		else
			printk(KERN_ERR "Set Logic RET State OFF FAILED!!!"
				" with value %d\n", val);
	}

	for (i = 1; powerdomains_omap[i] != NULL; i++) {
		val = pwrdm_set_logic_retst(p = powerdomains_omap[i],
						 PWRDM_POWER_RET);
		if (val == 0)
			printk(KERN_INFO "Logic RET State RET for %s Set\n",
							 p->name);
		else if (val == -EINVAL)
			printk(KERN_INFO "RET State not supported for %s\n",
							 p->name);
		else
			printk(KERN_ERR "Logic RET State RET FAILED!!!"
				" with value %d\n", val);
	}

	for (i = 1; powerdomains_omap[i] != NULL; i++) {
		val = pwrdm_read_pwrst(p = powerdomains_omap[i]);
		printk(KERN_INFO "PwrState of %s = %d\n", p->name, val);
	}

	for (i = 1; powerdomains_omap[i] != NULL; i++) {
		val = pwrdm_set_next_pwrst(p = powerdomains_omap[i],
						 PWRDM_POWER_OFF);
		if (val == 0)
			printk(KERN_INFO "Next PWRST for %s Set to OFF\n",
							 p->name);
		else if (val == -EINVAL)
			printk(KERN_INFO "OFF not supported for %s\n",
							 p->name);
		else
			printk(KERN_ERR "Next PWRST Set to OFF FAILED!!!"
				" with value %d\n", val);
	}

	for (i = 1; powerdomains_omap[i] != NULL; i++) {
		val = pwrdm_set_next_pwrst(p = powerdomains_omap[i],
						 PWRDM_POWER_RET);
		if (val == 0)
			printk(KERN_INFO "Next PWRST for %s Set to RET\n",
							 p->name);
		else if (val == -EINVAL)
			printk(KERN_INFO "RET not supported for %s\n",
							 p->name);
		else
			printk(KERN_ERR "Next PWRST Set to RET FAILED!!!"
				" with value %d\n", val);
	}

	for (i = 1; powerdomains_omap[i] != NULL; i++) {
		val = pwrdm_set_next_pwrst(p = powerdomains_omap[i],
						 PWRDM_POWER_ON);
		if (val == 0)
			printk(KERN_INFO "Next PWRST for %s Set to ON\n",
							 p->name);
		else if (val == -EINVAL)
			printk(KERN_INFO "ON not supported for %s\n",
							 p->name);
		else
			printk(KERN_ERR "Next PWRST Set to ON FAILED!!!"
				" with value %d\n", val);
	}

	for (i = 1; powerdomains_omap[i] != NULL; i++) {
		val = pwrdm_read_next_pwrst(p = powerdomains_omap[i]);
		printk(KERN_INFO "Next Power State of %s = %d\n",
							 p->name, val);
	}

	for (i = 1; powerdomains_omap[i] != NULL; i++) {
		val = pwrdm_read_pwrst(p = powerdomains_omap[i]);
		printk(KERN_INFO "Current Power State of %s = %d\n",
							 p->name, val);
	}

	for (i = 1; powerdomains_omap[i] != NULL; i++) {
		for (bank = 0; bank < PWRDM_MAX_MEM_BANKS; bank++) {
			val = pwrdm_set_mem_onst(p = powerdomains_omap[i],
						 bank, PWRDM_POWER_OFF);
			if (val == 0)
				printk(KERN_INFO "Memory ON State OFF for %s"
							" Set\n", p->name);
			else if (val == -EINVAL)
				printk(KERN_INFO "OFF State not supported"
						" for %s\n", p->name);
			else if (val == -EEXIST)
				printk(KERN_ERR "Memory Bank %d not present"
						" for %s\n", bank, p->name);
			else
				printk(KERN_ERR "Memory ON State OFF FAILED!!!"
					" with value %d\n", val);
		}
	}

	for (i = 1; powerdomains_omap[i] != NULL; i++) {
		for (bank = 0; bank < PWRDM_MAX_MEM_BANKS; bank++) {
			val = pwrdm_set_mem_onst(p = powerdomains_omap[i],
						 bank, PWRDM_POWER_RET);
			if (val == 0)
				printk(KERN_INFO "Memory ON State RET for %s"
							" Set\n", p->name);
			else if (val == -EINVAL)
				printk(KERN_INFO "RET State not supported"
						" for %s\n", p->name);
			else if (val == -EEXIST)
				printk(KERN_ERR "Memory Bank %d not present"
						" for %s\n", bank, p->name);
			else
				printk(KERN_ERR "Memory ON State RET FAILED!!!"
					" with value %d\n", val);
		}
	}

	for (i = 1; powerdomains_omap[i] != NULL; i++) {
		for (bank = 0; bank < PWRDM_MAX_MEM_BANKS; bank++) {
			val = pwrdm_set_mem_retst(p = powerdomains_omap[i],
						bank, PWRDM_POWER_OFF);
			if (val == 0)
				printk(KERN_INFO "Memory RET State OFF for"
							" %s Set\n", p->name);
			else if (val == -EINVAL)
				printk(KERN_INFO "OFF State not supported for"
							" %s\n", p->name);
			else if (val == -EEXIST)
				printk(KERN_ERR "Memory Bank %d not present"
						" for %s\n", bank, p->name);
			else
				printk(KERN_ERR "Memory ON State OFF FAILED!!!"
					" with value %d\n", val);
		}
	}

	for (i = 1; powerdomains_omap[i] != NULL; i++) {
		for (bank = 0; bank < PWRDM_MAX_MEM_BANKS; bank++) {
			val = pwrdm_set_mem_retst(p = powerdomains_omap[i],
						bank, PWRDM_POWER_RET);
			if (val == 0)
				printk(KERN_INFO "Memory RET State RET for"
							" %s Set\n", p->name);
			else if (val == -EINVAL)
				printk(KERN_INFO "RET State not supported for"
							" %s\n", p->name);
			else if (val == -EEXIST)
				printk(KERN_ERR "Memory Bank %d not present"
						" for %s\n", bank, p->name);
			else
				printk(KERN_ERR "MEM PWRST Set FAILED!!!"
						" with value %d\n", val);
		}
	}

	for (i = 1; powerdomains_omap[i] != NULL; i++) {
		for (bank = 0; bank < PWRDM_MAX_MEM_BANKS; bank++) {
			val = pwrdm_read_mem_pwrst(p = powerdomains_omap[i],
									bank);
			if (val == -EEXIST)
				printk(KERN_ERR "Memory Bank %d not present"
						" for %s\n", bank, p->name);
			else if (val == -EINVAL)
				printk(KERN_ERR "MEM PWRST Read FAILED!!!"
						" with value %d\n", val);
			else
				printk(KERN_INFO "MEM PWRST for bank %d of"
					" %s = %d\n", bank, p->name, val);
		}
	}
}
EXPORT_SYMBOL(power_domain_test);
