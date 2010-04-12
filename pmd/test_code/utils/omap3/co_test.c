#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kthread.h>
#include <linux/sched.h>
#include <linux/version.h>
#include <linux/kernel.h>

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 27))
#include <mach/prcm_34xx.h>
#include <mach/prcm-debug.h>
#include <linux/resource.h>
#else
#include <asm/arch/prcm-debug.h>
#include <asm/arch/prcm_34xx.h>
#include <asm/arch/resource.h>
#endif

MODULE_LICENSE("GPL");

#define REQUEST_CO 0x1
#define RELEASE_CO 0x2
#define PRCM_LATENCY_CONSTRAINT 0x5

static uint co_type;
module_param(co_type, int, S_IRUGO|S_IWUSR);
static uint preempt;
module_param(preempt, int, S_IRUGO|S_IWUSR);
static uint  target_val;
module_param(target_val, int, S_IRUGO|S_IWUSR);

u32 map_table[5] = {
	PRCM_VDD1_CONSTRAINT,
	PRCM_VDD2_CONSTRAINT,
	PRCM_ARMFREQ_CONSTRAINT,
	PRCM_DSPFREQ_CONSTRAINT,
	PRCM_LATENCY_CONSTRAINT,
};

struct constraint_param thread_input[5];
struct constraint_param *thread_input_ptr;
struct constraint_param * constraint_test(unsigned int, unsigned long, int,
						struct constraint_param *);
void msleep(unsigned int  msecs);

int kernel_preemption_test(struct constraint_param *constraint_param_ptr)
{

	printk(" Starting VDD1 OPP constraint\n");
	constraint_param_ptr = constraint_test(map_table[0], 1, REQUEST_CO,
							constraint_param_ptr);
	constraint_test(map_table[0], 1, RELEASE_CO, constraint_param_ptr);

	constraint_param_ptr = constraint_test(map_table[0], 2, REQUEST_CO,
							constraint_param_ptr);
	constraint_test(map_table[0], 2, RELEASE_CO, constraint_param_ptr);

	constraint_param_ptr = constraint_test(map_table[0], 3, REQUEST_CO,
							constraint_param_ptr);
	constraint_test(map_table[0], 3, RELEASE_CO, constraint_param_ptr);

	constraint_param_ptr = constraint_test(map_table[0], 4, REQUEST_CO,
							constraint_param_ptr);
	constraint_test(map_table[0], 4, RELEASE_CO, constraint_param_ptr);
	msleep(50);
	constraint_param_ptr = constraint_test(map_table[0], 5, REQUEST_CO,
							constraint_param_ptr);
	constraint_test(map_table[0], 5, RELEASE_CO, constraint_param_ptr);

	printk("End of  VDD1 OPP constraint\n");


	printk("Starting VDD2 OPP constraint\n");
	constraint_param_ptr = constraint_test(map_table[1], 2, REQUEST_CO,
							constraint_param_ptr);
	constraint_test(map_table[1], 2, RELEASE_CO, constraint_param_ptr);
	msleep(50);
	constraint_param_ptr = constraint_test(map_table[1], 3, REQUEST_CO,
							constraint_param_ptr);
	constraint_test(map_table[1], 3, RELEASE_CO, constraint_param_ptr);

	printk("End of  VDD2 OPP constraint\n");

	printk("Starting ARM FREQUENCY constraint\n");
	constraint_param_ptr = constraint_test(map_table[2], 200, REQUEST_CO,
							constraint_param_ptr);
	constraint_test(map_table[2], 200, RELEASE_CO, constraint_param_ptr);

	constraint_param_ptr = constraint_test(map_table[2], 300, REQUEST_CO,
							constraint_param_ptr);
	constraint_test(map_table[2], 300, RELEASE_CO, constraint_param_ptr);

	constraint_param_ptr = constraint_test(map_table[2], 350, REQUEST_CO,
							constraint_param_ptr);
	constraint_test(map_table[2], 350, RELEASE_CO, constraint_param_ptr);

	constraint_param_ptr = constraint_test(map_table[2], 400, REQUEST_CO,
							constraint_param_ptr);
	constraint_test(map_table[2], 400, RELEASE_CO, constraint_param_ptr);

	constraint_param_ptr = constraint_test(map_table[2], 425, REQUEST_CO,
							constraint_param_ptr);
	constraint_test(map_table[2], 425, RELEASE_CO, constraint_param_ptr);

	constraint_param_ptr = constraint_test(map_table[2], 500, REQUEST_CO,
							constraint_param_ptr);
	constraint_test(map_table[2], 500, RELEASE_CO, constraint_param_ptr);

	constraint_param_ptr = constraint_test(map_table[2], 525, REQUEST_CO,
							constraint_param_ptr);
	constraint_test(map_table[2], 525, RELEASE_CO, constraint_param_ptr);
	msleep(50);
	constraint_param_ptr = constraint_test(map_table[2], 575, REQUEST_CO,
							constraint_param_ptr);
	constraint_test(map_table[2], 575, RELEASE_CO, constraint_param_ptr);
	printk("End of  ARM FREQUENCY constraint\n");

	printk("Starting DSP FREQUENCY constraint\n");

	constraint_param_ptr = constraint_test(map_table[3], 100,  REQUEST_CO,
							constraint_param_ptr);
	constraint_test(map_table[3], 100, RELEASE_CO, constraint_param_ptr);

	constraint_param_ptr = constraint_test(map_table[3], 150, REQUEST_CO,
							constraint_param_ptr);
	constraint_test(map_table[3], 150, RELEASE_CO, constraint_param_ptr);

	constraint_param_ptr = constraint_test(map_table[3], 175, REQUEST_CO,
							constraint_param_ptr);
	constraint_test(map_table[3], 175, RELEASE_CO, constraint_param_ptr);
	msleep(50);
	constraint_param_ptr = constraint_test(map_table[3], 200, REQUEST_CO,
							constraint_param_ptr);
	constraint_test(map_table[3], 200, RELEASE_CO, constraint_param_ptr);

	constraint_param_ptr = constraint_test(map_table[3], 250, REQUEST_CO,
							constraint_param_ptr);
	constraint_test(map_table[3], 250, RELEASE_CO, constraint_param_ptr);

	constraint_param_ptr = constraint_test(map_table[3], 300, REQUEST_CO,
							constraint_param_ptr);
	constraint_test(map_table[3], 300, RELEASE_CO, constraint_param_ptr);

	printk("End of DSP FREQUENCY constraint\n");
	return 0;

}


