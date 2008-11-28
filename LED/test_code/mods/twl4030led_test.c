/*
 * drivers/i2c/chips/twl4030_core.c
 *
 * Copyright (C) 2007 Texas Instruments, Inc.
 *
 * This file is licensed under the terms of the GNU General Public License 
 * version 2. This program is licensed "as is" without any warranty of any 
 * kind, whether express or implied.
 *
 */

#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/module.h>
#include <linux/time.h>
#include <linux/delay.h>
#include <asm/arch/twl4030.h>

static int led = 1;
static int cmd = 0;
static int mode = 0;
static int level = 10;

module_param(led, int, S_IRUGO);
module_param(cmd, int, S_IRUGO);
module_param(mode, int, S_IRUGO);
module_param(level, int, S_IRUGO);

void gpio_led_init(void);
void gpio_led_set(int state);
void gpio_led_exit(void);

int on_off_test(u8 led);
int brightness_test(u8 led);
int led_busy_test(u8 led);
int blinking(u8 led, int mode, u8 level);

int case1(u8 led);
int case2(u8 led);
int case3(void);

#define GPIO_LED 7

void delay(int milliS)
{
	typeof(jiffies) j;
	j = jiffies + msecs_to_jiffies(milliS);
	while (time_before(jiffies, j))
		schedule();
}

static int __init twl4030_led_test_init(void)
{
	switch (cmd) {
	case 1:
		on_off_test(led);
		break;
	case 2:
		brightness_test(led);
		break;
	case 3:
		led_busy_test(led);
		break;
	case 4:
		blinking(led, mode, level);
		break;
	default:
		printk("\n Please include a valid cmd parameter!\n");
		break;
	}

	return -ENODEV;
}

static void __exit twl4030_led_test_exit(void)
{
	return;
}

int case1(u8 led)
{
	return 0;
}

int case2(u8 led)
{
	gpio_led_init();
	twl4030ledSetBlinkingMode(led, BLINKING_EXTERNAL);
	gpio_led_set(1);
	delay(1000);
	gpio_led_set(0);
	delay(1000);
	gpio_led_set(1);
	delay(1000);
	gpio_led_set(0);
	delay(1000);
	twl4030ledSetBlinkingMode(led, BLINKING_OFF);
	gpio_led_exit();
	return 0;
}

int case3(void)
{
	return 0;
}

void gpio_led_init(void)
{
	int ret;

	ret = twl4030_request_gpio(GPIO_LED);
	if (ret < 0) {
		printk("\nGpio can not be acquired");
		return;
	}
	twl4030_set_gpio_direction(GPIO_LED, TWL4030_GPIO_IS_OUTPUT);
}

void gpio_led_set(int state)
{
	twl4030_set_gpio_dataout(GPIO_LED, state);
	udelay(100);
}

void gpio_led_exit(void)
{
	twl4030_free_gpio(GPIO_LED);
}

int on_off_test(u8 led)
{
	int count;

	if (twl4030ledRequest(led)) {
		printk("\nError: (%s) could not be acquired",
		       (led == LEDEN_LEDA) ? "LED A" : "LED B");
		return -1;
	}

	twl4030ledSetDutyCycle(led, 10);

	printk("\n****** ON/OFF test selected ******\n");
	printk("\nLed under test (%s)",
	       (led == LEDEN_LEDA) ? "LED A" : "LED B");
	printk("\nThe led should turn ON/OFF six times");
	for (count = 0; count < 6; count++) {
		twl4030ledSetState(led, LED_ON);
		delay(500);
		twl4030ledSetState(led, LED_OFF);
		delay(500);
	}
	printk("\nDone.\n");
	twl4030ledRelease(led);

	return 0;
}

int brightness_test(u8 led)
{
	int level;
	int try;

	if (twl4030ledRequest(led)) {
		printk("\nError: (%s) could not be acquired",
		       (led == LEDEN_LEDA) ? "LED A" : "LED B");
		return -1;
	}

	twl4030ledSetDutyCycle(led, 10);
	twl4030ledSetState(led, LED_ON);

	printk("\n****** Brightness test selected ******\n");
	printk("\nLed under test (%s)",
	       (led == LEDEN_LEDA) ? "LED A" : "LED B");
	printk
	    ("\nThe led should decreased brightness level to the minimum and then increased to maximum three times");
	for (try = 0; try < 3; try++) {
		/* Decreasing */
		for (level = 10; level >= 0; level--) {
			twl4030ledSetDutyCycle(led, level);
			delay(140);
		}
		/* Increasing */
		for (level = 0; level <= MAX_LEVEL; level++) {
			twl4030ledSetDutyCycle(led, level);
			delay(140);
		}
	}
	twl4030ledSetState(led, LED_OFF);
	printk("\nDone.\n");
	twl4030ledRelease(led);
	return 0;
}

