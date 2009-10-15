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

#include <linux/irq.h>
#include <linux/io.h>
#include <linux/serial.h>
#include <mach/dma.h>
#include <mach/mcbsp.h>

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 00))
#include <linux/dma-mapping.h>
#endif

#define OMAP_MCBSP_WORDLEN_NONE 255

#define OMAP_MCBSP_MASTER			1
#define OMAP_MCBSP_SLAVE			0

/* McBSP interface operating mode */
#define OMAP_MCBSP_MASTER			1
#define OMAP_MCBSP_SLAVE			0

#define OMAP_MCBSP_AUTO_RST_NONE		(0x0)
#define OMAP_MCBSP_AUTO_RRST			(0x1 << 1)
#define OMAP_MCBSP_AUTO_XRST			(0x1 << 2)

/* SRG ENABLE/DISABLE state */
#define OMAP_MCBSP_ENABLE_FSG_SRG		1
#define OMAP_MCBSP_DISABLE_FSG_SRG		2
/* mono to mono mode*/
#define OMAP_MCBSP_SKIP_NONE			0
/* mono to stereo mode */
#define OMAP_MCBSP_SKIP_FIRST			(0x1 << 1)
#define OMAP_MCBSP_SKIP_SECOND			(0x1 << 2)
/* RRST STATE */
#define OMAP_MCBSP_RRST_DISABLE			0
#define OMAP_MCBSP_RRST_ENABLE			1
/*XRST STATE */
#define OMAP_MCBSP_XRST_DISABLE			0
#define OMAP_MCBSP_XRST_ENABLE			1

#define OMAP_MCBSP_FRAME_SINGLEPHASE		1
#define OMAP_MCBSP_FRAME_DUALPHASE		2

/* Sample Rate Generator Clock source */
#define OMAP_MCBSP_SRGCLKSRC_CLKS		1
#define OMAP_MCBSP_SRGCLKSRC_FCLK		2
#define OMAP_MCBSP_SRGCLKSRC_CLKR		3
#define OMAP_MCBSP_SRGCLKSRC_CLKX		4

/* SRG input clock polarity */
#define OMAP_MCBSP_CLKS_POLARITY_RISING		1
#define OMAP_MCBSP_CLKS_POLARITY_FALLING	2

#define OMAP_MCBSP_CLKX_POLARITY_RISING		1
#define OMAP_MCBSP_CLKX_POLARITY_FALLING	2

#define OMAP_MCBSP_CLKR_POLARITY_RISING		1
#define OMAP_MCBSP_CLKR_POLARITY_FALLING	2

/* SRG Clock synchronization mode */
#define OMAP_MCBSP_SRG_FREERUNNING		1
#define OMAP_MCBSP_SRG_RUNNING			2

/* Frame Sync Source */
#define OMAP_MCBSP_TXFSYNC_EXTERNAL		0
#define OMAP_MCBSP_TXFSYNC_INTERNAL		1

#define OMAP_MCBSP_RXFSYNC_EXTERNAL		0
#define OMAP_MCBSP_RXFSYNC_INTERNAL		1

#define OMAP_MCBSP_CLKRXSRC_EXTERNAL		1
#define OMAP_MCBSP_CLKRXSRC_INTERNAL		2

#define OMAP_MCBSP_CLKTXSRC_EXTERNAL		1
#define OMAP_MCBSP_CLKTXSRC_INTERNAL		2

/* Justification */
#define OMAP_MCBSP_RJUST_ZEROMSB		0
#define OMAP_MCBSP_RJUST_SIGNMSB		1
#define OMAP_MCBSP_LJUST_ZEROLSB		2

#define OMAP_MCBSP_DATADELAY0			0
#define OMAP_MCBSP_DATADELAY1			1
#define OMAP_MCBSP_DATADELAY2			2

/* Reverse mode for 243X and 34XX */
#define OMAP_MCBSP_MSBFIRST			0
#define OMAP_MCBSP_LSBFIRST			1

