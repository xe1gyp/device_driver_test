/*********************************************************************
 *                
 * Filename:      irlmp.c
 * Version:       
 * Description:   
 * Status:        Experimental.
 * Author:        Dag Brattli <dagb@cs.uit.no>
 * Created at:    Wed Mar 31 15:30:46 1999
 * Modified at:   Wed Dec  8 09:46:16 1999
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

#include <sys/socket.h>
#include <sys/types.h>

#include <string.h>		/* For strstr */
#include <stdio.h>		/* For printf */

#include <irda.h>

#include "irdadump.h"

extern struct ias_query last_ias;
extern struct lsap_state conn[];

/*
 * Function parse_iriap (buf)
 *
 *    Try to parse and print the LM-IAS query
 *
 */
inline void parse_iriap_command(GNetBuf *buf, GString *str, guint8 slsap_sel)
{
	guint8 opcode;
	char name[255];
	char attr[255];
	int n = 0;
	int name_len;
	int attr_len;

	/* Kill "unused" warning */
	slsap_sel = slsap_sel;

	opcode = buf->data[n++];

	opcode &= ~IAP_LST; /* Mask away LST bit */
	switch (opcode) {
	case GET_INFO_BASE:
	case GET_VALUE_BY_CLASS:
		name_len = buf->data[n++];
		memcpy( name, buf->data+n, name_len); n+=name_len;
		name[name_len] = '\0';
		
		attr_len = buf->data[n++]; 
		memcpy( attr, buf->data+n, attr_len); n+=attr_len;
		attr[attr_len] = '\0';

		g_string_sprintfa(str, "GET_VALUE_BY_CLASS: \"%s\" \"%s\" ",
				  name, attr);

		/* Check if this is a TinyTP lookup */
		if (strstr(attr, "TinyTP"))
			last_ias.ttp = 1;
		else
			last_ias.ttp = 0;
		/* Reset to undefined (handle IAS failures properly) */
		last_ias.lsap_sel = LSAP_ANY;

		/* Check if this is a OBEX lookup */
		if (strstr(name, "OBEX"))
			last_ias.obex = 1;
		else
			last_ias.obex = 0;
		/* Check if this is a IrCOMM lookup */
		if (strstr(name, "IrCOMM") || strstr(name, "IrLPT"))
			last_ias.ircomm = 1;
		else
			last_ias.ircomm = 0;
		/* Check if this is a IrNET lookup */
		if (strstr(name, "IrNet"))
			last_ias.irnet = 1;
		else
			last_ias.irnet = 0;
		break;
	default:
		break;
	}
}

/*
 * Function parse_irias_value (buf)
 *
 *    
 *
 */
void parse_irias_value(GNetBuf *buf, GString *str)
{
	char string[255];
	int n = 0;
	int charset;
	guint32 value_len;
	guint32 tmp_cpu32;
	guint16 tmp_cpu16;
	guint16 obj_id;
	guint16 len;
	guint8  type;

	/* Get length, MSB first */
	memcpy(&len, buf->data+n, 2); n += 2;
	len = GUINT16_FROM_BE(len); 

	/* Get object ID, MSB first */
	memcpy(&obj_id, buf->data+n, 2); n += 2;
	obj_id = GUINT16_FROM_BE(obj_id);

	type = buf->data[n++];

	switch(type) {
	case IAS_INTEGER:
		memcpy(&tmp_cpu32, buf->data+n, 4); n += 4;
		tmp_cpu32 = GUINT32_FROM_BE(tmp_cpu32);

		/*  Legal values restricted to 0x01-0x6f, page 15 irttp */
		g_string_sprintfa(str, "Integer: %02x ", tmp_cpu32);
		last_ias.lsap_sel = tmp_cpu32;
		break;
	case IAS_STRING:
		charset = buf->data[n++];

		switch (charset) {
		case CS_ASCII:
			break;
		case CS_ISO_8859_1:
		case CS_ISO_8859_2:
		case CS_ISO_8859_3:
		case CS_ISO_8859_4:
		case CS_ISO_8859_5:
		case CS_ISO_8859_6:
		case CS_ISO_8859_7:
		case CS_ISO_8859_8:
		case CS_ISO_8859_9:
		case CS_UNICODE:
		default:
			break;
		}
		value_len = buf->data[n++];
		
		/* Make sure the string is null-terminated */
		memcpy(string, buf->data+n, value_len);
		string[value_len] = 0x00;
		
		g_string_sprintfa(str, "String: %s ", string);
		break;
	case IAS_OCT_SEQ:
		memcpy(&tmp_cpu16, buf->data+n, 2);  n += 2;
		tmp_cpu16 = GUINT16_FROM_BE(tmp_cpu16);
		value_len = tmp_cpu16;
		
		g_netbuf_pull(buf, n);
		if (last_ias.ircomm) {
			g_string_append(str, "\n\tIrCOMM Parameters ");
			parse_ircomm_params(value_len, buf, str);
		} else
			g_string_append(str, "N/A ");
		break;
	default:
		g_print("%s() Unknown IAS value type! ", __FUNCTION__);
		break;
	}
}

