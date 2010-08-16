/*
 * omap2_mcbsp_test.c
 *
 * Test Driver for OMAP2 McBSP driver
 *
 * Copyright (C) 2006 Texas Instruments, Inc.
 *
 * This file is licensed under the terms of the GNU General Public License
 * version 2. This program is licensed "as is" without any warranty of any
 * kind, whether express or implied.
 *
 * Author : Syed Mohammed Khasim
 * Date   : 01 March 2006
 */

#include <linux/module.h>
#include <linux/types.h>
#include <linux/init.h>
#include <linux/errno.h>
#include <linux/netdevice.h>
#include <linux/slab.h>
#include <linux/rtnetlink.h>
#include <linux/interrupt.h>
#include <linux/delay.h>
#include <linux/ioport.h>
#include <linux/proc_fs.h>
#include <linux/version.h>
#include <linux/kthread.h>

#include <asm/irq.h>
#include <asm/io.h>
#include <asm/serial.h>
#include <asm/dma.h>

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 27) && LINUX_VERSION_CODE <= KERNEL_VERSION(2, 6, 31))
 #include <mach/dma.h>
 #include <mach/mcbsp.h>
#elif LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 32)
 #include <plat/dma.h>
 #include <plat/mcbsp.h>
#else
 #include <asm/arch/dma.h>
 #include <asm/arch/mcbsp.h>
#endif

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 00))
#include <linux/dma-mapping.h>
#endif
#define OMAP_MCBSP_WORDLEN_NONE 255

static int start_mcbsp_transmission(int);
struct mcbsp_info_struct {
	int mcbsp_id;
	int mode;		/* Master or Slave */
	dma_addr_t tx_buf_dma_phys;
	unsigned int tx_buf_dma_virt;
	dma_addr_t rx_buf_dma_phys;
	unsigned int rx_buf_dma_virt;
	int rx_cnt;
	int tx_cnt;
	int num_of_tx_transfers;
	int num_of_rx_transfers;
	int send_cnt;
	int recv_cnt;
	spinlock_t rx_lock;
	spinlock_t tx_lock;

};

static struct mcbsp_info_struct mcbsptest_info[2];

struct omap_mcbsp_cfg_param tp1;
struct omap_mcbsp_cfg_param rp1;
struct omap_mcbsp_srg_fsg_cfg cfg;

/* Module parameters are initialized to default values */

static int rx_ready;
static int buffer_size = 1024;
static int no_of_trans = 10;
static int no_of_tx;
static int no_of_rx;
static int no_of_tx2;
static int no_of_rx2;
static int sample_rate = 8000;
static int phase = OMAP_MCBSP_FRAME_SINGLEPHASE;
static int clkr_polarity = OMAP_MCBSP_CLKR_POLARITY_RISING;
static int fsr_polarity = OMAP_MCBSP_FS_ACTIVE_LOW;
static int clkx_polarity = OMAP_MCBSP_CLKX_POLARITY_RISING;
static int fsx_polarity = OMAP_MCBSP_FS_ACTIVE_LOW;
static int justification = OMAP_MCBSP_RJUST_ZEROMSB;
static int word_length = OMAP_MCBSP_WORD_32;
static int word_length1 = OMAP_MCBSP_WORD_32;
static int word_length2 = OMAP_MCBSP_WORD_32;
static int test_mcbsp_id = OMAP_MCBSP2;
static int words_per_frame = 1;
static int test_mcbsp_smp;
long int tx_sec = 0, tx_usec = 0;

module_param(buffer_size, int, 0);
module_param(no_of_trans, int, 0);
module_param(no_of_tx, int, 0);
module_param(no_of_rx, int, 0);
module_param(sample_rate, int, 0);
module_param(phase, int, 0);
module_param(clkr_polarity, int, 0);
module_param(fsr_polarity, int, 0);
module_param(clkx_polarity, int, 0);
module_param(fsx_polarity, int, 0);
module_param(justification, int, 0);
module_param(word_length1, int, 0);
module_param(word_length2, int, 0);
module_param(words_per_frame, int, 0);
module_param(test_mcbsp_id, int, 0);
module_param(test_mcbsp_smp, int, 0);

unsigned int bits_per_sample[6] = { 8, 12, 16, 20, 24, 32 };
struct timeval tx_start_time, tx_end_time;
/* Proc interface declaration */
static struct proc_dir_entry *mcbsp_test_dir, *transmission_file, *status_file;
static int file_type[2] = { 0, 1 };

