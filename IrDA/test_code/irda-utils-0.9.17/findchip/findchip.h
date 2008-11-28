/*********************************************************************
 *                
 * Filename:      findchip.h
 * Version:       
 * Description:   
 * Status:        Experimental.
 * Author:        Dag Brattli <dagb@cs.uit.no>
 * Created at:    Sun Jan  9 22:24:34 2000
 * Modified at:   Wed Jan 19 10:38:10 2000
 * Modified by:   Dag Brattli <dagb@cs.uit.no>
 * 
 *     Copyright (c) 2000 Dag Brattli, All Rights Reserved.
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

#ifndef FINDCHIP_H
#define FINDCHIP_H

struct chip_info {
	char *manufacturer;
	char *name;
	int  chip_id;
        int  revision;
	int  cfg_base;
	int  sir_base;
	int  fir_base;
	int  irq;
	int  dma;
	int  uart_comp;
	int  enabled;     /* Device enabled */
	int  suspended;   /* Device suspended */
	int  delay;       /* Half duplex delay */
};

extern void print_info(struct chip_info *info);
extern int debug;

#define DEBUG(args...) (debug ? printf(args):0)

#endif
