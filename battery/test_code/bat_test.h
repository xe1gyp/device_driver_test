/*
 *  \linux\test\device_driver_test\battery\test_code\bat_test.h
 *
 * TWL4030 Battery Driver test module 1 header file
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
 
#ifndef BAT_TEST_H_
#define BAT_TEST_H_

#define REG_BCICH0				0x057 /* BTEMP */
#define REG_BCICH1				0x059 /* ASBVBUS*/
#define REG_BCICH2				0x05B /* ICHG*/
#define REG_BCICH3				0x05D /* VCHG*/
#define REG_BCICH4				0x05F /* VBAT */
#define REG_GPCH9				0x049 /* VBKP*/


/* Activating USB charger Registers*/
#define REG_BOOT_BCI			0x007

/* Activating USB charger Values*/
#define BCIAUTOWEN				0x020
#define CONFIG_DONE			0x010
#define CVENAC					0x004
#define BCIAUTOUSB				0x002
#define BCIAUTOAC				0x001

#define REG_POWER_CTRL			0x0AC /* TWL4030_MODULE_USB */
#define REG_POWER_CTRL_SET 	0x0AD
#define REG_POWER_CTRL_CLR		0x0AE

#define REG_PHY_CLK_CTRL		0x0FE
#define REG_PHY_CLK_CTRL_STS 	0x0FF

#define OTG_EN					0x020

#define REG_BCIMFSTS4			0x010

#define HTSAVESTS				0x001
#define USBSLOWMCHG			0x002
#define USBFASTMCHG			0x004
#define SUSPENDM				0x010
#define SYSACTIV					0x020
	
#define CLK32K_EN	 			0x02
#define CLOCKGATING_EN			0x04
	
#define PHY_DPLL_CLK				0x01

#endif /*BAT_TEST_H_*/