/* Multi-Channel partition mode */
#define OMAP_MCBSP_TWOPARTITION_MODE		0
#define OMAP_MCBSP_EIGHTPARTITION_MODE		1

/* Rx Multichannel selection */
#define OMAP_MCBSP_RXMUTICH_DISABLE		0
#define OMAP_MCBSP_RXMUTICH_ENABLE		1

/* Tx Multichannel selection */
#define OMAP_MCBSP_TXMUTICH_DISABLE		0
#define OMAP_MCBSP_TXMUTICH_ENABLE		1

#define OMAP_MCBSP_FRAMELEN_N(NUM_WORDS)	((NUM_WORDS - 1) & 0x7F)

static int start_mcbsp_transmission(void);
struct mcbsp_info_struct {
	int mcbsp_id;
	int mode; /* Master or Slave */
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

struct omap_mcbsp_cfg_param {
	u8 fsync_src;
	u8 fs_polarity;
	u8 clk_polarity;
	u8 clk_mode;
	u8 frame_length1;
	u8 frame_length2;
	u8 word_length1;
	u8 word_length2;
	u8 justification;
	u8 reverse_compand;
	u8 phase;
	u8 data_delay;
};
struct omap_mcbsp_srg_fsg_cfg {
	u32 period;     /* Frame period */
	u32 pulse_width; /* Frame width */
	u8 fsgm;
	u32 sample_rate;
	u32 bits_per_sample;
	u32 srg_src;
	u8 sync_mode;   /* SRG free running mode */
	u8 polarity;
	u8 dlb;         /* digital loopback mode */
};

#if 0
struct omap_mcbsp_dma_transfer_parameters {
	/* Skip the alternate element use fro stereo mode */
	u8 skip_alt;
	/* Automagically handle Transfer [XR]RST? */
	u8   auto_reset;
	/* word length of data */
	u32 word_length1;

} omap_mcbsp_dma_transfer_params;
#endif

static struct mcbsp_info_struct mcbsptest_info;

struct omap_mcbsp_cfg_param tp1;
struct omap_mcbsp_cfg_param rp1;
struct omap_mcbsp_srg_fsg_cfg   cfg;

/* Module parameters are initialized to default values */

static int buffer_size   = 256;
static int no_of_trans   = 128;
static int no_of_words_tx;
static int no_of_words_rx;
static int sample_rate   = 8000;

static int phase         = OMAP_MCBSP_FRAME_SINGLEPHASE;
static int clkr_polarity = OMAP_MCBSP_CLKR_POLARITY_RISING;
static int fsr_polarity  = OMAP_MCBSP_FS_ACTIVE_LOW;
static int clkx_polarity = OMAP_MCBSP_CLKX_POLARITY_RISING;
static int fsx_polarity  = OMAP_MCBSP_FS_ACTIVE_LOW;
static int justification = OMAP_MCBSP_RJUST_ZEROMSB;
static int word_length1   = OMAP_MCBSP_WORD_8;
static int word_length2   = OMAP_MCBSP_WORD_8;
static int test_mcbsp_id = OMAP_MCBSP1;
static int words_per_frame = 1;
long int tx_sec;
long int tx_usec;

module_param(buffer_size, int, 0);
module_param(no_of_trans, int , 0);
module_param(no_of_words_tx, int , 0);
module_param(no_of_words_rx, int, 0);
module_param(sample_rate, int, 0);
module_param(phase, int, 0);
module_param(clkr_polarity, int, 0);
module_param(fsr_polarity, int , 0);
module_param(clkx_polarity, int, 0);
module_param(fsx_polarity, int , 0);
module_param(justification, int, 0);
module_param(word_length1, int , 0);
module_param(word_length2, int , 0);
module_param(words_per_frame, int, 0);
module_param(test_mcbsp_id, int, 0);

unsigned int bits_per_sample[6] = {8, 12, 16, 20, 24, 32};
struct timeval tx_start_time, tx_end_time;
/* Proc interface declaration */
static struct proc_dir_entry  *mcbsp_test_dir, *transmission_file, *status_file;
static int file_type[2] = {0, 1};


struct omap_mcbsp_reg_cfg mcbsp_cfg, regs;

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
	p += sprintf(p, "=============================================== \n\n");
	p += sprintf(p, "           OMAP2 MCBSP TEST STATUS                \n");
	p += sprintf(p, "=============================================== \n\n");
	p += sprintf(p, "McBSP ID                                 : %8d \n",
								test_mcbsp_id);
	p += sprintf(p, "Buffer size (bytes)                      : %8d \n",
								buffer_size);
	p += sprintf(p, "Number of transfers                      : %8d \n",
								no_of_trans);
	p += sprintf(p, "No. of words transmitted               : %8d \n",
								no_of_words_tx);
	p += sprintf(p, "No. of words received                  : %8d \n",
								no_of_words_rx);
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

