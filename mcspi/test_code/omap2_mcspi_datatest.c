/*
 * omap2_mcspi_datatest.c
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
#include <asm/system.h>
#include <asm/irq.h>
#include <asm/hardware.h>
#include <asm/io.h>
#include <asm/arch/clock.h>
#include <asm/arch/hardware.h>
#include <asm/arch/dma.h>
#include <linux/version.h>

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

static int configure_dma(void);

int spi_data = 1;
int spi2_rxdma =0;
int spi3_txdma =0;
int spi2_txdma =0;
int spi3_rxdma =0;

dma_addr_t spi2_rx_buf_dma_phys = 0;
unsigned int spi2_rx_buf_dma_virt;
dma_addr_t spi3_tx_buf_dma_phys = 0;
unsigned int spi3_tx_buf_dma_virt;

dma_addr_t spi2_tx_buf_dma_phys;
unsigned int spi2_tx_buf_dma_virt;
dma_addr_t spi3_rx_buf_dma_phys;
unsigned int spi3_rx_buf_dma_virt;

static unsigned int buffer_size		= 256;
static unsigned int clk_div			= OMAP2_McSPI_CHCFG_CLKD_2;
static unsigned int clk_phase		= 0;
static unsigned int clk_polarity	= 0;
static unsigned int cs_polarity		= 0;
static unsigned int word_length		= OMAP2_McSPI_CHCFG_WL16;
static unsigned int transmit_mode	= OMAP2_McSPI_CHCFG_TRANSRECEIVE;

static unsigned int buffers_allocated = 0;
struct timeval tx_start_time, tx_end_time;

/* Proc interface declaration */
static struct proc_dir_entry  *mcspi_test_dir,*transmission_file,*status_file;
static int file_type[2]={0,1};


long int tx_sec=0,tx_usec=0;

#if (LINUX_VERSION_CODE <= KERNEL_VERSION(2,6,00))

	MODULE_PARM( buffer_size, "i");
	MODULE_PARM( clk_div, "i");
	MODULE_PARM( clk_phase, "i");
	MODULE_PARM( clk_polarity, "i");
	MODULE_PARM( cs_polarity, "i");
	MODULE_PARM( word_length, "i");
#else
	module_param( buffer_size, int, 0);
	module_param( clk_div, int, 0);
	module_param( clk_phase, int, 0);
	module_param( clk_polarity, int, 0);
	module_param( cs_polarity, int, 0);
	module_param( word_length, int, 0);
#endif

static struct omap2_mcspi_dev spi_dev2;
static struct omap2_mcspi_dev spi_dev3;

static void
omap_spi2_rx_dma_callback (void *data,u32 ch_status)
{
	char *ptr_r2;
	int i;//3430 test remove
	ptr_r2 = (char *) spi2_rx_buf_dma_virt;
	printk("\n Entered in SPI2 Rx DMA callback %8x\n", ch_status);
}

static void
omap_spi3_tx_dma_callback (void *data,u32 ch_status)
{
	printk("\n Entered in SPI3 Tx DMA callback %8x\n", ch_status);
}
static void
omap_spi3_rx_dma_callback (void *data,u32 ch_status)
{
	char *ptr_r3;
	ptr_r3 = (char *)spi3_rx_buf_dma_virt;
	printk("\n Entered in SPI3 Rx DMA callback %8x\n", ch_status);

}

static void
omap_spi2_tx_dma_callback (void *data,u32 ch_status)
{
	printk("\n Entered in SPI2 Tx DMA callback %8x\n", ch_status);
	do_gettimeofday(&tx_end_time);
	tx_sec = tx_end_time.tv_sec-tx_start_time.tv_sec;
	tx_usec= tx_end_time.tv_usec-tx_start_time.tv_usec;
	if(tx_usec < 0){
			tx_usec = 0;
	}
}

