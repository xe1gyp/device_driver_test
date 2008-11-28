/*
 * omap2_mcspi_seclcdtest.c
 *
 * Test Secondary LCD using new McSPI driver adhering to LDM.
 *
 * Copyright (C) 2008 Texas Instruments, Inc.
 *
 * This file is licensed under the terms of the GNU General Public License
 * version 2. This program is licensed "as is" without any warranty of any
 * kind, whether express or implied.
 *
 * Author : Girish S G
 * Date	  : Jan 2008
 *
 * Initial code: Syed Mohammed Khasim
 */
	     

#define __NO_VERSION__  1

#include <linux/module.h> 

#include <linux/init.h>
#include <linux/kernel.h> 
#include <linux/sched.h>
#include <linux/errno.h>
#include <linux/delay.h>
#include <linux/types.h>
#include <asm/system.h>
#include <asm/irq.h>
#include <asm/hardware.h>
#include <linux/interrupt.h>
#include <asm/io.h>
#include <asm/arch/clock.h>
#include <linux/proc_fs.h>
#include <linux/version.h>
#include <asm/arch/hardware.h>
#include <linux/spi/spi.h>

#include "war.h"
static struct spi_device *sublcd_spi;

/*=================== MACROS=================================================*/

#define SET_COLUMN_ADDR                   0x15
#define SET_PAGE_ADDR                     0x75
#define SET_COM_OUTPUT_SCAN_DIR           0xBB
#define SET_DATA_OUTPUT_SCAN_DIR          0xBC
#define SET_COLOUR_LOOKUP_TABLE           0xCE
#define SET_DISPLAY_CONTROL               0xCA /*Select driver duty from 1/8 to 1/68*/
#define SET_AREA_SCROLL                   0xAA
#define SET_SCROLL_START                  0xAB
#define SET_POWER_CONTROL_REG             0x20
#define SET_CONTRAST_LVL_AND_RESI_RATIO   0x81
#define INCREMENT_CONTRAST_SET            0xD6
#define DECREMENT_CONTRAST_SET            0xD7
#define SET_NORMAL_DISPLAY                0xA6
#define SET_INVERSE_DISPLAY               0xA7
#define ENTER_PARTIAL_DISPLAY             0xA8
#define EXIT_PARTIAL_DISPLAY              0xA9
#define SET_DISPLAY_OFF                   0xAE
#define SET_DISPLAY_ON                    0xAF
#define ENTER_SLEEP_MODE                  0x95
#define EXIT_SLEEP_MODE                   0x94
#define ENABLE_INT_OSCILLATOR             0xD1
#define DISABLE_INT_OSCILLATOR            0xD2 /*NEED TO CHECK*/
#define SET_TEMP_COMPN_COEFFICIENT        0x82 
#define N0_OPERATION                      0x25
#define WRITE_DISPLAY_DATA                0x5C

/*GRAPHIC COMMANDS*/
#define DRAW_LINE                         0x83
#define FILL_ENABLE_DISABLE               0x92 /* send 1 EN, 0 for DIS as data*/
#define DRAW_RECTANGLE                    0x84
#define COPY                              0x8A
#define DIM_WINDOW                        0x8C
#define CLEAR_WINDOW                      0x8E

/*EXTENDED COMMANDS*/
#define SET_BIAS_RATIO                    0xFB
#define SET_FRAME_FRQ_NLINE_INVER         0xF2
#define SELECT_PWD_OR_FRC                 0xF7


/*FRAME FREQUENCY */
#define FRAME_FREQ_111MHZ     0xF
#define FRAME_FREQ_103_5MHZ   0xE
#define FRAME_FREQ_98MHZ      0xD
#define FRAME_FREQ_93MHZ      0xC
#define FRAME_FREQ_89MHZ      0xB
#define FRAME_FREQ_85MHZ      0xA
#define FRAME_FREQ_81_5MHZ    0x9
#define FRAME_FREQ_78MHZ      0x8
#define FRAME_FREQ_76_5MHZ    0x7
#define FRAME_FREQ_73_5MHZ    0x6
#define FRAME_FREQ_71MHZ      0x5
#define FRAME_FREQ_68MHZ      0x4
#define FRAME_FREQ_66_5MHZ    0x3
#define FRAME_FREQ_64_5MHZ    0x2
#define FRAME_FREQ_62_5MHZ    0x1
#define FRAME_FREQ_60_5MHZ    0x0

/*Booster Level For SPI LCD*/
#define BOOST_LEVEL_3X   0x0
#define BOOST_LEVEL_4X   0x4
#define BOOST_LEVEL_5X   0x8
#define BOOST_LEVEL_6X   0xC

