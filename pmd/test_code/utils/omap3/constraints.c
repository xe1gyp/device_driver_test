#include <linux/init.h>
#include <linux/module.h>
#include <linux/debugfs.h>

#include <plat/omap-pm.h>
#include <plat/resource.h>
#define OCP_INIT_ID	2

static struct device *constraints_dummy_dev;
static struct dentry *constraints_debugfs_root;

/*
 * constraints from arch/arm/plat-omap/omap-pm-srf.c:
 *
 * omap_pm_dsp_set_min_opp(u8 opp_id)
 * omap_pm_set_max_sdma_lat(struct device *dev, long t)
 * omap_pm_set_max_dev_wakeup_lat(struct device *dev, long t)
 * omap_pm_set_min_bus_tput(struct device *dev, u8 agent_id, unsigned long r)
 * omap_pm_set_max_mpu_wakeup_lat(struct device *dev, long t)
 */

static int constraints_debug_set(void *data, u64 constraint)
{
	char * sr_name;

	sr_name = (char *) data;

	if (!sr_name || IS_ERR(sr_name)) {
		pr_err("constraints: %s: sr_name is invalid\n", __func__);
		return -EINVAL;
	}

	if (!constraint) {
		resource_release(sr_name, constraints_dummy_dev);
		return 0;
	}

	if (!strcmp(sr_name, "mpu_freq")) {
		omap_pm_set_min_mpu_freq(constraints_dummy_dev, constraint);
	} else if (!strcmp(sr_name, "vdd2_opp")) {
		omap_pm_set_min_bus_tput(constraints_dummy_dev, OCP_INIT_ID,
				constraint);
	} else {
		pr_err("constraints: %s: error!  should not be here\n",
				__func__);
	}

	return 0;
}

DEFINE_SIMPLE_ATTRIBUTE(constraints_debug_fops, NULL,
		constraints_debug_set, "%llu\n");

static int __init constraints_init(void)
{
	struct dentry *d;

	pr_info("constraints: loaded\n");
	pr_info("constraints: tests can be found at debugfs/constraints\n");

	constraints_dummy_dev = kmalloc(sizeof(struct device *), GFP_KERNEL);

	d = debugfs_create_dir("constraints", NULL);
	if (IS_ERR(d))
		return -ENOMEM;

	constraints_debugfs_root = d;

	(void) debugfs_create_file("min_mpu_freq", S_IWUGO,
			constraints_debugfs_root, "mpu_freq",
			&constraints_debug_fops);

	(void) debugfs_create_file("min_bus_tput", S_IWUGO,
			constraints_debugfs_root, "vdd2_opp",
			&constraints_debug_fops);

	return 0;
}

static void __exit constraints_exit(void)
{
	pr_info("constraints: unloaded\n");

	resource_release("mpu_freq", constraints_dummy_dev);
	resource_release("vdd2_opp", constraints_dummy_dev);
	kfree(constraints_dummy_dev);

	debugfs_remove_recursive(constraints_debugfs_root);
}

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Mike Turquette <mturquette@ti.com>");

module_init(constraints_init);
module_exit(constraints_exit);
