#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/irq.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/version.h>
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 27) && LINUX_VERSION_CODE <= KERNEL_VERSION(2, 6, 31))
 #include <mach/dmtimer.h>
#elif LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 32)
 #include <plat/dmtimer.h>
#else
 #include <asm/arch/dmtimer.h>
#endif
#include <linux/clk.h>
#include <linux/types.h>
#include <linux/io.h>

#define MAX_TIMERS 12

MODULE_DESCRIPTION("GP Timer Request and Reserve Test Module");
MODULE_LICENSE("GPL");

/*Pointer to timer struct*/
static struct omap_dm_timer *timer_ptr;

/*The IRQ # for the current gp timer*/
static int32_t timer_irq;

/*Clock type to use*/
static uint gptimer_id;

module_param(gptimer_id, int, S_IRUGO|S_IWUSR);

static int __init gptimer_request_init(void)
{
	struct clk *gt_fclk;
	uint32_t gt_rate;

	if (gptimer_id <= 0 || gptimer_id > MAX_TIMERS) {
		printk(KERN_ERR "Invalid gptimer_id value, ");
		printk(KERN_ERR "it should be between 1-12\n");
		return -1;

	}
	/*Requesting for any available timer*/
	timer_ptr = omap_dm_timer_request_specific(gptimer_id);

	if (timer_ptr == NULL) {
		/*Timers are not available*/
		printk(KERN_ERR "GPtimers are not available\n");
		return -1;
	}
	 /*Set the clock source*/
#ifdef CONFIG_ARCH_OMAP4
	if (gptimer_id > 4 && gptimer_id < 9)
		omap_dm_timer_set_source(timer_ptr, 2);
	else
#endif
		omap_dm_timer_set_source(timer_ptr, OMAP_TIMER_SRC_SYS_CLK);

	/*Figure out what IRQ our timer triggers*/
	timer_irq = omap_dm_timer_get_irq(timer_ptr);

	/*Get clock rate in Hz*/
	gt_fclk = omap_dm_timer_get_fclk(timer_ptr);
	gt_rate = clk_get_rate(gt_fclk);

	/*Start the timer!*/
	omap_dm_timer_start(timer_ptr);

	printk(KERN_INFO "GP Timer initialized and started (%lu Hz, IRQ %d)\n",
		(long unsigned)gt_rate, timer_irq);
	return 0;
}

static void __exit gptimer_request_exit(void)
{
	/*Stop the timer*/
	omap_dm_timer_stop(timer_ptr);

	/*Release the timer*/
	omap_dm_timer_free(timer_ptr);

	printk(KERN_INFO "Gptimer has been finalized\n");
}

module_init(gptimer_request_init);
module_exit(gptimer_request_exit);