/* COM output scan Direction*/
#define COM_SCAN_DIR1    0x0 /* direction is COM0->COM33 COM34->COM67*/
#define COM_SCAN_DIR2    0x1 /* direction is COM0->COM33 COM67<-COM34*/
#define COM_SCAN_DIR3    0x2 /* direction is COM33<-COM0 COM34->COM67*/
#define COM_SCAN_DIR4    0x3 /* direction is COM33<-COM0 COM67<-COM34*/

/* DATA output scan Direction*/
#define PAGE_ADDR_NORMAL       0x0 
#define PAGE_ADDR_INVERSE      0x1
#define COLUMN_ADDR_NORMAL     0x0 
#define COLUMN_ADDR_INVERSE    0x2
#define SCAN_DIR_TO_COLUMN     0x0
#define SCAN_DIR_TO_PAGE       0x4

#define COLUMN_SCAN_CN_PN    ( SCAN_DIR_TO_COLUMN | PAGE_ADDR_NORMAL | COLUMN_ADDR_NORMAL)
#define COLUMN_SCAN_CN_PI    ( SCAN_DIR_TO_COLUMN | PAGE_ADDR_INVERSE | COLUMN_ADDR_NORMAL) 
#define COLUMN_SCAN_CI_PN    ( SCAN_DIR_TO_COLUMN | PAGE_ADDR_NORMAL | COLUMN_ADDR_INVERSE)
#define COLUMN_SCAN_CI_PI    ( SCAN_DIR_TO_COLUMN | PAGE_ADDR_INVERSE | COLUMN_ADDR_INVERSE)
#define PAGE_SCAN_CN_PN      ( SCAN_DIR_TO_PAGE | PAGE_ADDR_NORMAL | COLUMN_ADDR_NORMAL)
#define PAGE_SCAN_CN_PI      ( SCAN_DIR_TO_PAGE | PAGE_ADDR_INVERSE | COLUMN_ADDR_NORMAL) 
#define PAGE_SCAN_CI_PN      ( SCAN_DIR_TO_PAGE | PAGE_ADDR_NORMAL | COLUMN_ADDR_INVERSE)
#define PAGE_SCAN_CI_PI      ( SCAN_DIR_TO_PAGE | PAGE_ADDR_INVERSE | COLUMN_ADDR_INVERSE)

/* DISPLAY MODES*/
#define DISPLAY_NORMAL       0x0 
#define DISPLAY_INVERSE      0x1

/* GRAY SCALE MODE*/
#define GRAY_SCALE_8_BIT_MODE       0x1
#define GRAY_SCALE_16_BIT_MODE      0x2

/*Contrast Levels*/
#define CONTRAST_LEVEL_DEFAULT      0x4f
#define CONTRAST_LEVEL1             0x1
#define CONTRAST_LEVEL2             0x2
#define CONTRAST_LEVEL3             0x4
#define CONTRAST_LEVEL4             0x8
#define CONTRAST_LEVEL5             0xC
#define CONTRAST_LEVEL6             0x10
#define CONTRAST_LEVEL7             0x20

#ifdef CONFIG_ARCH_OMAP34XX

	/*mux spi1 base address*/
	#define	 PADCONF_McSPI1_CLK          0x480021C8
	#define  PADCONF_McSPI1_SIMO         0x480021CA
	#define  PADCONF_McSPI1_SOMI         0x480021CC
	#define  PADCONF_McSPI1_CS0          0x480021CE
	#define  PADCONF_McSPI1_CS1          0x480021D0
	#define  PADCONF_McSPI1_CS2          0x480021D2
	#define  PADCONF_McSPI1_CS3          0x480021D4
	#define  PADCONF_etk_clk	     0x48002A28
	#define  PADCONF_etk_clk_ES2	     0x480025DE
	#define  PADCONF_sysboot5	     0x48002A14

#endif

static int
spi_lcd_cmd_send(unsigned char cmd)
{
	int t1[25];
	int ret=0;
	unsigned int cmd_word = 0;
	cmd_word = (0x000) | cmd;

	spi_write(sublcd_spi,&cmd_word, 2);
	
	
	return ret;
}


static int
spi_lcd_data_send(unsigned char data_tx)
{
	int ret;
	unsigned int data_word = 0;
	
	data_word = (0x100) | data_tx;

	spi_write(sublcd_spi, &data_word, 2);
	
	
	return ret;
}



static int
reset_seclcd(void)
{
	#ifdef CONFIG_ARCH_OMAP24XX
		omap_set_gpio_direction(153,0);
	
		omap_set_gpio_dataout(153,1);
		udelay(10);
		omap_set_gpio_dataout(153,0);
		udelay(10);
		omap_set_gpio_dataout(153,1);
		udelay(10);
	#endif

	#ifdef CONFIG_ARCH_OMAP34XX
		if (is_sil_rev_less_than(OMAP3430_REV_ES1_0)) {
			twl4030_set_gpio_direction(12,0);

	        	twl4030_set_gpio_dataout(12,1);
			udelay(10);
	        	twl4030_set_gpio_dataout(12,0);
	        	udelay(10);
	        	twl4030_set_gpio_dataout(12,1);
	        	udelay(10);
		} else {
			twl4030_request_gpio(15);
			twl4030_set_gpio_direction(15,0);

	        	twl4030_set_gpio_dataout(15,1);
			udelay(10);
	        	twl4030_set_gpio_dataout(15,0);
	        	udelay(10);
	        	twl4030_set_gpio_dataout(15,1);
	        	udelay(10);
		}
	#endif

	return 0;
}

