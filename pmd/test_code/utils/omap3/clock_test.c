#include <linux/init.h>
#include <linux/clk.h>
#include <linux/module.h>
#include <asm/uaccess.h>        // get_fs(), set_fs(), KERNEL_DS
#include <linux/file.h>         // fput()
#include <linux/mm.h>           // GFP_KERNEL
#include <linux/kthread.h>
#include <linux/sched.h>
#include <linux/kernel.h>
#include <linux/version.h>

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 27))
#include <mach/clock.h>
#include <mach/io.h>
#include <linux/io.h>
#include <linux/fs.h>
#define DPLL_OUTPUT             (1 << 14)       /* DPLL output */
#else
#include <asm/arch/clock.h>
#endif

MODULE_LICENSE("GPL");
int resource_test(void);
void msleep ( unsigned int   msecs); 
int clock_test_init(void );
const char *clk_thread_name[5]={"Clk_Thread_1","Clk_Thread_2","Clk_Thread_3","Clk_thread_4","Clk_Thread_5"};
struct task_struct *Clk_Threads[5];

int write_to_file(struct file *f, char *buffer, int pos);

static uint preempt =0;
module_param (preempt, int, S_IRUGO|S_IWUSR);

static const char *clk_names[] = {
	"omap_32k_fck",
	"osc_sys_ck",
	"sys_ck",
	"sysaltck",
	"core_ck",
	"core_x2_ck",
	"emul_core_alwon_ck",
	"func_96m_ck",
	"dpll4_m3x2_ck",
	"emul_per_alwon_ck",
	"func_48m_ck",
	"func_12m_ck",
#ifndef CONFIG_ARCH_OMAP3410
	"dss_tv_fck",
#endif
	"sys_clkout1",
	"sys_clkout2",
	"l3_ck",
	"l4_ck",
	"rm_ick",
	"dpll1_fck",
	"dpll2_fck",
	"mpu_ck",
	"iva2_ck",
#ifndef CONFIG_OMAP3430_ES2
	"gfx_l3_ck",
	"gfx_fck",
	"gfx_ick",
	"gfx_2d_fck",
	"gfx_3d_fck",
	"d2d_fck",
	"d2d_ick",
#endif
	"hsotgusb_ick",
	"sdrc_ick",
	"pka_ick",
	"aes2_ick",
	"sha12_ick",
	"des2_ick",
	"aes1_ick",
	"sha11_ick",
	"des1_ick",
	"rng_ick",
	"mcbsp1_fck",
	"mcbsp1_ick",
	"mcbsp5_fck",
	"mcbsp5_ick",
	"mspro_fck",
	"mspro_ick",
	"mmchs_fck",
	"mmchs_ick",
	"uart2_fck",
	"uart2_ick",
	"hdq_fck",
	"hdq_ick",
	"gpt10_fck",
	"gpt10_ick",
	"gpt11_fck",
	"gpt11_ick",
	"omapctrl_ick",
	"mailboxes_ick",
#ifndef CONFIG_OMAP3430_ES2
	"fac_ick",
#endif
	"ssi_ick",
	"ssi_ssr_sst_fck",
	"dss1_alwon_fck",
	"dss2_alwon_fck",
	"cam_mclk",
	"cam_ick",
	"gpt1_fck",
	"gpt1_ick",
	"sync_32k_fck",
	"gpio1_fck",
	"gpio1_ick",
	"gpt12_fck",
	"gpt12_ick",
	"mcbsp2_fck",
	"mcbsp2_ick",
	"mcbsp3_fck",
	"mcbsp3_ick",
	"mcbsp4_fck",
	"mcbsp4_ick",
	"uart3_fck",
	"uart3_ick",
	"gpio3_fck",
	"gpio3_ick",
	"gpio4_fck",
	"gpio4_ick",
	"gpio5_fck",
	"gpio5_ick",
	"gpio6_fck",
	"gpio6_ick",
	"gpt2_fck",
	"gpt2_ick",
	"gpt3_fck",
	"gpt3_ick",
	"gpt4_fck",
	"gpt4_ick",
	"gpt5_fck",
	"gpt5_ick",
	"gpt6_fck",
	"gpt6_ick",
	"gpt7_fck",
	"gpt7_ick",
	"gpt8_fck",
	"gpt8_ick",
	"gpt9_fck",
	"gpt9_ick",
	"sr1_fck",
	"sr2_fck",
#ifdef CONFIG_OMAP3430_ES2
	"omap_120m_fck",
	"usbhost_ick",
	"usbhost_48m_fck",
	"usbhost_120m_fck",
	"usbtll_fck",
	"usbtll_ick",
#else
	"fsusb_fck",
	"fsusb_ick",
#endif
	/* External mcbsp clock */
	"ext_mcbsp_ck",
	"virt_vdd1_prcm_set",
	"virt_vdd2_prcm_set"
};