u16 buf;

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
	p += sprintf(p,
		     "================================================= \n\n");
	p += sprintf(p,
		"                  OMAP2 MCBSP TEST STATUS                \n");
	p += sprintf(p,
		     "================================================ \n\n");
	p += sprintf(p, "McBSP ID                                 : %8d \n",
		     test_mcbsp_id);
	p += sprintf(p, "Buffer size (bytes)                      : %8d \n",
		     buffer_size);
	p += sprintf(p, "Number of transfers                      : %8d \n",
		     no_of_trans);
	p += sprintf(p, "No. of buffers transmitted               : %8d \n",
		     no_of_tx);
	p += sprintf(p, "No. of buffers received                  : %8d \n",
		     no_of_rx);
	p += sprintf(p, "Sampling Rate (frequency, hertz)         : %8d \n",
		     sample_rate);
	p += sprintf(p, "Phase [1=Single, 2=Dual]                 : %8d \n",
		     phase);
	p += sprintf(p, "CLKR_Polarity [1=Rising, 2=Falling]      : %8d\n",
		     clkr_polarity);
	p += sprintf(p, "FSR_Polarity  [0=High, 1=Low]            : %8d\n",
		     fsr_polarity);
	p += sprintf(p, "CLKX_Polarity [1=Rising, 2=Falling]      : %8d\n",
		     clkx_polarity);
	p += sprintf(p, "FSX_Polarity  [0=High, 1=Low]            : %8d\n",
		     fsx_polarity);
	p += sprintf(p, "Justification [0-RJUST,1-SRJUST,2-LJUST] : %8d\n",
		     justification);
	p += sprintf(p, "Word Length1  [0-8,2-16,5-32]bits       : %8d\n",
		     word_length1);
	p += sprintf(p, "Word Length2(DualPhase)[0-8,2-16,5-32]bits: %8d\n",
		     word_length2);
	p += sprintf(p, "Words Per frame [1-1, 2-2]               : %8d\n",
		     words_per_frame);
	p += sprintf(p,
		"Time taken for transmission   : %08li sec %08li usec\n\n\n",
		tx_sec, tx_usec);
readproc_status_end:
	len = (p - page);
	*eof = 1;
	if (off >= len)
		return 0;
	*start = page + off;
	/*MOD_DEC_USE_COUNT;*/
	return min(count, len - (int)off);
}

static int
write_proc_entry(struct file *file, const char *buffer,
		 unsigned long count, void *data)
{
	int len, i;
	char val[10];

	if (!buffer || (count == 0))
		return 0;
	len = (count > 7) ? 7 : count;
	for (i = 0; i < len; i++)
		val[i] = buffer[i];
	val[i] = '\0';

	if (strncmp(val, "start", 4) == 0) {
		start_mcbsp_transmission(0);
	} else if (strncmp(val, "stop", 4) == 0) {
			omap_mcbsp_stop(mcbsptest_info[0].mcbsp_id , 1 , 1);
	} else if (strncmp(val, "suspend", 4) == 0) {
		return 0;
	} else if (strncmp(val, "resume", 4) == 0) {
		start_mcbsp_transmission(1);
		return 0;
	} else
		return -EINVAL;

	return count;
}

static int create_proc_file_entries(void)
{
	if (!(mcbsp_test_dir = proc_mkdir("driver/mcbsp_test", NULL))) {
		printk(KERN_ERR"\n No mem to create proc file \n");
		return -ENOMEM;
	}
	if (!(transmission_file =
		create_proc_entry("transmission", 0644, mcbsp_test_dir)))
		goto no_transmission;
	transmission_file->data = &file_type[0];

	if (!(status_file = create_proc_entry("status", 0644, mcbsp_test_dir)))
		goto no_status;
	status_file->data = &file_type[0];

	status_file->read_proc = read_proc_status;
	transmission_file->write_proc = write_proc_entry;

	return 0;

no_status:
	remove_proc_entry("status", mcbsp_test_dir);
no_transmission:
	remove_proc_entry("transmission", mcbsp_test_dir);
	return -ENOMEM;
}

static
void remove_proc_file_entries(void)
{
	remove_proc_entry("transmission", mcbsp_test_dir);
	remove_proc_entry("status", mcbsp_test_dir);
	remove_proc_entry("driver/mcbsp_test", NULL);
}

