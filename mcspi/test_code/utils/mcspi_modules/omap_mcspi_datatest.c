/*
 * omap2_mcspi_new_datatest.c
 *
 * Test DMA based data transfer for OMAP McSPI driver
 *
 * Copyright (C) 2006-2009 Texas Instruments, Inc.
 *
 * This file is licensed under the terms of the GNU General Public License
 * version 2. This program is licensed "as is" without any warranty of any
 * kind, whether express or implied.
 *
 * Author : Syed Mohammed Khasim
 * Date   : 12 June 2006

 * Rewritten for new SPI APIs : Hemanth V

 * Connection to be made as following between two boards
 * using J43 expansion connector
 *
 * C19 <-> C19 (SPI2_CS0)
 * C18<-> C18 (SPI2_SOMI)
 * C17<-> C17 (SPI2_SIMO)
 * C16<-> C16 (SPI2_CLK)
 * C5 <-> C5 (GND)

 * Following commands can be issued, first on slave and then
 * master
 *
 * echo rx > /proc/driver/mcspi_test/transmission : Receive Mode
 * echo tx > /proc/driver/mcspi_test/transmission : Transmit Mode
 * echo txrx > /proc/driver/mcspi_test/transmission : Full Duplex Mode
 */


#include <linux/init.h>
#include <linux/module.h>
#include <linux/ioctl.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/list.h>
#include <linux/errno.h>
#include <linux/mutex.h>
#include <linux/slab.h>
#include <linux/smp_lock.h>
#include <linux/wait.h>
#include <linux/sched.h>
#include <linux/kthread.h>

#include <linux/spi/spi.h>
#include <linux/io.h>
#include <linux/dma-mapping.h>
#include <linux/version.h>
#include <linux/proc_fs.h>


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
	#define mmc1_clk	0x48002144

	/* SPI 3 Master */
	#define spi3_clk        0x48002A32 /* spi3_clk  mode 1*/
	#define spi3_cs0        0x48002A38 /* spi3_cs0  mode 1*/
	#define spi3_simo       0x48002A34 /* spi3_simo mode 1*/
	#define spi3_somi       0x48002A36 /* spi3_somi mode 1*/
	#define spi3_cs1        0x48002A3A /* spi3_cs1  mode 1*/
#endif

struct spi_device *spi_g, *spi_g2;

static unsigned int slave_mode = 0;
module_param(slave_mode, int, 0);

static unsigned int systst_mode = 0;
module_param(systst_mode, int, 0);

dma_addr_t spi2_rx_buf_dma_phys;
void *spi2_rx_buf_dma_virt;

dma_addr_t spi2_tx_buf_dma_phys;
void *spi2_tx_buf_dma_virt;

dma_addr_t spi2_rx_buf_dma_phys2;
void *spi2_rx_buf_dma_virt2;

dma_addr_t spi2_tx_buf_dma_phys2;
void *spi2_tx_buf_dma_virt2;

static unsigned int trans1;
static unsigned int trans2;

static unsigned int buffer_size		= 1024;
static unsigned int clk_freq		= 0;
static unsigned int clk_phase		= 0;
static unsigned int clk_polarity	= 0;
static unsigned int cs_polarity		= 0;
static unsigned int word_length		= 0;
static unsigned int transmit_mode	= 0;
static unsigned int test_mcspi_smp;

static unsigned int buffers_allocated = 0;
struct timeval tx_start_time1, tx_end_time1;
struct timeval tx_start_time2, tx_end_time2;
static int spitst_trans(int);
static int spitst_trans1(void);
static int spitst_remove(struct spi_device *spi);


/* Proc interface declaration */
static struct proc_dir_entry  *mcspi_test_dir,
			*transmission_file, *status_file;
static int file_type[2] = {0, 1};

long int tx_sec1 = 0, tx_usec1 = 0;
long int tx_sec2 = 0, tx_usec2 = 0;
struct spi_message	m, msg;
struct spi_transfer	t1, t2;
static int flag = 1;

#if (LINUX_VERSION_CODE <= KERNEL_VERSION(2, 6, 00))
	MODULE_PARM(buffer_size, "i");
	MODULE_PARM(clk_div, "i");
	MODULE_PARM(clk_phase, "i");
	MODULE_PARM(clk_polarity, "i");
	MODULE_PARM(cs_polarity, "i");
	MODULE_PARM(word_length, "i");
