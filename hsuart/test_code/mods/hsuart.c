/*
 * linux/drivers/char/omap24xx_hsuart.c
 *
 * HS-UART
 *
 * This driver is only meant for testing the UART at high speeds (baudrate > 115200bps) 
 * on TI OMAP24XX. The driver will be removed from this directory later. 
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
 * History:
 * -------
 * Modified by Pragya Srivastava for Kernel version 2.6
 */

//#include <linux/config.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/init.h>
#include <linux/errno.h>
#include <linux/slab.h>
#include <linux/interrupt.h>
#include <linux/delay.h>
#include <linux/ioport.h>
#include <linux/proc_fs.h>

#include <asm/irq.h>
#include <asm/io.h>
#include <asm/hardware.h>
#include <asm/serial.h>
#include <asm/dma.h>
#include <asm/arch/gpio.h>
#include <asm/arch/mux.h>
#include <asm/arch/omap24xx-uart.h>

#include <linux/version.h>


#define PROC_FILE "driver/hsuart"

struct omap24xx_hsuart {
	unsigned char open;
	int speed;		/* Current hsuart speed */
	int newspeed;
	int irq;
	int rx_status;
	int tx_status;
	long int tx_sec, tx_usec;
};
struct omap24xx_hsuart *si;
static char *rx_buf;
static char *tx_buf;
static int failed = 0;

/* forward declarations */
static struct proc_dir_entry *hsuart_dir, *status_file, *starttx_file;
static int omap24xx_hsuart_tx(void);
static int omap24xx_hsuart_start(void);
static int omap24xx_hsuart_initialize(void);
static int omap24xx_hsuart_stop(void);

//static int baudrate = 921600;
static int baudrate = 3686400;
static int loopback = 1;
static int bufsize = 10000;
static int autortscts = 0;
static int sfc = 0;
static int uart_no = 1;

/* changes for version 2.6 and greater. */
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,00))
module_param(baudrate, int,0);
module_param(loopback, int,1);
module_param(bufsize,int,0);
module_param(autortscts, int,0);
module_param(sfc, int,0);
module_param(uart_no,int,1);
#else
MODULE_PARM(baudrate, "i");
MODULE_PARM(loopback, "i");
MODULE_PARM(bufsize, "i");
MODULE_PARM(autortscts, "i");
MODULE_PARM(sfc, "i");
MODULE_PARM(uart_no, "i");
#endif

struct timeval tx_start_time, tx_end_time;
static int file_type[2] = { 1, 2 };
static int showtxtime = 0;
static int starttx = 0;

static
    void
omap24xx_hsuart_rx_callback(int lch, u16 ch_status, void *data)
{
	struct omap24xx_hsuart *si = data;
	unsigned char *addr;
	int i = 0, j = 0;
	int w = 0;
	u8 reg_data;
	si->rx_status = 1;
	if (loopback == 1)
		printk("Rx DMA callback entered\n");

	omap24xx_uart_rx(uart_no, rx_buf, &w);
	addr = (unsigned char *) rx_buf;
	if (loopback == 0) {
		for (i = 0; i < w;) {
			printk("%c", *(addr + i));
			i = i + 1;
		}
	} else {
		for (i = 0, j = 0; i < w;) {
			if (*(addr + i) != j) {
				printk("\n Testing UART failed...!\n");
				printk("buf[%d] is not equal to %d\n", i, j);
				return;
			}
			i = i + 1;
			j = j + 1;
			if (i % 256 == 0)
				j = 0;
		}
		printk("\nData transfer in loopback mode is successful...\n");
		printk("<press enter>\n");
	}
	/* Clear interrupts */
	omap24xx_uart_get_parms(uart_no, &reg_data, REG_IIR, LCR_MODE1);
	omap24xx_uart_get_parms(uart_no, &reg_data, REG_RESUME, LCR_MODE1);
	if (loopback == 1)
		printk("Starting Rx DMA\n");
	si->rx_status = 0;
	omap24xx_uart_start_rx(uart_no, bufsize);

}

/* DMA TX callback - calling when frame transfer has been finished */

static
    void