/*
 * Function parse_iriap_response (buf)
 *
 *    
 *
 */
inline void parse_iriap_response(GNetBuf *buf, GString *str, guint8 dlsap_sel)
{
	guint8 opcode, rsp;

	/* Kill "unused" warning */
	dlsap_sel = dlsap_sel;

	opcode = buf->data[0] & ~IAP_LST; /* Mask away LST bit */
	rsp = buf->data[1];

	g_netbuf_pull(buf, 2);

	switch(opcode) {
	case GET_INFO_BASE:
		g_print("%s() Sorry, GET_INFO_BASE not implemented!\n", __FUNCTION__);
		break;
	case GET_VALUE_BY_CLASS:
		g_string_append(str, "GET_VALUE_BY_CLASS: ");
		break;
	default:
		g_print("%s() Sorry, not implemented!\n", __FUNCTION__);
		break;
	}

	switch(rsp) {
	case IAS_SUCCESS:
		g_string_append(str, "Success ");
		parse_irias_value(buf, str);
		break;
	case IAS_CLASS_UNKNOWN:
		g_string_append(str, "No such class ");
		break;
	case IAS_ATTRIB_UNKNOWN:
		g_string_append(str, "No such attribute ");
		break;
	default:
		g_string_append(str, "Unknown response code ");
	}
}

/*
 * Function parse_irttp (buf, str)
 *
 *    Parse IrTTP data frame
 *
 */
inline void parse_irnet(GNetBuf *buf, GString *str) 
{
	/* Kill "unused" warning */
	buf = buf;

	g_string_sprintfa(str, "IrNET ");
	/* If you want to hook PPP frame decoding, you can do it here.
	 * I don't think it's worth it, PPP has debug capabilities.
	 * Jean II */
}

/*
 * Function parse_irttp (buf, str)
 *
 *    Parse IrTTP data frame
 *
 */
inline void parse_irttp(GNetBuf *buf, GString *str) 
{
	g_string_sprintfa(str, "TTP credits=%d ", buf->data[0] & 0x7f);

	if (buf->data[0] & 0x80)
		g_string_append(str, "More ");

	/* Remove TTP header */
	g_netbuf_pull(buf, 1);
}

/*
 * Function parse_irttp_connect (buf, str)
 *
 *    Parse IrTTP connect frame
 *
 */
inline void parse_irttp_connect(GNetBuf *buf, GString *str)
{
	guint8 plen, pi, pl;
	guint16 tmp_cpu;

	g_string_sprintfa(str, "TTP credits=%d ", buf->data[0] & 0x7f);

	if (buf->data[0] & 0x80) {
		plen = buf->data[1];
		pi   = buf->data[2];
		pl   = buf->data[3];

		memcpy(&tmp_cpu, buf->data+4, 2); /* Align value */
		tmp_cpu = GUINT16_FROM_BE(tmp_cpu); /* Convert to host order */

		/* Remove TTP parameters */
		g_netbuf_pull(buf, plen);

		g_string_sprintfa(str, "MaxSduSize=%d ", tmp_cpu);
	}
	/* Remove TTP header */
	g_netbuf_pull(buf, 1);
}

/*
 * Function parse_irlmp (buf)
 *
 *    Parse IrLMP frame
 *
 */
