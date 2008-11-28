/*********************************************************************
 *                
 * Filename:      winbond.c
 * Version:       0.1
 * Description:   Hardware probe for Winbond infrared controllers
 * Status:        Experimental.
 * Author:        Takahide Higuchi <thiguchi@pluto.dti.ne.jp>
 * Created at:    Tue Jan 18 00:23:00 2000
 * Modified at:   Tue Jan 18 00:35:04 2000
 * Modified by:   Takahide Higuchi <thiguchi@pluto.dti.ne.jp>
 * 
 *     Copyright (c) 2000 Takahide Higuchi, All Rights Reserved.
 *     
 *     This program is free software; you can redistribute it and/or 
 *     modify it under the terms of the GNU General Public License as 
 *     published by the Free Software Foundation; either version 2 of 
 *     the License, or (at your option) any later version.
 * 
 *     This program is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *     GNU General Public License for more details.
 * 
 *     You should have received a copy of the GNU General Public License 
 *     along with this program; if not, write to the Free Software 
 *     Foundation, Inc., 59 Temple Place, Suite 330, Boston, 
 *     MA 02111-1307 USA
 *     
 ********************************************************************/

#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <asm/io.h>

#include "findchip.h"

#define MANUFACTURER "WINBOND"

#define	CR07	0x07
#define	CR09	0x09
#define	CR0B	0x0B
#define	CR0C	0x0C
#define	CR0D	0x0D
#define	CR16	0x16
#define	CR20	0x20
#define	CR21	0x21
#define	CR25	0x25
#define	CR26	0x26
#define	CR28	0x28
#define	CR2A	0x2A
#define	CR2C	0x2C

#define	CR60	0x60
#define	CR61	0x61
#define	CR70	0x70
#define	CR74	0x74
#define	CR75	0x75
#define	CRF0	0xF0


#define W83977_DEVICE_ID         0x97


/* bitmasks */
#define W83877_HEFRAS_MASK       0x01 
#define W83877_HEFERE_MASK       0x20
#define W83977_HEFRAS_MASK       0x40

#define CHIPID                   0x0F
#define IRTXDRQSEL               0xF0
#define IRRXDRQSEL               0x0F
#define URBIQS                   0x0F
#define URBAD                    0xFE
#define TXW4C                    0x40
#define RXW4C                    0x20
#define RXW4C_977                0x08
#define TXW4C_977                0x04

#define ENBKIRSL                 0x10
#define URIRSEL                  0x08
#define HDUPLX                   0x08
#define IRMODE                   0x07

#define ENBNKSEL                 0x08
#define ENBNKSEL_977             0x01

/* indexs */
#define W83877_HEFRAS_INDEX      CR16
#define W83877_HEFERE_INDEX      CR0C
#define W83977_HEFRAS_INDEX      CR26
#define NOT_EXIST                -1


enum {
	FIR_SUPPORT,
	NO_FIR_SUPPORT,
};

enum {
	W83977_PNP, /* 
		       not implemented yet :
		       W83977 chip will be Comply-PNP mode 
		       if PIN #51 (~DTRB) is pulled up 
		     */
	W83977,     /* compatible-PNP mode */
	W83877, 
};
	
struct winbond_chip {
	char *name;
	int chip_id;        /* Device Rev (CR21) for 83977s,  
			       CHIP ID (CR09) for 83877s */
	int fir_support;    /* SUPPORT_FIR or 0 */
};

struct winbond_efer {
	int chip_type;  /* W83877 , W83977 or W83977_PNP */
	int efer_addr;  /* address of Extended Function Enable Register */
	int writes;     /* write start_flag to EFER "writes" times */ 
	int start_flag;
	int end_flag;   /* finally write it to EFER */ 
	int efir_addr;  /* Extended Function Index Reg. */
	int efdr_addr;  /* Extended Function Data Reg. */

	int hefras_index;     /* Register index of HEFERE bit */
	int hefras_mask;      /* bitmask for HEFERE */
	int hefras;           /* HEFRAS bit should be (1 or 0) */

	/*  if hefere_index == -1, HEFERE will be ignored */
	int hefere_index;     /* Register index of HEFERE bit */
	int hefere_mask;      /* bitmask for HEFERE */
	int hefere;           /* HEFERE bit should be (1 or 0) */
};


typedef struct winbond_chip winbond_chip_t;
typedef struct winbond_efer winbond_efer_t;



