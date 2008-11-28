/*********************************************************************
 *                
 * Filename:      nsc.c
 * Version:       0.4
 * Description:   Hardware probe for NSC infrared controllers
 * Status:        Experimental.
 * Author:        Dag Brattli <dagb@cs.uit.no>
 * Created at:    Thu Jan  6 09:46:13 2000
 * Modified at:   Tue Jan 25 10:02:20 2000
 * Modified by:   Dag Brattli <dagb@cs.uit.no>
 * 
 *     Copyright (c) 1999-2000 Dag Brattli, All Rights Reserved.
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
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <asm/io.h>

#include "findchip.h"

#define MANUFACTURER "NSC"

/* Config registers for the '108 */
#define CFG_BAIC 0x00
#define CFG_CSRT 0x01
#define CFG_MCTL 0x02

/* Config registers for the '338 */
#define CFG_FER  0x00
#define CFG_FAR  0x01
#define CFG_PTR  0x02
#define CFG_PNP0 0x1b
#define CFG_PNP1 0x1c
#define CFG_PNP3 0x4f

struct nsc_chip {
	char *name;
	int cfg[3];
	u_int8_t cid_index;
	u_int8_t cid_value;
	u_int8_t cid_mask;
	void (*probe)(struct nsc_chip *nsc_chip, int cfg_base, 
		      int chip_id, int revision);
};
typedef struct nsc_chip nsc_chip_t;

void nsc_probe_108(nsc_chip_t *chip, int cfg_base, int chip_id, int revision);
void nsc_probe_338(nsc_chip_t *chip, int cfg_base, int chip_id, int revision);

static nsc_chip_t chips[] = {
	{ "PC87108", { 0x150, 0x398, 0xea }, 0x05, 0x10, 0xf0, nsc_probe_108 },
	{ "PC87338", { 0x398, 0x15c, 0x2e }, 0x08, 0xb0, 0xf0, nsc_probe_338 },
	{ NULL }
};

/*
 * Function nsc_probe_108 (nsc_chip, cfg_base, chip_id, revision)
 *
 *    
 *
 */
void nsc_probe_108(nsc_chip_t *chip, int cfg_base, int chip_id, int revision)
{
	struct chip_info info;
	int reg;

	memset(&info, 0, sizeof(struct chip_info));
	
	/* Read address and interrupt control register (BAIC) */
	outb(CFG_BAIC, cfg_base);
	reg = inb(cfg_base+1);
	
	switch (reg & 0x03) {
	case 0:
		info.sir_base = 0x3e8;
		break;
	case 1:
		info.sir_base = 0x2e8;
		break;
	case 2:
		info.sir_base = 0x3f8;
		break;
	case 3:
		info.sir_base = 0x2f8;
		break;
	}
	info.fir_base = info.sir_base;

	/* Read control signals routing register (CSRT) */
	outb(CFG_CSRT, cfg_base);
	reg = inb(cfg_base+1);

	switch (reg & 0x07) {
	case 0:
		info.irq = -1;
		break;
	case 1:
		info.irq = 3;
		break;
	case 2:
		info.irq = 4;
		break;
	case 3:
		info.irq = 5;
		break;
	case 4:
		info.irq = 7;
		break;
	case 5:
		info.irq = 9;
		break;
	case 6:
		info.irq = 11;
		break;
	case 7:
		info.irq = 15;
		break;
	}

	/* Currently we only read Rx DMA but it will also be used for Tx */
	switch ((reg >> 3) & 0x03) {
	case 0:
		info.dma = -1;
		break;
	case 1:
		info.dma = 0;
		break;
	case 2:
		info.dma = 1;
		break;
	case 3:
		info.dma = 3;
		break;
	}
	
	/* Read mode control register (MCTL) */
	outb(CFG_MCTL, cfg_base);
	reg = inb(cfg_base+1);

	info.enabled = reg & 0x01;
	info.suspended = !((reg >> 1) & 0x01);

	/* Other stuff we must include */
	info.manufacturer = MANUFACTURER;
	info.name = chip->name;
	info.revision = revision;
	info.chip_id = chip_id;
	info.cfg_base = cfg_base;
	info.uart_comp = 1; /* We know it is */

	print_info(&info);
}

/*
 * Function nsc_probe_338 (nsc_chip, cfg_base, chip_id, revision)
 *
 *    
 *
 */