#else
	module_param(buffer_size, int, 0);
	module_param(clk_freq, int, 0);
	module_param(clk_phase, int, 0);
	module_param(clk_polarity, int, 0);
	module_param(cs_polarity, int, 0);
	module_param(word_length, int, 0);
	module_param(test_mcspi_smp, int, 0);
#endif


/****************************************************************************/
/* PROC FILE */

/* Proc interface modules */
static int
read_proc_status(char *page, char **start, off_t off, int count,
		int *eof, void *data)
{
	int len = 0;
	char *p = page;
	if (off != 0)
		goto readproc_status_end;

	p += sprintf(p, "\n\n\n\n");
	p += sprintf(p, "====================================="
			"=============================== \n\n");
	p += sprintf(p, "                          OMAP McSPI "
			"TEST STATUS                      \n");
	p += sprintf(p, "====================================="
			"=============================== \n\n");
	p += sprintf(p, "Buffer size (bytes)                      "
				": %8d\n", buffer_size);
	p += sprintf(p, "Bit Rate   (frequency hz)                "
				": %8d\n", clk_freq);
	p += sprintf(p, "Clk Phase [0=ODD, 1=Even]                "
				": %8d\n", clk_phase);
	p += sprintf(p, "Clk_Polarity [0=High, 1=Low]             "
				": %8d\n", clk_polarity);
	p += sprintf(p, "CS Polarity  [1=High, 0=Low]             "
				": %8d\n", cs_polarity);
	p += sprintf(p, "Word Length   [4 to 32]bits              "
				": %8d\n", word_length);
	p += sprintf(p, "Transmission mode [1-rx,2-tx,0-txrx]     "
				": %8d\n", transmit_mode);
	p += sprintf(p, "Time taken for transmission              "
				": %08li sec %08li usec\n\n\n",
				tx_sec1, tx_usec1);
	p += sprintf(p, "Time taken for transmission              "
				": %08li sec %08li usec\n\n\n",
				tx_sec2, tx_usec2);

readproc_status_end:
	len = (p - page);
	*eof = 1;
	if (off >= len)
		return 0;
	*start = page + off;
	return min(count, len - (int)off);
}

static int
write_proc_entry(struct file *file, const char *buffer,
		unsigned long count, void *data)
{
	int len, i;
	char val[10];
	int trans2 = 1;
	int trans1 = 1;

	if (!buffer || (count == 0))
		return 0;

	len = (count > 7) ? 7 : count;
	for (i = 0; i < len; i++)
		val[i] = buffer[i];
	val[i] = '\0';

	if (strncmp(val, "txrx", 4) == 0) {
		if (!test_mcspi_smp)
			spitst_trans(0);
		spitst_trans1();
	} else if (strncmp(val, "tx", 2) == 0) {
		if (!test_mcspi_smp)
			spitst_trans(2);
		spitst_trans1();
	} else if (strncmp(val, "rx", 2) == 0) {
		if (!test_mcspi_smp)
			spitst_trans(1);
		spitst_trans1();
	} else {
		return -EINVAL;
	}

	return count;
}

static int
create_proc_file_entries(void)
{
	mcspi_test_dir = proc_mkdir("driver/mcspi_test", NULL);
	if (!mcspi_test_dir) {
		printk(KERN_INFO "\n No mem to create proc file \n");
		return -ENOMEM;
	}
	transmission_file = create_proc_entry("transmission",
			0644, mcspi_test_dir);
	if (!transmission_file)
		goto no_transmission;
	transmission_file->data = &file_type[0];

	status_file = create_proc_entry("status", 0644, mcspi_test_dir);
	if (!status_file)
		goto no_status;
	status_file->data = &file_type[0];

	status_file->read_proc  = read_proc_status;
	transmission_file->write_proc = write_proc_entry;
#if 0
  mcspi_test_dir->owner = status_file->owner =
			transmission_file->owner = THIS_MODULE;
#endif
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
	remove_proc_entry("driver/mcspi_test", NULL);
}

static int spitst_probe(struct spi_device *spi)
{
	int status;
	spi_g = spi;
	spi_g->mode = SPI_MODE_0;

	if (clk_freq != 0)
		spi_g->max_speed_hz = clk_freq;

	spi_g->mode |= (clk_phase & SPI_CPHA);
	spi_g->mode |= (clk_polarity & SPI_CPOL);
	spi_g->mode |= (cs_polarity & SPI_CS_HIGH);
	if (word_length != 0)
		spi_g->bits_per_word = word_length;

	status = spi_setup(spi_g);

	if (buffer_size == 0)
		buffer_size = 1024;

	clk_freq = spi_g->max_speed_hz;

	clk_phase = (spi_g->mode & SPI_CPHA) >> 0 ;
	clk_polarity = (spi_g->mode & SPI_CPOL) >> 1;
	cs_polarity = (spi_g->mode & SPI_CS_HIGH) >> 2;

	word_length = spi_g->bits_per_word;

	printk(KERN_INFO "spi_setup status %d buffer_size %d\n",
			status, buffer_size);

	return status;
}