int
sec_lcd_bklight_enable()
{
	
	#ifdef CONFIG_ARCH_OMAP24XX
		omap_set_gpio_direction(147,0);
		omap_set_gpio_dataout(147,1);
		udelay(10);
	#endif

	#ifdef CONFIG_ARCH_OMAP34XX
		twl4030_request_gpio(7);
       	        twl4030_set_gpio_direction(7,0);
                twl4030_set_gpio_dataout(7,1);
		udelay(10);	
	#endif	
}

int
initialize_sec_lcd()
{
	unsigned short int contrast_level = CONTRAST_LEVEL_DEFAULT;
	unsigned char frame_frq = FRAME_FREQ_78MHZ;
	unsigned char booster_level = BOOST_LEVEL_5X;        
	unsigned char com_scan_dir = COM_SCAN_DIR2;
	unsigned char data_scan_dir = COLUMN_SCAN_CN_PN;
	unsigned char display_mode = DISPLAY_NORMAL;
	unsigned char grayscale_mode = GRAY_SCALE_16_BIT_MODE;
	
	/*----------------enable interal oscillator-----------------*/
	spi_lcd_cmd_send(ENABLE_INT_OSCILLATOR);
	/*----------------set power control register----------------*/
	spi_lcd_cmd_send(SET_POWER_CONTROL_REG);
	/* enable the reference voltage genarator */
	/* internal regulator and voltage follower */
	booster_level |= 0x03; 
	spi_lcd_data_send(booster_level);//boost level//
	/*-----------set contrast level & interal regulator----------*/
	spi_lcd_cmd_send(SET_CONTRAST_LVL_AND_RESI_RATIO);
	spi_lcd_data_send(contrast_level);// contrast level
	spi_lcd_data_send(0x07); // resistor ratio for intrnal
	// regulator gain(1+r2/r1)=8.89
	/*----------------------exit sleep mode----------------------*/
	spi_lcd_cmd_send(EXIT_SLEEP_MODE);
	/*---------------------set biasing ratio---------------------*/
	spi_lcd_cmd_send(SET_BIAS_RATIO);
	spi_lcd_data_send(0x03);//bias ratio - 1/7
	/*---------------------select PWM/FRC------------------------*/
	spi_lcd_cmd_send(SELECT_PWD_OR_FRC);
	spi_lcd_data_send(0x28);//constant value
	spi_lcd_data_send(0x2E);/*2PWR + 2FRC*/
	spi_lcd_data_send(0x5); //constant value
	/*----------------set com output scan direction--------------*/
	spi_lcd_cmd_send(SET_COM_OUTPUT_SCAN_DIR);
	spi_lcd_data_send(com_scan_dir);//com_scan_dir
	/*----------------set data out scan direction----------------*/
	spi_lcd_cmd_send(SET_DATA_OUTPUT_SCAN_DIR);
	spi_lcd_data_send(data_scan_dir);//Normal or inverse,
	spi_lcd_data_send(0x00);//RGB MODE
	spi_lcd_data_send(0x02);//gray scale selection;grayscale_mode
	/*---------------------set display control------------------*/
	spi_lcd_cmd_send(SET_DISPLAY_CONTROL);
	spi_lcd_data_send(0x00);//dummy byte
	spi_lcd_data_send(0x0F);// display control
	spi_lcd_data_send(0x00);//dummy byte
	/*--------set frame frequency and n-line inversion----------*/
	spi_lcd_cmd_send(SET_FRAME_FRQ_NLINE_INVER);
	spi_lcd_data_send(frame_frq);//frame frq; 
	spi_lcd_data_send(0x06);// N-line inversion
	/*--------------------normal/inveDCe display----------------*/
	spi_lcd_cmd_send(SET_INVERSE_DISPLAY);//
	/*----------------------set display on----------------------*/
	spi_lcd_cmd_send(SET_DISPLAY_ON);
	return (0);
}

