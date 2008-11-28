/*
 * omap2_mcspi_irqtest.c
 *
 * Test DMA based data transfer for OMAP2 McSPI driver
 *
 * Copyright (C) 2006 Texas Instruments, Inc.
 *
 * This file is licensed under the terms of the GNU General Public License
 * version 2. This program is licensed "as is" without any warranty of any
 * kind, whether express or implied.
 *
 * Author : Syed Mohammed Khasim
 * Date   : 12 June 2006
 */



#define __NO_VERSION__  1
//#include <linux/version.h>

//#ifdef  MODULE
#include <linux/module.h> 
//#endif

#include <linux/config.h>

#include <linux/init.h>
#include <linux/kernel.h> 
#include <linux/sched.h>
#include <linux/errno.h>
#include <linux/delay.h>
#include <linux/types.h>
#include <linux/interrupt.h>
#include <linux/proc_fs.h>
#include <linux/version.h>

#include <asm/system.h>
#include <asm/irq.h>
#include <asm/hardware.h>
#include <asm/io.h>
#include <asm/arch/clock.h>
#include <asm/arch/hardware.h>
#include <asm/arch/dma.h>
#include <asm/arch/omap2_mcspi.h>

#ifdef CONFIG_ARCH_OMAP24XX
/* SPI 2 SLAVE */
#define spi2_clk	0x49002116 /* spi2_clk  mode 0*/
#define spi2_simo	0x49002117 /* spi2_simo mode 0*/
#define spi2_somi	0x49002118 /* spi2_somi mode 0*/
#define spi2_cs0	0x49002119 /* spi2_cs0  mode 0*/
#define spi2_cs1	0x49002121 /* spi2_cs1  mode 4*/

/* SPI 3 Master */
#define spi3_clk	0x490020E8 /* spi3_clk  mode 1*/
#define spi3_cs0	0x490020E9 /* spi3_cs0  mode 1*/
#define spi3_simo	0x490020EA /* spi3_simo mode 1*/
#define spi3_somi	0x490020EB /* spi3_somi mode 1*/
#define spi3_cs1	0x490020EC /* spi3_cs1  mode 1*/
#endif

#ifdef CONFIG_ARCH_OMAP34XX
        /* SPI 2 SLAVE */
        #define spi2_clk        0x480021D6 /* spi2_clk  mode 0*/
        #define spi2_simo       0x480021D8 /* spi2_simo mode 0*/
        #define spi2_somi       0x480021DA /* spi2_somi mode 0*/
        #define spi2_cs0        0x480021DC /* spi2_cs0  mode 0*/
        #define spi2_cs1        0x480021DE /* spi2_cs1  mode 4*/

        /* SPI 3 Master */
        #define spi3_clk        0x48002A32 /* spi3_clk  mode 1*/
        #define spi3_cs0        0x48002A38 /* spi3_cs0  mode 1*/
        #define spi3_simo       0x48002A34 /* spi3_simo mode 1*/
        #define spi3_somi       0x48002A36 /* spi3_somi mode 1*/
        #define spi3_cs1        0x48002A3A /* spi3_cs1  mode 1*/
#endif

#define INPUT_CLK	48000000

static unsigned int buffer_size		= 256;
static unsigned int clk_div			= OMAP2_McSPI_CHCFG_CLKD_2;
static unsigned int clk_phase		= 0;
static unsigned int clk_polarity	= 0;
static unsigned int cs_polarity		= 0;
static unsigned int word_length		= OMAP2_McSPI_CHCFG_WL32;
static unsigned int transmit_mode	= OMAP2_McSPI_CHCFG_TRANSRECEIVE;
unsigned int rxdata =0;
static struct omap2_mcspi_dev spi_dev2;
static struct omap2_mcspi_dev spi_dev3;

static void omap2_spi2_irq_handler(void *arg, unsigned int irq_status,struct pt_regs *regs)
{
	static int irq_count =0;
	int ret;
	printk("In SPI2 Irq Handler status %8x\n",irq_status);
	irq_count++;
	
	if(irq_count == 10){
		/* Disable the McSPI interface */
		if ( (ret = (omap2_mcspi_disable_channel(&spi_dev2))) ) {
			printk(KERN_ERR "could not disable spi2 channel: ret: %d\n", ret);
		}
		omap2_mcspi_irqmask_cfg(&spi_dev2,0);	
		omap2_mcspi_irqmask_cfg(&spi_dev3,0);	
		return;
	}
	
	omap2_mcspi_raw_readfromchannel(&spi_dev2,&rxdata);
	if ( (ret = (omap2_mcspi_enable_channel(&spi_dev2))) ) {
			printk(KERN_ERR "could not disable spi2 channel: ret: %d\n", ret);
	}
	
}