static int 
configure_dma()
{
	int i;
	char *ptr_r2,*ptr_r3,*ptr_t2,*ptr_t3;

#if (LINUX_VERSION_CODE <= KERNEL_VERSION(2,6,00))
	spi2_rx_buf_dma_virt = (void *)
		consistent_alloc(GFP_KERNEL | GFP_DMA, buffer_size, &spi2_rx_buf_dma_phys);
	spi3_tx_buf_dma_virt = (void *)
		consistent_alloc(GFP_KERNEL | GFP_DMA, buffer_size, &spi3_tx_buf_dma_phys);
	spi2_tx_buf_dma_virt = (void *)
		consistent_alloc(GFP_KERNEL | GFP_DMA, buffer_size, &spi2_tx_buf_dma_phys);
	spi3_rx_buf_dma_virt = (void *)
		consistent_alloc(GFP_KERNEL | GFP_DMA, buffer_size, &spi3_rx_buf_dma_phys);						
#else
	       	
	spi2_rx_buf_dma_virt = (void *)
		dma_alloc_coherent(NULL,buffer_size,&spi2_rx_buf_dma_phys,GFP_KERNEL | GFP_DMA);
	spi3_tx_buf_dma_virt = (void *)
		dma_alloc_coherent(NULL,buffer_size,&spi3_tx_buf_dma_phys,GFP_KERNEL | GFP_DMA);
	spi2_tx_buf_dma_virt = (void *)
		dma_alloc_coherent(NULL,buffer_size,&spi2_tx_buf_dma_phys,GFP_KERNEL | GFP_DMA);
	spi3_rx_buf_dma_virt = (void *)
		dma_alloc_coherent(NULL,buffer_size,&spi3_rx_buf_dma_phys,GFP_KERNEL | GFP_DMA);
#endif
	buffers_allocated = 1;
	printk("Virt spi2 %8x spi3 %8x Rx buf addr\n",spi2_rx_buf_dma_virt,spi3_rx_buf_dma_virt);
	ptr_r2 = (char *)spi2_rx_buf_dma_virt;
	ptr_r3 = (char *)spi3_rx_buf_dma_virt;
	ptr_t2 = (char *)spi2_tx_buf_dma_virt;
	ptr_t3 = (char *)spi3_tx_buf_dma_virt;
	
	for(i=0;i<buffer_size;){
		ptr_r2[i] = 0x00; 
		ptr_r3[i] = 0x00;
		ptr_t2[i] = 0xAA;
		ptr_t3[i] = 0x55;
		i=i+1;
	}
	printk("\n Calling Receive and Transmit buffer APIs of McSPI library\n");
	do_gettimeofday(&tx_start_time);
	omap2_mcspi_receive_buf(&spi_dev2,spi2_rx_buf_dma_phys,buffer_size);
	omap2_mcspi_transmit_buf(&spi_dev3,spi3_tx_buf_dma_phys,buffer_size);
	omap2_mcspi_transmit_buf(&spi_dev2,spi2_tx_buf_dma_phys,buffer_size);
	omap2_mcspi_receive_buf(&spi_dev3,spi3_rx_buf_dma_phys,buffer_size);
	return 0;
}

/****************************************************************************/
/* PROC FILE */

/* Proc interface modules */
static int
read_proc_status(char *page, char **start, off_t off, int count,
                          int *eof, void *data)
{
	int len=0;
	char *p = page;
	if (off != 0)
		goto readproc_status_end;
				
	p += sprintf(p, "\n\n\n\n");
	p += sprintf(p, "==================================================================== \n\n");
	p += sprintf(p, "                          OMAP2 McSPI TEST STATUS                      \n");
	p += sprintf(p, "==================================================================== \n\n");
	p += sprintf(p, "Buffer size (bytes)                      : %8d\n",buffer_size);
	p += sprintf(p, "Bit Rate   (frequency hz)                : %8d\n",(INPUT_CLK/(clk_div+1)));
	p += sprintf(p, "Clk Phase [0=ODD, 1=Even]                : %8d\n",clk_phase);
	p += sprintf(p, "Clk_Polarity [0=High, 1=Low]             : %8d\n",clk_polarity);
	p += sprintf(p, "CS Polarity  [0=High, 1=Low]             : %8d\n",cs_polarity);
	p += sprintf(p, "Word Length   [4 to 32]bits              : %8d\n",word_length);
	p += sprintf(p, "Transmission mode [2-Tx,1-Rx,0-TxRX]     : %8d\n",transmit_mode);
	p += sprintf(p, "Time taken for transmission              : %08li sec %08li usec\n\n\n",tx_sec,tx_usec);
	
readproc_status_end:
	len = (p - page);
	*eof = 1;
	if (off >= len) return 0;
	*start = page + off;
	
	return min(count, len - (int)off);
}