static winbond_chip_t chips[] = {
	{ "W83977TF", 0x73,  NO_FIR_SUPPORT}, 
	{ "W83977ATF", 0x74, FIR_SUPPORT}, 

	{ "W83877AF",  0x0b, FIR_SUPPORT},
	{ "W83877TF",  0x0c, NO_FIR_SUPPORT},
	{ "W83877ATF",  0x0d, FIR_SUPPORT},


/* 
   Entries below here are not well confirmed.

   if you have one of these chips, please mail me an output of 
   "findchip -d -v WINBOND".
 */

/*	{ "W83977F/AF", 0x71, FIR_SUPPORT }, */ 
/* 	{ "W83877F",  0x0a, NO_FIR_SUPPORT}, */

	{ NULL }
};

static winbond_efer_t efers[] = {

	{ W83977,
	  0x3F0, 2, 0x87 ,0xAA, 0x3F0, 0x3F1, 
	  W83977_HEFRAS_INDEX, W83977_HEFRAS_MASK, 0,
	  NOT_EXIST, 0 ,0 },  

	{ W83977,
	  0x370, 2, 0x87 ,0xAA, 0x370, 0x371, 
	  W83977_HEFRAS_INDEX, W83977_HEFRAS_MASK, 1,
	  NOT_EXIST, 0 ,0 },


	{ W83877,
	  0x250, 1, 0x88 ,0x88, 0x251, 0x252,
	  W83877_HEFRAS_INDEX, W83877_HEFRAS_MASK, 0,
	  W83877_HEFERE_INDEX, W83877_HEFERE_MASK, 0 },
	{ W83877,
	  0x250, 1, 0x89 ,0x89, 0x251, 0x252,
	  W83877_HEFRAS_INDEX, W83877_HEFRAS_MASK, 0,
	  W83877_HEFERE_INDEX, W83877_HEFERE_MASK, 1 },
	{ W83877,
	  0x3F0, 2, 0x86 ,0xAA, 0x3F0, 0x3F1,
	  W83877_HEFRAS_INDEX, W83877_HEFRAS_MASK, 1,
	  W83877_HEFERE_INDEX, W83877_HEFERE_MASK, 0 },
	{ W83877,
	  0x3F0, 2, 0x87 ,0xAA, 0x3F0, 0x3F1,
	  W83877_HEFRAS_INDEX, W83877_HEFRAS_MASK, 1,
	  W83877_HEFERE_INDEX, W83877_HEFERE_MASK, 1 },

	{ 0 }
};




/*
 * Function w83977_read_info()
 *
 *    if Chip ID is valid, return non-zero
 *
 */

int w83977_read_info(winbond_efer_t *eferinfo)
{
	struct chip_info info;
	winbond_chip_t *chipinfo;
	int rxdma, txdma, devid, revision, misc;

	devid = 0;
	
	outb( CR20, eferinfo->efir_addr);
	devid = (inb(eferinfo->efdr_addr));
		
	outb( CR21, eferinfo->efir_addr);
	revision = (inb(eferinfo->efdr_addr));
	DEBUG("chip id = 0x%02x, revision = 0x%02x\n", devid, revision);

	if(devid != W83977_DEVICE_ID) 
	{
		DEBUG("Wrong device ID = 0x%02x\n", devid);
		return 0;
	}

	for(chipinfo = chips; chipinfo->name; chipinfo++){
		if (revision == chipinfo->chip_id)
			break;
	}

	if(chipinfo->name == NULL){
		DEBUG("Device Revision is 0x%02x :" 
		      "no known chip was detected.\n", revision);
		return 0;
	}

	info.chip_id = devid;
	info.revision = revision;
	DEBUG("Device Revision 0x%02x : %s is detected!\n",
	      info.chip_id, chipinfo->name);

	if(chipinfo->fir_support != FIR_SUPPORT)
		DEBUG("this chip does not have FIR block.n");


	/* select a logical device */
	outb( CR07, eferinfo->efir_addr);
	outb( 0x06 ,eferinfo->efdr_addr);  /* FIR block */


	/* read DMA settings */

	outb(CR74, eferinfo->efir_addr);
	rxdma = inb(eferinfo->efdr_addr);
	if(rxdma > 3){
		DEBUG(" RX DMA is inactive\n");		
		rxdma = -1;
	}

	outb(CR75, eferinfo->efir_addr);
	txdma = inb(eferinfo->efdr_addr);
	if(txdma > 3){
		DEBUG(" TX DMA is inactive, or single DMA mode\n");		
		txdma = -1;
	}

	info.dma = rxdma;

	/* read IRQ settings */

	outb(CR70, eferinfo->efir_addr);
	info.irq = inb(eferinfo->efdr_addr) & 0x0f;


	/* read I/O base address */
	outb(CR60, eferinfo->efir_addr);
	info.sir_base = (inb(eferinfo->efdr_addr) << 8);

	outb(CR61, eferinfo->efir_addr);
	info.sir_base |= (inb(eferinfo->efdr_addr) & 0xff);
	info.fir_base = info.sir_base ;  

	info.manufacturer = MANUFACTURER;
	info.name = chipinfo->name;
	info.cfg_base = eferinfo->efer_addr;
	info.uart_comp = 1;      /*  we knows it :) */

	
	/* read misc (no! very important) settings */

	outb(CRF0, eferinfo->efir_addr);
	misc = inb(eferinfo->efdr_addr) ;
	if(misc & TXW4C_977)
		DEBUG(" RX->TX Turn around delay(4 char time) is enabled\n");
	else
		DEBUG(" RX->TX Turn around delay(4 char time) is disabled\n");
	
	if(misc & RXW4C_977)
		DEBUG(" TX->RX Turn around delay(4 char time) is enabled\n");
	else
		DEBUG(" TX->RX Turn around delay(4 char time) is disabled\n");

	if(misc & ENBNKSEL_977)	{
		DEBUG(" Bank selection si enabled.\n");
		info.enabled = 1;
	} else {
		DEBUG(" Bank selection is disabled(CRF0 = 0x%02x).\n", misc);
		info.enabled = 0;
	}

	info.suspended = 0;   /* todo: handling of this */


	print_info(&info);
	return 1;
}


