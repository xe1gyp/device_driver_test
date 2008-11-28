/*********************************************************************
 *                
 * Filename:      irdadump.h
 * Version:       
 * Description:   
 * Status:        Experimental.
 * Author:        Dag Brattli <dagb@cs.uit.no>
 * Created at:    Tue Feb 16 13:31:29 1999
 * Modified at:   Wed Dec  8 09:45:35 1999
 * Modified by:   Dag Brattli <dagb@cs.uit.no>
 * 
 *     Copyright (c) 1999 Dag Brattli, All Rights Reserved.
 *      
 *     This program is free software; you can redistribute it and/or 
 *     modify it under the terms of the GNU General Public License as 
 *     published by the Free Software Foundation; either version 2 of 
 *     the License, or (at your option) any later version.
 *  
 *     Neither Dag Brattli nor University of Tromsø admit liability nor
 *     provide warranty for any of this software. This material is 
 *     provided "AS-IS" and at no charge.
 *     
 ********************************************************************/

#ifndef IRDADUMP_H
#define IRDADUMP_H

#include <netbuf.h>

#ifndef AF_IRDA
#  define PF_IRDA          23        /* <linux/socket.h> */
#  define AF_IRDA          PF_IRDA
#endif

#ifndef ARPHRD_IRDA
#  define ARPHRD_IRDA      783
#endif

#ifndef ETH_P_IRDA
#  define ETH_P_IRDA       0x0017
#endif

#define MAX_FRAME_SIZE 2050

#define IRADDR_ANY 0xffffffff

/* Frame types and templates */
#define INVALID   0xff

/* Unnumbered (U) commands */
#define SNRM_CMD  0x83 /* Set Normal Response Mode */
#define DISC_CMD  0x43 /* Disconnect */
#define XID_CMD   0x2f /* Exchange Station Identification */
#define TEST_CMD  0xe3 /* Test */

/* Unnumbered responses */
#define RNRM_RSP  0x83 /* Request Normal Response Mode */
#define UA_RSP    0x63 /* Unnumbered Acknowledgement */
#define FRMR_RSP  0x87 /* Frame Reject */
#define DM_RSP    0x0f /* Disconnect Mode */
#define RD_RSP    0x43 /* Request Disconnection */
#define XID_RSP   0xaf /* Exchange Station Identification */
#define TEST_RSP  0xe3 /* Test frame */

/* Supervisory (S) */
#define RR        0x01 /* Receive Ready */
#define REJ       0x09 /* Reject */
#define RNR       0x05 /* Receive Not Ready */
#define SREJ      0x0d /* Selective Reject */

/* Information (I) */
#define I_FRAME   0x00 /* Information Format */
#define UI_FRAME  0x03 /* Unnumbered Information */

#define CMD_FRAME  0x01
#define RSP_FRAME  0x00

#define PF_BIT 0x10 /* Poll/final bit */

#define BROADCAST  0xffffffff /* Broadcast device address */
#define CBROADCAST 0xfe       /* Connection broadcast address */

/* IrLMP frame opcodes */
#define CONNECT_CMD    0x01
#define CONNECT_RSP    0x81
#define DISCONNECT     0x02
#define ACCESSMODE_CMD 0x03
#define ACCESSMODE_RSP 0x83

#define CONTROL_BIT    0x80

/* LSAP-SEL's */
#define LSAP_MASK      0x7f
#define LSAP_IAS       0x00
#define LSAP_ANY       0xff

#define IAP_LST 0x80
#define IAP_ACK 0x40

/* IrIAP Op-codes */
#define GET_INFO_BASE      0x01
#define GET_OBJECTS        0x02
#define GET_VALUE          0x03
#define GET_VALUE_BY_CLASS 0x04
#define GET_OBJECT_INFO    0x05
#define GET_ATTRIB_NAMES   0x06

#define IAS_SUCCESS        0x00
#define IAS_CLASS_UNKNOWN  0x01
#define IAS_ATTRIB_UNKNOWN 0x02

/* LM-IAS Attribute types */
#define IAS_MISSING 0
#define IAS_INTEGER 1
#define IAS_OCT_SEQ 2
#define IAS_STRING  3