omap24xx_hsuart_tx_callback(int lch, u16 ch_status, void *data)
{
	struct omap24xx_hsuart *si = data;
	if (unlikely(!si)) {
		printk(KERN_CRIT "AIEE HSUART internal data is NULL\n");
		BUG();
		return;
	}
	si->tx_status = 1;
	printk("Tx DMA callback entered\n");
	do_gettimeofday(&tx_end_time);
	omap24xx_uart_stop_tx(uart_no);

	si->tx_sec = tx_end_time.tv_sec - tx_start_time.tv_sec;
	si->tx_usec = tx_end_time.tv_usec - tx_start_time.tv_usec;
	if (si->tx_usec < 0) {
		//si->tx_usec = (0xFFFFFFFF - tx_start_time.tv_usec) + tx_end_time.tv_usec;
		si->tx_usec = 0;
	}
	showtxtime = 1;
	starttx = 0;
	printk
	    ("Data size       %8d bytes\nBaudrate        %8d bps\nTime taken      %08li sec %08li usec\n",
	     bufsize, baudrate, si->tx_sec, si->tx_usec);

}

/*
 * Set the hsuart communications speed.
 * Interrupt have to be disabled here.
 */

static
    void
omap24xx_hsuart_startup(void)
{
	struct uart_config uartcfg;
	uartcfg.mode = UART_MODE;
	if (autortscts == 1) {
		printk("Enabling Hardware flow Control(Auto RTS and CTS) .....");
		uartcfg.efr = 0xD0;
		uartcfg.mcr = 1 << 6;
		printk("Done\n");
	}
	else if ( (autortscts == 0) && ( sfc == 0)){
		printk("!!!! Caution:....Flow Control(H/W or S/W) is not specified for data transfer...\n");
		printk("!!!! Transferring data without flow control may/may not result in receiving/transmitting incorrect or corrupted data...\n");
		uartcfg.efr = 1 << 4;
		uartcfg.mcr = 1 << 6;
	}	
	else if ( (sfc == 1)){
		struct uart_setparm uart_set;
		printk("Enabling Software flow Control(XON/XOFF).....");
		
		/* Program XON1 and XOFF1 character */
		uart_set.lcr = LCR_MODE2;
	        uart_set.reg = REG_XON1_ADDR1;
		uart_set.reg_data = 0x11; // Decimal value 17
		omap24xx_uart_set_parms(uart_no, &uart_set);
		
		uart_set.lcr = LCR_MODE2;
	        uart_set.reg = REG_XOFF1;
		uart_set.reg_data = 0x13; // Decimal value 19
		omap24xx_uart_set_parms(uart_no, &uart_set);
		
		/* Enable Software flow control, special character detect
		 * and access to IER,FCR and MCR registers.
		 */
		
		uartcfg.efr = 1 << 5 | 1 << 4 | (1 << 3 | 1 << 2 | 1 << 1 | 1 << 0);
		
		/* Enable Xon any function , and enable access TLR and TCR regs */
		uartcfg.mcr = 1 << 5 | 1 << 6;
		printk("Done\n");
	}	

	/* Enable access to UART1_TLR and UART1_TCR registers */
	uartcfg.scr = 0;
	/* Set Rx trigger to 1 and Tx trigger to 1 */
	uartcfg.tlr = 0;

	/* Set LCR to 8 bits and 1 stop bit */
	uartcfg.lcr = 0x03;

	/* Clear RX and TX FIFO and enable FIFO */
	/* Use DMA Req for transfers */

	uartcfg.fcr = (1 << 2) | (1 << 1) | (1 << 3) | (1 << 4) | (1 << 6) | 1;

	uartcfg.scr = (1 << 7) | (1 << 6);

	/* Enable UART1 16x Mode,(Frame-length method to end frames) */
	uartcfg.mdr1 = 0;

	/* Set Status FIFO trig to 1 */
	uartcfg.mdr2 = 0;

	/* Enables RXIR input */
	/* and disable TX underrun */
	/* SEND_SIP pulse */

	uartcfg.acreg = (1 << 6) | (1 << 4);

	/* Enable EOF Interrupt only */
	uartcfg.ier = (1 << 7) | (1 << 5);
	if (omap24xx_uart_config(uart_no, &uartcfg)) {
		printk(KERN_ERR
		       "!!! %s !!!! Error ...UART configuration Failed",
		       __FUNCTION__);
	}

}

static
    int
omap24xx_hsuart_tx()
{
	int i;
	unsigned char *txaddr;
	txaddr = (unsigned char *) tx_buf;
	for (i = 0; i < bufsize;) {
		*(txaddr + i) = i;
		i = i + 1;
	}

	omap24xx_uart_tx(uart_no, txaddr, bufsize);
	si->tx_status = 0;
	showtxtime = 0;
	do_gettimeofday(&tx_start_time);
	omap24xx_uart_start_tx(uart_no, bufsize);
	return 0;
}