static int
write_proc_entry(struct file *file, const char *buffer,
                         unsigned long count, void *data)
{
	int len,i,ret;
	char val[10];

	if (!buffer || (count == 0))
		return 0;

	len = (count > 7) ? 7 : count;
	for(i=0;i<len;i++) val[i]=buffer[i];
	val[i]='\0';

	if (strncmp(val, "start", 4) == 0){
		configure_dma();
	
		/* Enable the McSPI interface */
		if ( (ret = (omap2_mcspi_enable_channel(&spi_dev2))) ) {
			printk(KERN_ERR "could not enable spi2 channel: ret: %d\n", ret);
			return -1;
		}
	
		if ( (ret = (omap2_mcspi_enable_channel(&spi_dev3))) ) {
			printk(KERN_ERR "could not enable spi3 channel: ret: %d\n", ret);
			return -1;
		}
	
	}
	else if (strncmp(val, "stop", 4) == 0){
		omap2_mcspi_stop_receive_buf(&spi_dev2);
		omap2_mcspi_stop_receive_buf(&spi_dev3);
		omap2_mcspi_stop_transmit_buf(&spi_dev2);
		omap2_mcspi_stop_transmit_buf(&spi_dev3);
	}
	else if (strncmp(val, "suspend", 4) == 0){
		omap2_mcspi_put(&spi_dev2);
		omap2_mcspi_put(&spi_dev3);
	}
	else if (strncmp(val, "resume", 4) == 0){
		omap2_mcspi_get(&spi_dev2);
		omap2_mcspi_get(&spi_dev3);
	}
	else
		return -EINVAL;
		
	return count;
}


static int
create_proc_file_entries(void)
{
	if (!(mcspi_test_dir = proc_mkdir("driver/mcspi_test", NULL))){
		printk("\n No mem to create proc file \n");
		return -ENOMEM;
	}
	
	if (!(transmission_file = create_proc_entry("transmission", 0644, mcspi_test_dir)))
		goto no_transmission;
	transmission_file->data = &file_type[0];
	
	if (!(status_file = create_proc_entry("status", 0644, mcspi_test_dir)))
		goto no_status;		
        status_file->data = &file_type[0];
	
	status_file->read_proc  = read_proc_status;
	transmission_file->write_proc = write_proc_entry;
	
	mcspi_test_dir->owner = status_file->owner = transmission_file->owner = THIS_MODULE;
	return 0;
	
no_status:
	remove_proc_entry("status", mcspi_test_dir);
no_transmission:
	remove_proc_entry("transmission", mcspi_test_dir);
	return -ENOMEM;
}

static 
void remove_proc_file_entries(void)
{
	remove_proc_entry("transmission", mcspi_test_dir);
	remove_proc_entry("status", mcspi_test_dir);
}

