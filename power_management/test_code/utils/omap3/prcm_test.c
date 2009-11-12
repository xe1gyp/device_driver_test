#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/irq.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <mach-omap2/prm.h>
#include <mach-omap2/prcm-common.h>
#include <linux/clk.h>
#include <linux/types.h>
#include <linux/io.h>

#define MAX_MODULES 18

static int __init prcm_test_init(void)
{
	u32 prcm_test = 0;
	s16 module_array[] = {OMAP3430_CCR_MOD, OMAP3430ES2_SGX_MOD,
		OMAP3430_DSS_MOD, OMAP3430_CAM_MOD, OMAP3430_PER_MOD,
		OMAP3430_NEON_MOD, OMAP3430ES2_USBHOST_MOD, CORE_MOD,
		OMAP3430_IVA2_MOD, CORE_MOD, CORE_MOD, WKUP_MOD, WKUP_MOD,
		WKUP_MOD, WKUP_MOD, OMAP3430_PER_MOD, OMAP3430_PER_MOD,
		WKUP_MOD};

	u16 idx_array[] = {OMAP3_PRM_CLKOUT_CTRL_OFFSET, PM_WKDEP,
		PM_WKDEP, PM_WKDEP, PM_WKDEP, PM_WKDEP, PM_WKDEP,
		OMAP3430_PM_MPUGRPSEL1, OMAP3430_PM_IVAGRPSEL1,
		OMAP3430ES2_PM_MPUGRPSEL3, OMAP3430ES2_PM_IVAGRPSEL3,
		OMAP3430_PM_MPUGRPSEL, OMAP3430_PM_IVAGRPSEL,
		OMAP3430_PM_MPUGRPSEL, OMAP3430_PM_IVAGRPSEL,
		OMAP3430_PM_MPUGRPSEL, OMAP3430_PM_IVAGRPSEL, PM_WKEN};

	char *name_array[] = {
		"[OMAP3430_CCR_MOD, OMAP3_PRM_CLKOUT_CTRL_OFFSE[T",
		"[OMAP3430ES2_SGX_MOD, PM_WKDEP]",
		"[OMAP3430_DSS_MOD, PM_WKDEP]",
		"[OMAP3430_CAM_MOD, PM_WKDEP]",
		"[OMAP3430_PER_MOD, PM_WKDEP]",
		"[OMAP3430_NEON_MOD, PM_WKDEP]",
		"[OMAP3430ES2_USBHOST_MOD, PM_WKDEP]",
		"[CORE_MOD, OMAP3430_PM_MPUGRPSEL1]",
		"[OMAP3430_IVA2_MOD, OMAP3430_PM_IVAGRPSEL1]",
		"[CORE_MOD, OMAP3430ES2_PM_MPUGRPSEL3]",
		"[CORE_MOD, OMAP3430ES2_PM_MPUGRPSEL3]",
		"[WKUP_MOD, OMAP3430_PM_MPUGRPSEL]",
		"[WKUP_MOD, OMAP3430_PM_IVAGRPSEL]",
		"[WKUP_MOD, OMAP3430_PM_MPUGRPSEL]",
		"[WKUP_MOD, OMAP3430_PM_IVAGRPSEL]",
		"[OMAP3430_PER_MOD, OMAP3430_PM_MPUGRPSEL]",
		"[OMAP3430_PER_MOD, OMAP3430_PM_IVAGRPSEL]",
		"[WKUP_MOD, PM_WKEN]"};

	int i;


	for (i = 0; i < MAX_MODULES; ++i) {
		prcm_test = prm_read_mod_reg(module_array[i], idx_array[i]);
		printk(KERN_INFO "%s = %d\n", name_array[i], prcm_test);
	}


	return 0;
}

static void __exit prcm_test_exit(void)
{
}

module_init(prcm_test_init);
module_exit(prcm_test_exit);