inline void parse_irlmp(GNetBuf *buf, GString *str,
			guint8 caddr, int type, int cmd)
{
	guint8 slsap_sel, dlsap_sel;
	int ctrl;
	int rsvd;
	int i;
	
	ctrl = buf->data[0] & CONTROL_BIT;
	dlsap_sel = buf->data[0] & LSAP_MASK;
	slsap_sel = buf->data[1];
	
	/* Remove IrLMP header */
	g_netbuf_pull(buf, 2);

	g_string_sprintfa(str, "LM slsap=%02x dlsap=%02x ", slsap_sel, 
			  dlsap_sel);
	
	/* Control or data? */
	if (ctrl) {
		ctrl = buf->data[0];
		rsvd = buf->data[1];	/* reason/status/rsvd */

		/* Remove IrLMP control header (2 bytes) */
		g_netbuf_pull(buf, 2);

		switch (ctrl) {
		case CONNECT_CMD:
			g_string_append(str, "CONN_CMD ");
			if (dlsap_sel == last_ias.lsap_sel) {
				i = find_free_connection();
				if (i == -1) {
					printf("No more space for connection!\n");
					return;
				}
				conn[i].valid = 1;
				conn[i].caddr = caddr;
				if (type) {
					conn[i].slsap_sel = slsap_sel;
					conn[i].dlsap_sel = dlsap_sel;
				} else {
					conn[i].slsap_sel = dlsap_sel;
					conn[i].dlsap_sel = slsap_sel;
				}
				conn[i].ttp = last_ias.ttp;
				conn[i].obex = last_ias.obex;
				conn[i].ircomm = last_ias.ircomm;
				conn[i].irnet = last_ias.irnet;

				if (conn[i].ttp)
					parse_irttp_connect(buf, str);
				if (conn[i].ircomm)
					parse_ircomm_connect(buf, str);
				if (conn[i].irnet)
					parse_irnet(buf, str);
			} else {
				if((config_force_ttp) && (dlsap_sel != 0x00))
					parse_irttp_connect(buf, str);
			}
			break;
		case CONNECT_RSP:
			g_string_append(str, "CONN_RSP ");
			if (type)
				i = find_connection(slsap_sel, dlsap_sel);
			else
				i = find_connection(dlsap_sel, slsap_sel);
			if (i != -1) {
				if (conn[i].ttp)
					parse_irttp_connect(buf, str);
				if (conn[i].ircomm)
					parse_ircomm_connect(buf, str);
				if (conn[i].irnet)
					parse_irnet(buf, str);
			} else {
				if((config_force_ttp) && (slsap_sel != 0x00))
					parse_irttp_connect(buf, str);
			}
			break;
		case DISCONNECT:
			g_string_append(str, "DISC ");

			/* Mark this connection as invalid */
			if (type)
				i = find_connection(slsap_sel, dlsap_sel);
			else
				i = find_connection(dlsap_sel, slsap_sel);
			if (i != -1) {
				conn[i].valid = 0;
			}
			break;
		default:
			break;
		}
	} else {
		/* Check if this is a LM-IAS query */
		if (dlsap_sel == 0x00)
			parse_iriap_command(buf, str, slsap_sel);
		else if (slsap_sel == 0x00)
			parse_iriap_response(buf, str, dlsap_sel);
		
		/* Find connection */
		if (type)
			i = find_connection(slsap_sel, dlsap_sel);
		else
			i = find_connection(dlsap_sel, slsap_sel);
		if (i != -1) {
			if (conn[i].valid && conn[i].ttp)
				parse_irttp(buf, str);
			if (conn[i].valid && conn[i].obex)
				parse_obex(buf, str, cmd);
			if (conn[i].valid && conn[i].ircomm) {
				if (conn[i].ttp)
					parse_ircomm_ttp(buf, str);
				else
					parse_ircomm_lmp(buf, str);
			}
			if (conn[i].valid && conn[i].irnet)
				parse_irnet(buf, str);
		}
		/* Not IAS and unknow connection */
		if((dlsap_sel != 0x00) && (slsap_sel != 0x00) && (i == -1)) {
			if(config_force_ttp)
				parse_irttp(buf, str);
		}
	}
}


/*
 * Function parse_ui_irlmp (buf)
 *
 *    Parse IrLMP frame in UI frame
 *
 */
inline void parse_ui_irlmp(GNetBuf *buf, GString *str, int type)
{
	guint8 slsap_sel, dlsap_sel;
	int ctrl;
	
	/* Kill "unused" warning */
	type = type;

	ctrl = buf->data[0] & CONTROL_BIT;
	dlsap_sel = buf->data[0] & LSAP_MASK;
	slsap_sel = buf->data[1];
	
	/* Remove IrLMP header */
	g_netbuf_pull(buf, 2);

	g_string_sprintfa(str, "LM slsap=%02x dlsap=%02x ", slsap_sel, 
			  dlsap_sel);

	/* Let's see if it's Ultra, and decode it - Jean II */
	if((slsap_sel == 0x70) && (dlsap_sel == 0x70))
	  {
	    int	upid = buf->data[0] & 0x7F;
	    g_netbuf_pull(buf, 1);

	    g_string_sprintfa(str, " Ultra-PID=%02x ", upid);

	    /* Check Obex over Ultra */
	    if(upid == 0x01)
	      {
		/* Remove SAR field */
		g_netbuf_pull(buf, 1);

		/* Decode Obex stuff - no connection, always a command */
		parse_obex(buf, str, 1);
	      }
	  }
}