int 
__init test_mcspi_init(void)
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
	buffers_allocated = 0;

	cphase = (clk_phase == 0)?OMAP2_McSPI_CHCFG_PHA_ODD:OMAP2_McSPI_CHCFG_PHA_EVEN;
	cpol = (clk_polarity ==0)?OMAP2_McSPI_CHCFG_POL_HIGH:OMAP2_McSPI_CHCFG_POL_LOW;
	epol = (cs_polarity == 0)?OMAP2_McSPI_CHCFG_EPOL_HIGH:OMAP2_McSPI_CHCFG_EPOL_LOW;
	wlength = word_length - 1;	

	
	create_proc_file_entries();
	
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
	omap2_mcspi_set_polarity(spi_dev2,cpol);
	omap2_mcspi_set_clkdivisor(spi_dev2,clk_div);
	omap2_mcspi_set_epol(spi_dev2,epol);
	omap2_mcspi_set_wordlen(spi_dev2,wlength);
	
	omap2_mcspi_set_transmode(spi_dev2,OMAP2_McSPI_CHCFG_TRANSRECEIVE);
	omap2_mcspi_set_dpe0(spi_dev2,OMAP2_McSPI_CHCFG_DPE0_TRANSMIT );
	omap2_mcspi_set_dpe1(spi_dev2,OMAP2_McSPI_CHCFG_DPE1_NO_TRANSMIT );
	omap2_mcspi_set_is(spi_dev2,OMAP2_McSPI_CHCFG_IS_DL1RECEIVE);
	spi_dev2.dma_tx_callback = omap_spi2_tx_dma_callback;
	spi_dev2.dma_rx_callback = omap_spi2_rx_dma_callback;
	spi_dev2.dma_read = OMAP2_McSPI_CHAN_DMA_READ;
	spi_dev2.dma_write = OMAP2_McSPI_CHAN_DMA_WRITE;

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
	
	omap2_mcspi_set_transmode(spi_dev3,OMAP2_McSPI_CHCFG_TRANSRECEIVE);
	omap2_mcspi_set_dpe0(spi_dev3,OMAP2_McSPI_CHCFG_DPE0_NO_TRANSMIT);
	omap2_mcspi_set_dpe1(spi_dev3,OMAP2_McSPI_CHCFG_DPE1_TRANSMIT);
	omap2_mcspi_set_is(spi_dev3,OMAP2_McSPI_CHCFG_IS_DL0RECEIVE);
	spi_dev3.dma_tx_callback = omap_spi3_tx_dma_callback;
	spi_dev3.dma_rx_callback = omap_spi3_rx_dma_callback;
	spi_dev3.dma_read = OMAP2_McSPI_CHAN_DMA_READ;
	spi_dev3.dma_write = OMAP2_McSPI_CHAN_DMA_WRITE;
	
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

	return 0;
}

static void __exit test_mcspi_exit(void)
{
	remove_proc_file_entries();
	
	if(buffers_allocated == 1){
	
#if (LINUX_VERSION_CODE <= KERNEL_VERSION(2,6,00))
	consistent_free((void *)spi2_rx_buf_dma_virt, buffer_size, spi2_rx_buf_dma_phys);
	consistent_free((void *)spi2_tx_buf_dma_virt, buffer_size, spi2_tx_buf_dma_phys);
	consistent_free((void *)spi3_rx_buf_dma_virt, buffer_size, spi3_rx_buf_dma_phys);
	consistent_free((void *)spi3_tx_buf_dma_virt, buffer_size, spi3_tx_buf_dma_phys);

#else

	dma_free_coherent(NULL,buffer_size,(void *)spi2_rx_buf_dma_virt,spi2_rx_buf_dma_phys);
	dma_free_coherent(NULL,buffer_size,(void *)spi2_tx_buf_dma_virt,spi2_tx_buf_dma_phys);
	dma_free_coherent(NULL,buffer_size,(void *)spi3_rx_buf_dma_virt,spi3_rx_buf_dma_phys);
	dma_free_coherent(NULL,buffer_size,(void *)spi3_tx_buf_dma_virt,spi3_tx_buf_dma_phys);
	
#endif
	}
	
	omap2_mcspi_put(&spi_dev2);
	omap2_mcspi_put(&spi_dev3);
	omap2_mcspi_release_interface(&spi_dev2);
	omap2_mcspi_release_interface(&spi_dev3);
	return;
}

module_init(test_mcspi_init);
module_exit(test_mcspi_exit);

MODULE_AUTHOR("Texas Instruments");
MODULE_DESCRIPTION("McSPI driver Library");
MODULE_LICENSE("GPL");