int create_thread(void){

	int i;
	if (preempt ==1) {
		for (i=0; i <5; i++){
        	        Clk_Threads[i]=kthread_create((void *)clock_test_init,NULL,clk_thread_name[i]);
			set_user_nice(Clk_Threads[i],19-i);
			wake_up_process(Clk_Threads[i]);
			msleep(50);
        	}
	} else { 
		clock_test_init();	
	}
        return(0);


}

int clock_test_init(void)
{
        u32 ret=0, usecount;
	int i;
	

        unsigned long rate, valid_rate;
        struct clk *tclk, *temp_clock, *mpu_ck, *iva2_ck, *l3_ck;
	struct device *dev = NULL;


        struct file *f = NULL;
        mm_segment_t orig_fs;
        int pos;
        char buffer[100];
	
        /* Obtain a file object pointer */
        f = filp_open("/clock_test.msg", O_WRONLY | O_CREAT, 0600);
        if (!f || !f->f_op || !f->f_op->write) {
                printk("WARNING: File (write) object is a null pointer!!!\n");
        }
        f->f_pos = 0;
        pos = 0;
        orig_fs = get_fs();
        set_fs(KERNEL_DS);
	
	printk(KERN_ALERT "Starting clock test\n");
	for (i = 0; i < ARRAY_SIZE(clk_names); i++) {
		printk(KERN_INFO "\nclock: %s",clk_names[i]);
	
		tclk = clk_get(dev, clk_names[i]);
		if(tclk == NULL) {
			printk(KERN_ERR "clk_get returned null value for clk %s \n",clk_names[i]);
			ret = 1;
			continue;
		}
	
		printk("\nClk name: %s\n", tclk->name);
		if (preempt ==1)
			msleep(50);    

		usecount = clk_get_usecount(tclk);
                if (usecount)
                        printk(KERN_INFO "Usecount is %d\n", usecount);
		if((usecount == 0) && (strcmp(tclk->name, "iva2_ck"))) {
		omap_writel(0x1,0x48002404);
                omap_writel(0x0,0x54006050);
                omap_writel(0xFFFFFFFF,0x54006058);
		usecount = clk_get_usecount(tclk);
                        printk(KERN_INFO "Usecount is %d\n",usecount);

		}

                ret = clk_enable(tclk);
		if (preempt ==1)
			msleep(50);
                if (ret != 0)
                        printk(KERN_ERR
                               "Error: clk_enable for clock %s returned %d\t \n",
				tclk->name, ret);
#ifndef CONFIG_OMAP3430_ES2
		if (!(strcmp(tclk->name, "gfx_l3_ck"))) {
			omap_writel(0x6, 0x48004B00);
			omap_writel(0x6, 0x50000108);
			omap_writel(0x0, 0x48004B00);
		}
#endif
                if (ret == 0) {
                        clk_disable(tclk);
                }

                rate = clk_get_rate(tclk);
                printk(KERN_INFO "Rate of clk %s is %lu \n", tclk->name,tclk->rate);

                printk(KERN_INFO "Flags %u \n", tclk->flags);
                if ((tclk->flags & RATE_CKCTL) || (tclk->flags & DPLL_OUTPUT)) {
                        printk("Testing round rate:\n");
                        valid_rate = clk_round_rate(tclk, 1000000000);
                        /*All clocks should return highest possible values
                         *depending on dividers that can be changed */
                        printk("Valid rate when input is 1G: %lu \n",
                               valid_rate);
                        ret = clk_set_rate(tclk, valid_rate);
                        printk("Set rate returned : %d for valid rate : %lu\t  \n", ret, valid_rate);
			if (preempt ==1)
				msleep(50);
                        rate = clk_get_rate(tclk);
                        printk(KERN_INFO "Rate of clk %s is %lu\n", tclk->name,
                               tclk->rate);

                        /* Check closest possible rate to 600 Mhz */
                        valid_rate = clk_round_rate(tclk, 600000000);
                        printk("Valid rate when input is 600M: %lu\n",
                               valid_rate);
                        ret = clk_set_rate(tclk, valid_rate);
                        printk("Setrate returned:%d for valid rate:%lu \n", ret, valid_rate);
			if (preempt ==1)
				msleep(50);
                        rate = clk_get_rate(tclk);
                        printk(KERN_INFO "Rate of clk %s is %lu \n", tclk->name,
                               tclk->rate);
                        /* Check closest possible rate to 500 Mhz */
                        valid_rate = clk_round_rate(tclk, 500000000);
                        printk("Valid rate when input is 500M: %lu \n",
                               valid_rate);
                        ret = clk_set_rate(tclk, valid_rate);
                        printk("Set rate returned : %d for valid rate : %lu \n", ret, valid_rate);
                        rate = clk_get_rate(tclk);
                        printk(KERN_INFO "Rate of clk %s is %lu \n", tclk->name,
                               tclk->rate);
                        /* Check closest possible rate to 400 Mhz */
                        valid_rate = clk_round_rate(tclk, 400000000);
                        printk("Valid rate when input is 400M: %lu \n",
                               valid_rate);
                        ret = clk_set_rate(tclk, valid_rate);
                        printk("Set rate returned : %d for valid rate : %lu  \n", ret, valid_rate);
                        rate = clk_get_rate(tclk);
                        printk(KERN_INFO "Rate of clk %s is %lu \n", tclk->name,

                               tclk->rate);

                        /* Check closest possible rate to 300 Mhz */
                        valid_rate = clk_round_rate(tclk, 300000000);
                        printk("Valid rate when input is 300M: %lu \n",
                               valid_rate);
                                ret = clk_set_rate(tclk, valid_rate);
                                printk("Set rate returned : %d for valid rate : %lu  \n", ret, valid_rate);
                        rate = clk_get_rate(tclk);
                        printk(KERN_INFO "Rate of clk %s is %lu \n", tclk->name,
                               tclk->rate);
                        /* Check closest possible rate to 200 Mhz */
                        valid_rate = clk_round_rate(tclk, 200000000);
                        printk("Valid rate when input is 200M: %lu \n",
                               valid_rate);
                        ret = clk_set_rate(tclk, valid_rate);
                        printk("Set rate returned : %d for valid rate : %lu \n", ret, valid_rate);
                        rate = clk_get_rate(tclk);
                        printk(KERN_INFO "Rate of clk %s is %lu \n", tclk->name,
                               tclk->rate);

                        /* Check closest possible rate to 100 Mhz */
                        valid_rate = clk_round_rate(tclk, 100000000);
                        printk("Valid rate when input is 100M: %lu \n",
                               valid_rate);
                        ret = clk_set_rate(tclk, valid_rate);
                        printk("Set rate returned : %d for valid rate : %lu \n", ret, valid_rate);
                        rate = clk_get_rate(tclk);
                        printk(KERN_INFO "Rate of clk %s is %lu \n", tclk->name,
                               tclk->rate);
                }

		temp_clock = clk_get(dev, "virt_vdd1_prcm_set");
                if (tclk == temp_clock) {
			mpu_ck = clk_get(dev, "mpu_ck");
			if (preempt ==1)
				msleep(50);
			iva2_ck = clk_get(dev, "iva2_ck");
                        valid_rate = clk_round_rate(tclk, 500000000);
                        printk("Valid rate for 500M for virt1 clock : %lu \n",
                               valid_rate);
                        valid_rate = clk_round_rate(tclk, 400000000);
                        printk("Valid rate for 400M for virt1 clock : %lu  \n",
                               valid_rate);
                        ret = clk_set_rate(tclk, valid_rate);
                        printk("set rate returned: %d  \n", ret);
                        printk("New rate = %lu \n", tclk->rate);
                        printk("Mpu speed:%lu, iva speed : %lu \n", mpu_ck->rate,
                               iva2_ck->rate);
                        valid_rate = clk_round_rate(tclk, 200000000);
                        printk("Valid rate for 200M for virt1 clock : %lu \n",
                               valid_rate);
                        ret = clk_set_rate(tclk, valid_rate);
                        printk("set rate returned: %d  \n", ret);
                        printk("New rate = %lu  \n", tclk->rate);
                        printk("Mpu speed:%lu, iva speed : %lu  \n", mpu_ck->rate,
                               iva2_ck->rate);
                        valid_rate = clk_round_rate(tclk, 100000000);
                        printk("Valid rate for 100M for virt1 clock : %lu \n",
                               valid_rate);
                        ret = clk_set_rate(tclk, valid_rate);
                        printk("set rate returned: %d \n", ret);
                        printk("New rate = %lu \n", tclk->rate);
                        printk("Mpu speed:%lu, iva speed : %lu \n", mpu_ck->rate,
                               iva2_ck->rate);
                        valid_rate = clk_round_rate(tclk, 400000000);
                        printk("Valid rate for 400M for virt1 clock : %lu \n",
                               valid_rate);
                        ret = clk_set_rate(tclk, valid_rate);
                        printk("set rate returned: %d \n", ret);
                        printk("New rate = %lu \n", tclk->rate);
                        printk("Mpu speed:%lu, iva speed : %lu \n", mpu_ck->rate,
                               iva2_ck->rate);
                }
		temp_clock = clk_get(dev, "virt_vdd2_prcm_set");
                if (tclk == temp_clock) {
			l3_ck = clk_get(dev, "l3_ck");
			if (preempt ==1)
				msleep(50);
                        valid_rate = clk_round_rate(tclk, 200000000);
                        printk("Valid rate for 200M for virt1 clock : %lu \n",
                               valid_rate);
                        ret = clk_set_rate(tclk, valid_rate);
                        printk("set rate returned: %d \n", ret);
                        printk("New rate = %lu \n", tclk->rate);
                        printk("L3 speed:%lu \n", l3_ck->rate);
                        valid_rate = clk_round_rate(tclk, 100000000);
                        printk("Valid rate for 100M for virt1 clock : %lu \n",
                               valid_rate);
                        ret = clk_set_rate(tclk, valid_rate);
                        printk("set rate returned: %d \n", ret);
                        printk("New rate = %lu \n", tclk->rate);
                        printk("L3 speed:%lu \n", l3_ck->rate);
                        valid_rate = clk_round_rate(tclk, 200000000);
                        printk("Valid rate for 200M for virt1 clock : %lu  \n",
                               valid_rate);
                        ret = clk_set_rate(tclk, valid_rate);
                        printk("set rate returned: %d \n", ret);
                        printk("New rate = %lu \n", tclk->rate);
                        printk("L3 speed:%lu \n", l3_ck->rate);
                }
                clk_put(tclk);
                tclk = NULL;
                clk_put(tclk);
                tclk = NULL;
        }
	if (ret != 0) {
		printk("Test failed \n");
	        sprintf(buffer, "Test failed \n");
	        pos = write_to_file(f, buffer, pos);
	} else {
		printk("Test passed \n");
		sprintf(buffer, "Test passed \n");
                pos = write_to_file(f, buffer, pos);
	}

        printk(KERN_INFO "End of clock test \n");
        printk(KERN_INFO "Since system settings are changed, there could be issues  \n");
        printk(KERN_INFO "The system needs to be rebooted now \n");

	return 0;

}

static void clock_test_exit(void)
{
	printk("Exiting\n");
	printk("Done\n");

}

// Write to file
int write_to_file(struct file *f, char *buffer, int pos) {
        int i, len, bufsize=0;
        const int MAXBUFSIZE=100;

        for (i=0; i<MAXBUFSIZE; i++) {
                if (buffer[i]=='\0')
                        break;
                bufsize++;
        }

        f->f_pos = pos;
        len = f->f_op->write(f, buffer, bufsize, &f->f_pos);

        return pos+len;
}

module_init(create_thread);
module_exit(clock_test_exit);