	if (!buffer || (count == 0))
		return 0;

	len = (count > 7) ? 7 : count;
	for (i = 0; i < len; i++)
		val[i] = buffer[i];
	val[i] = '\0';

	if (strncmp(val, "start", 4) == 0)
		start_mcbsp_transmission();
	else if (strncmp(val, "stop", 4) == 0) {
		omap_mcbsp_stop(mcbsptest_info.mcbsp_id);
		printk(KERN_INFO "McBSP%d Stopped\n", mcbsptest_info.mcbsp_id);
	}
	else if (strncmp(val, "suspend", 4) == 0)
		return 0;
	else if (strncmp(val, "resume", 4) == 0)
		return 0;
	else
		return -EINVAL;

	return count;
}

static int create_proc_file_entries(void)
{
	mcbsp_test_dir = proc_mkdir("driver/mcbsp_test", NULL);
	if (!mcbsp_test_dir) {
		printk(KERN_ERR "\n No mem to create proc file \n");
		goto no_dir;
	}

	transmission_file = create_proc_entry("transmission",
						0644, mcbsp_test_dir);
	if (!transmission_file)
		goto no_transmission;

	transmission_file->data = &file_type[0];

	status_file = create_proc_entry("status", 0644, mcbsp_test_dir);
	if (!status_file)
		goto no_status;

	status_file->data = &file_type[0];

	status_file->read_proc = read_proc_status;
	transmission_file->write_proc = write_proc_entry;

	return 0;

no_status:
	remove_proc_entry("status", mcbsp_test_dir);
no_transmission:
	remove_proc_entry("transmission", mcbsp_test_dir);
no_dir:
	remove_proc_entry("driver/mcbsp_test", NULL);
	return -ENOMEM;
}

static void remove_proc_file_entries(void)
{
	remove_proc_entry("transmission", mcbsp_test_dir);
	remove_proc_entry("status", mcbsp_test_dir);
	remove_proc_entry("driver/mcbsp_test", NULL);
}

static void configure_mcbsp_interface(void)
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
}

static void configure_mcbsp_tx(void)
{
#if 0
	struct omap_mcbsp_dma_transfer_parameters tp;
	/* Configure transfer params */
	tp.skip_alt = OMAP_MCBSP_SKIP_NONE;
	tp.auto_reset = OMAP_MCBSP_AUTO_RST_NONE;
	tp.word_length1 = word_length1;
#endif
	clkx_polarity = clkr_polarity;
	fsx_polarity = fsr_polarity;

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
}

static void configure_mcbsp_rx(void)
{
#if 0
	struct omap_mcbsp_dma_transfer_parameters rp;

	/* Configure receiver params */
	printk(KERN_INFO "\n configuring mcbsp rx \n");
	rp.skip_alt = OMAP_MCBSP_SKIP_NONE;
	rp.auto_reset = OMAP_MCBSP_AUTO_RST_NONE;
	rp.word_length1 = word_length1;
#endif

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
}

