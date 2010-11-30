#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

static int __init debug_module_test_init(void)
{
	printk("debug_module_test_start\n");
	printk(KERN_EMERG "Printing: KERN_EMERG\n");
	printk(KERN_ALERT "Printing: KERN_ALERT\n");
	printk(KERN_CRIT "Printing: KERN_CRIT\n");
	printk(KERN_ERR "Printing: KERN_ERR\n");
	printk(KERN_WARNING "Printing: KERN_WARNING\n");
	printk(KERN_NOTICE "Printing: KERN_NOTICE\n");
	printk(KERN_INFO "Printing: KERN_INFO\n");
	printk(KERN_DEBUG "Printing: KERN_DEBUG\n");
	printk("debug_module_test_end\n");
	return 0;
}

static void __exit debug_module_test_exit(void)
{
        printk(KERN_INFO "Unloading debug_module_test.ko module\n");
}

module_init(debug_module_test_init);
module_exit(debug_module_test_exit);

MODULE_AUTHOR("Texas Instruments");
MODULE_DESCRIPTION("Debug Module Test");
MODULE_LICENSE("GPL");

