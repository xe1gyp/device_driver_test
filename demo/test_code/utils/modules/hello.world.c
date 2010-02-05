#include <linux/types.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/sched.h>
#include <linux/spinlock.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/version.h>
#include <linux/uaccess.h>

static int __init hello_world_module_init(void)
{
	printk(KERN_INFO "Hello World!\n");
	return 0;
}

static void __exit hello_world_module_exit(void)
{
	printk(KERN_INFO "Bye Hello World!\n");
}

module_init(hello_world_module_init);
module_exit(hello_world_module_exit);

MODULE_AUTHOR("Texas Instruments");
MODULE_DESCRIPTION("Hello World Module Driver");
MODULE_LICENSE("GPL");
