#include <linux/init.h>
#include <linux/module.h>
MODULE_LICENSE("GPL");

int save_pri[96];

static int pri_init(void)
{
	int i, j;

	printk(KERN_ALERT "Starting priority test\n");

	for (i=0, j=0; i<96; i++, j++) {
		save_pri[i] = omap_get_hw_irq_proirity(i);
		if(j > (63 + 4))
			j = 0;
		omap_set_hw_irq_proirity(i, j);
	}
	printk(KERN_ALERT "cat /proc/irqomap24xx and check settings \n");
	printk(KERN_ALERT "unload pri_tst1 module to clean up \n");

	return 0;
}

static void pri_exit(void)
{
	int i;

	printk(KERN_ALERT "Cleaning up pri_tst1\n");

	for (i=0; i<96; i++)
		omap_set_hw_irq_proirity(i, save_pri[i]);
}
module_init(pri_init);
module_exit(pri_exit);
