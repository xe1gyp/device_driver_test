/*********************************************************************
 *                
 * Filename:      findchip.c
 * Version:       0.5
 * Description:   Hardware probe for infrared controllers
 * Status:        Experimental.
 * Author:        Jonathan Hudson <jrhudson@bigfoot.com>
 * Created at:    Thu Jan  1 09:45:49 2000
 * Modified at:   Wed Jan 19 10:36:57 2000
 * Modified by:   Dag Brattli <dagb@cs.uit.no>
 * 
 *     Copyright (c) 2000 Jonathan Hudson <jrhudson@bigfoot.com>,
 *     Copyright (c) 2000 Dag Brattli <dagb@cs.uit.no>, 
 *     All Rights Reserved.
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
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/io.h> 
#include <string.h>

#include "findchip.h"

#define AUTHORS "Jonathan Hudson and Dag Brattli"

extern int probe_smc(void);
extern int probe_nsc(void);
extern int probe_winbond(void);

static int verbose = 0;
int debug = 0;

typedef struct {
        char *chip;
        int (*probe_func) (void);
} chipset_t;

static chipset_t allchips[] =
{
        { "SMC", probe_smc },
        { "NSC", probe_nsc },
        { "WINBOND", probe_winbond },
        { NULL, NULL }
};

void list_chips(void)
{
        chipset_t *probe;
        for (probe = allchips; probe->chip; probe++) {
                printf("%s\n", probe->chip);
        }
}

void print_info(struct chip_info *info)
{
	if (verbose) {
		printf("Found %s %s Controller at 0x%x, DevID=0x%02x, Rev. %d\n",
		       info->manufacturer, info->name, info->cfg_base, 
		       info->chip_id, info->revision);
		printf("    SIR Base 0x%x, FIR Base 0x%x\n", 
		       info->sir_base, info->fir_base);
		printf("    IRQ = %d, DMA = %d\n", info->irq, info->dma);
		printf("    Enabled: %s, Suspended: %s\n",
		       info->enabled ? "yes" : "no", 
		       info->suspended ? "yes" : "no");
		printf("    UART compatible: %s\n",
		       info->uart_comp ? "yes" : "no");
		printf("    Half duplex delay = %d us\n", info->delay);
	} else {
		printf("%s,%s,%d.%d,0x%x,0x%x,0x%x,%d,%d,%d,%d,%d\n",
		       info->manufacturer, info->name,
		       info->chip_id, info->revision, info->cfg_base,
		       info->sir_base, info->fir_base, info->irq, info->dma,
		       info->enabled, info->suspended, info->uart_comp);
	}
}

int main(int argc, char **argv)
{
        chipset_t *probe;
	int c;

	while ((c = getopt(argc, argv, "vdlh")) != -1) {
		switch (c) {
		case 'v':
			verbose = 1;
			break;
		case 'd':
			debug = 1;
			break;
		case 'h':			
			fputs("Usage: findchip [-d] [-v] [-l] [chip1 ...  chipN]\n",
			      stderr);
			fputs("\n", stderr);
			fputs("-d     Enable debug output\n", stderr);
			fputs("-v     Be verbose\n", stderr);
			fputs("-l     List supported chipsets\n", stderr);
			fputs("-h     Print help (this message)\n", stderr);
			exit(0);
		case 'l':
                        list_chips();
                        exit(0);
                        break;
		}
	}

	if (ioperm(0x0, 0x3ff, 1)) {
		perror("Set i/o permission");
		exit(0);
	}

	/* Probe the chips! */
	do {
                for (probe = allchips; probe->chip; probe++) {
                        if (argv[optind] == NULL ||
                          (0 == strcasecmp(argv[optind], probe->chip))) {
                                probe->probe_func();
                        }
                }
        } while (++optind < argc);

	return 0;
}
