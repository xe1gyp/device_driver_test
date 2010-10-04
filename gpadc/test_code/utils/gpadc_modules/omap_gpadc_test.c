/*
 * omap4_gpadc_test.c
 *
 * Test Driver for OMAP4 GPADC driver
 *
 * Copyright (C) 2010 Texas Instruments, Inc.
 *
 * This file is licensed under the terms of the GNU General Public License
 * version 2. This program is licensed "as is" without any warranty of any
 * kind, whether express or implied.
 *
 * Author : Piyush Kumar Garg
 * Date   : 16 June 2010
 */

#include <linux/module.h>
#include <linux/types.h>
#include <linux/init.h>
#include <linux/errno.h>
#include <linux/i2c/twl6030-gpadc.h>

#define TWL6030_GPADC_WAIT 0
#define TWL6030_GPADC_SW2 1

extern int twl6030_gpadc_conversion(struct twl6030_gpadc_request *);

static int channels = 131071; /* Read All 17 Channels */
module_param(channels, int, 0);

static
int __init omap_gpadc_init(void)
{
        struct twl6030_gpadc_request req;
        int index,ret;
 
        req.channels=channels;
        req.do_avg = 0;
        req.method = TWL6030_GPADC_SW2;
        req.type = TWL6030_GPADC_WAIT;
        req.active = 0;
        req.result_pending = 0;
        req.func_cb = NULL;
        
        ret = twl6030_gpadc_conversion(&req);

        if(ret>0)
        {
           for(index=0;index<TWL6030_GPADC_MAX_CHANNELS;index++)
           {
               if(channels & (1 <<index))
                    printk("\n \t Channel No = %d \t Value = %d",index,req.rbuf[index]);
           }
	   printk("\n OMAP GPADC TEST driver installed successfully \n");
       	   return 0;
        }
        else
        {
	   printk("\n FAIL, return value = %d \n",ret);
           return 1; 
        }
}

static
void __exit omap_gpadc_exit(void)
{
	printk("\n OMAP GPADC TEST driver un-installed successfully \n");
	return;
}

module_init(omap_gpadc_init);
module_exit(omap_gpadc_exit);

MODULE_AUTHOR("Texas Instruments");
MODULE_DESCRIPTION("GPADC Test Driver");
MODULE_LICENSE("GPL");