void nsc_probe_338(nsc_chip_t *chip, int cfg_base, int chip_id, int revision)
{
	struct chip_info info;
	int com = 0;
	int reg;
	int pnp;

	memset(&info, 0, sizeof(struct chip_info));

	/* Read funtion enable register (FER) */
	outb(CFG_FER, cfg_base);
	reg = inb(cfg_base+1);

	info.enabled = (reg >> 2) & 0x01;

	/* Check if we are in Legacy or PnP mode */
	outb(CFG_PNP0, cfg_base);
	reg = inb(cfg_base+1);
	
	pnp = (reg >> 4) & 0x01;
	if (pnp) {
		DEBUG("Chip is in PnP mode\n");
		outb(0x46, cfg_base);
		reg = (inb(cfg_base+1) & 0xfe) << 2;

		outb(0x47, cfg_base);
		reg |= ((inb(cfg_base+1) & 0xfc) << 8);

		info.sir_base = reg;
	} else {
		DEBUG("Chip is in Legacy mode\n");

		/* Read function address register (FAR) */
		outb(CFG_FAR, cfg_base);
		reg = inb(cfg_base+1);
		printf("reg=0x%03x\n", reg);
		switch ((reg >> 4) & 0x03) {
		case 0:
			info.sir_base = 0x3f8;
			break;
		case 1:
			info.sir_base = 0x2f8;
			break;
		case 2:
			com = 3;
			break;
		case 3:
			com = 4;
			break;
		}
		
		if (com) {
			switch ((reg >> 6) & 0x03) {
			case 0:
				if (com == 3)
					info.sir_base = 0x3e8;
				else
					info.sir_base = 0x2e8;
				break;
			case 1:
				if (com == 3)
					info.sir_base = 0x338;
				else
					info.sir_base = 0x238;
				break;
			case 2:
				if (com == 3)
					info.sir_base = 0x2e8;
				else
					info.sir_base = 0x2e0;
				break;
			case 3:
				if (com == 3)
					info.sir_base = 0x220;
				else
					info.sir_base = 0x228;
				break;
			}
		}		
	}
	info.fir_base = info.sir_base;

	/* Read PnP register 1 (PNP1) */
	outb(CFG_PNP1, cfg_base);
	reg = inb(cfg_base+1);

	info.irq = reg >> 4;

	/* Read PnP register 3 (PNP3) */
	outb(CFG_PNP3, cfg_base);
	reg = inb(cfg_base+1);

	info.dma = (reg & 0x07) - 1;

	/* Read power and test register (PTR) */
	outb(CFG_PTR, cfg_base);
	reg = inb(cfg_base+1);

	info.suspended = reg & 0x01;

	info.manufacturer = MANUFACTURER;
	info.name = chip->name;
	info.revision = revision;
	info.chip_id = chip_id;
	info.uart_comp = 1; /* We know it is */
	info.cfg_base = cfg_base;

	print_info(&info);
}

/*
 * Function probe_nsc ()
 *
 *    
 *
 */
int probe_nsc(void)
{
	nsc_chip_t *chip;
	u_int8_t index, id;
	int cfg_base;
	int cfg;

	/* Search all chips */
	for (chip = chips; chip->name; chip++) {
		DEBUG("Probing for %s ...\n", chip->name);

		/* Try all config registers for this chip */
		for (cfg=0; cfg<3; cfg++) {
			cfg_base = chip->cfg[cfg];
			if (!cfg_base)
				continue;

			/* Read index register */
			index = inb(cfg_base);
			if (index == 0xff) {
				DEBUG("no chip at 0x%03x\n", cfg_base);
				continue;
			}
		
			/* Read chip identification register */
			outb(chip->cid_index, cfg_base);
			id = inb(cfg_base+1);
			if ((id & chip->cid_mask) == chip->cid_value) {
				DEBUG("Found %s chip, revision=%d\n",
				      chip->name, id & ~chip->cid_mask);

				/* Print chip information */
				if (!chip->probe)
					continue;
				
				chip->probe(chip, cfg_base, 
					    (id & chip->cid_mask) >> 4,
					    id & ~chip->cid_mask);
			} else {
				DEBUG("Wrong chip id=0x%02x\n", id);
			}
		}
	}
	return 0;
}