static int spitst_probe2(struct spi_device *spi)
{
	int status;
	spi_g2 = spi;
	spi_g2->mode = SPI_MODE_0;

	status = spi_setup(spi_g2);

	if (buffer_size == 0)
		buffer_size = 1024;

	printk(KERN_INFO "spi_setup status %d buffer_size %d\n",
			status, buffer_size);
	return status;
}

static int spitst_trans1()
{
	int status;

	spi_message_init(&m);
	m.is_dma_mapped = 1;

	spi_message_add_tail(&t1, &m);

	do_gettimeofday(&tx_start_time1);

	status = spi_sync(spi_g, &m);

	do_gettimeofday(&tx_end_time1);

	tx_sec1 = tx_end_time1.tv_sec-tx_start_time1.tv_sec;
	tx_usec1 = tx_end_time1.tv_usec-tx_start_time1.tv_usec;

	printk(KERN_INFO "spi_sync status %d\n", status);

	printk(KERN_INFO "Receive Buf %s\n", (char *)spi2_rx_buf_dma_virt);
	printk(KERN_INFO "Transmisstion status %s\n",
	strcmp(spi2_tx_buf_dma_virt, spi2_rx_buf_dma_virt) ?
				"FAIL" : "SUCCESS");

	return status;
}

static int spitst_trans2()
{
	int status;

	spi_message_init(&msg);
	msg.is_dma_mapped = 1;

	spi_message_add_tail(&t2, &msg);

	do_gettimeofday(&tx_start_time2);

	status = spi_sync(spi_g2, &msg);

	do_gettimeofday(&tx_end_time2);

	tx_sec2 = tx_end_time2.tv_sec-tx_start_time2.tv_sec;
	tx_usec2 = tx_end_time2.tv_usec-tx_start_time2.tv_usec;

	printk(KERN_INFO "spi_sync status %d\n", status);

	printk(KERN_INFO "Receive Buf %s\n", (char *)spi2_rx_buf_dma_virt2);
	printk(KERN_INFO "Transmisstion status %s\n",
	strcmp(spi2_tx_buf_dma_virt2, spi2_rx_buf_dma_virt2) ? "FAIL" :
				"SUCCESS");
	return status;
}