/* LAP parameters definitions (high bit masked) */
#define PI_BAUD_RATE     0x01
#define PI_MAX_TURN_TIME 0x02
#define PI_DATA_SIZE     0x03
#define PI_WINDOW_SIZE   0x04
#define PI_ADD_BOFS      0x05
#define PI_MIN_TURN_TIME 0x06
#define PI_LINK_DISC     0x08

#define IR_115200_MAX 0x3f

/* Baud rates (first byte) */
#define IR_2400     0x01
#define IR_9600     0x02
#define IR_19200    0x04
#define IR_38400    0x08
#define IR_57600    0x10
#define IR_115200   0x20
#define IR_576000   0x40
#define IR_1152000  0x80

/* Baud rates (second byte) */
#define IR_4000000  0x01
#define IR_16000000 0x02

struct xid_frame {
	guint8  caddr; /* Connection address */
	guint8  control;
	guint8  ident; /* Should always be XID_FORMAT */ 
	guint32 saddr; /* Source device address */
	guint32 daddr; /* Destination device address */
	guint8  flags; /* Discovery flags */
	guint8  slotnr;
	guint8  version;
	guint8  discovery_info[0];
} __attribute__((packed));

struct test_frame {
	guint8 caddr;          /* Connection address */
	guint8 control;
	guint32 saddr;         /* Source device address */
	guint32 daddr;         /* Destination device address */
	guint8 info[0];        /* Information */
} __attribute__((packed));

struct ua_frame {
	guint8 caddr;
	guint8 control;

	guint32 saddr; /* Source device address */
	guint32 daddr; /* Dest device address */
	guint8  params[0];
} __attribute__((packed));
	
struct i_frame {
	guint8 caddr;
	guint8 control;
	guint8 data[0];
} __attribute__((packed));

struct s_frame {
	guint8 caddr;
	guint8 control;
} __attribute__((packed));

struct rnrm_frame {
	guint8 caddr;          /* Connection address */
	guint8 control;
	guint32 saddr;         /* Source device address */
	guint32 daddr;         /* Destination device address */
}__attribute__((packed));

struct snrm_frame {
	guint8  caddr;
	guint8  control;
	guint32 saddr;
	guint32 daddr;
	guint8  ncaddr;
	guint8  params[0];
} __attribute__((packed));

#define MAX_CONNECTIONS 15

struct ias_query {
	guint8 lsap_sel;
	int ttp;
	int obex;
	int ircomm;
	int irnet;
};

struct lsap_state {
	gboolean valid;      /* Is this a valid connection */
	gboolean ttp;        /* True if TTP connection */
	gboolean obex;       /* True if OBEX connection */	
	gboolean ircomm;     /* True if IrCOMM/IrLTP connection */
	gboolean irnet;      /* True if IrNET connection */
	guint32  saddr;      /* Source device address */
	guint32  daddr;      /* Destination device address */
	guint8   slsap_sel;  /* Source logical service access point */
	guint8   dlsap_sel;  /* Destination logical service access point */
	guint8   caddr;      /* LAP connection address (for garbage collect) */
};

inline void parse_obex(GNetBuf *buf, GString *str, int cmd);
inline void parse_irlmp(GNetBuf *buf, GString *str,
			guint8 caddr, int type, int cmd);
inline void parse_ui_irlmp(GNetBuf *buf, GString *str, int type);
void parse_ircomm_params(guint8 clen, GNetBuf *buf, GString *str);
void parse_ircomm_connect(GNetBuf *buf, GString *str);
void parse_ircomm_lmp(GNetBuf *buf, GString *str);
void parse_ircomm_ttp(GNetBuf *buf, GString *str);
int find_connection(guint8 slsap_sel, guint8 dlsap_sel);
int find_free_connection(void);
void parse_irlap_frame(int type, GNetBuf *buf, GString *str);
int irlap_init(void);
int irdadump_init(char *ifdev);
int irdadump_loop(GString *str);

/* Configuration options */
extern int config_print_diff;
extern int config_print_irlap;
extern int config_dump_frame;
extern int config_print_irlap;
extern int config_print_irlmp;
extern int config_print_lost_frames;
extern int config_snaplen;
extern int config_dump_bytes;
extern int config_snapcols;
extern int config_force_ttp;
extern int config_capturewrite;
extern int config_captureread;

#endif /* IRDADUMP_H */