void omap_mcbsp_send_cb2(unsigned short int ch_status, void *arg)
{
	printk(KERN_INFO"\nMCBSP_TEST: in %s  %d\n", __func__,
		mcbsptest_info[1].send_cnt++);
	if (no_of_tx2 < no_of_trans) {

		mcbsptest_info[1].tx_buf_dma_phys =
			mcbsptest_info[1].tx_buf_dma_phys;

		if (0 != omap2_mcbsp_send_data(mcbsptest_info[1].mcbsp_id,
				&mcbsptest_info[1],
				mcbsptest_info[1].tx_buf_dma_phys,
				buffer_size)){
			printk(KERN_ERR "McBSP Test Driver:\
					Master Send data failed \n");
			return;
		}
		no_of_tx2++;

	} else {
		printk(KERN_INFO"McBSP Data Transmission (using DMA)\
				is completed successfully \n");
	}
}

/* Callback interface modules */

void omap_mcbsp_send_cb(unsigned short int ch_status, void *arg)
{
	printk("\nMCBSP_TEST: in %s  %d\n", __func__,
		mcbsptest_info[0].send_cnt++);
	if (no_of_tx < no_of_trans) {
		mcbsptest_info[0].tx_buf_dma_phys =
		mcbsptest_info[0].tx_buf_dma_phys;
		if (0 != omap2_mcbsp_send_data(mcbsptest_info[0].mcbsp_id,
				&mcbsptest_info[0],
				mcbsptest_info[0].tx_buf_dma_phys,
				buffer_size)){
			printk(KERN_ERR "McBSP Test Driver:\
				Master Send data failed \n");
			return;
		}
		no_of_tx++;

	} else {
		printk
			(KERN_INFO "McBSP Data Transmission (using DMA)\
			is completed successfully \n");
	}
}

void omap_mcbsp_recv_cb(unsigned short int ch_status, void *arg)
{
	char *ptr1, *ptr2;
	int k , flag = 0;
	u16 w;
	printk(KERN_INFO"\nMCBSP_TEST: in %s  %d\n", __func__,
		mcbsptest_info[0].recv_cnt++);
	if (no_of_rx < no_of_trans) {
		if (0 !=
			omap2_mcbsp_receive_data(mcbsptest_info[0].mcbsp_id,
					&mcbsptest_info[0],
					mcbsptest_info[0].rx_buf_dma_phys,
					buffer_size)) {
			printk(KERN_ERR
				"McBSP Test Driver: Slave Send data failedn");
			return;
		}

		no_of_rx++;
	} else {
		do_gettimeofday(&tx_end_time);
		tx_sec = tx_end_time.tv_sec - tx_start_time.tv_sec;
		tx_usec = tx_end_time.tv_usec - tx_start_time.tv_usec;
		if (tx_usec < 0)
			tx_usec = 0;
		printk(KERN_INFO "McBSP Data Reception (using DMA) is\
			completed successfully \n");
	}
	ptr1 = mcbsptest_info[0].tx_buf_dma_virt;
	ptr2 =  mcbsptest_info[0].rx_buf_dma_virt;

	for (k = 0; k < buffer_size ; ) {
		if (ptr2[k] !=  ptr1[k])
			flag |= 1;
		k = k + 1;
	}
	if (flag == 0)
		printk(KERN_INFO" Data Matched cb0\n");
	else {
		printk(KERN_INFO" Data MisMatched cb0 \n");
/* Debug help
	printk("TxBuffer\n");
	for (k = 0; k < buffer_size ; k++)
	{
		printk("%x ", ptr1[k]);
		if (((k+1)%16) == 0)
			printk("\n");
	}
	printk("RxBuffer\n");

	for (k = 0; k < buffer_size ; k++)
	{
	printk("%x ", ptr2[k]);
	if (((k+1)%8) == 0)
		printk("\n");
	}
*/
	}
}