void omap2_mcbsp_set_trans_param(unsigned int id,
				struct omap_mcbsp_reg_cfg *mcbsp_cfg,
				struct omap_mcbsp_cfg_param *tp)
{
	mcbsp_cfg->xcr2 = XCOMPAND(tp->reverse_compand) |
					XDATDLY(tp->data_delay);
	if (tp->phase == OMAP_MCBSP_FRAME_SINGLEPHASE)
		mcbsp_cfg->xcr2 = mcbsp_cfg->xcr2 & ~(XPHASE);
	else
		mcbsp_cfg->xcr2 = mcbsp_cfg->xcr2 | (XPHASE) |
			RWDLEN2(tp->word_length2) | RFRLEN2(tp->frame_length2);
	mcbsp_cfg->xcr1 = XWDLEN1(tp->word_length1) |
		XFRLEN1(tp->frame_length1);
	if (tp->fs_polarity == OMAP_MCBSP_FS_ACTIVE_LOW)
		mcbsp_cfg->pcr0 = mcbsp_cfg->pcr0 | FSXP;
	if (tp->fsync_src == OMAP_MCBSP_TXFSYNC_INTERNAL)
		mcbsp_cfg->pcr0 = mcbsp_cfg->pcr0 | FSXM;
	if (tp->clk_mode == OMAP_MCBSP_CLKTXSRC_INTERNAL)
		mcbsp_cfg->pcr0 = mcbsp_cfg->pcr0 | CLKXM;
	if (tp->clk_polarity == OMAP_MCBSP_CLKX_POLARITY_FALLING)
		mcbsp_cfg->pcr0 = mcbsp_cfg->pcr0 | CLKXP;
}

void omap2_mcbsp_set_recv_param(unsigned int id,
				struct omap_mcbsp_reg_cfg *mcbsp_cfg,
				struct omap_mcbsp_cfg_param *rp)
{
	mcbsp_cfg->spcr1 = RJUST(rp->justification);
	mcbsp_cfg->rcr2 = RCOMPAND(rp->reverse_compand) |
				RDATDLY(rp->data_delay);
	if (rp->phase == OMAP_MCBSP_FRAME_SINGLEPHASE)
		mcbsp_cfg->rcr2 = mcbsp_cfg->rcr2 & ~(RPHASE);
	else
		mcbsp_cfg->rcr2 = mcbsp_cfg->rcr2  | (RPHASE) |
			RWDLEN2(rp->word_length2) | RFRLEN2(rp->frame_length2);
	mcbsp_cfg->rcr1 = RWDLEN1(rp->word_length1) |
		RFRLEN1(rp->frame_length1);
	if (rp->fsync_src == OMAP_MCBSP_RXFSYNC_INTERNAL)
		mcbsp_cfg->pcr0 = mcbsp_cfg->pcr0 | FSRM;
	if (rp->clk_mode == OMAP_MCBSP_CLKRXSRC_INTERNAL)
		mcbsp_cfg->pcr0 = mcbsp_cfg->pcr0 | CLKRM;
	if (rp->clk_polarity == OMAP_MCBSP_CLKR_POLARITY_RISING)
		mcbsp_cfg->pcr0 = mcbsp_cfg->pcr0 | CLKRP;
	if (rp->fs_polarity == OMAP_MCBSP_FS_ACTIVE_LOW)
		mcbsp_cfg->pcr0 = mcbsp_cfg->pcr0 | FSRP;
}
void omap2_mcbsp_set_srg_cfg_param(unsigned int id, int interface_mode,
					struct omap_mcbsp_reg_cfg *mcbsp_cfg,
					struct omap_mcbsp_srg_fsg_cfg *param)
{
	u32 clk_rate, clkgdv;

	mcbsp_cfg->srgr1 = FWID(param->pulse_width);

	if (interface_mode == OMAP_MCBSP_MASTER) {
		/* clk_rate = clk_get_rate(omap_mcbsp_clk[id].fck); */
		clk_rate = 96000000;
		clkgdv = clk_rate / (param->sample_rate *
				(param->bits_per_sample - 1));
		mcbsp_cfg->srgr1 = mcbsp_cfg->srgr1 | CLKGDV(clkgdv);
	}
	if (param->dlb)
		mcbsp_cfg->spcr1 = mcbsp_cfg->spcr1 & ~(ALB);