/*
 * Function winbond_read_info()
 *
 *    if Chip ID is valid, return non-zero
 *
 */
int w83877_read_info(winbond_efer_t *eferinfo)
{

	struct chip_info info;
	winbond_chip_t *chipinfo;
	int rxdma, txdma, chipid, misc;
	int disabled = 0;

	/* find chip_ID */

	outb( CR09, eferinfo->efir_addr);
	chipid = (inb(eferinfo->efdr_addr)) & CHIPID;
	
	for(chipinfo = chips; chipinfo->name; chipinfo++){
		if (chipid == chipinfo->chip_id)
			break;
	}

	if(chipinfo->name == NULL){
		DEBUG("chip ID is 0x%02x : no known chip was detected.\n",
		      chipid);
		return 0;
	}

	info.chip_id = chipid;
	DEBUG("chip ID is 0x%02x : %s is detected!\n",
	      info.chip_id, chipinfo->name);

	if(chipinfo->fir_support != FIR_SUPPORT)
		DEBUG("this chip does not have FIR block.n");
		

	/* read DMA settings */

	outb(CR2A, eferinfo->efir_addr);
	rxdma = inb(eferinfo->efdr_addr);
	txdma = (rxdma & IRTXDRQSEL) >> 4;
	rxdma &= IRRXDRQSEL;

	DEBUG(" RX DMA = %d, TX DMA = %d\n", rxdma, txdma);
	if( txdma == rxdma )
		info.dma = txdma;
	else
	{
		if(txdma !=0)
			/* todo: redefinition of chip_info ? */
			DEBUG(" -- Configured for Dual DMA mode\n");
	}

	if(info.dma == 0)
		DEBUG("  -- DMA is disabled.\n");		


	/* read IRQ settings */

	outb(CR28, eferinfo->efir_addr);
	info.irq = inb(eferinfo->efdr_addr) & URBIQS;

	outb(CR25, eferinfo->efir_addr);
	info.sir_base = (inb(eferinfo->efdr_addr) & URBAD) << 2;
	info.fir_base = info.sir_base ;  

	info.manufacturer = MANUFACTURER;
	info.name = chipinfo->name;
	info.cfg_base = eferinfo->efer_addr;
	info.uart_comp = 1;      /*  we knows it :) */


	/* read misc (no! very important) settings */

	outb(CR0B, eferinfo->efir_addr);
	misc = inb(eferinfo->efdr_addr) ;
	if(misc & TXW4C)
		DEBUG(" RX->TX Turn around delay(4 char time) is enabled\n");
	else
		DEBUG(" RX->TX Turn around delay(4 char time) is disabled\n");
	
	if(misc & RXW4C)
		DEBUG(" TX->RX Turn around delay(4 char time) is enabled\n");
	else
		DEBUG(" TX->RX Turn around delay(4 char time) is disabled\n");
	


	outb(CR0C, eferinfo->efir_addr);
	misc = inb(eferinfo->efdr_addr) ;
	if(misc & ENBKIRSL)
		DEBUG(" Config Registers for IR functions are activated. good.\n");
	else {
		DEBUG(" Config Registers for IR functions are "
		      "inactivated(CR0C = 0x%02x).\n", misc);
		disabled = 1;
	}

	
	if(misc & URIRSEL)
		DEBUG(" UART B is IR mode. \n");
	else {
		DEBUG(" UART B is not IR mode(CR0C = 0x%02x). \n",misc);
		disabled = 1;
	}
	

	outb(CR0D, eferinfo->efir_addr);
	misc = inb(eferinfo->efdr_addr) ;
	if(misc & HDUPLX)
		DEBUG(" Configured for Half-Duplex\n");
	else
		DEBUG(" Configured for Full-Duplex\n");

	switch(misc & IRMODE){
	case 0:
	case 1:
		DEBUG(" IR support are disabled. bad.\n");
		disabled = 1;
		break;
	
	case 2:
		DEBUG(" Configured for IrDA. (SIR pulse width = 1.6us)\n");
		break;

	case 3:
		DEBUG(" Configured for IrDA. (SIR pulse width = 3/16)\n");
		break;

	default:
		DEBUG(" Configured for ASK-IR(0x%02x). \n",misc & IRMODE);
		disabled = 1;
		break;
	}
	
	outb(CR2C, eferinfo->efir_addr);
	misc = inb(eferinfo->efdr_addr);
	if(misc & ENBNKSEL)
		DEBUG(" Bank selection is enabled. \n");
	else {
		DEBUG(" Bank selection is disabled(CR2C = 0x%02x)\n",misc);
		disabled = 1;
	}
	
	if(disabled){
		info.enabled = 0;
	}
	else
		info.enabled = 1;


	/*
	  I think we should not read revision number because it resides in 
	  Advanced UART, which may be activated as a normal UART by
	  the Linux kernel. once I've tried to read the number, but 
	  kernel got frozen :<

	  note:
	  Device ID and Device Rev do not exist in EFRs 
	  because W83877 is not ISA-PNP spec compliant
	*/
	info.revision = -1;

	info.suspended = 0;   /* todo: handling of this */


	print_info(&info);
	return 1; /* found */
}



