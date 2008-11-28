/*
* ================================================================================
*  sdio.c
*  Test module for SDIO driver
* 
* * Texas Instruments OMAP(TM) Platform Software
* * (c) Copyright Texas Instruments, Incorporated. All Rights Reserved.
* *
* * Use of this software is controlled by the terms and conditions found
* * in the license agreement under which this software has been supplied.
* *
* ================================================================================
* */
#include <linux/init.h>
#include <linux/module.h>
#include <linux/mmc/mmc.h>
#include <linux/mmc/host.h>
#include <linux/slab.h>
#include <asm/arch/dma.h>
#include <asm/uaccess.h>        // get_fs(), set_fs(), KERNEL_DS
#include <linux/file.h>         // fput()
#include <linux/mm.h>           // GFP_KERNEL

MODULE_LICENSE("GPL");

#define WRITE 1
#define READ  0

/*SDIO card CCCR register offsets.
 */
#define CCCR_SDIO_REV   0x00
#define SD_SPEC_REV     0x01
#define IO_ENABLE       0x02
#define IO_READY        0x03
#define INT_ENABLE      0x04
#define INT_PENDING     0x05
#define IO_ABORT        0x06
#define BUS_INTF_CTRL   0x07
#define CARD_CAPABILITY 0x08
#define COMMON_CIS_PTR1 0x09
#define COMMON_CIS_PTR2 0x0A
#define COMMON_CIS_PTR3 0x0B
#define BUS_SUSPEND     0x0C
#define FUNC_SELECT     0x0D
#define EXEC_FLAGS      0x0E
#define READY_FLAGS     0x0F
#define FN0_BLK_SIZE1   0x10
#define FN0_BLK_SIZE2   0x11

/*SDIO Function numbers.
 */
#define FUNC_0 0
#define FUNC_1 1
#define FUNC_2 2
#define FUNC_3 3
#define FUNC_4 4
#define FUNC_5 5
#define FUNC_6 6

#define BLK_MODE  1
#define BYTE_MODE 0

#define INC_OPCODE      1
#define CONSTANT_OPCODE 0

int buf[128];
int buf_size;
static int block_mode;
unsigned char cia[18];
extern int io_rw_extended (struct mmc_request *mrq, int rw, int func, int blkm,
			   int opcode, unsigned long regaddr, int bytec);
int io_rw_direct (int rw, int func,int rawFlag, unsigned long regaddr, int value);
int sdio_test(void);

// Write to file
int write_to_file(struct file *f, char *buffer, int pos);

struct io_blk_request {
	struct mmc_request mrq;
	struct mmc_command cmd;
	struct mmc_data data;
};

struct io_blk_request *sdioq;