static int spitst_trans(int mode)
{
	int status, i = buffer_size = 1024;
	char *string = "McSPI Master Slave Testing";
	int len = strlen(string);
	void *tmp, *tmp1;

	printk("\n spitst_trans\n");
	{
		spi2_rx_buf_dma_virt2 =
			dma_alloc_coherent(NULL, buffer_size,
			&spi2_rx_buf_dma_phys2, GFP_KERNEL | GFP_DMA);

		spi2_tx_buf_dma_virt2 =
			dma_alloc_coherent(NULL, buffer_size,
			&spi2_tx_buf_dma_phys2, GFP_KERNEL | GFP_DMA);

		if ((spi2_rx_buf_dma_virt2 != NULL) &&
			(spi2_tx_buf_dma_virt2 != NULL))
			buffers_allocated = 1;
		else
			status = -ENOMEM;

	spi2_rx_buf_dma_virt =
		dma_alloc_coherent(NULL, buffer_size,
		&spi2_rx_buf_dma_phys, GFP_KERNEL | GFP_DMA);

	spi2_tx_buf_dma_virt =
		dma_alloc_coherent(NULL, buffer_size,
		&spi2_tx_buf_dma_phys, GFP_KERNEL | GFP_DMA);

	if ((spi2_rx_buf_dma_virt != NULL) && (spi2_tx_buf_dma_virt != NULL))
		buffers_allocated = 1;
	else
		status = -ENOMEM;

	memset(spi2_tx_buf_dma_virt, 0, buffer_size);
	memset(spi2_rx_buf_dma_virt, 0, buffer_size);
	if (test_mcspi_smp) {
		memset(spi2_tx_buf_dma_virt2, 0, buffer_size);
		memset(spi2_rx_buf_dma_virt2, 0, buffer_size);
	}

	tmp  = spi2_tx_buf_dma_virt;
	tmp1 = spi2_tx_buf_dma_virt2;
	transmit_mode = mode;

	while (i) {
		strncpy(tmp, string, i < len ? i : len);
		strncpy(tmp1, string, i < len ? i : len);
		tmp += len;
		tmp1 += len;
		if ((i -= len) < 0)
			i = 0;
		}
	}
	if (mode == 0) {
		t1.tx_buf		= spi2_tx_buf_dma_virt;
		t1.tx_dma		= spi2_tx_buf_dma_phys;
		t1.rx_buf		= spi2_rx_buf_dma_virt;
		t1.rx_dma		= spi2_rx_buf_dma_phys;
		t1.len 			= buffer_size;
		if (test_mcspi_smp) {
			t2.tx_buf	= spi2_tx_buf_dma_virt2;
			t2.tx_dma	= spi2_tx_buf_dma_phys2;
			t2.rx_buf	= spi2_rx_buf_dma_virt2;
			t2.rx_dma	= spi2_rx_buf_dma_phys2;
			t2.len		= buffer_size;
		}
			printk("\n spitst_trans\n");
			printk(KERN_INFO "Transmit Buf1 %s\n Buf2 = %s \n ",
						(char *) spi2_tx_buf_dma_virt);
			printk(KERN_INFO "Transmit Buf2 %s\n Buf2 = %s \n ",
						(char *) spi2_tx_buf_dma_virt2);
			printk("\n spitst_trans\n");
	} else if (mode == 2) {

		if (test_mcspi_smp) {
			t2.tx_buf	= spi2_tx_buf_dma_virt2;
			t2.tx_dma	= spi2_tx_buf_dma_phys2;
			t2.rx_buf	= 0;
			t2.rx_dma	= 0;
			t2.len		= buffer_size;
		}
		t1.tx_buf		= spi2_tx_buf_dma_virt;
		t1.tx_dma		= spi2_tx_buf_dma_phys;
		t1.rx_buf		= 0;
		t1.rx_dma		= 0;
		t1.len			= buffer_size;
		/*
			printk(KERN_INFO "Transmit Buf %s\n",
					(char *) spi2_tx_buf_dma_virt );
			printk(KERN_INFO "len %d %d \n",
					len, strlen(spi2_tx_buf_dma_virt));
		*/
	} else if (mode == 1) {
		if (test_mcspi_smp) {
			t2.rx_buf	= spi2_rx_buf_dma_virt2;
			t2.rx_dma	= spi2_rx_buf_dma_phys2;
			t2.tx_buf	= 0;
			t2.tx_dma	= 0;
			t2.len		= buffer_size;
		}
		t1.rx_buf		= spi2_rx_buf_dma_virt;
		t1.rx_dma		= spi2_rx_buf_dma_phys;
		t1.tx_buf		= 0;
		t1.tx_dma		= 0;
		t1.len			= buffer_size;
	} else
		return -1;

	return status;
}

static struct spi_driver spitst_spi = {
	.driver = {
		.name =         "spitst",
		.owner =        THIS_MODULE,
	},
	.probe =        spitst_probe,
	.remove =       __devexit_p(spitst_remove),
};

static struct spi_driver spitst_spi2 = {
	.driver = {
		.name =         "spitst2",
		.owner =        THIS_MODULE,
	},
	.probe =        spitst_probe2,
	.remove =       __devexit_p(spitst_remove),

};

static
int  omap2_mcspi_test1()
{
	int status;

	status = spi_register_driver(&spitst_spi);
	if (status < 0)
		printk(KERN_ERR "spi_register_driver failed, status %d",
							status);
	else
		printk(KERN_INFO "spi_register_driver successful \n");
	{
	/*		while (trans1 != 1)
				schedule_timeout(1000);
	*/
		spitst_trans1();
		trans1 = 0;
		return status;
	}
}

static
int  omap2_mcspi_test2()
{
	int status;

	status = spi_register_driver(&spitst_spi2);
	if (status < 0)
		printk(KERN_ERR "spi_register_driver failed, status %d",
						status);
	else
		printk(KERN_INFO "spi_register_driver successful \n");
	{
		/* while (trans2 != 1)
			schedule_timeout(1000);
		*/
		spitst_trans2();
		trans2 = 0;
		return status;
	}
}

static int spitst_remove(struct spi_device *spi)
{
	printk(KERN_INFO "In spitst_remove \n");
	return 0;
}


int __init test_mcspi_init(void)

