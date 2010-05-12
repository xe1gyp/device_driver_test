#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/moduleparam.h>
MODULE_LICENSE("GPL");

static uint opt1;
static uint opt2;
module_param(opt1, int, S_IRUGO|S_IWUSR);
module_param(opt2, int, S_IRUGO|S_IWUSR);

int resource_test(void);
int memory_resource_test(int);
int dpll_resource_test(int);

int resource_test_init(void)
{

	switch (opt1) {
	case 0x1:
			resource_test();
			break;
	case 0x2:
			memory_resource_test(opt2);
			break;
	case 0x3:
			dpll_resource_test(opt2);

			break;
	default:
		printk(" INVALID PARAMETERS-please use below HELP MENU for testcase usage \n");
		printk(" ************************ HELP MENU ***************************** \n");
		printk(" ::::::::::::::Use hex value - test function mapping::::::::::::: \n");
		printk(" opt1=0x1      -	             resource_test                 \n");
		printk(" opt1=0x2      - opt2=0x1 to 0x8    memory resource _test         \n");
		printk(" opt1=0x2      - opt2=0x3F          All memory resource  tests    \n");
		printk(" opt1=0x3      - opt2=0x1 to 0xa    Dpll resource test            \n");
		printk(" opt1=0x3      - opt2=0x3F          All DPll  resource  tests     \n");
		printk(" Usage: insmod resource_test.ko opt1=<option> opt2=<option>       \n");
		printk(" ************************** HELP ******************************** \n");

		return -1;
	}
	return 0;
}

void resource_test_exit(void)
{
	printk(KERN_INFO "Exiting \n Done\n");
	return;
}

module_init(resource_test_init);
module_exit(resource_test_exit);
