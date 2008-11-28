/*********************************************************************
 *                
 * Filename:      obex.c
 * Version:       
 * Description:   
 * Status:        Experimental.
 * Author:        Dag Brattli <dagb@cs.uit.no>
 * Created at:    Wed Mar 31 14:09:47 1999
 * Modified at:   Wed Apr 21 15:52:03 1999
 * Modified by:   Dag Brattli <dagb@cs.uit.no>
 * 
 *     Copyright (c) 1999 Dag Brattli, All Rights Reserved.
 *     Copyright (c) 1999 Jean Tourrilhes, All Rights Reserved.
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
#include "obex.h"

#include <stdio.h>
#include <string.h>
#include <netinet/in.h>		/* ntohs */

void unicode_to_char(guint8 *buf)
{
	guint8 *buf2 = buf;
	int n=0;
	
	while ((buf2[n] = buf[n*2+1]))
		n++;
}

/*
 * Function parse_obex_header (buf)
 *
 *    
 *
 */
int parse_obex_header(GNetBuf *buf, GString *str, int istext)
{
	char string[255];
	guint32 tmp_int;
	guint16 tmp_short;
	int len = 0;

	/* g_print("%s()", __FUNCTION__);fflush(stdout); */

	switch (buf->data[0] & OBEX_HI_MASK) {
	case OBEX_UNICODE:
		/* g_print("OBEX_UNICODE");fflush(stdout); */
		memcpy(&tmp_short, buf->data+1, 2); /* Align value */
		tmp_short = GINT16_FROM_BE(tmp_short) - 3;
		len += 3;
		
		memcpy(string, buf->data+3, tmp_short);
		unicode_to_char(string);
		len += tmp_short;

		/* g_print("%s ", string);fflush(stdout); */
		g_string_sprintfa(str, "\"%s\" ", string);
		break;
	case OBEX_BYTE_STREAM:
                /* g_print("OBEX_BYTE_STREAM");fflush(stdout); */
		memcpy(&tmp_short, buf->data+1, 2); /* Align value */
		tmp_short = GINT16_FROM_BE(tmp_short) - 3;
		len += tmp_short + 3;
		if(istext) {
			memcpy(string, buf->data+3, tmp_short);
			string[tmp_short] = '\0';
			g_string_sprintfa(str, "\"%s\" ", string);
		} else
			g_string_sprintfa(str, "[%d bytes] ", tmp_short);
		break;
	case OBEX_BYTE:
		/* g_print("OBEX_BYTE");fflush(stdout); */
		g_string_sprintfa(str, "%d ", buf->data[0]);
                len += 2;
		break;
	case OBEX_INT:
		/* g_print("OBEX_INT");fflush(stdout); */
		memcpy(&tmp_int, buf->data+1, 4); /* Align value */
		len += 5;
		/* printf("%ld ", ntohl(tmp_int));fflush(stdout); */
		g_string_sprintfa(str, "%d ", GINT32_FROM_BE(tmp_int));
		break;
	default:
		g_print("******");fflush(stdout);
		break;
	}
	
	return len;
}

/*
 * Function parse_obex_put (buf)
 *
 *    Parse an OBEX put command
 *
 */
inline void parse_obex_headers(GNetBuf *buf, GString *str)
{
	struct obex_minimal_frame *frame;
	int final;
	guint16 size;
	int len;

	frame = (struct obex_minimal_frame *) buf->data;

	/* We know it's a put frame, but we have to check if it's the final */
	final = frame->opcode & OBEX_FINAL;

	/* Length of this frame */
	size = ntohs(frame->len);
	
	/* Remove the OBEX common header */
	g_netbuf_pull(buf, 3);

	g_string_sprintfa(str, "final=%d len=%d ", final >> 7, size);

	/* Parse all headers */
	while (buf->len > 0) {
		/* Read the header identifier */
		/* g_print("hi=%02x", buf->data[0]);fflush(stdout); */
		switch (buf->data[0]) {
		case HEADER_NAME:
		case HEADER_ANAME:
			g_string_append(str, "Name=");
			len = parse_obex_header(buf, str, 1);
			break;
		case HEADER_DESCRIPTION:
			g_string_append(str, "Description=");
			len = parse_obex_header(buf, str, 0);
			break;
		case HEADER_LENGTH:
			g_string_append(str, "Lenght=");
			len = parse_obex_header(buf, str, 0);
			break;
		case HEADER_TYPE:
			g_string_append(str, "Type=");
			len = parse_obex_header(buf, str, 1);
			break;
		case HEADER_TARGET:
			g_string_append(str, "Target=");
			len = parse_obex_header(buf, str, 1);
			break;
		case HEADER_BODY:
			g_string_append(str, "body=");
			len = parse_obex_header(buf, str, 0);
			break;
		case HEADER_BODY_END:
			g_string_append(str, "body-end=");
			len = parse_obex_header(buf, str, 0);
			break;
		default:
			g_string_append(str, "custom=");
			len = parse_obex_header(buf, str, 0);
			break;
		}
                /* g_print("len=%d\n", len);fflush(stdout); */
		if(g_netbuf_pull(buf, len) == NULL) {
			g_string_append(str, "{unterminated} ");
			break;
		}
	}
}