static
    int
omap24xx_hsuart_start()
{

	omap24xx_hsuart_startup();

	omap24xx_uart_set_speed(uart_no, si->speed);
	si->open = 1;

	/* Start RX DMA */
	if (loopback == 1)
		printk("Starting Rx DMA\n");
	si->rx_status = 0;
	omap24xx_uart_start_rx(uart_no, bufsize);

	return 0;
}

static
    int
omap24xx_hsuart_stop()
{
	if (si->tx_status == 0) {
		omap24xx_uart_stop_tx(uart_no);
	}

	if (si->rx_status == 0) {
		omap24xx_uart_stop_rx(uart_no);
	}

	omap24xx_uart_stop(uart_no);
	kfree(si);

	return 0;
}

static int
read_proc_entries(char *page, char **start, off_t off, int count,
		  int *eof, void *data)
{
	int len = 0;
	int *procfiletype = (int *) data;
	char *p = page;

	if (off != 0)
		goto readproc_end;

//	MOD_INC_USE_COUNT;

	if (*procfiletype == 1) {
		p += sprintf(p, "OMAP High Speed UART test\n\n");
		p += sprintf(p, "Data size        %8d bytes\n", bufsize);
		p += sprintf(p, "Baudrate         %8d bps\n", baudrate);
		p += sprintf(p, "Autoctsrts       %8d [1:ON / 0:OFF]\n",
			     autortscts);
		p += sprintf(p, "SFC              %8d [1:ON / 0:OFF]\n",sfc);
		p += sprintf(p, "Loopback         %8d [1:ON / 0:OFF]\n",
			     loopback);
		p += sprintf(p, "UART port        %8d [0:UART1 1:UART2 2:UART3]\n", uart_no);
		if (showtxtime == 1) {
			p += sprintf(p,
				     "Time taken       %08li sec %08li usec\n",
				     si->tx_sec, si->tx_usec);
		}
	} else if (*procfiletype == 2) {
		if (starttx == 1) {
			p += sprintf(p, "UART Tx is ON\n");
		} else {
			p += sprintf(p, "UART Tx is OFF\n");
		}
	}

      readproc_end:
	len = (p - page);
	*eof = 1;
	if (off >= len)
		return 0;
	*start = page + off;

//	MOD_DEC_USE_COUNT;
	return min(count, len - (int) off);

}

static int
write_proc_entries(struct file *file, const char *buffer,
		   unsigned long count, void *data)
{
	int len, i;
	char val[10];

	if (!buffer || (count == 0))
		return 0;

//	MOD_INC_USE_COUNT;

	len = (count > 3) ? 3 : count;
	for (i = 0; i < len; i++)
		val[i] = buffer[i];
	val[i] = '\0';

//	MOD_DEC_USE_COUNT;

	if (strncmp(val, "ON", 2) == 0) {
		if (si->tx_status == 1) {
			showtxtime = 0;
			omap24xx_hsuart_tx();
		}
		starttx = 1;
	} else if (strncmp(val, "OFF", 3) == 0) {
		if (si->tx_status == 0) {
			omap24xx_uart_stop_tx(uart_no);
			si->tx_status = 0;
		}
		starttx = 0;
	} else
		return -EINVAL;

	return count;

}

#define HSUART_DIR "driver/hsuart"
#define HSUART_ROOT NULL
static int
create_proc_file_entries(void)
{
	if (!(hsuart_dir = proc_mkdir(HSUART_DIR, HSUART_ROOT)))
		return -ENOMEM;

	if (!(status_file = create_proc_entry("status", 0644, hsuart_dir)))
		goto no_status;

	status_file->data = &file_type[0];

	if (!(starttx_file = create_proc_entry("starttx", 0644, hsuart_dir)))
		goto no_starttx;
	starttx_file->data = &file_type[1];

	status_file->read_proc = starttx_file->read_proc = read_proc_entries;
	status_file->write_proc = starttx_file->write_proc = write_proc_entries;

	hsuart_dir->owner = status_file->owner = starttx_file->owner =
	    THIS_MODULE;

	return 0;

      no_starttx:
	remove_proc_entry("starttx", hsuart_dir);
	remove_proc_entry("status", hsuart_dir);
      no_status:
	remove_proc_entry(HSUART_DIR, HSUART_ROOT);
	return -ENOMEM;
}