	if (param->sync_mode == OMAP_MCBSP_SRG_FREERUNNING)
		mcbsp_cfg->spcr2 = mcbsp_cfg->spcr2 | FREE;
	mcbsp_cfg->srgr2 = FPER(param->period) | (param->fsgm ? FSGM : 0);

	switch (param->srg_src) {

	case OMAP_MCBSP_SRGCLKSRC_CLKS:
		mcbsp_cfg->pcr0 = mcbsp_cfg->pcr0 & ~(SCLKME);
		mcbsp_cfg->srgr2 = mcbsp_cfg->srgr2 & ~(CLKSM);
		/*
		 * McBSP master operation at low voltage is only possible if
		 * CLKSP=0 In Master mode, if client driver tries to configiure
		 * input clock polarity as falling edge, we force it to Rising
		 */

		if ((param->polarity == OMAP_MCBSP_CLKS_POLARITY_RISING) ||
					(interface_mode == OMAP_MCBSP_MASTER))
			mcbsp_cfg->srgr2 = mcbsp_cfg->srgr2  & ~(CLKSP);
		else
			mcbsp_cfg->srgr2 = mcbsp_cfg->srgr2  |  (CLKSP);
		break;


	case OMAP_MCBSP_SRGCLKSRC_FCLK:
		mcbsp_cfg->pcr0 = mcbsp_cfg->pcr0 & ~(SCLKME);
		mcbsp_cfg->srgr2 = mcbsp_cfg->srgr2 | (CLKSM);

		break;

	case OMAP_MCBSP_SRGCLKSRC_CLKR:
		mcbsp_cfg->pcr0 = mcbsp_cfg->pcr0   | (SCLKME);
		mcbsp_cfg->srgr2 = mcbsp_cfg->srgr2 & ~(CLKSM);
		if (param->polarity == OMAP_MCBSP_CLKR_POLARITY_FALLING)
			mcbsp_cfg->pcr0 = mcbsp_cfg->pcr0  & ~(CLKRP);
		else
			mcbsp_cfg->pcr0 = mcbsp_cfg->pcr0  | (CLKRP);
		break;

	case OMAP_MCBSP_SRGCLKSRC_CLKX:
		mcbsp_cfg->pcr0 = mcbsp_cfg->pcr0   | (SCLKME);
		mcbsp_cfg->srgr2 = mcbsp_cfg->srgr2 | (CLKSM);

		if (param->polarity == OMAP_MCBSP_CLKX_POLARITY_RISING)
			mcbsp_cfg->pcr0 = mcbsp_cfg->pcr0  & ~(CLKXP);
		else
			mcbsp_cfg->pcr0 = mcbsp_cfg->pcr0  | (CLKXP);
		break;

	}

	if (param->sync_mode == OMAP_MCBSP_SRG_FREERUNNING)
		mcbsp_cfg->srgr2 = mcbsp_cfg->srgr2 & ~(GSYNC);
	else if (param->sync_mode == OMAP_MCBSP_SRG_RUNNING)
		mcbsp_cfg->srgr2 = mcbsp_cfg->srgr2 | (GSYNC);

	if (param->dlb)
		mcbsp_cfg->xccr = mcbsp_cfg->xccr | (DILB);
}


int omap2_mcbsp_params_cfg(unsigned int id, int interface_mode,
				struct omap_mcbsp_cfg_param *tp,
				struct omap_mcbsp_cfg_param *rp,
				struct omap_mcbsp_srg_fsg_cfg *param)
{
	if (rp)
		omap2_mcbsp_set_recv_param(id, &mcbsp_cfg, rp);
	if (tp)
		omap2_mcbsp_set_trans_param(id, &mcbsp_cfg, tp);
	if (param)
		omap2_mcbsp_set_srg_cfg_param(id,
				interface_mode, &mcbsp_cfg, param);
	omap_mcbsp_config(id, &mcbsp_cfg);
	omap_mcbsp_start(id);
	return 0;
}

