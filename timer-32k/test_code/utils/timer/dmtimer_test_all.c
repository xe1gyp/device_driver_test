#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/irq.h>
#include <linux/clockchips.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/version.h>
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 27) && LINUX_VERSION_CODE <= KERNEL_VERSION(2, 6, 31))
 #include <mach/dmtimer.h>
#elif LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 32)
 #include <plat/dmtimer.h>
 #include <plat/common.h>
#else
 #include <asm/arch/dmtimer.h>
 #include <plat/common.h>
#endif
#include <linux/clk.h>
#include <linux/types.h>
#include <linux/io.h>

MODULE_DESCRIPTION("GP Timer Test Module");
MODULE_LICENSE("GPL");

/* pointer to current timer */
static struct omap_dm_timer *timer;

/* completion flag */
static int done = 0;

/* count value */
static u32 local_jiffies;

/* independent elapsed time computation using 32KHz Sync timer*/
static u32 start_count, end_count;

/* user parameter list */
static uint id; /* gptimer number: [1-12] */
static uint clk = 1; /* gptimer clock source: [1(sysclk), 2(32khz)] */
static uint to = 2; /* timeout in seconds */

/*
 * Usage format:
 * #insmod dmtimer_test.ko <gptimer number> <clock source> <timeout in seconds>
 *
 * Where:
 * <gptimer number> 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12
 * <clock source> 1:system clock, 2:32KHz clock, 3:Alternate clock
 * <timeout> in terms of number of seconds
 *
 * Example: run the test for gptimer#3 with 32KHz as functional clock for 10 seconds.
 * #insmod dmtimer_test.ko 3 2 10
 */
static int dmtimer_test_set_next_event(unsigned long cycles,
                                         struct clock_event_device *evt)
{
        omap_dm_timer_set_load_start(timer, 0, 0xffffffff - cycles);
        return 0;
}

static void dmtimer_test_set_mode(enum clock_event_mode mode,
                                    struct clock_event_device *evt)
{
        u32 period;

        omap_dm_timer_stop(timer);
        switch (mode) {
        case CLOCK_EVT_MODE_PERIODIC:
                period = clk_get_rate(omap_dm_timer_get_fclk(timer)) / HZ;
                period -= 1;
                omap_dm_timer_set_load_start(timer, 1, 0xffffffff - period);
                break;
        case CLOCK_EVT_MODE_ONESHOT:
                break;
        case CLOCK_EVT_MODE_UNUSED:
        case CLOCK_EVT_MODE_SHUTDOWN:
        case CLOCK_EVT_MODE_RESUME:
                break;
        }
}

static struct clock_event_device dmtimer_clockevent = {
        .name           = "dm-timer-test",
        .features       = CLOCK_EVT_FEAT_PERIODIC | CLOCK_EVT_FEAT_ONESHOT,
        .shift          = 32,
        .set_next_event = dmtimer_test_set_next_event,
        .set_mode       = dmtimer_test_set_mode,
};


static irqreturn_t dmtimer_test_isr(int irq, void *dev_id)
{
	struct omap_dm_timer *t = (struct omap_dm_timer *)dev_id;
	//struct clock_event_device *evt = &dmtimer_clockevent;

	/* acknowledge overflow interrupt */
	omap_dm_timer_write_status(t, OMAP_TIMER_INT_OVERFLOW);

	//evt->event_handler(evt);
	local_jiffies += 1;
	if (local_jiffies >= (to * 100))
		done = 1;

	return IRQ_HANDLED;
}

static struct irqaction dmtimer_irq = {
	.name		= "dm timer",
	.flags		= IRQF_DISABLED | IRQF_TIMER | IRQF_IRQPOLL,
	.handler	= dmtimer_test_isr,
};

static int __init dmtimer_test_init(void)
{
	uint32_t rate, period;
	int ret = 0;
	static int flag = 0;

	while (clk < 3) {
		for (id = 3; id < 12; id++) {
			/* requesting for the specified timer */
			timer = omap_dm_timer_request_specific(id);
			if (timer == NULL) {
				printk(KERN_ERR "ERROR: gptimer acquire FAILED\n");
				return -1;
			}
			printk("[dmtimer-test]dmtimer=%d ",id);
			if (clk == 1) {
				if (cpu_is_omap44xx()) {
					if (id > 4 && id < 9)
						clk += 2;
				}
			}
			/* set the clock source */
			switch(clk) {
			case 1:
				printk("clk=sysclk ");
				rate = clk_get_rate(omap_dm_timer_get_fclk(timer));
				omap_dm_timer_set_source(timer, OMAP_TIMER_SRC_SYS_CLK);
				if (rate == clk_get_rate(omap_dm_timer_get_fclk(timer))) {
					flag = 1;
					ret = -1;
				}
				break;
			case 2:
				printk("clk=32khz ");
				rate = clk_get_rate(omap_dm_timer_get_fclk(timer));
				if (rate == 32768)
					break;
				omap_dm_timer_set_source(timer, OMAP_TIMER_SRC_32_KHZ);
				if (rate == clk_get_rate(omap_dm_timer_get_fclk(timer))) {
					flag = 1;
					ret = -1;
				}
				break;
			case 3:
				printk("clk=divclk ");
				rate = clk_get_rate(omap_dm_timer_get_fclk(timer));
				omap_dm_timer_set_source(timer, 2);
				if (rate == clk_get_rate(omap_dm_timer_get_fclk(timer))) {
					flag = 1;
					ret = -1;
				}
				clk -= 2;
				break;
			default:
				omap_dm_timer_set_source(timer, OMAP_TIMER_SRC_32_KHZ);
				printk("clk=32khz ");
			}

			/* set clock source & calculate clock period */
			rate = clk_get_rate(omap_dm_timer_get_fclk(timer));
			period = (rate / 1000) * 10; /* 10ms */

			/* install interrupt & service routine */
			dmtimer_irq.dev_id = (void *)timer;
			setup_irq(omap_dm_timer_get_irq(timer), &dmtimer_irq);

			/* enable overflow interrupt */
			omap_dm_timer_set_int_enable(timer, OMAP_TIMER_INT_OVERFLOW);

			/* initialize the variables */
			local_jiffies = 0;
			done = 0 ;

			/* start the timer */
			omap_dm_timer_set_load_start(timer, 1, 0xffffffff - period);
			printk("timeout=2sec status= ");
			while (!done) {
				cpu_relax();
			}
			(flag == 1) ? printk("FAILED\n") : printk("PASSED\n");
			flag = 0;
			omap_dm_timer_stop(timer);
			remove_irq(omap_dm_timer_get_irq(timer), &dmtimer_irq);
			omap_dm_timer_free(timer);
		}/*for(...)*/
		clk++;
	}/* while (i < 3)*/
	printk("ret=%d ",ret);
	return ret;
}

static void __exit dmtimer_test_exit(void)
{
	/* stop the timer */
	//omap_dm_timer_stop(timer);

	/* un-install interrupt */
	//remove_irq(omap_dm_timer_get_irq(timer), &dmtimer_irq);

	/* release the timer */
	//omap_dm_timer_free(timer);

	printk(KERN_INFO "Gptimer has been released...\n");
}

module_init(dmtimer_test_init);
module_exit(dmtimer_test_exit);