int co_test_init(void)
{
	int i;
	const char *Con_thread_name[5] = {"Con_Thread_1", "Con_Thread_2",
					  "Con_Thread_3", "Con_thread_4",
					  "Con_Thread_5"
					};
	struct task_struct *Con_Threads[5];
	co_type = 0;
	preempt = 0;
	target_val = 0;
	printk("************************ HELP **********************\n");
	printk("USAGE: ./co_test.ko co_type=\"option\" target_val=\"value\""
					" preempt=\"enable or disable\" \n");
	printk("Following are the valid co_type options\n");
	printk("co_type=0x1\t target_val=\t preempt=0\t- VDD1 OPP constgaint\n");
	printk("co_type=0x2\t target_val=\t preempt=0\t- VDD2 OPP constraint\n");
	printk("co_type=0x3\t target_val=\t preempt=0\t- ARM FREQUENCY constraint\n");
	printk("co_type=0x4\t target_val=\t preempt=0\t- DSP FREQUENCY constraint\n");
	printk("co_type=0x5\t target_val=\t preempt=0\t- LOGICAL constraint\n");
	printk("co_type=0x6\t target_val=1\t preempt=1\t- Kernel_Preemption_test\n");
	printk("************************ HELP ***********************\n");

	if ((!co_type) || !(target_val)) {
		printk ("\nPlease refer the above Menu\n\n");
		return -1;
	}

	if (co_type == 0x6 && target_val == 1 && preempt == 1) {
		for (i = 0; i < 5; i++) {
			thread_input_ptr = (thread_input + i);
			thread_input_ptr->nb_test_pre_ptr = &nb_test_pre[i];
			thread_input_ptr->nb_test_post_ptr = &nb_test_post[i];
			Con_Threads[i] =
				kthread_create((void *)kernel_preemption_test,
				thread_input + i, Con_thread_name[i]);
			set_user_nice(Con_Threads[i], 19 - i);
			wake_up_process(Con_Threads[i]);
		}
	}

	if (co_type >= 0x1 && co_type < 0x6 && preempt == 0) {
		thread_input_ptr = &thread_input[0];
		thread_input_ptr->nb_test_pre_ptr = &nb_test_pre[0];
		thread_input_ptr->nb_test_post_ptr = &nb_test_post[0];
		thread_input_ptr  = constraint_test(map_table[co_type-1],
			target_val, REQUEST_CO, thread_input_ptr);
	}
	return 0;
}

void co_test_exit(void)
{

	constraint_test(map_table[co_type-1], target_val, RELEASE_CO,
							thread_input_ptr);
	return;
}

module_init(co_test_init);
module_exit(co_test_exit);