static void omap2_spi3_irq_handler(void *arg, unsigned int irq_status,struct pt_regs *regs)
{
	static int irq_count;
	int ret;
	
	printk("In SPI3 Irq Handler status %8x \n",irq_status);
	irq_count++;
	
	if(irq_count == 10){
		/* Disable the McSPI interface */
		if ( (ret = (omap2_mcspi_disable_channel(&spi_dev3))) ) {
			printk(KERN_ERR "could not disable spi2 channel: ret: %d\n", ret);
		}	
		omap2_mcspi_irqmask_cfg(&spi_dev3,0);
		return;
	}
	
	if ( (ret = (omap2_mcspi_enable_channel(&spi_dev3))) ) {
			printk(KERN_ERR "could not disable spi3 channel: ret: %d\n", ret);
	}
	omap2_mcspi_raw_writetochannel(&spi_dev3,0xAAAAAAAA);
}

int 
__init test_mcspi_irq_init(void)
{
	int ret;
	unsigned int cphase;
	unsigned int cpol;
	unsigned int epol;
	unsigned int wlength;
	
	printk("\n Entered into McSPI Init \n");
	printk("\n This test requires CH0 of McSPI 2 and 3 cross connected on SDP \n");
	printk("\n SPI 3 CH0 is configured as Master\n");
	printk("\n SPI 2 CH0 is configured as Slave\n");

	cphase = (clk_phase == 0)?OMAP2_McSPI_CHCFG_PHA_ODD:OMAP2_McSPI_CHCFG_PHA_EVEN;
	cpol = (clk_polarity ==0)?OMAP2_McSPI_CHCFG_POL_HIGH:OMAP2_McSPI_CHCFG_POL_LOW;
	epol = (cs_polarity == 0)?OMAP2_McSPI_CHCFG_EPOL_HIGH:OMAP2_McSPI_CHCFG_EPOL_LOW;
	wlength = word_length - 1;	
	
	#ifdef CONFIG_ARCH_OMAP24XX
		/* SPI 2 is slave */
		omap_writeb(0x18,spi2_clk);
		omap_writeb(0x18,spi2_simo);
		omap_writeb(0x10,spi2_somi);
		omap_writeb(0x18,spi2_cs0);
		omap_writeb(0x10,spi2_cs1);

		/* SPI 3 is master */
		omap_writeb(0x11,spi3_clk);
		omap_writeb(0x11,spi3_cs0);
		omap_writeb(0x11,spi3_simo);
		omap_writeb(0x19,spi3_somi);
		omap_writeb(0x11,spi3_cs1);
	#endif

	 #ifdef CONFIG_ARCH_OMAP34XX

                omap_writew(0x0118,spi2_clk);
                omap_writew(0x0118,spi2_simo);
                omap_writew(0x0010,spi2_somi);
                omap_writew(0x0118,spi2_cs0);

                /* SPI 3 is master */
                omap_writew(0x0002,spi3_clk);
                omap_writew(0x0002,spi3_simo);
                omap_writew(0x011A,spi3_somi);
                omap_writew(0x0002,spi3_cs0);

        #endif

	
	printk ("\n Mux config done \n");
	
	/* Request the SPI channels */
	memset(&spi_dev2, 0, sizeof(spi_dev2));
	memset(&spi_dev3, 0, sizeof(spi_dev3));

	/* Configure SPI2 */
	omap2_mcspi_set_spi_bus(spi_dev2,OMAP2_McSPI_BUS1);
	omap2_mcspi_set_spi_chan(spi_dev2,OMAP2_McSPI_CH0);
	
	/* Put SPI2 in Slave mode */
	omap2_mcspi_set_master(spi_dev2,OMAP2_McSPI_BUS_SLAVE );
	
	omap2_mcspi_set_phase(spi_dev2,cphase);
	omap2_mcspi_set_polarity(spi_dev2,OMAP2_McSPI_CHCFG_POL_LOW);
	omap2_mcspi_set_clkdivisor(spi_dev2,clk_div);
	omap2_mcspi_set_epol(spi_dev2,epol);
	omap2_mcspi_set_wordlen(spi_dev2,wlength);
	
	omap2_mcspi_set_transmode(spi_dev2,OMAP2_McSPI_CHCFG_RECEIVEONLY);
	omap2_mcspi_set_dpe0(spi_dev2,OMAP2_McSPI_CHCFG_DPE0_TRANSMIT );
	omap2_mcspi_set_dpe1(spi_dev2,OMAP2_McSPI_CHCFG_DPE1_NO_TRANSMIT );
	omap2_mcspi_set_is(spi_dev2,OMAP2_McSPI_CHCFG_IS_DL1RECEIVE);

	/* Configure SPI3 */
	
	omap2_mcspi_set_spi_bus(spi_dev3,OMAP2_McSPI_BUS2);
	omap2_mcspi_set_spi_chan(spi_dev3,OMAP2_McSPI_CH0);
	
	/* Put SPI3 in Master mode */
	omap2_mcspi_set_master(spi_dev3,OMAP2_McSPI_BUS_MASTER);
	
	omap2_mcspi_set_phase(spi_dev3,cphase);
	omap2_mcspi_set_polarity(spi_dev3,cpol);
	omap2_mcspi_set_clkdivisor(spi_dev3,clk_div);
	omap2_mcspi_set_epol(spi_dev3,epol);
	omap2_mcspi_set_wordlen(spi_dev3,wlength);
	
	omap2_mcspi_set_transmode(spi_dev3,OMAP2_McSPI_CHCFG_TRANSONLY);
	omap2_mcspi_set_dpe0(spi_dev3,OMAP2_McSPI_CHCFG_DPE0_NO_TRANSMIT);
	omap2_mcspi_set_dpe1(spi_dev3,OMAP2_McSPI_CHCFG_DPE1_TRANSMIT);
	omap2_mcspi_set_is(spi_dev3,OMAP2_McSPI_CHCFG_IS_DL0RECEIVE);
	
	/*Request SPI2 */
	if ( (ret = (omap2_mcspi_request_interface(&spi_dev2))) ) {
		printk(KERN_ERR "could not alloc spi %d:%d\n", spi_dev2.spi_bus, spi_dev2.spi_chan);
		return -1;
	}
	
	if(omap2_mcspi_get(&spi_dev2)!=0 ) {
		printk(KERN_ERR "unable to get the McSPI2 interface \n");
		return -1;
	}
	if ( (ret = (omap2_mcspi_config_channel(&spi_dev2))) ) {
		printk(KERN_ERR "could not config spi2 channel\n");
		return -1;
	}

	/* Request SPI3 */
	if ( (ret = (omap2_mcspi_request_interface(&spi_dev3))) ) {
		printk(KERN_ERR "could not alloc spi %d:%d\n", spi_dev2.spi_bus, spi_dev2.spi_chan);
		return -1;
	}
	
	if(omap2_mcspi_get(&spi_dev3)!=0 ) {
		printk(KERN_ERR "unable to get the McSPI2 interface \n");
		return -1;
	}
	if ( (ret = (omap2_mcspi_config_channel(&spi_dev3))) ) {
		printk(KERN_ERR "could not config spi2 channel\n");
		return -1;
	}
	/* Disable the McSPI interface */
	if ( (ret = (omap2_mcspi_disable_channel(&spi_dev2))) ) {
		printk(KERN_ERR "could not enable spi2 channel: ret: %d\n", ret);
		return -1;
	}
	
	if ( (ret = (omap2_mcspi_disable_channel(&spi_dev3))) ) {
		printk(KERN_ERR "could not enable spi3 channel: ret: %d\n", ret);
		return -1; 
	}
	
	omap2_mcspi_register_isr(&spi_dev2,omap2_spi2_irq_handler,0,
				(OMAP2_McSPI_IRQ_RX_FULL));
			
	omap2_mcspi_register_isr(&spi_dev3,omap2_spi3_irq_handler,0,
				(OMAP2_McSPI_IRQ_TX_EMPTY));
				

	if ( (ret = (omap2_mcspi_enable_channel(&spi_dev2))) ) {
		printk(KERN_ERR "could not enable spi2 channel: ret: %d\n", ret);
		return -1;
	}
	
	if ( (ret = (omap2_mcspi_enable_channel(&spi_dev3))) ) {
		printk(KERN_ERR "could not enable spi3 channel: ret: %d\n", ret);
		return -1; 
	}
				
	return 0;
}

static void __exit test_mcspi_irq_exit(void)
{
	omap2_mcspi_unregister_isr(&spi_dev2);
	omap2_mcspi_unregister_isr(&spi_dev3);

	omap2_mcspi_put(&spi_dev2);
	omap2_mcspi_put(&spi_dev3);
	
	omap2_mcspi_release_interface(&spi_dev2);
	omap2_mcspi_release_interface(&spi_dev3);
	return;
}

module_init(test_mcspi_irq_init);
module_exit(test_mcspi_irq_exit);

MODULE_AUTHOR("Texas Instruments");
MODULE_DESCRIPTION("McSPI driver Library");
MODULE_LICENSE("GPL");
