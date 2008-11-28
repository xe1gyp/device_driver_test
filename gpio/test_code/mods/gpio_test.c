#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/irq.h>
#include <asm/irq.h>


#include <linux/version.h>

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,27))
	#include <mach/gpio.h>
	#include <mach/irqs.h>
#else
	#include <asm/arch/gpio.h>
	#include <asm/arch/irqs.h>
#endif

char buffer[100];

static uint test = 0;
static uint gpio = 0;
static uint value = 0;

module_param (test, int, S_IRUGO|S_IWUSR);
module_param (gpio, int, S_IRUGO|S_IWUSR);
module_param (value, int, S_IRUGO|S_IWUSR);

static int omap_gpio_test_request(void){
        int ret;
        printk("Reserving GPIO line %d\n", gpio);
        ret = omap_request_gpio(gpio);
        if (!ret)
          printk("Succesfully Reserved GPIO %d\n", gpio);
        else
          printk(KERN_ERR "\n\nGPIO line %d request: failed!\n", gpio);
        return ret;
}

/* omap_free_gpio does not return any value */
static void omap_gpio_test_free(void){
        omap_free_gpio(gpio);
        printk("Freeing GPIO line %d\n", gpio);
}

/* gpio_direction_input does not return any value */
static void omap_gpio_test_direction_input(void){
	if (!gpio_direction_output(gpio, 0))
		printk("Input configuration succesful\n");
	else  
		printk("Input configuration failed\n");
}

/* gpio_direction_output does not return any value */
static void omap_gpio_test_direction_output(void){
	if (!gpio_direction_input(gpio))
		printk("Output configuration succesful\n");
	else  
		printk("Output configuration failed\n");
}

static int omap_gpio_test_irq(void){
	int ret;
	ret = OMAP_GPIO_IRQ(gpio);

	#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,27))
		
	#else
		printk("Setting IRQ %i for GPIO Line %i type: RISING\n", ret, gpio);
		set_irq_type(OMAP_GPIO_IRQ(gpio), IRQT_RISING);
		printk("Setting IRQ %i for GPIO Line %i type: FALLING\n", ret, gpio);
		set_irq_type(OMAP_GPIO_IRQ(gpio), IRQT_FALLING);
	#endif
		

	
	return ret;
}


static int omap_gpio_test_read(void){
	int ret;
	ret = omap_get_gpio_datain(gpio);      /* Reading the value of pin # */
	printk("Value of pin # %d = %d\n", gpio, ret);
	return ret;
}


/* gpio_set_value does not return any value */
static int omap_gpio_test_write(void){
	printk("Writing to GPIO line %d Value %d\n", gpio, value);
	gpio_set_value(gpio, value);
	return 0;
}

static int omap_gpio_test(void){

	int ret;

	switch(test){

		case 1: /* Reserve GPIO Line */
			ret = omap_gpio_test_request();
			break;

		case 2: /* Free GPIO Line */
			omap_gpio_test_free();
			break;

		case 3: /* Set GPIO input direction */
			omap_gpio_test_direction_input();
			break;

		case 4: /* Set GPIO output direction */
			omap_gpio_test_direction_output();
			break;

		case 5: /* GPIO read */
			omap_gpio_test_read();
			break;

		case 6: /* GPIO write */
			omap_gpio_test_write();
			break;

		case 7:
			omap_gpio_test_irq();
			break;

		default:
			printk("Test option not available.\n");
	}
	
	return ret;
}

static int __init omap_gpio_test_init(void)
{
	printk("\nOMAP GPIO Test Module Initialized\n\n");
 	omap_gpio_test();
	return 0;
}

static void __exit omap_gpio_test_exit(void)
{
	printk("\nOMAP GPIO Test Module Removal\n\n");
}

module_init(omap_gpio_test_init);
module_exit(omap_gpio_test_exit);

MODULE_AUTHOR("Texas Instruments");
MODULE_DESCRIPTION("GPIO Test Driver");
MODULE_LICENSE("GPL");
