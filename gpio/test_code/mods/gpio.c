#include <linux/types.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/sched.h>
#include <linux/spinlock.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/version.h>
#include <asm/uaccess.h>

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,27))
	#include <mach/gpio.h>
#else
	#include <asm/arch/gpio.h>
#endif

#define		DELAY_MAJOR		241
#define		DELAY_DEV_NAME	        "gpiotest"

extern int twl4030_free_gpio(int gpio);
extern int twl4030_request_gpio(int gpio);
extern int twl4030_set_gpio_direction(int gpio, int is_input);
extern int twl4030_set_gpio_dataout(int gpio, int enable);
extern int twl4030_set_gpio_debounce(int gpio, int enable);

int preempt_gpio1,preempt_gpio2;

static int gio_open(struct inode * inode, struct file * file)
{
	return 0;
}

static int gio_release(struct inode * inode, struct file * file)
{
	return 0;
}

static ssize_t gio_write(struct file * file, const char * buf, size_t count, loff_t *x)
{
	int appType;
	int time_mics = 30;

	if (count != sizeof(int)) return -EINVAL;
	get_user(appType, (int*)buf);


	switch (appType)
	{
		case 3: 
		{
			preempt_gpio1 =appType;
			printk(KERN_INFO"Request for GPIO = %d\n",preempt_gpio1);

                	//mdelay(time_mics);
        		if (twl4030_request_gpio(preempt_gpio1) < 0) {
                		printk(KERN_ERR "can't get GPIO\n");
                		return -EINVAL;
        		}
        		twl4030_set_gpio_direction(preempt_gpio1, 1);
        		twl4030_set_gpio_debounce(preempt_gpio1, 1);
			twl4030_set_gpio_dataout(preempt_gpio1, 0);
        		twl4030_set_gpio_dataout(preempt_gpio1, 1);
			twl4030_free_gpio(preempt_gpio1);

			break;	
		}	
		case 4:
		{
                        preempt_gpio2 =appType;
			printk(KERN_INFO"Request for GPIO = %d\n",preempt_gpio2);
	
        		if (omap_request_gpio(preempt_gpio2) < 0) {
                		printk(KERN_ERR "can't get GPIO\n");
                		return -EINVAL;
        		}
       			twl4030_set_gpio_direction(preempt_gpio2, 1);
			twl4030_set_gpio_debounce(preempt_gpio2, 1);
		        twl4030_set_gpio_dataout(preempt_gpio2, 0);
		        twl4030_set_gpio_dataout(preempt_gpio2, 1);
		        omap_free_gpio(preempt_gpio2);

			break;
		}
		default:
			printk (KERN_INFO"Entered default\n");
			return -EINVAL ;	
	}
	return sizeof(int);
}

static struct file_operations gio_fops = {
  .owner	=	THIS_MODULE,
  .write 	=	gio_write,
  .open		=	gio_open,
  .release	= 	gio_release
};


static int __init gio_module_init(void)
{

	if (register_chrdev(DELAY_MAJOR, DELAY_DEV_NAME, &gio_fops)) {
		printk(KERN_ERR "FAILURE: Insert Module, MAJOR = %d.\n",DELAY_MAJOR);
		return -EIO;
	}
	printk(KERN_INFO"SUCESS: Insert Module\n");
	return 0;
}

static void __exit gio_module_exit(void)
{
	unregister_chrdev(DELAY_MAJOR, DELAY_DEV_NAME);
        printk(KERN_INFO"SUCCESS: Exit Module\n");
}

module_init(gio_module_init);
module_exit(gio_module_exit);

MODULE_AUTHOR("Texas Instruments");
MODULE_DESCRIPTION("TWL4030 GPIO preempt test driver");
MODULE_LICENSE("GPL");