int led_busy_test(u8 led)
{
	int error = 0;
	printk("\n****** Led Busy Test selected ******\n");
	printk("\nLed under test (%s)",
	       (led == LEDEN_LEDA) ? "LED A" : "LED B");
	printk
	    ("\nNo any action performed by led. Just trying to acquire a LED when it is busy");

	if (twl4030ledRequest(led)) {
		printk("\nError: (%s) could not be acquired",
		       (led == LEDEN_LEDA) ? "LED A" : "LED B");
		return -1;
	}
	printk("\n\n(%s) could be acquired",
	       (led == LEDEN_LEDA) ? "LED A" : "LED B");
	printk("\nTrying to acquire %s again...",
	       (led == LEDEN_LEDA) ? "LED A" : "LED B");
	if (twl4030ledRequest(led)) {
		printk
		    ("\nError: (%s) could not be acquired <--- This is what is supposed to do",
		     (led == LEDEN_LEDA) ? "LED A" : "LED B");
	} else {
		printk("\nSomething is wrong. This is not supposed to be done");
		error++;
	}

	twl4030ledRelease(led);
	printk("\n(%s) is now free", (led == LEDEN_LEDA) ? "LED A" : "LED B");
	printk("\n\nTrying to acquire %s again...",
	       (led == LEDEN_LEDA) ? "LED A" : "LED B");
	if (twl4030ledRequest(led)) {
		printk("\nError: (%s) could not be acquired",
		       (led == LEDEN_LEDA) ? "LED A" : "LED B");
		error++;
	} else {
		printk("\n\n(%s) could be acquired",
		       (led == LEDEN_LEDA) ? "LED A" : "LED B");
		twl4030ledRelease(led);
	}

	if (!error)
		printk("\nTEST PASSED!\n");
	return 0;
}

int blinking(u8 led, int mode, u8 level)
{
	int counter;

	printk("\n****** Blinking Mode Test selected ******\n");
	printk("\nLed under test (%s)",
	       (led == LEDEN_LEDA) ? "LED A" : "LED B");

	if (twl4030ledRequest(led)) {
		printk("\nError: (%s) could not be acquired",
		       (led == LEDEN_LEDA) ? "LED A" : "LED B");
		return -1;
	}

	if (mode == BLINKING_EXTERNAL) {
		printk("\nActivating external sync control for %s",
		       (led == LEDEN_LEDA) ? "LED A" : "LED B");
		printk
		    ("\nIf GPIO is properly working then we should have LED on and toggling (10 times)");
		gpio_led_init();
		twl4030ledSetBlinkingMode(led, mode);
		twl4030ledSetState(led, LED_ON);

		for (counter = 0; counter < 10; counter++) {
			gpio_led_set(1);
			delay(500);
			gpio_led_set(0);
			delay(500);
		}

		twl4030ledSetBlinkingMode(led, BLINKING_OFF);
		gpio_led_exit();
	} else if (mode == BLINKING_INTERNAL) {
		printk("\nActivating internal blinking control for %s",
		       (led == LEDEN_LEDA) ? "LED A" : "LED B");
		twl4030ledSetBlinkingMode(led, mode);
		twl4030ledSetDutyCycle(led, level);

		/* 10 seconds */
		for (counter = 0; counter < 10; counter++) {
			delay(1000);
		}

		twl4030ledSetBlinkingMode(led, BLINKING_OFF);
	}

	twl4030ledRelease(led);
	printk("\nDone.\n");
	return 0;
}

module_init(twl4030_led_test_init);
module_exit(twl4030_led_test_exit);

MODULE_AUTHOR("Texas Instruments");
MODULE_DESCRIPTION("Simple LED driver test using twl4030");
MODULE_LICENSE("GPL");