void omap_mcbsp_recv_cb2(unsigned short int ch_status, void *arg)
{
	char *ptr1, *ptr2;
	int k , flag = 0;
	printk(KERN_INFO"\nMCBSP_TEST: in %s  %d\n", __func__,
		mcbsptest_info[1].recv_cnt++);
	if (no_of_rx2 < no_of_trans) {
		if (0 !=
			omap2_mcbsp_receive_data(mcbsptest_info[1].mcbsp_id,
					&mcbsptest_info[1],
					mcbsptest_info[1].rx_buf_dma_phys,
					buffer_size)) {
			printk(KERN_ERR
				"McBSP Test Driver: Slave Send data failedn");
			return;
		}
		no_of_rx2++;
	} else {
		do_gettimeofday(&tx_end_time);
		tx_sec = tx_end_time.tv_sec - tx_start_time.tv_sec;
		tx_usec = tx_end_time.tv_usec - tx_start_time.tv_usec;
		if (tx_usec < 0)
			tx_usec = 0;

		printk(KERN_INFO"McBSP Data Reception (using DMA)\
				is completed successfully \n");
	}
	ptr1 = mcbsptest_info[1].tx_buf_dma_virt;
	ptr2 =  mcbsptest_info[1].rx_buf_dma_virt;


	for (k = 0; k < buffer_size ; ) {
		if (ptr2[k] !=  ptr1[k])
			flag |= 1;
		k = k + 1;
	}
	if (flag == 0)
		printk(KERN_INFO" Data Matched cb2 \n");
	else {
		printk(KERN_INFO" Data MisMatched cb2 \n");
/* Debug help in case of failure
	printk(KERN_INFO"TxBuffer\n");
	for (k = 0; k < buffer_size ; k++)
	{
		printk("%x ", ptr1[k]);
		if (((k+1)%16) == 0)
			printk("\n");
	}
	printk(KERN_INFO"RxBuffer\n");

	for (k = 0; k < buffer_size ; k++)
	{
		printk("%x ", ptr2[k]);
		if (((k+1)%8) == 0)
			printk("\n");
	}

*/
	}
}
static
int configure_mcbsp_interface(void)
{
	if (phase == OMAP_MCBSP_FRAME_SINGLEPHASE)
		word_length2 = word_length1;
	if (phase == OMAP_MCBSP_FRAME_DUALPHASE)
		words_per_frame = 1;
	cfg.period = bits_per_sample[word_length1] + 8;
	if (phase == OMAP_MCBSP_FRAME_DUALPHASE)
		cfg.period += (bits_per_sample[word_length2] + 8);
	cfg.fsgm = 1;
	cfg.sample_rate = sample_rate;
	cfg.pulse_width = (bits_per_sample[(word_length1 > word_length2 ?
					    word_length1 : word_length2)] + 1);
	cfg.bits_per_sample = bits_per_sample[(word_length1 > word_length2 ?
					       word_length1 : word_length2)];
	cfg.srg_src = OMAP_MCBSP_SRGCLKSRC_FCLK;
	cfg.sync_mode = OMAP_MCBSP_SRG_FREERUNNING;
	cfg.polarity = 0;
	cfg.dlb = 1;
	if (word_length1 < word_length2)
		word_length = word_length1;
	else
		word_length = word_length2;
}

static
int configure_mcbsp_tx(int id)
{
	struct omap_mcbsp_dma_transfer_parameters tp;
	int k;
	char *ptr, *ptr1;

	/* XRST Reset */
	if (0 !=
		omap2_mcbsp_set_xrst(mcbsptest_info[id].mcbsp_id,
				 OMAP_MCBSP_XRST_DISABLE)) {
		printk(KERN_ERR "McBSP Test Driver: TX RST failed\n");
		return -1;
	}


	/* Configure transfer params */
	tp.callback = &omap_mcbsp_send_cb;
	tp.skip_alt = OMAP_MCBSP_SKIP_NONE;
	tp.auto_reset = OMAP_MCBSP_AUTO_RST_NONE;
	tp.word_length1 = word_length;

	tp1.fsync_src = OMAP_MCBSP_TXFSYNC_INTERNAL;
	tp1.fs_polarity = fsx_polarity;
	tp1.clk_polarity = clkx_polarity;
	tp1.clk_mode = OMAP_MCBSP_CLKTXSRC_INTERNAL;
	tp1.frame_length1 = OMAP_MCBSP_FRAMELEN_N(words_per_frame);
	tp1.frame_length2 = OMAP_MCBSP_FRAMELEN_N(words_per_frame);
	tp1.word_length1 = word_length1;
	tp1.word_length2 = word_length2;
	tp1.justification = justification;
	tp1.reverse_compand = OMAP_MCBSP_MSBFIRST;
	tp1.phase = phase;
	tp1.data_delay = 1;

	if (id == 1)
		tp.callback = &omap_mcbsp_send_cb2;

	if (0 != omap2_mcbsp_dma_trans_params(mcbsptest_info[id].mcbsp_id,
						&tp)) {
		printk(KERN_ERR "McBSP Test Driver:\
				Configuring transfer params failed\n");
		return -1;
	}
	mcbsptest_info[id].tx_buf_dma_virt = (void *)
		dma_alloc_coherent(NULL, buffer_size,
				&mcbsptest_info[id].tx_buf_dma_phys,
				GFP_KERNEL | GFP_DMA);



	ptr = mcbsptest_info[id].tx_buf_dma_virt;
	for (k = 0; k < 1024;) {
		ptr[k] = k;
		k = k + 1;
	}


	return 1;
}