{
	struct task_struct *p1, *p2;
	int x;

#define MODCTRL		0xd809a028
#define SYSTST		0xd809a024

	int status;
	int count = 10;
	int val;

	/* Required only if kernel does not configure
	 * SPI2 Mux settings
	 */

	if (slave_mode) {

		printk(KERN_INFO "configuring slave mode\n");

//		omap_writew(0x1700, spi2_clk);
//		omap_writew(0x1700, spi2_simo);
//		omap_writew(0x1700, spi2_somi);
//		omap_writew(0x1708, spi2_cs0);

	} else {

		printk(KERN_INFO "configuring master mode \n");

//		omap_writew(0x1700, spi2_clk);
//		omap_writew(0x1700, spi2_simo);
//		omap_writew(0x1700, spi2_somi);
//		omap_writew(0x1708, spi2_cs0);

	}
	create_proc_file_entries();

	if (test_mcspi_smp) {
		spitst_trans(0);
		p1 = kthread_create(omap2_mcspi_test1, NULL, "mcspitest/0");
		p2 = kthread_create(omap2_mcspi_test2, NULL, "mcspitest/1");

		kthread_bind(p1, 0);
		kthread_bind(p2, 1);

		x = wake_up_process(p1);
		x = wake_up_process(p2);
	}
       if (systst_mode == 1 && !test_mcspi_smp) {

		/* SPI clocks need to be always enabled for this to work */
		__raw_writel(0x8, MODCTRL);
		printk(KERN_INFO "MODCTRL %x\n", __raw_readl(MODCTRL));

		if (slave_mode == 0) /* Master */
			__raw_writel(0x100, SYSTST);
		else
			__raw_writel(0x600, SYSTST);

		printk(KERN_INFO "SYSTST Mode setting %x\n",
				__raw_readl(SYSTST));

		while (count--) {
			if (slave_mode == 0) {
				val = ((count & 0x1) << 6) | 0x100;
				val = ((count & 0x1) << 5) | val;
				val = ((count & 0x1) << 0) | val;

				__raw_writel(val, SYSTST);
			} else {
				val = ((count & 0x1) << 4) | 0x600;
				__raw_writel(val, SYSTST);
			}
			printk(KERN_INFO "SYSTST %x val %x\n",
					__raw_readl(SYSTST)&0xff1, val);
			set_current_state(TASK_INTERRUPTIBLE);
			schedule_timeout(100);
		}
	}

	if (systst_mode == 0 && !test_mcspi_smp) {
		status = spi_register_driver(&spitst_spi);
		if (status < 0)
			printk(KERN_ERR "spi_register_driver failed, status %d",
					status);
		else
			printk("spi_register_driver successful \n");
		return status;
	}
	return 0;
}

static void __exit test_mcspi_exit(void)
{

	spi_unregister_driver(&spitst_spi);
	if (test_mcspi_smp)
		spi_unregister_driver(&spitst_spi2);

	remove_proc_file_entries();

	if (buffers_allocated == 1) {
		if (test_mcspi_smp) {
	#if (LINUX_VERSION_CODE <= KERNEL_VERSION(2, 6, 00))
		consistent_free((void *)spi2_rx_buf_dma_virt2, buffer_size,
				spi2_rx_buf_dma_phys2);
		consistent_free((void *)spi2_tx_buf_dma_virt2, buffer_size,
				spi2_tx_buf_dma_phys2);
	#else
		dma_free_coherent(NULL, buffer_size,
			(void *)spi2_rx_buf_dma_virt2, spi2_rx_buf_dma_phys2);
		dma_free_coherent(NULL, buffer_size,
			(void *)spi2_tx_buf_dma_virt2, spi2_tx_buf_dma_phys2);
	#endif
		}
	#if (LINUX_VERSION_CODE <= KERNEL_VERSION(2, 6, 00))
		consistent_free((void *)spi2_rx_buf_dma_virt2, buffer_size,
				spi2_rx_buf_dma_phys2);
		consistent_free((void *)spi2_tx_buf_dma_virt2, buffer_size,
				spi2_tx_buf_dma_phys2);
	#else

		dma_free_coherent(NULL, buffer_size,
			(void *)spi2_rx_buf_dma_virt2, spi2_rx_buf_dma_phys2);
		dma_free_coherent(NULL, buffer_size,
			(void *)spi2_tx_buf_dma_virt2, spi2_tx_buf_dma_phys2);
	#endif
	}
	return;
}

module_init(test_mcspi_init);
module_exit(test_mcspi_exit);

MODULE_AUTHOR("Texas Instruments");
MODULE_DESCRIPTION("McSPI Test Code");
MODULE_LICENSE("GPL");
