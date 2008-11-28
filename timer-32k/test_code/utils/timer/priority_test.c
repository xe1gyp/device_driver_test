#include <linux/init.h>
#include <linux/module.h>
#include <linux/delay.h>
#include <asm/irq.h>
#include <asm/arch/hardware.h>
#include <asm/arch/gptimer.h>

MODULE_LICENSE("GPL");

/* This test registers 3 GPTIMER interrupts, sets them all expire at the same time.
 * Each of the three are configured to a different prioirty.
 * ARM interrupts are masked to ensure that all 3 will be pending in hardware.  When
 * ARM interrupts are unmasked the 3 interrupts should fire in prioirty order.
 *
 * Note: for this to work on normal kernels SA_INTERRUPT must be set at the handlers else
 *  they will get interrupted.
 *
 * For real time kernels it is also necessary to set the scheduling priority of the irq
 *  thread.  Else they may not happen in the expected order.
 */

struct gptimer p_isr;

int result[6];
int result_idx = 0;

int save_pri[MAX_GPTIMERS];

static void pri_isr(u32 irq, void *dev_id, struct pt_regs *regs, u32 status)
{
	if(result_idx > 5)
		return;

	result[result_idx] = irq;
	++result_idx;
}

static int pri_init(void)
{
	unsigned long flags;
	int n2, n, i, j, pri;

	printk(KERN_ALERT "Starting pri_tst2 test\n");

	p_isr.dev_name = "prioirty irq test";
	p_isr.dev = NULL;
	p_isr.client_isr = pri_isr;

	for(n = GPTIMER8; n < GPTIMER11 ; n++){
		omap2_request_gptimer(n, GPTIMER_PARENT_SYSCLK);
		omap2_gptimer_register_isr(n, &p_isr, SA_INTERRUPT);
		omap2_gptimer_config(n, TCLR_PRESCALAR_ENABLE | TCLR_AUTORELOAD_MODE | TCLR_START_TIMER);
		omap2_gptimer_intpt_enable(n, TIER_ENABLE_OVRFLW_INTPT);		
	}

	for(j = 0; j < 2; j++){
		local_irq_save(flags);
		for(n = GPTIMER8, n2 = GPTIMER11, i = INT_GPT8_IRQ; n < GPTIMER11 ; n2--, n++, i++){
			if(j == 0){
				pri = n;  /* forward order firt 3 array entries */
				result_idx = 0; /* fill first 3 entries */
				save_pri[n] = omap_get_hw_irq_proirity(i);
			}else{
				pri = n2; /* reverse order last 3 array entries */
				result_idx = 3; /* fill up 2nd 3 array entries */
			}
			omap_set_hw_irq_proirity(i, pri);
			omap2_gptimer_start(n, (13000000/100)); /* 1/100 of a second */
		}
		mdelay(100); /* spin till all active */
		local_irq_restore(flags);  /* open up ISRs and see order */
	}

	for(i=0; i<6; i++)
		printk(KERN_ALERT "result[%d]=%d \n", i, result[i]);

	printk(KERN_ALERT "rmmod pri_tst2 module to clean up\n");
	return 0;
}

static void pri_exit(void)
{
	int n, i;

	printk(KERN_ALERT "Cleaning up pri_tst2\n");

	for(n = GPTIMER8, i = INT_GPT8_IRQ; n < GPTIMER11 ; n++, i++){
		omap2_gptimer_stop(n);
		omap2_gptimer_unregister_isr(n);
		omap2_free_gptimer(n);
		omap_set_hw_irq_proirity(i, save_pri[n]);
	}
}
module_init(pri_init);
module_exit(pri_exit);