static
int configure_mcbsp_rx(int id)
{

	struct omap_mcbsp_dma_transfer_parameters rp;


	/* RRST */
	if (0 !=
		omap2_mcbsp_set_rrst(mcbsptest_info[id].mcbsp_id,
				 OMAP_MCBSP_RRST_DISABLE))
		printk(KERN_ERR"\n Receiver Reset failed \n");


	/* Configure receiver params */
	printk(KERN_INFO"\n configuring mcbsp rx \n");

	rp.callback = &omap_mcbsp_recv_cb;
	rp.skip_alt = OMAP_MCBSP_SKIP_NONE;
	rp.auto_reset = OMAP_MCBSP_AUTO_RST_NONE;
	rp.word_length1 = word_length;
	rp1.fsync_src = OMAP_MCBSP_RXFSYNC_INTERNAL;
	rp1.fs_polarity = fsr_polarity;
	rp1.clk_polarity = clkr_polarity;
	rp1.clk_mode = OMAP_MCBSP_CLKRXSRC_EXTERNAL;
	rp1.frame_length1 = OMAP_MCBSP_FRAMELEN_N(words_per_frame);
	rp1.frame_length2 = OMAP_MCBSP_FRAMELEN_N(words_per_frame);
	rp1.word_length1 = word_length1;
	rp1.word_length2 = word_length2;
	rp1.justification = justification;
	rp1.reverse_compand = OMAP_MCBSP_MSBFIRST;
	rp1.phase = phase;
	rp1.data_delay = 1;

	if (id == 1)
		rp.callback = &omap_mcbsp_recv_cb2;

	if (0 != omap2_mcbsp_dma_recv_params
			(mcbsptest_info[id].mcbsp_id, &rp)) {
		printk(KERN_ERR "McBSP Test Driver: Configuring\
				transfer params failed\n");
		return -1;
	}
	mcbsptest_info[id].rx_buf_dma_virt = (void *)
		dma_alloc_coherent(NULL, buffer_size,
				&mcbsptest_info[id].rx_buf_dma_phys,
				GFP_KERNEL | GFP_DMA);


	return 0;
}

/* transmit mode = receive (0) or transmit (1) */
static
int start_mcbsp_transmission(id)
{
	if (id == 0) {
		no_of_tx = 0;
		no_of_rx = 0;
	} else {
		no_of_tx2 = 0;
		no_of_rx2 = 0;
	}

	/* Configure the Master, it should generate the FSX and CLKX */
	configure_mcbsp_tx(id);
	configure_mcbsp_rx(id);
	omap2_mcbsp_params_cfg(mcbsptest_info[id].mcbsp_id, OMAP_MCBSP_MASTER,
				&tp1, &rp1, &cfg);

	mcbsptest_info[id].rx_cnt = 0;
	mcbsptest_info[id].recv_cnt = 0;

	mcbsptest_info[id].tx_cnt = 0;
	mcbsptest_info[id].send_cnt = 0;

	printk(" Start DMA Rx \n");
	if (0 !=
		omap2_mcbsp_receive_data(mcbsptest_info[id].mcbsp_id,
					&mcbsptest_info[id],
					mcbsptest_info[id].rx_buf_dma_phys,
					buffer_size)) {
		printk(KERN_ERR "McBSP Test Driver: Slave Send data failedn");
		return -1;
	}

	printk(" Start DMA Tx \n");
	if (0 !=
		omap2_mcbsp_send_data(mcbsptest_info[id].mcbsp_id,
				&mcbsptest_info[id],
				mcbsptest_info[id].tx_buf_dma_phys,
				buffer_size)) {
		printk(KERN_ERR
			"McBSP Test Driver: Master Send data failed \n");
		return -1;
	}
	omap_mcbsp_start(mcbsptest_info[id].mcbsp_id, 1, 1);
	return 0;
}