/* transmit mode = receive (0) or transmit (1) */
static int start_mcbsp_transmission(void)
{
	int j, k, data1, data2, ret, tx_err = 0, rx_err = 0;
	u32 temp;

	if (word_length1 == OMAP_MCBSP_WORD_8)
		data1 = 0x80;
	else if (word_length1 == OMAP_MCBSP_WORD_16)
		data1 = 0xA500;
	else if (word_length1 == OMAP_MCBSP_WORD_32)
		data1 = 0x5A5A0000;

	if (word_length2 == OMAP_MCBSP_WORD_8)
		data2 = 0x80;
	else if (word_length2 == OMAP_MCBSP_WORD_16)
		data2 = 0xA500;
	else if (word_length2 == OMAP_MCBSP_WORD_32)
		data2 = 0xA5A50000;

	/* Configure the Master, it should generate the FSX and CLKX */
	configure_mcbsp_tx();
	configure_mcbsp_rx();
	omap2_mcbsp_params_cfg(mcbsptest_info.mcbsp_id, OMAP_MCBSP_MASTER,
							&tp1, &rp1, &cfg);

	for (j = 0, no_of_words_tx = 0; j < no_of_trans; j += 128) {
		for (k = 0; k < 128; k++) {
			if ((j+k) >= no_of_trans)
				break;
			ret = omap_mcbsp_pollwrite(mcbsptest_info.mcbsp_id,
						(data1 + k));
			if (ret)
				tx_err = -1;
			else
				no_of_words_tx++;

			k++;
			if ((j+k) >= no_of_trans)
				break;
			ret = omap_mcbsp_pollwrite(mcbsptest_info.mcbsp_id,
						(data2 + k));
			if (ret)
				tx_err = -1;
			else
				no_of_words_tx++;
		}

		for (k = 0; k < 128; k++) {
			if ((j+k) >= no_of_trans)
				break;
			ret = omap_mcbsp_pollread(mcbsptest_info.mcbsp_id,
							&temp);
			if (ret)
				rx_err = -2;
			else if ((justification == OMAP_MCBSP_RJUST_ZEROMSB) &&
					(temp == (data1 + k)))
				no_of_words_rx++;
			else if ((justification == OMAP_MCBSP_LJUST_ZEROLSB) &&
					(temp == ((data1 + k) << 24)))
				no_of_words_rx++;
			else if ((justification == OMAP_MCBSP_RJUST_SIGNMSB) &&
					(temp == ((data1 + k) | 0xFFFFFF00)))
				no_of_words_rx++;
			else {
				rx_err = -1;
				no_of_words_rx++;
			}

			k++;
			if ((j+k) >= no_of_trans)
				break;
			ret = omap_mcbsp_pollread(mcbsptest_info.mcbsp_id,
							&temp);
			if (ret)
				rx_err = -2;
			else if ((justification == OMAP_MCBSP_RJUST_ZEROMSB) &&
					(temp == (data2 + k)))
				no_of_words_rx++;
			else if ((justification == OMAP_MCBSP_LJUST_ZEROLSB) &&
					(temp == ((data2 + k) << 24)))
				no_of_words_rx++;
			else if ((justification == OMAP_MCBSP_RJUST_SIGNMSB) &&
					(temp == ((data2 + k) | 0xFFFFFF00)))
				no_of_words_rx++;
			else {
				rx_err = -1;
				no_of_words_rx++;
			}
		}
	}

	if (tx_err == -1)
		printk(KERN_ERR "\nData-Tx Failed\n");
	else
		printk(KERN_ERR "\nData-Tx Success\n");

	if (rx_err == -2)
		printk(KERN_ERR "\nData-Rx Failed\n");
	else if (rx_err == -1)
		printk(KERN_ERR "\nFailed! Data Mismatch Error\n");
	else
		printk(KERN_ERR "\nData-Rx Success\n");

	return 0;
}
#if 0
	mcbsptest_info.tx_buf_dma_virt = (void *) dma_alloc_coherent(NULL,
				buffer_size, &mcbsptest_info.tx_buf_dma_phys,
							GFP_KERNEL | GFP_DMA);

	mcbsptest_info.rx_buf_dma_virt = (void *) dma_alloc_coherent(NULL,
				buffer_size, &mcbsptest_info.rx_buf_dma_phys,
							GFP_KERNEL | GFP_DMA);

	ptr = mcbsptest_info.tx_buf_dma_virt;
	for (k = 0; k < buffer_size; k++) {
		ptr[k] = 0xAA;
		k = k + 1;
	}

	ret = omap_mcbsp_xmit_buffer(mcbsptest_info.mcbsp_id,
				mcbsptest_info.tx_buf_dma_phys, buffer_size);
	if (ret) {
		printk(KERN_ERR "McBSP Test Driver: Slave Send data failedn");
		return -1;
	}

	ptr = mcbsptest_info.tx_buf_dma_virt;
	printk(KERN_INFO " ptr = 0x%x \n", *ptr);

	ret = omap_mcbsp_recv_buffer(mcbsptest_info.mcbsp_id,
				mcbsptest_info.rx_buf_dma_phys, buffer_size);
	if (ret) {
		printk(KERN_ERR "McBSP Test Driver: Slave Send data failedn");
		return -1;
	}

	printk(KERN_INFO "\n i am in %s at %d\n", __func__, __LINE__);

	ptr = mcbsptest_info.rx_buf_dma_virt;
	printk(KERN_INFO " ptr = 0x%x \n", *ptr);
