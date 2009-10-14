/*
 * SRF/clock_test.c
 * OMAP3 Clock framework test
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
#include <mach/clock.h>
#include <linux/clk.h>

MODULE_LICENSE("GPL");

static const char *clk_names[][2] = {
	{ NULL,	"omap_32k_fck"	},
	{ NULL,	"virt_12m_ck"	},
	{ NULL,	"virt_13m_ck"	},
	{ NULL,	"virt_16_8m_ck"	},
	{ NULL,	"virt_19_2m_ck"	},
	{ NULL,	"virt_26m_ck"	},
	{ NULL,	"virt_38_4m_ck"	},
	{ NULL,	"osc_sys_ck"	},
	{ NULL,	"sys_ck"	},
	{ NULL,	"sys_altclk"	},
	{ NULL,	"mcbsp_clks"	},
	{ NULL,	"sys_clkout1"	},
	{ NULL,	"dpll1_x2_ck"	},
	{ NULL,	"dpll1_x2m2_ck"	},
	{ NULL,	"dpll2_ck"	},
	{ NULL,	"dpll2_m2_ck"	},
	{ NULL,	"dpll3_ck"	},
	{ NULL,	"core_ck"	},
	{ NULL,	"dpll3_x2_ck"	},
	{ NULL,	"dpll3_m2_ck"	},
	{ NULL,	"dpll3_m2x2_ck"	},
	{ NULL,	"dpll3_m3_ck"	},
	{ NULL,	"dpll3_m3x2_ck"	},
	{ NULL,	"emu_core_alwon_ck"	},
	{ NULL,	"omap_96m_alwon_fck"	},
	{ NULL,	"omap_96m_fck"	},
	{ NULL,	"cm_96m_fck"	},
	{ NULL,	"omap_54m_fck"	},
	{ NULL,	"omap_48m_fck"	},
	{ NULL,	"omap_12m_fck"	},
	{ NULL,	"clkout2_src_ck"	},
	{ NULL,	"sys_clkout2"	},
	{ NULL,	"corex2_fck"	},
	{ NULL,	"dpll1_fck"	},
	{ NULL,	"mpu_ck"	},
	{ NULL,	"arm_fck"	},
	{ NULL,	"emu_mpu_alwon_ck"	},
	{ NULL,	"dpll2_fck"	},
	{ NULL,	"iva2_ck"	},
	{ NULL,	"l3_ick"	},
	{ NULL,	"l4_ick"	},
	{ NULL,	"rm_ick"	},
	{ NULL,	"sgx_fck"	},
	{ NULL,	"sgx_ick"	},
	{ NULL,	"modem_fck"	},
	{ NULL,	"sad2d_ick"	},
	{ NULL,	"mad2d_ick"	},
	{ NULL,	"gpt10_fck"	},
	{ NULL,	"gpt11_fck"	},
	{ NULL,	"cpefuse_fck"	},
	{ NULL,	"ts_fck"	},
	{ NULL,	"usbtll_fck"	},
	{ NULL,	"core_96m_fck"	},
	{ NULL,	"mspro_fck"	},
	{ NULL,	"core_48m_fck"	},
	{ NULL,	"uart2_fck"	},
	{ NULL,	"uart1_fck"	},
	{ NULL,	"core_12m_fck"	},
	{ NULL,	"ssi_ssr_fck"	},
	{ NULL,	"ssi_sst_fck"	},
	{ NULL,	"core_l3_ick"	},
	{ NULL,	"sdrc_ick"	},
	{ NULL,	"gpmc_fck"	},
	{ NULL,	"security_l3_ick"	},
	{ NULL,	"pka_ick"	},
	{ NULL,	"core_l4_ick"	},
	{ NULL,	"usbtll_ick"	},
	{ NULL,	"icr_ick"	},
	{ NULL,	"aes2_ick"	},
	{ NULL,	"sha12_ick"	},
	{ NULL,	"des2_ick"	},
	{ NULL,	"mspro_ick"	},
	{ NULL,	"uart2_ick"	},
	{ NULL,	"uart1_ick"	},
	{ NULL,	"gpt11_ick"	},
	{ NULL,	"gpt10_ick"	},
	{ NULL,	"mailboxes_ick"	},
	{ NULL,	"omapctrl_ick"	},
	{ NULL,	"ssi_l4_ick"	},
	{ NULL,	"ssi_ick"	},
	{ NULL,	"security_l4_ick2"	},
	{ NULL,	"aes1_ick"	},
	{ NULL,	"sha11_ick"	},
	{ NULL,	"des1_ick"	},
	{ NULL,	"cam_mclk"	},
	{ NULL,	"cam_ick"	},
	{ NULL,	"csi2_96m_fck"	},
	{ NULL,	"usbhost_120m_fck"	},
	{ NULL,	"usbhost_48m_fck"	},
	{ NULL,	"usbhost_ick"	},
	{ NULL,	"usim_fck"	},
	{ NULL,	"gpt1_fck"	},
	{ NULL,	"wkup_32k_fck"	},
	{ NULL,	"gpio1_dbck"	},
	{ NULL,	"wkup_l4_ick"	},
	{ NULL,	"usim_ick"	},
	{ NULL,	"wdt1_ick"	},
	{ NULL,	"gpio1_ick"	},
	{ NULL,	"omap_32ksync_ick"	},
	{ NULL,	"gpt12_ick"	},
	{ NULL,	"gpt1_ick"	},
	{ NULL,	"per_96m_fck"	},
	{ NULL,	"per_48m_fck"	},
	{ NULL,	"uart3_fck"	},
	{ NULL,	"gpt2_fck"	},
	{ NULL,	"gpt3_fck"	},
	{ NULL,	"gpt4_fck"	},
	{ NULL,	"gpt5_fck"	},
	{ NULL,	"gpt6_fck"	},
	{ NULL,	"gpt7_fck"	},
	{ NULL,	"gpt8_fck"	},
	{ NULL,	"gpt9_fck"	},
	{ NULL,	"per_32k_alwon_fck"	},
	{ NULL,	"gpio6_dbck"	},
	{ NULL,	"gpio5_dbck"	},
	{ NULL,	"gpio4_dbck"	},
	{ NULL,	"gpio3_dbck"	},
	{ NULL,	"gpio2_dbck"	},
	{ NULL,	"wdt3_fck"	},
	{ NULL,	"per_l4_ick"	},
	{ NULL,	"gpio6_ick"	},
	{ NULL,	"gpio5_ick"	},
	{ NULL,	"gpio4_ick"	},
	{ NULL,	"gpio3_ick"	},
	{ NULL,	"gpio2_ick"	},
	{ NULL,	"wdt3_ick"	},
	{ NULL,	"uart3_ick"	},
	{ NULL,	"gpt9_ick"	},
	{ NULL,	"gpt8_ick"	},
	{ NULL,	"gpt7_ick"	},
	{ NULL,	"gpt6_ick"	},
	{ NULL,	"gpt5_ick"	},
	{ NULL,	"gpt4_ick"	},
	{ NULL,	"gpt3_ick"	},
	{ NULL,	"gpt2_ick"	},
	{ NULL,	"emu_src_ck"	},
	{ NULL,	"pclk_fck"	},
	{ NULL,	"pclkx2_fck"	},
	{ NULL,	"atclk_fck"	},
	{ NULL,	"traceclk_src_fck"	},
	{ NULL,	"traceclk_fck"	},
	{ NULL,	"sr1_fck"	},
	{ NULL,	"sr2_fck"	},
	{ NULL,	"sr_l4_ick"	},
	{ NULL,	"secure_32k_fck"	},
	{ NULL,	"gpt12_fck"	},
	{ NULL,	"wdt1_fck"	},
#if 0 //This Affects UART Settings
      //But FB Console will be working
	{ NULL,	"dpll4_ck"	},
	{ NULL,	"dpll4_x2_ck"	},
	{ NULL,	"dpll4_m2_ck"	},
	{ NULL,	"dpll4_m2x2_ck"	},
	{ NULL,	"dpll4_m3_ck"	},
	{ NULL,	"dpll4_m3x2_ck"	},
	{ NULL,	"dpll4_m4_ck"	},
	{ NULL,	"dpll4_m4x2_ck"	},
	{ NULL,	"dpll4_m5_ck"	},
	{ NULL,	"dpll4_m5x2_ck"	},
	{ NULL,	"dpll4_m6_ck"	},
	{ NULL,	"dpll4_m6x2_ck"	},
#endif
	{ NULL,	"emu_per_alwon_ck"	},
	{ NULL,	"dpll5_ck"	},
	{ NULL,	"dpll5_m2_ck"	},
};

static int clock_test_init(void)
{
        u32 ret=0, usecount;
	int i;
	

        unsigned long rate, valid_rate;
        struct clk *tclk, *temp_clock, *mpu_ck, *iva2_ck, *l3_ck;
	struct device *dev = NULL;
	
	printk(KERN_ALERT "Starting clock test\n");

	for (i = 0; i < ARRAY_SIZE(clk_names); i++) {
		printk(KERN_INFO "\nclock: %s\n",clk_names[i][1]);
		tclk = clk_get(clk_names[i][0], clk_names[i][1]);
		if(tclk == NULL) {
			printk(KERN_ERR "clk_get returned null value for clk %s\n",clk_names[i][1]);
			ret = 1;
			continue;
		}

		printk("\nClk name: %s\n", tclk->name);

                ret = clk_enable(tclk);
                if (ret != 0)
                        printk(KERN_ERR
                               "Error: clk_enable for clock %s returned %d\n",
                               tclk->name, ret);

                if (ret == 0) {
                        clk_disable(tclk);
                }

                rate = clk_get_rate(tclk);
                printk(KERN_INFO "Rate of clk %s is %lu\n", tclk->name,
                       tclk->rate);

                printk(KERN_INFO "Flags %u\n", tclk->flags);

                if ((tclk->dpll_data)) {
                        printk("Testing round rate:\n");
                        valid_rate = clk_round_rate(tclk, 1000000000);
                        /*All clocks should return highest possible values
                         *depending on dividers that can be changed */
                        printk("Valid rate when input is 1G: %lu\n",
                               valid_rate);
                        ret = clk_set_rate(tclk, valid_rate);
                        printk("Set rate returned : %d for valid rate : %lu\n", ret, valid_rate);
                        rate = clk_get_rate(tclk);
                        printk(KERN_INFO "Rate of clk %s is %lu\n", tclk->name,
                               tclk->rate);

                        /* Check closest possible rate to 600 Mhz */
                        valid_rate = clk_round_rate(tclk, 600000000);
                        printk("Valid rate when input is 600M: %lu\n",
                               valid_rate);
                        ret = clk_set_rate(tclk, valid_rate);
                        printk("Setrate returned:%d for valid rate:%lu\n", ret, valid_rate);
                        rate = clk_get_rate(tclk);
                        printk(KERN_INFO "Rate of clk %s is %lu\n", tclk->name,
                               tclk->rate);
                        /* Check closest possible rate to 500 Mhz */
                        valid_rate = clk_round_rate(tclk, 500000000);
                        printk("Valid rate when input is 500M: %lu\n",
                               valid_rate);
                        ret = clk_set_rate(tclk, valid_rate);
                        printk("Set rate returned : %d for valid rate : %lu\n", ret, valid_rate);
                        rate = clk_get_rate(tclk);
                        printk(KERN_INFO "Rate of clk %s is %lu\n", tclk->name,
                               tclk->rate);
                        /* Check closest possible rate to 400 Mhz */
                        valid_rate = clk_round_rate(tclk, 400000000);
                        printk("Valid rate when input is 400M: %lu\n",
                               valid_rate);
                        ret = clk_set_rate(tclk, valid_rate);
                        printk("Set rate returned : %d for valid rate : %lu\n", ret, valid_rate);
                        rate = clk_get_rate(tclk);
                        printk(KERN_INFO "Rate of clk %s is %lu\n", tclk->name,

                               tclk->rate);

                        /* Check closest possible rate to 300 Mhz */
                        valid_rate = clk_round_rate(tclk, 300000000);
                        printk("Valid rate when input is 300M: %lu\n",
                               valid_rate);
                                ret = clk_set_rate(tclk, valid_rate);
                                printk("Set rate returned : %d for valid rate : %lu\n", ret, valid_rate);
                        rate = clk_get_rate(tclk);
                        printk(KERN_INFO "Rate of clk %s is %lu\n", tclk->name,
                               tclk->rate);
                        /* Check closest possible rate to 200 Mhz */
                        valid_rate = clk_round_rate(tclk, 200000000);
                        printk("Valid rate when input is 200M: %lu\n",
                               valid_rate);
                        ret = clk_set_rate(tclk, valid_rate);
                        printk("Set rate returned : %d for valid rate : %lu\n", ret, valid_rate);
                        rate = clk_get_rate(tclk);
                        printk(KERN_INFO "Rate of clk %s is %lu\n", tclk->name,
                               tclk->rate);

                        /* Check closest possible rate to 100 Mhz */
                        valid_rate = clk_round_rate(tclk, 100000000);
                        printk("Valid rate when input is 100M: %lu\n",
                               valid_rate);
                        ret = clk_set_rate(tclk, valid_rate);
                        printk("Set rate returned : %d for valid rate : %lu\n", ret, valid_rate);
                        rate = clk_get_rate(tclk);
                        printk(KERN_INFO "Rate of clk %s is %lu\n", tclk->name,
                               tclk->rate);
                }
                clk_put(tclk);
                tclk = NULL;
        }
	if (ret != 0)
		printk("Test failed\n");
	else
		printk("Test passed\n");

        printk(KERN_INFO "End of clock test\n");
        printk(KERN_INFO "Since system settings are changed, there could be issues\n");
        printk(KERN_INFO "The system needs to be rebooted now\n");

	return 0;

}

static void clock_test_exit(void)
{
	printk("Exiting\n");
	printk("Done\n");

}

module_init(clock_test_init);
module_exit(clock_test_exit);