/*
 * Function probe_winbond ()
 *
 *    
 *
 */

int probe_winbond(void)
{
	int found = 0;
	winbond_efer_t *eferinfo;
	int i, hefras, hefere;


	/* search for the address of EFIR. */
	for (eferinfo = efers; eferinfo->efer_addr && found == 0; eferinfo++) {
		DEBUG("Probing for EFER at 0x%04x ...\n", eferinfo->efer_addr);


		/* knock EFER */
		for (i = 0; i < eferinfo->writes; i++){
			outb( eferinfo->start_flag, eferinfo->efer_addr);
		}

		if(eferinfo->hefras_index >= 0)
		{
			outb( eferinfo->hefras_index, eferinfo->efir_addr);
			hefras = (inb(eferinfo->efdr_addr));

			DEBUG("hefras = 0x%02x \n", hefras);

			hefras = (hefras & eferinfo->hefras_mask) ? 1 : 0;
			if(hefras != eferinfo->hefras)
				goto skip;
		}

		if(eferinfo->hefere_index >= 0)
		{
			outb( eferinfo->hefere_index, eferinfo->efir_addr);
			hefere = (inb(eferinfo->efdr_addr));

			DEBUG("hefere = 0x%02x \n", hefere);

			hefere = (hefere & eferinfo->hefere_mask) ? 1 : 0;
			if(hefere != eferinfo->hefere)
				goto skip;
		}

		DEBUG("EFER seems to be probed at 0x%04x\n",
		      eferinfo->efer_addr);

		switch(eferinfo->chip_type){
		case W83877:
			found = w83877_read_info(eferinfo);
			break;

		case W83977:			
			found = w83977_read_info(eferinfo);
			break;

		default:
			DEBUG("BUG in the source code.. :(");
			break;
		}

	skip:

		outb( eferinfo->end_flag, eferinfo->efer_addr);

		if(found)
			return 0;

	}

	DEBUG("Couldn't find Winbond superI/O chip.\n");
	return 0;
}