static int
sdio_init (void){
        int i;

        printk("\n-- SDIO Test module--\n");	
	for (i=0;i<2;i++)
	    sdio_test();
	return 0;
}
int sdio_test(void)
{
	u32 *readValue;
        u32 *strvalue;
	int i,j=0, readCnt,err;
	int nob = 0x12;
	int card_cap;
        struct file *f = NULL;
        mm_segment_t orig_fs;
        int pos;
        char buffer[100];

        /* Obtain a file object pointer */
        f = filp_open("/sdio.msg", O_WRONLY | O_CREAT, 0600);
        if (!f || !f->f_op || !f->f_op->write) {
                printk("WARNING: File (write) object is a null pointer!!!\n");
        }
        f->f_pos = 0;
        pos = 0;
        orig_fs = get_fs();
        set_fs(KERNEL_DS);

	sdioq = kmalloc (sizeof (struct io_blk_request), GFP_KERNEL);
	if (!sdioq) {
		printk ("\n Failed to allocate memory");
		return 0;
	}


	memset (sdioq, 0, sizeof (struct io_blk_request));
	sdioq->mrq.cmd = &sdioq->cmd;

        sdioq->data.sdio_buffer_virt=(void *)kmalloc(512, GFP_KERNEL|GFP_DMA);
	if (!sdioq->data.sdio_buffer_virt)
	       printk("\n DMA Allocation failure\n");	
	
	sdioq->mrq.data = &sdioq->data;
	sdioq->data.mrq = &sdioq->mrq;
	
	readValue = (u32 *)  sdioq->data.sdio_buffer_virt;
        strvalue=readValue;
	readCnt = (nob / 4);
	if (nob % 4)
		readCnt++;

	err = io_rw_direct(WRITE,0,0,FUNC_SELECT,FUNC_0);  
	if (err == 1){
	   printk(KERN_ERR"CMD52 error\n");
	   return 0;
	}   
	
	card_cap = io_rw_direct(READ,0,0,CARD_CAPABILITY,0x0); /* Read card capability */

	if (block_mode == 1){
	if (card_cap & (1<<1)){
		printk("The card supports block mode operation\n");
                printk("Reading CCCR registers in block mode\n");	
	        io_rw_direct(WRITE,0,0,FN0_BLK_SIZE1,nob);
	        io_rw_extended (&sdioq->mrq, READ, FUNC_0, BLK_MODE, INC_OPCODE, CCCR_SDIO_REV, 0x1);
	}
	else {
		printk("The card does not support block mode operation\n");
		return 0;
	  }	
	}
	if (block_mode == 2){
	if (card_cap & (1<<1)){
		printk("The card supports block mode operation\n");
                printk("Reading CCCR registers in block mode\n");	
	        io_rw_direct(WRITE,0,0,FN0_BLK_SIZE1,0x10);
	        io_rw_extended (&sdioq->mrq, 0, 0, 1, 1, 0x0, 0x1);
	        io_rw_direct(WRITE,0,0,FN0_BLK_SIZE1,0x10);
	        io_rw_extended (&sdioq->mrq, READ, FUNC_0, BLK_MODE, INC_OPCODE, CCCR_SDIO_REV, 0x1);
	}
	else {
		printk("The card does not support block mode operation\n");
		return 0;
	  }	
	}
	
        printk("\nReading CCCR registers bytewise\n");	
	io_rw_direct(WRITE,0,0,FN0_BLK_SIZE1,nob);
	io_rw_extended (&sdioq->mrq, READ, FUNC_0, BYTE_MODE, INC_OPCODE, CCCR_SDIO_REV, nob);
	

	for (i = 0; i < readCnt; i++){
		buf[i] = *readValue++;
	}
	
	for (i = 0; i < readCnt; i++){
		cia[j++] = (buf[i] & 0x000000ff);
		cia[j++] = (buf[i] & 0x0000ff00) >> 8;
		cia[j++] = (buf[i] & 0x00ff0000) >> 16;
		cia[j++] = (buf[i] & 0xff000000) >> 24;
	}
	
	printk("\nThe card common control registers\n");
	printk("\n");
        printk("CCCR/SDIO Revision register 0x%02x\n",cia[0]);
        printk("SD specification revision register 0x%02x\n",cia[1]);
        printk("I/O enable register 0x%02x\n",cia[2]);
        printk("I/O ready register 0x%02x\n",cia[3]);
        printk("Interrupt enable register 0x%02x\n",cia[4]);
        printk("Interrupt pending register 0x%02x\n",cia[5]);
        printk("I/O abort register 0x%02x\n",cia[6]);
        printk("BUS interface control register 0x%02x\n",cia[7]);
        printk("Card capacity register 0x%02x\n",cia[8]);
        printk("Common CIS pointer 0x%02x\n",cia[9]);
        printk("Common CIS pointer 0x%02x\n",cia[10]);
        printk("Common CIS pointer 0x%02x\n",cia[11]);
        printk("BUS suspend 0x%02x\n",cia[12]);
        printk("Function select 0x%02x\n",cia[13]);
        printk("Exec flags 0x%02x\n",cia[14]);
        printk("Ready flags 0x%02x\n",cia[15]);
        printk("FN0 Block size 0x%02x\n",cia[16]);
        printk("FN0 Block size 0x%02x\n",cia[17]);
	printk("\n");

        /* Write SDIO information to a file */
        sprintf(buffer, "CCCR/SDIO Revision register 0x%02x\n",cia[0]);
        pos = write_to_file(f, buffer, pos);
        sprintf(buffer, "SD specification revision register 0x%02x\n",cia[1]);
        pos = write_to_file(f, buffer, pos);
        sprintf(buffer, "I/O enable register 0x%02x\n",cia[2]);
        pos = write_to_file(f, buffer, pos);
        sprintf(buffer, "I/O ready register 0x%02x\n",cia[3]);
        pos = write_to_file(f, buffer, pos);
        sprintf(buffer, "Interrupt enable register 0x%02x\n",cia[4]);
        pos = write_to_file(f, buffer, pos);
        sprintf(buffer, "Interrupt pending register 0x%02x\n",cia[5]);
        pos = write_to_file(f, buffer, pos);
        sprintf(buffer, "I/O abort register 0x%02x\n",cia[6]);
        pos = write_to_file(f, buffer, pos);
        sprintf(buffer, "BUS interface control register 0x%02x\n",cia[7]);
        pos = write_to_file(f, buffer, pos);
        sprintf(buffer, "Card capacity register 0x%02x\n",cia[8]);
        pos = write_to_file(f, buffer, pos);
        sprintf(buffer, "Common CIS pointer 0x%02x\n",cia[9]);
        pos = write_to_file(f, buffer, pos);
        sprintf(buffer, "Common CIS pointer 0x%02x\n",cia[10]);
        pos = write_to_file(f, buffer, pos);
        sprintf(buffer, "Common CIS pointer 0x%02x\n",cia[11]);
        pos = write_to_file(f, buffer, pos);
        sprintf(buffer, "BUS suspend 0x%02x\n",cia[12]);
        pos = write_to_file(f, buffer, pos);
        sprintf(buffer, "Function select 0x%02x\n",cia[13]);
        pos = write_to_file(f, buffer, pos);
        sprintf(buffer, "Exec flags 0x%02x\n",cia[14]);
        pos = write_to_file(f, buffer, pos);
        sprintf(buffer, "Ready flags 0x%02x\n",cia[15]);
        pos = write_to_file(f, buffer, pos);
        sprintf(buffer, "FN0 Block size 0x%02x\n",cia[16]);
        pos = write_to_file(f, buffer, pos);
        sprintf(buffer, "FN0 Block size 0x%02x\n",cia[17]);
        pos = write_to_file(f, buffer, pos);

	return 0;

}
static void
sdio_exit (void)
{
	kfree(sdioq->data.sdio_buffer_virt);
	kfree(sdioq);
}

// Write to file
int write_to_file(struct file *f, char *buffer, int pos) {
        int i, len, bufsize=0;
        const int MAXBUFSIZE=100;

        for (i=0; i<MAXBUFSIZE; i++) {
                if (buffer[i]=='\0')
                        break;
                bufsize++;
        }

        f->f_pos = pos;
        len = f->f_op->write(f, buffer, bufsize, &f->f_pos);

        return pos+len;
}

module_param(block_mode, int, S_IRUGO);
module_init (sdio_init);
module_exit (sdio_exit);
