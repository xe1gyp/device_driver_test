/*********************************************************************
 *                
 * Filename:      ircomm.c
 * Version:       
 * Description:   
 * Status:        Experimental.
 * Author:        Dag Brattli <dagb@cs.uit.no>
 * Created at:    Sun Jun  6 13:40:30 1999
 * Modified at:   Fri Jun 11 10:47:08 1999
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

#include "irdadump.h"
#include "ircomm.h"

#if 0
static char *ircomm_service_type[] =
{
	"N/A",
	"THREE_WIRE_RAW",
	"THREE_WIRE",
	"N/A",
	"NINE_WIRE",
	"N/A",
	"N/A",
	"N/A",
	"CENTRONICS"
};

static char *ircomm_port_type[] =
{
	"SERIAL",
	"PARALLEL"
};
#endif

static inline guint bytes_to_uint(unsigned char *buf)
{
	guint	ret;
	ret = (buf[0] << 24) | (buf[1] << 16) | (buf[2] << 8) | buf[3];
	return(ret);
}

void parse_ircomm_params(guint8 clen, GNetBuf *buf, GString *str)
{
	guint pi, pl;
	guint pv_byte;
	guint n = 0;

	while (n < clen) {
		pi = buf->data[n] & 0x7f; /* Remove critical bit */
		pl = buf->data[n+1];

		switch (pi) {
		case SERVICE_TYPE:
			pv_byte = buf->data[n+2];
			g_string_sprintfa(str, "Service Type=");
			if (pv_byte & IRCOMM_CENTRONICS)
				g_string_sprintfa(str, "CENTRONICS ");
			if (pv_byte & IRCOMM_9_WIRE)
				g_string_sprintfa(str, "NINE_WIRE ");
			if (pv_byte & IRCOMM_3_WIRE)
				g_string_sprintfa(str, "THREE_WIRE ");
			if (pv_byte & IRCOMM_3_WIRE_RAW)
				g_string_sprintfa(str, "THREE_WIRE_RAW ");
			if (!(pv_byte & IRCOMM_VALID_SERVICES))
				g_string_sprintfa(str, "N/A ");
			break;
		case PORT_TYPE:
			pv_byte = buf->data[n+2];
			g_string_sprintfa(str, "Port Type=");
			if (pv_byte & IRCOMM_SERIAL)
				g_string_sprintfa(str, "SERIAL ");
			if (pv_byte & IRCOMM_PARALLEL)
				g_string_sprintfa(str, "PARALLEL ");
			if (!(pv_byte & IRCOMM_VALID_PORT_TYPES))
				g_string_sprintfa(str, "N/A ");
			break;
		case DATA_RATE:
			pv_byte = bytes_to_uint(&(buf->data[n+2]));
			g_string_sprintfa(str, "Data Rate=%d ", pv_byte);
			break;
		case DATA_FORMAT:
			g_string_sprintfa(str, "Data Format=%02x ",
					  buf->data[n+2]);
			break;
		case FLOW_CONTROL:
			g_string_sprintfa(str, "Flow Control=%02x ",
					  buf->data[n+2]);
			break;
		case XON_XOFF_CHAR:
			g_string_sprintfa(str, "XON/XOFF=%02x,%02x ",
					  buf->data[n+2], buf->data[n+3]);
			break;
		case DTELINE_STATE:
			g_string_sprintfa(str, "DTEline State=%02x ",
					  buf->data[n+2]);
			break;
		default:
			break;
		}
		n += pl+2;
	}
	g_netbuf_pull(buf, clen);
}

void parse_ircomm_connect(GNetBuf *buf, GString *str)
{
	parse_ircomm_ttp(buf, str);
}

void parse_ircomm_lmp(GNetBuf *buf, GString *str)
{
	/* Kill "unused" warning */
	buf = buf;

	g_string_append(str, "IrCOMM (IrLPT) ");

	return;
}

void parse_ircomm_ttp(GNetBuf *buf, GString *str)
{
	int	len;
	guint8 clen;

	/* Check for empty frames - Jean II */
	len = g_netbuf_get_len(buf);
	if(len == 0)
		return;

	clen = buf->data[0];

	g_netbuf_pull(buf, 1);

	if (clen) {
		g_string_append(str, "\n\tIrCOMM ");
		parse_ircomm_params(clen, buf, str);
	} else
		g_string_append(str, "IrCOMM ");
}
