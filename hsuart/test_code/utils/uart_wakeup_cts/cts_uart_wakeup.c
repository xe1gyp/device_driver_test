#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <mach/omap-serial.h>

/*port*/
static uint port;

module_param(port, int, S_IRUGO|S_IWUSR);

/*status*/
static uint status;

module_param(status, int, S_IRUGO|S_IWUSR);

static int __init cts_wakeup_init(void)
{
	int ret;

	ret = omap_uart_cts_wakeup(port, status);

	if (ret != 0)
		printk(KERN_INFO "CTS_UART Test initialization failed.\n");
	else 
		 printk(KERN_INFO "CTS_UART Test initialized success.\n");

	return ret;
}

static void __exit cts_wakeup_exit(void)
{
	printk(KERN_INFO "CTS_UART Test finalized.\n");
}

module_init(cts_wakeup_init);
module_exit(cts_wakeup_exit);

