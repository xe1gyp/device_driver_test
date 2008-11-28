/*
 * 
 * \linux\test\device_driver_test\battery\test_code\bat_test_evt.c
 * 
 * TWL4030 Battery Driver test module 2
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

int option = 0;

static int battery_level(int batt_level_event)
{
	switch (batt_level_event)
	{
		case EVT_LVL_1:
			printk("BATTERY TEST: main battery voltage threshold 1 reached.\n");
			break;
		case EVT_LVL_2:
			printk("BATTERY TEST: main battery voltage threshold 2 reached.\n");
			break;
		case EVT_LVL_3:
			printk("BATTERY TEST: main battery voltage threshold 3 reached.\n");
			break;
		case EVT_LVL_4:
			printk("BATTERY TEST: main battery voltage threshold 4 reached.\n");
			break;
		default:			
			printk("BATTERY TEST: main battery voltage threshold !!!!   error !!!!.\n");		
	}
		
	return 0;
}
	
static int battery_presence(int batt_pres_event)
{
	if (batt_pres_event & EVT_BATTSTS_CONN)
	{
		printk("BATTERY TEST: main battery inserted.\n");
	}
	else if(batt_pres_event & EVT_BATTSTS_DISC)
	{
		printk("BATTERY TEST: main battery removed.\n");
	}
	else
	{
		printk("BATTERY TEST: main battery presence !!!!   error !!!!.\n");
	}
	
	return 0;
}

static int charger_presence(int chgr_pres_event)
{
	if (chgr_pres_event & EVT_PWSPLY_AC_CONN)
	{
		printk("BATTERY TEST: AC charger connected.\n");
	}
	else if (chgr_pres_event & EVT_PWSPLY_AC_DISC)
	{
		printk("BATTERY TEST: AC charger disconnected.\n");
	}
	else
	{
		printk("BATTERY TEST: charger presence !!!! error !!!!.\n");
	}
	
	if (chgr_pres_event & EVT_PWSPLY_USB_CONN)
	{
		printk("BATTERY TEST: USB charger connected.\n");
	}
	else if(chgr_pres_event & EVT_PWSPLY_USB_DISC)
	{
		printk("BATTERY TEST: USB charger disconnected.\n");
	}
	else
	{
		printk("BATTERY TEST: charger presence !!!! error !!!!.\n");
	}	
		
	return 0;
}

static int bci_status(int bci_sts_event)
{	
	switch (bci_sts_event)
	{
		case CHG_STS_NONE:
			printk("BATTERY DRIVER: charger status No charging device \n");
			return 0;
		break;

		case CHG_STS_OFF:
			printk("BATTERY DRIVER: charger status OFF mode \n");
			return 0;
		break;

		case CHG_STS_STBY:
			printk("BATTERY DRIVER: charger status standby mode \n");
			return 0;
		break;

		case CHG_STS_OPN:
			printk("BATTERY DRIVER: charger status open battery mode \n");
			return 0;
		break; 
	}

	if(bci_sts_event & CHG_STS_USB)
	{
		printk("BATTERY DRIVER: charger status USB charge mode \n");
	}
	
	if(bci_sts_event & CHG_STS_AC)
	{
		printk("BATTERY DRIVER: charger status AC charge mode \n");
	}
	
	bci_sts_event &=  ~(CHG_STS_USB | CHG_STS_AC);
	
	switch (bci_sts_event)
	{
		case CHG_STS_CONS:
			printk("BATTERY DRIVER: charger status constant voltage mode \n");
			return 0;
		break;
		
		case CHG_STS_OVVT:
			printk("BATTERY DRIVER: charger status adapter overvoltage  \n");
			return 0;
		break;		
	}
		
	if(bci_sts_event & CHG_STS_STD)
	{
		bci_sts_event &= ~CHG_STS_STD;
		
		if (bci_sts_event < 3)
		{
			printk("BATTERY DRIVER: charger status standard charge state stop %d \n",
				bci_sts_event + 1);
		}
		else
		{
			printk("BATTERY DRIVER: charger status standard charge state charge %d \n",
				bci_sts_event - 2);
		}
	}
	else
	{
		bci_sts_event &= ~CHG_STS_STD;
		
		printk("BATTERY DRIVER: charger status quick charge state %d \n",
			bci_sts_event - 1);
	}
	
	return 0;
}



struct twl4030battery_events valid = 
	{
		.battery_level = battery_level,
		.battery_presence= battery_presence,
		.charger_presence= charger_presence,
		.bci_status= bci_status,
		.battery_sw_level_event_cfg = (EVT_LVL_4_EN | EVT_LVL_3_EN |
			EVT_LVL_2_EN | EVT_LVL_1_EN),		
	},
	invalid = 
	{
		.battery_level = NULL,
		.battery_presence= NULL,
		.charger_presence= NULL,
		.bci_status= NULL,
		.battery_sw_level_event_cfg = 0,		
	};

struct bqbattery_events bq_valid = 
	{
		.battery_level = battery_level,
		//.battery_level = NULL,
		.battery_presence= battery_presence,
		//.battery_presence = NULL,			
		.battery_sw_level_event_cfg = (EVT_LVL_4_EN | EVT_LVL_3_EN |
			EVT_LVL_2_EN | EVT_LVL_1_EN),
		.battery_level_1 = 2894,
		.battery_level_2 = 3451,
		.battery_level_3 = 3902,
		.battery_level_4 = 3949,
	},
	bq_invalid = 
	{
		.battery_level = NULL,
		.battery_presence= NULL,
		.battery_sw_level_event_cfg = 0,
	};


static int test_init(void)
{	
	if (option)
	{
		return bqbattery_event_register(bq_valid);
	}
	else
	{
		return twl4030battery_event_register(valid);
	}
}

static void test_exit(void)
{
	if (option)
	{
		bqbattery_event_register(bq_invalid);
	}
	else
	{
		twl4030battery_event_register(invalid);
	}
		
}

module_param(option, bool, S_IRUSR);

module_init(test_init);
module_exit(test_exit);

MODULE_LICENSE("GPL");
