/*
 * linux/drivers/i2c/chips/twl4030_core.c
 *
 * TWL4030 Sound Services
 *
 * This driver is only meant for testing TWL4030 Sound Services 
 * on TI OMAP24XX.
 *
 * Copyright (C) 2004-2005 Texas Instruments, Inc.
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

#include <asm/arch/twl4030.h>


MODULE_LICENSE("GPL");


int duty_cycle = -1;

int turn_on = 0; 

int get_status = 0;

/* Vibrator Duty cycle resquest function */
void vib_status(void)
{
  int res;

  res = twl4030_sndsrv_status();
  if (res == 0){
    printk ( KERN_INFO "Sound Services: vibrator status is OFF \n");
  }
  else if (res == 1){
    printk ( KERN_INFO "Sound Services: vibrator status is ON \n");
  }
  else {
    printk ( KERN_ERR "Sound Services: vibrator status read error !!!  \n");
  }
     
}

void vib_dutycyl(void)
{

  int res;  
  res = twl4030_sndsrv_get_dtcyl();
  
  switch (res){
    case VIB_DUTY_CYCLE_100:
      printk ( KERN_INFO "Sound Services: vibrator's PWM generator duty cycle is 100%% \n");
      break;

    case VIB_DUTY_CYCLE_75:
      printk ( KERN_INFO "Sound Services: vibrator's PWM generator duty cycle is 75%% \n");
      break;

    case VIB_DUTY_CYCLE_50:
      printk ( KERN_INFO "Sound Services: vibrator's PWM generator duty cycle is 50%% \n");
      break;

    case VIB_DUTY_CYCLE_25:
      printk ( KERN_INFO "Sound Services: vibrator's PWM generator duty cycle is 25%% \n");
      break;

    default:
      printk ( KERN_ERR "Sound Services: vibrator duty cycle read error !!! \n");
  }

}

int vib_set_dutycyl(void)
{
  switch (duty_cycle){
    case VIB_DUTY_CYCLE_100:
      twl4030_sndsrv_set_dtcyl(VIB_DUTY_CYCLE_100);
      printk ( KERN_INFO "\nSound Services: Setting vibrator's PWM generator to 100%% \n");
      return 0;
      break;

    case VIB_DUTY_CYCLE_75:
      twl4030_sndsrv_set_dtcyl(VIB_DUTY_CYCLE_75);
      printk ( KERN_INFO "\nSound Services: Setting vibrator's PWM generator to 75%% \n");
      return 0;
      break;

    case VIB_DUTY_CYCLE_50:
      twl4030_sndsrv_set_dtcyl(VIB_DUTY_CYCLE_50);
      printk ( KERN_INFO "\nSound Services: Setting vibrator's PWM generator to 50%% \n");
      return 0;
      break;

    case VIB_DUTY_CYCLE_25:
      twl4030_sndsrv_set_dtcyl(VIB_DUTY_CYCLE_25);
      printk ( KERN_INFO "\nSound Services: Setting vibrator's PWM generator to 25%% \n");
      return 0;
      break;

    default:
      printk ( KERN_ERR "Sound Services: vibrator duty cycle inadequate value !!! \n");
      return -EPERM;

  }
 
}

static int test_init(void)

{
  
  int ret = 0;

  if (turn_on){
    printk ( KERN_INFO "\nSound Services: Turning Sound services vibrator ON \n");
    if (twl4030_sndsrv_on() != 0){
      printk ( KERN_INFO "\nSound Services: Fail to turn ON the vibrator\n");
      return -EPERM;
    }
  }

  if (get_status){
    vib_status();
  }

  if (duty_cycle >= 0){
    ret = vib_set_dutycyl();
    vib_dutycyl();
  }

  return ret;

}



static void test_exit(void)
{

  twl4030_sndsrv_off();
  printk ( KERN_INFO "\nSound Services: Turning Sound services vibrator OFF \n");

  if (get_status){
    vib_status();
  }

  if (duty_cycle >= 0){
    vib_dutycyl();
  }

}


module_param(duty_cycle, int, S_IRUSR);
module_param(turn_on, int, S_IRUSR);
module_param(get_status, int, S_IRUSR);

module_init(test_init);
module_exit(test_exit);