inline void parse_obex_connect(GNetBuf *buf, GString *str)
{
	struct obex_connect_frame *frame;
	guint16 length;
	guint8 version;
	int flags;
	guint16 mtu;
	
	frame = (struct obex_connect_frame *) buf->data;

	length  = ntohs(frame->len);

	/* Check if it contains connection setup parameters - Jean II */
	if(length == 7) {
		version = frame->version;
		flags   = frame->flags;
		mtu     = ntohs(frame->mtu);

		g_string_sprintfa(str,
				  "CONNECT len=%d ver=%d.%d flags=%d mtu=%d ", 
				  length, ((version & 0xf0) >> 4),
				  version & 0x0f, flags, mtu);
	} else
		g_string_sprintfa(str, "CONNECT len=%d ", length);
}

/*
 * The first success frame contains the negociated Obex parameters
 * We also need to parse the anser to GET request properly
 * Jean II
 */
inline void parse_obex_success(GNetBuf *buf, GString *str)
{
	struct obex_connect_frame *frame;
	guint16 length;
	guint8 version;
	int flags;
	guint16 mtu;
	
	frame = (struct obex_connect_frame *) buf->data;

	length  = ntohs(frame->len);

	switch(length) {
	case 7:
		/* Frame contains connection setup parameters */
		version = frame->version;
		flags   = frame->flags;
		mtu     = ntohs(frame->mtu);

		g_string_sprintfa(str,
				  "SUCCESS len=%d ver=%d.%d flags=%d mtu=%d ", 
				  length, ((version & 0xf0) >> 4),
				  version & 0x0f, flags, mtu);
		break;
	case 3:
		/* Frame contains nothing */
		g_string_sprintfa(str, "SUCCESS len=%d ", length);
		break;
	default:
		/* Frame contains some headers (probably a GET reply) */
		g_string_append(str, "SUCCESS ");
		parse_obex_headers(buf, str);
		break;
	}
}

/*
 * Function parse_obex (buf)
 *
 *    Parse OBEX commands and responses
 *
 */
inline void parse_obex(GNetBuf *buf, GString *str, int cmd)
{
	guint8	opcode;
	int	len;

	/* g_print(__FUNCTION__);fflush(stdout); */

	g_string_append(str, "\n\tOBEX ");

	/* Check for empty frames - Jean II */
	len = g_netbuf_get_len(buf);
	if(len == 0)
	  return;

	opcode = buf->data[0] & ~OBEX_FINAL; /* Remove final bit */

	/* Check if it's a command or response frame - Jean II */
	if (!cmd) {
		switch (opcode) {
		case OBEX_CONTINUE:
			g_string_append(str, "CONTINUE ");
			parse_obex_headers(buf, str);
			break;
		case OBEX_SWITCH_PRO:
			g_string_append(str, "SWITCH_PRO ");
			break;
		case OBEX_SUCCESS:
			parse_obex_success(buf, str);
			break;
		case OBEX_CREATED:
			g_string_append(str, "CREATED ");
			break;
		case OBEX_ACCEPTED:
			g_string_append(str, "ACCEPTED ");
			break;
		case OBEX_BAD_REQUEST:
			g_string_append(str, "BAD REQUEST ");
			break;
		case OBEX_FORBIDDEN:
			g_string_append(str, "FORBIDDEN ");
			break;
		case OBEX_CONFLICT:
			g_string_append(str, "CONFLICT ");
			break;
		default:
			g_string_sprintfa(str, "Unknown response %02x ", 
					  opcode);
			break;
		}
	} else {
		switch (opcode) {
		case OBEX_CONNECT:
			parse_obex_connect(buf, str);
			break;
		case OBEX_PUT:
			g_string_append(str, "PUT ");
			parse_obex_headers(buf, str);
			break;
		case OBEX_GET:
			g_string_append(str, "GET ");
			parse_obex_headers(buf, str);
			break;
		case OBEX_DISCONNECT:
			g_string_append(str, "DISC ");
			break;
		case OBEX_ABORT:
			g_string_append(str, "ABORT ");
			break;
		case OBEX_SETPATH:
			g_string_append(str, "SETPATH ");
			break;
		default:
			if ((opcode > 0x04) && (opcode < 0x10))
				g_string_append(str, "RESERVED ");
			else if ((opcode > 0x0f) && (opcode < 0x20))
				g_string_append(str, "USER_DEFINED ");
			else 
				g_string_sprintfa(str, "unknown opcode %#x ",
						 opcode);
			break;
		}
	}
}