int
spi_lcd_fill_colour()
{   
	unsigned int i, j;
	unsigned char page_addr_start;
	unsigned char page_addr_end;
	unsigned char column_addr_start;
	unsigned char column_addr_end;
    
	unsigned char colour1, colour2, colour3;

	colour1 = 0xf0;
	colour2 = 0x0f;
	colour3 = 0x00;

	/*Complete Sub LCD Screen*/
	page_addr_start = 0;
	page_addr_end = 67;
	column_addr_start = 0 ;
	column_addr_end = 49;

	/*Set the page address*/
	spi_lcd_cmd_send(SET_PAGE_ADDR);
	spi_lcd_data_send(page_addr_start);
	spi_lcd_data_send(page_addr_end);
	
	/*Set the coloumn address*/
	spi_lcd_cmd_send(SET_COLUMN_ADDR);
	spi_lcd_data_send(column_addr_start);
	spi_lcd_data_send(column_addr_end);
	
	/*data write to LCD*/
	spi_lcd_cmd_send(WRITE_DISPLAY_DATA);
	/*Send the colour data to LCD*/
	for(i = page_addr_start; i< ((page_addr_start + page_addr_end)); i++)
	{  
		for(j = column_addr_start; j< (column_addr_start + column_addr_end); j++)
		{
			if(ar[i][j]==1){
				spi_lcd_data_send(0xff);
				spi_lcd_data_send(0xff);
				spi_lcd_data_send(0xff);
			}
			else if(ar[i][j]==0){
				spi_lcd_data_send(colour1);
				spi_lcd_data_send(colour2);
				spi_lcd_data_send(colour3);
			}
			else{
				spi_lcd_data_send(0x00);
				spi_lcd_data_send(0xF0);
				spi_lcd_data_send(0x0F);
			}
		}
	}
	return 0;
}
static int sublcd_probe(struct spi_device *spi)
{
	printk("in sublcd driver probe");

	sublcd_spi = spi;
	sublcd_spi->mode = SPI_MODE_0;
        sublcd_spi->bits_per_word = 9;
	spi_setup(sublcd_spi);


	reset_seclcd();

	initialize_sec_lcd();
	sec_lcd_bklight_enable();
	spi_lcd_fill_colour();

	return 0;
}
static int sublcd_remove(struct platform_device *pdev)
{
	return 0;
}

static int sublcd_suspend(struct platform_device *pdev, pm_message_t mesg)
{
	return 0;
}

static int sublcd_resume(struct platform_device *pdev)
{
	return 0;
}

static struct spi_driver sub_lcd_driver = {
	.probe		= sublcd_probe,
	.remove		= sublcd_remove,
	.suspend	= sublcd_suspend,
	.resume		= sublcd_resume,
	.driver		= {
		.name	= "sublcd",
		.bus = &spi_bus_type,
		.owner	= THIS_MODULE,
	},
};

static int __init test_mcspi_init(void)
{
	int ret;

	#ifdef CONFIG_ARCH_OMAP24XX

		/* MUXING */
		omap_writeb(0x03,0x49002119); /* SPI2_CS0 */
		omap_writeb(0x00,0x490020F2); /* GPIO 153 - F3*/
		omap_writeb(0x00,0x49002114); /* SPI1_NCS2 */
		omap_writeb(0x03,0x490020B6); /* GPIO 12 */
	
	#endif

	#ifdef CONFIG_ARCH_OMAP34XX

		/*Muxing for 3430*/
		omap_writew(0x0000,PADCONF_McSPI1_CLK);
		omap_writew(0x0000,PADCONF_McSPI1_CS0);		
		omap_writew(0x0000,PADCONF_McSPI1_CS1);		
		omap_writew(0x0000,PADCONF_McSPI1_CS2);		
		omap_writew(0x0000,PADCONF_McSPI1_CS3);		
		omap_writew(0x0000,PADCONF_McSPI1_SIMO);		
		omap_writew(0x0100,PADCONF_McSPI1_SOMI);		
		if (is_sil_rev_less_than(OMAP3430_REV_ES1_0))
			omap_writew(0x001C,PADCONF_etk_clk);/* SUB_LCD nRST - GPIO12-configured as output*/
		else
			omap_writew(0x001C,PADCONF_etk_clk_ES2);/* SUB_LCD nRST - GPIO15-configured as output*/
			
		omap_writew(0x0104,PADCONF_sysboot5);/*SUB_LCD GPIO_7- output */
	#endif

	if (is_sil_rev_less_than(OMAP3430_REV_ES1_0))
		twl4030_vaux3_ldo_use();
	else
		twl4030_vaux1_ldo_use();
		
	spi_register_driver(&sub_lcd_driver);
	
	return 0;
}
  


static void __exit test_mcspi_exit(void)
{
	spi_unregister_driver(&sub_lcd_driver);
	twl4030_free_gpio(7);
	twl4030_free_gpio(12);
	return;
}
  

module_init(test_mcspi_init);
module_exit(test_mcspi_exit);

MODULE_AUTHOR("Texas Instruments");
MODULE_DESCRIPTION("McSPI driver Library");
MODULE_LICENSE("GPL");