#endif

static void fill_global_structure(void)
{
	mcbsptest_info.mcbsp_id = test_mcbsp_id;
	mcbsptest_info.mode     = OMAP_MCBSP_MASTER ; /* Master or Slave */
	mcbsptest_info.rx_cnt   = 0;
	mcbsptest_info.tx_cnt   = 0;
	mcbsptest_info.num_of_tx_transfers = 256;
	mcbsptest_info.num_of_rx_transfers = 256;
	mcbsptest_info.send_cnt   = 0;
	mcbsptest_info.recv_cnt   = 0;
}

static int __init omap2_mcbsp_init(void)
{
	int ret;

	fill_global_structure();
	omap_mcbsp_set_io_type(mcbsptest_info.mcbsp_id, 0);

	/* Requesting interface */
	ret = omap_mcbsp_request(mcbsptest_info.mcbsp_id);
	if (ret) {
		printk(KERN_ERR "McBSP Test Driver:"
				"Requesting mcbsp interface failed\n");
		return -1;
	}

	configure_mcbsp_interface();
	create_proc_file_entries();
	printk(KERN_INFO "\n OMAP McBSP TEST driver installed successfully \n");
	return 0;
}

static
void __exit omap_mcbsp_exit(void)
{
#if (LINUX_VERSION_CODE <= KERNEL_VERSION(2, 6, 00))
	consistent_free((void *)mcbsptest_info.rx_buf_dma_virt, buffer_size,
						mcbsptest_info.rx_buf_dma_phys);
	consistent_free((void *)mcbsptest_info.tx_buf_dma_virt, buffer_size,
						mcbsptest_info.tx_buf_dma_phys);
#else
	if (mcbsptest_info.rx_buf_dma_virt != NULL ||
			mcbsptest_info.tx_buf_dma_virt != NULL) {
		dma_free_coherent(NULL, buffer_size,
				(void *)mcbsptest_info.rx_buf_dma_virt,
					mcbsptest_info.rx_buf_dma_phys);
		dma_free_coherent(NULL, buffer_size,
				(void *)mcbsptest_info.tx_buf_dma_virt,
					mcbsptest_info.tx_buf_dma_phys);
	}

#endif

	omap_mcbsp_free(mcbsptest_info.mcbsp_id);
	remove_proc_file_entries();
	return ;
}

module_init(omap2_mcbsp_init);
module_exit(omap_mcbsp_exit);

MODULE_AUTHOR("Texas Instruments");
MODULE_DESCRIPTION("McBSP Test Driver");
MODULE_LICENSE("GPL");
