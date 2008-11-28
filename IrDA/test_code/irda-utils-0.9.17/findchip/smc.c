/*********************************************************************
 *                
 * Filename:      smc.c
 * Version:       0.4
 * Description:   Hardware probe for SMC infrared controllers
 * Status:        Experimental.
 * Author:        Jonathan Hudson <jrhudson@bigfoot.com>
 * Created at:    Thu Jan  1 09:46:05 2000
 * Modified at:   Fri Jan 28 11:21:46 2000
 * Modified by:   Dag Brattli <dagb@cs.uit.no>
 * 
 *     Copyright (c) 2000 Jonathan Hudson <jrhudson@bigfoot.com>,
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

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <asm/io.h>

#include "findchip.h"

#define IRCC_MASTER	0x07
#define IRCC_ID_HIGH	0x00	/* 0x10 */
#define IRCC_ID_LOW	0x01	/* 0xB8 */
#define IRCC_CHIP_ID 	0x02	/* 0xF1 */
#define IRCC_VERSION	0x03	/* 0x01 */
#define IRCC_INTERFACE	0x04	/* low 4 = DMA, high 4 = IRQ */

#define SMC_DISCONF 0xAA	/* Disable Config */

#define	CR00	0x00
#define CR02    0x02
#define	CR0C	0x0c
#define	CR0D	0x0d
#define	CR24	0x24
#define	CR25	0x25
#define	CR28	0x28
#define	CR2B	0x2b
#define	CR2C	0x2c
#define	CR2D	0x2d

typedef struct {
	char *name;
	int cfg[2];
	unsigned char entr1;
	unsigned char entr2;
	unsigned char cid_index;
	unsigned char cid_value;
	int (*probe)(char *name, int confreg);
} smc_chip_info_t;

int probe_69(char *name, int confreg);
int probe_58(char *name, int confreg);

/* These are the currently known SMC chips */
static smc_chip_info_t chips[] =
{
	{ "FDC37C669",     {0x3f0, 0x370}, 0x55, 0x55, 0x0d, 0x03, probe_69 },
	{ "FDC37C669FR",   {0x3f0, 0x370}, 0x55, 0x55, 0x0d, 0x04, probe_69 },
	{ "FDC37N869",     {0x3f0, 0x370}, 0x55, 0x00, 0x0d, 0x29, probe_69 },
	{ "FDC37C93xFR",   {0x3f0, 0x370}, 0x55, 0x55, 0x20, 0x03, probe_58 },
	{ "FDC37N957FR",   {0x3f0, 0x370}, 0x55, 0x55, 0x20, 0x07, probe_58 },
	{ "FDC37N958FR",   {0x3f0, 0x370}, 0x55, 0x55, 0x20, 0x09, probe_58 },
	{ 0 }
};

static inline void register_bank(int port, int bank)
{
	outb(((inb(port + IRCC_MASTER) & 0xF0) | (bank & 0x07)),
	     port + IRCC_MASTER);
}

static inline unsigned int serial_in(int port, int offset)
{
	return inb(port + offset);
}

/*
 * Function probe_69 (name, confreg)
 *
 *    Probes for the FDC37C669 and FDC37N869
 *
 */
int probe_69(char *name, int confreg)
{
	struct chip_info info;
	int fir, enb;

	memset(&info, 0, sizeof(struct chip_info));

	outb(CR25, confreg);
	info.sir_base = inb(confreg + 1);
	info.sir_base <<= 2;

	outb(CR2C, confreg);
        info.dma = inb(confreg+1);

        outb(CR28, confreg);
        info.irq = inb(confreg+1) & 0x0F;

	outb(CR2B, confreg);
	fir = inb(confreg + 1);
	fir <<= 3;

	if (fir) {
		outb(CR2D, confreg);
		info.delay = inb(confreg + 1);

                register_bank(fir, 3);
		info.chip_id = inb(fir + IRCC_CHIP_ID) & 0x0f;
		info.revision = inb(fir + IRCC_VERSION);
	}

	outb(CR02, confreg);
	enb = inb(confreg + 1) & 0x80;

	info.manufacturer = "SMC";
	info.name = name;
	info.cfg_base = confreg;
	info.fir_base = fir;
	info.enabled = enb;
	info.uart_comp = 1; /* We know it is */
	
	print_info(&info);

	return fir;
}

/*
 * Function probe_58 (name, confreg)
 *
 *    Probes for the SMC FDC37N958
 *
 */
int probe_58(char *name, int confreg)
{
	struct chip_info info;
	int fir, enb;

	memset(&info, 0, sizeof(struct chip_info));

	/* Select logical device (UART2) */
	outb(0x07, confreg);
	outb(0x05, confreg + 1);

	/* Read SIR base */
	outb(0x60, confreg);
	info.sir_base = inb(confreg + 1) << 8;
	outb(0x61, confreg);
	info.sir_base |= inb(confreg + 1);
	
	/* Read DMA */
	outb(0x74, confreg);
        info.dma = inb(confreg+1);

	/* Read IRQ */
        outb(0x70, confreg);
        info.irq = inb(confreg+1) & 0x0F;

	/* Read FIR base */
	outb(0x62, confreg);
	fir = inb(confreg + 1) << 8;
	outb(0x63, confreg);
	fir |= inb(confreg + 1);

	if (fir) {
		outb(0xf2, confreg);
		info.delay = inb(confreg + 1);

                register_bank(fir, 3);
		info.chip_id = inb(fir + IRCC_CHIP_ID) & 0x0f;
		info.revision = inb(fir + IRCC_VERSION);
	}

	/* Check if device is enabled */
	outb(0x22, confreg);
	enb = inb(confreg + 1) & 0x20;

	info.manufacturer = "SMC";
	info.name = name;
	info.cfg_base = confreg;
	info.fir_base = fir;
	info.enabled = enb;
	info.uart_comp = 1; /* We know it is */
	
	print_info(&info);

	return fir;
}

int probe_smc(void)
{
	int confreg = 0;
	int fir = 0;
	smc_chip_info_t *chp;
	int i;

	/* Probe all chips */
	for (chp = chips; chp->name && fir == 0; chp++) {
		DEBUG("Probing for %s ...\n", chp->name);

		/* Try both config register */
		for (i = 0; i < 2 && fir == 0; i++) {
			int devid;

			outb(chp->entr1, chp->cfg[i]);
			if (chp->entr2) {
				outb(chp->entr2, chp->cfg[i]);
			}
			outb(chp->cid_index, chp->cfg[i]);
			devid = inb(chp->cfg[i] + 1);
			if (devid == chp->cid_value) {
				confreg = chp->cfg[i];
				fir = chp->probe(chp->name, confreg);
			} else {
				DEBUG("Wrong chip id=0x%02x\n", devid);
			}
			outb(SMC_DISCONF, chp->cfg[i]);
		}
	}
	return 0;
}
