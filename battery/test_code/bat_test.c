/*
 *  \linux\test\device_driver_test\battery\test_code\bat_test.c
 *
 * TWL4030 Battery Driver test module 1
 *
 * This Module is only meant for testing TWL4030 Battery Driver 
 * on TI OMAP24XX.
 *
 * Copyright (C) 2007 Texas Instruments, Inc.
 *
 * This package is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * THIS PACKAGE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/delay.h>
#include <linux/time.h>

#include <asm/types.h>

#include <asm/arch/twl4030.h>
#include <asm/arch/hdq.h>
#include <asm/arch/battery.h>

#include "bat_test.h"

int action = 0;

int status = 0;

static int read_madc_val (u8 mod_no, u8 reg_1)
{
	int ret, temp;
	u8 val;
	
	if ((ret = twl4030_i2c_read_u8(mod_no, &val, reg_1 + 1)))
	{
		printk (KERN_ERR "BATTERY DRIVER: Error trying to read %X  register on module %X \n", reg_1 + 1, mod_no);
		return ret;
	}
	
	temp = ((int) val) << 2;
	
	if ((ret = twl4030_i2c_read_u8(mod_no, &val, reg_1)))
	{
		printk (KERN_ERR "BATTERY DRIVER: Error trying to read %X  register on module %X \n", reg_1, mod_no);
		return ret;
	}

	return  temp + ((val & 0xC0) >> 6);

}

static void read_bq (void)
{
	printk(KERN_INFO "BATTERY DRIVER: BQ temperature %d  \n ", bqbattery_temperature());
	printk(KERN_INFO "BATTERY DRIVER: BQ voltage %d  \n ", bqbattery_voltage());
	printk(KERN_INFO "BATTERY DRIVER: BQ current %d  \n ", bqbattery_current());
	printk(KERN_INFO "BATTERY DRIVER: BQ RSOC %d  \n ", bqbattery_rsoc());
}

static void read_bci(void)
{
	
	printk (KERN_INFO " BATTERY DRIVER: BCI temperature battery %d \n ",
		twl4030battery_temperature());
		
	printk (KERN_INFO " BATTERY DRIVER: BCI voltage battery  %d \n ",
		 twl4030battery_voltage());
		
	printk (KERN_INFO " BATTERY DRIVER: BCI current battery  %d \n ",
		twl4030battery_current());	

	printk (KERN_INFO " BATTERY DRIVER: BCI voltage AC adapter %d \n ",
		twl4030charger_ac_voltage());
	
	printk (KERN_INFO " BATTERY DRIVER: BCI voltage USB adapter %d \n ",
		twl4030charger_usb_voltage());
	
}

static void backup_battery_voltage(void) 
{
	
	printk (" BATTERY DRIVER: voltage backup battery %d milivolts\n ",
		twl4030backupbatt_voltage());
				
}

static void main_battery_voltage(void)
{
	
	printk("BATTERY DRIVER: BQ RSOC %d  \n ", bqbattery_rsoc());
	
	printk("BATTERY DRIVER: BQ voltage %d milivolts  \n ", bqbattery_voltage());
	
	printk ("BATTERY DRIVER: BCI main battery voltage %d milivolts \n ",
		 twl4030battery_voltage());
		
	printk ("BATTERY DRIVER: MADC main battery voltage %d milivolts\n ",
		read_madc_val(TWL4030_MODULE_MADC, REG_BCICH4));
}

static void charger_status(void)
{

	int ret;
		
	ret = twl4030bci_status();
	
	switch (ret)
	{
		case CHG_STS_NONE:
			printk("BATTERY DRIVER: charger status No charging device \n");
			return;
		break;

		case CHG_STS_OFF:
			printk("BATTERY DRIVER: charger status OFF mode \n");
			return;
		break;

		case CHG_STS_STBY:
			printk("BATTERY DRIVER: charger status standby mode \n");
			return;
		break;

		case CHG_STS_OPN:
			printk("BATTERY DRIVER: charger status open battery mode \n");
			return;
		break; 
	}

	if(ret & CHG_STS_USB)
	{
		printk("BATTERY DRIVER: charger status USB charge mode \n");
	}
	
	if(ret & CHG_STS_AC)
	{
		printk("BATTERY DRIVER: charger status AC charge mode \n");
	}
	
	ret &=  ~(CHG_STS_USB | CHG_STS_AC);
	
	switch (ret)
	{
		case CHG_STS_CONS:
			printk("BATTERY DRIVER: charger status constant voltage mode \n");
			return;
		break;
		
		case CHG_STS_OVVT:
			printk("BATTERY DRIVER: charger status adapter overvoltage  \n");
			return;
		break;		
	}
		
	if(ret & CHG_STS_STD)
	{
		ret &= ~CHG_STS_STD;
		
		if (ret< 3)
		{
			printk("BATTERY DRIVER: charger status standard charge state stop %d \n", ret + 1);
		}
		else
		{
			printk("BATTERY DRIVER: charger status standard charge state charge %d \n", ret - 2);
		}
	}
	else
	{
		ret &= ~CHG_STS_STD;
		
		printk("BATTERY DRIVER: charger status quick charge state %d \n", ret - 1);
	}
}

static int test_init(void)
{
			
	switch (action)
	{
		case 1: /* Enabling/Disabling AC charge*/
			if (status)
			{
				printk("**** Enabling AC charge  ****\n");
				twl4030charger_ac_en(status);
			}
			else
			{
				printk("**** Disabling AC charge  ****\n");
				twl4030charger_ac_en(status);
			}
			twl4030charger_ac_en(status);
			break;
			
		case 2:/* Enabling/Disabling USB charge*/
			if (status)
			{
				printk("**** Enabling USB charge  ****\n");
				twl4030charger_usb_en(status);
			}
			else
			{
				printk("**** Disabling USB charge  ****\n");
				twl4030charger_usb_en(status);
			}			
			break;
		case 3: /* read backup battery power level */
			printk("**** Backup battery power level  ****\n");
			backup_battery_voltage();
		break;

		case 4: /* read main battery power level */
			printk("**** Main battery power level  ****\n");
			main_battery_voltage();
		break;
				
		default: /* read all values. */
			read_bq();
			read_bci();
			charger_status();						
	}
	return -ENODEV;	
}

static void test_exit(void)
{	
}

module_param(action, int, S_IRUSR);
module_param(status, int, S_IRUSR);

module_init(test_init);
module_exit(test_exit);

MODULE_LICENSE("GPL");