static
void fill_global_structure(u8 id)
{
	mcbsptest_info[id].mcbsp_id = test_mcbsp_id + id;
	if ( id == 1 )
		mcbsptest_info[id].mcbsp_id = (test_mcbsp_id + id) % 4;
	mcbsptest_info[id].mode = OMAP_MCBSP_MASTER;	/* Master or Slave */
	mcbsptest_info[id].rx_cnt = 0;
	mcbsptest_info[id].tx_cnt = 0;
	mcbsptest_info[id].num_of_tx_transfers = 256;
	mcbsptest_info[id].num_of_rx_transfers = 256;
	mcbsptest_info[id].send_cnt = 0;
	mcbsptest_info[id].recv_cnt = 0;
}

static
int omap2_mcbsp_test2(void)
{
	start_mcbsp_transmission(1);
}

static
int omap2_mcbsp_test1(void)
{
	start_mcbsp_transmission(0);
}

static
int __init omap2_mcbsp_init(void)
{
	struct task_struct *p1, *p2;
	int x;

	create_proc_file_entries();

	fill_global_structure(0);
	omap_mcbsp_set_io_type(mcbsptest_info[0].mcbsp_id, 0);
	/* Requesting interface */
	if (omap_mcbsp_request(mcbsptest_info[0].mcbsp_id) != 0) {
		printk(KERN_ERR "McBSP Test Driver:\
				Requesting mcbsp interface failed\n");
		return -1;
	}
	configure_mcbsp_interface();


	/*Test*/
	if (test_mcbsp_smp) {
		fill_global_structure(1);
		omap_mcbsp_set_io_type(mcbsptest_info[1].mcbsp_id, 0);

		/* Requesting interface */
		if (omap_mcbsp_request(mcbsptest_info[1].mcbsp_id) != 0) {
			printk(KERN_ERR "McBSP Test Driver:\
					Requesting mcbsp interface failed\n");
			return -1;
		}
	configure_mcbsp_interface();
	}

	if (test_mcbsp_smp) {
		p1 = kthread_create(omap2_mcbsp_test1, NULL, "mcbsptest/0");
		kthread_bind(p1, 0);
		x = wake_up_process(p1);

		p2 = kthread_create(omap2_mcbsp_test2, NULL, "mcbsptest/1");
		kthread_bind(p2, 1);
		x = wake_up_process(p2);

	}
	msleep(1000);
	printk("\n OMAP McBSP TEST driver installed successfully \n");
	return 0;
}

static
void __exit omap_mcbsp_exit(void)
{
	if (test_mcbsp_smp) {

#if (LINUX_VERSION_CODE <= KERNEL_VERSION(2, 6, 00))
		consistent_free((void *)mcbsptest_info[1].rx_buf_dma_virt,
				buffer_size, mcbsptest_info[1].rx_buf_dma_phys);
		consistent_free((void *)mcbsptest_info[1].tx_buf_dma_virt,
				buffer_size, mcbsptest_info[1].tx_buf_dma_phys);
#else
		dma_free_coherent(NULL, buffer_size,
				(void *)mcbsptest_info[1].rx_buf_dma_virt,
				mcbsptest_info[1].rx_buf_dma_phys);
		dma_free_coherent(NULL, buffer_size,
				(void *)mcbsptest_info[1].tx_buf_dma_virt,
				mcbsptest_info[1].tx_buf_dma_phys);

#endif
		omap_mcbsp_free(mcbsptest_info[1].mcbsp_id);
	}
#if (LINUX_VERSION_CODE <= KERNEL_VERSION(2, 6, 00))
	consistent_free((void *)mcbsptest_info[0].rx_buf_dma_virt, buffer_size,
			mcbsptest_info[0].rx_buf_dma_phys);
	consistent_free((void *)mcbsptest_info[0].tx_buf_dma_virt, buffer_size,
			mcbsptest_info[0].tx_buf_dma_phys);
#else
	dma_free_coherent(NULL, buffer_size,
			(void *)mcbsptest_info[0].rx_buf_dma_virt,
			mcbsptest_info[0].rx_buf_dma_phys);
	dma_free_coherent(NULL, buffer_size,
			(void *)mcbsptest_info[0].tx_buf_dma_virt,
			mcbsptest_info[0].tx_buf_dma_phys);
#endif
	omap_mcbsp_free(mcbsptest_info[0].mcbsp_id);
	remove_proc_file_entries();
	return;
}

module_init(omap2_mcbsp_init);
module_exit(omap_mcbsp_exit);

MODULE_AUTHOR("Texas Instruments");
MODULE_DESCRIPTION("McBSP Test Driver");
MODULE_LICENSE("GPL");
