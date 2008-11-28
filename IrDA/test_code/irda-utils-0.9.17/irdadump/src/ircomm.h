/*********************************************************************
 *                
 * Filename:      ircomm.h
 * Version:       
 * Description:   
 * Status:        Experimental.
 * Author:        Dag Brattli <dagb@cs.uit.no>
 * Created at:    Sun Jun  6 13:48:43 1999
 * Modified at:   Sat Oct 16 14:53:13 1999
 * Modified by:   Dag Brattli <dagb@cs.uit.no>
 * 
 *     Copyright (c) 1999 Dag Brattli, All Rights Reserved.
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

#ifndef IRCOMM_H
#define IRCOMM_H

#include <glib.h>
#include <netbuf.h>

/* Parameters common to all service types */
#define SERVICE_TYPE    0x00
#define PORT_TYPE       0x01
#define PORT_NAME       0x02
#define FIXED_PORT_NAME 0x82

/* Parameters for both 3 wire and 9 wire */
#define DATA_RATE       0x10
#define DATA_FORMAT     0x11
#define FLOW_CONTROL    0x12
#define XON_XOFF_CHAR   0x13
#define ENQ_ACK_CHAR    0x14
#define LINESTATUS      0x15
#define BREAK_SIGNAL    0x16

/* Parameters for 9 wire */
#define DTELINE_STATE          0x20
#define DCELINE_STATE          0x21
#define POLL_FOR_LINE_SETTINGS 0x22

/* Service type (details) */
#define IRCOMM_3_WIRE_RAW       0x01
#define IRCOMM_3_WIRE           0x02
#define IRCOMM_9_WIRE           0x04
#define IRCOMM_CENTRONICS       0x08
#define IRCOMM_VALID_SERVICES	0x0F

/* Port type (details) */
#define IRCOMM_SERIAL           0x01
#define IRCOMM_PARALLEL         0x02
#define IRCOMM_VALID_PORT_TYPES	0x03

#endif