static void
remove_proc_file_entries(void)
{
	remove_proc_entry("status", hsuart_dir);
	remove_proc_entry("starttx", hsuart_dir);
	remove_proc_entry(HSUART_DIR, HSUART_ROOT);
}

static void
omap24xx_uart_irq(u8 iir_data, void *dev_id)
{
	if (iir_data) {

	}
}

static
    int
omap24xx_hsuart_initialize()
{
	struct uart_callback uart_txrxisr;
	si = kmalloc(sizeof (struct omap24xx_hsuart), GFP_KERNEL);
	if (!si) {
		printk("\n Error! not able to allocate UART struct\n");
	}

	uart_txrxisr.mode = UART_DMA_MODE;
	uart_txrxisr.txrx_req_flag = TXRX;
	uart_txrxisr.uart_tx_dma_callback =
	    (void *) omap24xx_hsuart_tx_callback;
	uart_txrxisr.uart_rx_dma_callback =
	    (void *) omap24xx_hsuart_rx_callback;
	uart_txrxisr.tx_buf_size = bufsize;
	uart_txrxisr.rx_buf_size = bufsize;

#if 0
	printk("uart_txrxisr.tx_buf_size : %d\n", uart_txrxisr.tx_buf_size);
	printk("uart_txrxisr.rx_buf_size : %d\n", uart_txrxisr.rx_buf_size);
#endif

	uart_txrxisr.int_callback = (void *) omap24xx_uart_irq;

	uart_txrxisr.dev_name = "OMAP24XX HSUART";
	uart_txrxisr.dev = (void *) si;
	if (omap24xx_uart_request(uart_no, &uart_txrxisr)) {
		printk(KERN_ERR "%s : Requested UART is not available\n",
		       __FUNCTION__);
		failed = 1;
		kfree(si);
		return -EPERM;

	}

	tx_buf = kmalloc(bufsize, GFP_KERNEL);
	if (!tx_buf) {
		printk("Error! not able to allocate memory for tx_buf\n");
		goto exit_path1;
	}
	rx_buf = kmalloc(bufsize, GFP_KERNEL);
	if (!rx_buf) {
		printk("Error! not able to allocate memory for tx_buf\n");
		goto exit_path2;
	}

	memset(si, 0, sizeof (*si));
	memset(tx_buf, 0, bufsize);
	memset(rx_buf, 0, bufsize);
	
	if ((sfc == 1) && (autortscts == 1)){
		printk("ERROR! Hardware and Software flow control are not supported at a time\n");
		goto exit_path3;
	}	

	if (baudrate > 3686400 || baudrate < 300) {
		printk(" ERROR! Baudrate out of range [300 to 3686400]\n");
		printk(" Setting to default 921600\n");
	}
	if (loopback != 0 && loopback != 1) {
		printk(" ERROR! Invalide Loopback parameter [0/1] \n");
	}
	if (autortscts != 0 && autortscts != 1) {
		printk(" ERROR! Invalid Auto RTS CTS parameter [0/1] \n");
	}
	if ( sfc != 0 && sfc != 1 ){
		printk(" ERROR! Invalid SFC parameter [0/1] \n");
	}	
	if (starttx != 0 && starttx != 1) {
		printk(" ERROR! Invalid starttx parameter [0/1] \n");
	}
	if (bufsize <= 0 || bufsize > 0x100000) {
		printk
		    (" ERROR! Buffer Size should be greater than 1 and less than 0x100000\n");
	}

	si->speed = baudrate;
	si->tx_status = 1;
	si->rx_status = 1;
	showtxtime = 0;
	create_proc_file_entries();

	omap24xx_hsuart_start();
	return 0;

      exit_path3:
	kfree(rx_buf);
      exit_path2:
	kfree(tx_buf);
      exit_path1:
	kfree(si);
	return -EPERM;
}

static
int __init
omap24xx_hsuart_init(void)
{
	omap24xx_hsuart_initialize();
	return 0;
}

static
void __exit
omap24xx_hsuart_exit(void)
{
	if ( !failed ) {
		remove_proc_file_entries();
		omap24xx_hsuart_stop();
		omap24xx_uart_release(uart_no);
	}	
}

module_init(omap24xx_hsuart_init);
module_exit(omap24xx_hsuart_exit);
MODULE_AUTHOR("Texas Instruments");
MODULE_DESCRIPTION("OMAP24xx HSUART");
MODULE_LICENSE("GPL");
