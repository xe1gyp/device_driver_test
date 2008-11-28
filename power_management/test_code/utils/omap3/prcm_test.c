#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <asm/uaccess.h>        // get_fs(), set_fs(), KERNEL_DS
#include <linux/file.h>         // fput()
#include <linux/mm.h>           // GFP_KERNEL

MODULE_LICENSE("GPL");

static uint opt = 0;
static uint load = 0;
static uint itr = 0;
module_param (opt, int, S_IRUGO|S_IWUSR);
module_param (load, int, S_IRUGO|S_IWUSR);
module_param (itr, int, S_IRUGO|S_IWUSR);

int power_configuration_test(void);
int powerapi_test(void);
int clkapi_test(void);
int dpllapi_test(void);
int voltage_scaling_set_of_test(void);
int frequency_scaling_test(void);

int interrupt_latency_init(int load, int itr);
void interrupt_latency_exit(void);

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

static int prcm_test_init(void)
{
	int fail = 0;

	struct file *f = NULL;
	mm_segment_t orig_fs;
	int pos;
	char buffer[100];

	/* Obtain a file object pointer */
	f = filp_open("/prcm_test.msg", O_WRONLY | O_CREAT, 0600);
	if (!f || !f->f_op || !f->f_op->write) {
		printk("WARNING: File (write) object is a null pointer!!!\n");
	}
	f->f_pos = 0;
	pos = 0;
	orig_fs = get_fs();
	set_fs(KERNEL_DS);

	printk ("******************* HELP **************************\n");
	printk ("Use hex value - test function mapping\n");
	printk ("0x1\t- clkapi_test\n");
	printk ("0x2\t- dpllapi_test\n");
	printk ("0x4\t- powerapi_test\n");
	printk ("0x8\t- power_configuration_test\n");
	printk ("0x10\t- frequency_scaling_test\n");
	printk ("0x20\t- voltage_scaling_set_of_test\n");
	printk ("\nFor multiple tests OR the hex values\n"
		"for e.g. 0x3 will do both CLK and DPLL\n" 
		"API tests in sequence\n");
	printk ("\n0x3F\t- All tests\n");
	printk ("\nUsage: insmod prcm_test.ko opt=<option>\n\n");
	printk ("0x40 - Interrupt latency test start\n");
	printk ("[ Usage: insmod opt=0x40 load=<uSec> itr=<iterations> ]\n\n");
	printk ("After confirming the number of interrupts,\n(view with - "
		"cat /proc/interrupts)\n matches the iterations specified,\n"
		" then do an rmmod of this module to view the results\n");
	printk ("******************* HELP **************************\n");

	if (!opt || ((opt != 0x3F) && (opt != 0x40))) {
		printk ("\nPlease refer the above Menu\n\n");
		return -1;
	}

        if (opt & 0x1) {
		if (clkapi_test()) {
			printk ("CLOCK TEST FAILED\n");
			fail++;
		} else 
			printk ("CLOCK TEST PASSED\n");
	}

	if (opt & 0x2) {
		if (dpllapi_test ()) {
			printk ("DPLL TEST FAILED\n");
			fail++;
		} else 
			printk ("DPLL TEST PASSED\n");
	}


	if (opt & 0x4) {
		if (powerapi_test ()) {
			printk ("POWER TEST FAILED\n");
			fail++;
		} else 
			printk ("POWER TEST PASSED\n");
	}

	if (opt & 0x8) {
		if (power_configuration_test ()) {
			printk ("POWER CONFIGURATION TEST FAILED\n");
			fail++;
		} else 
			printk ("POWER CONFIGURATION TEST PASSED\n");
	}


	if (opt & 0x10) {
		if (frequency_scaling_test ()) {
			printk ("FREQUENCY SCALING TEST FAILED\n");
			fail++;
		} else 
			printk ("FREQUENCY SCALING TEST PASSED\n");
	}

	if (opt & 0x20) {
		if (voltage_scaling_set_of_test ()) {
			printk ("VOLTAGE SCALING TEST FAILED\n");
			fail++;
		} else 
			printk ("VOLTAGE SCALING TEST PASSED\n");
	}

	if (opt & 0x40) {
		if (!load) {
			load = 50000;
			printk ("INFO:"
				"Default timer load of 50ms is considered\n");
		}
		if (!itr) {
			itr = 1000;
			printk ("INFO:"
				"Default iterations of 1000 is considered\n");
		}
		if (interrupt_latency_init(load, itr))
		{
			printk ("Interrupt latency test failed to start\n");
			return -1;	
		} else {
			printk ("Interrupt latency test started ... \n");
			goto last;
		}
	}

	if (fail > 0) {
		printk("Error: %d tests failed\n",fail);
		sprintf(buffer, "Error: %d tests failed\n",fail);
		pos = write_to_file(f, buffer, pos);		
	}
	else {
		printk("All tests passed\n");
		sprintf(buffer, "All tests passed\n");
		pos = write_to_file(f, buffer, pos);
	}
	printk("Please reboot the system as system settings have changed\n");
last:
        return 0;
}

static void prcm_test_exit(void)
{
	interrupt_latency_exit ();
	printk("Exiting\nDone\n");
}

module_init(prcm_test_init);
module_exit(prcm_test_exit);
