;-------------------------------------------------------------------------
;
;	Test firmware for devices using EZ-USB Microcontroller
;
;	Copyright (c) 2003-2004 Martin Diehl
;
;	This program is free software; you can redistribute it and/or 
;	modify it under the terms of version 2 of the GNU General Public
;	License as published by the Free Software Foundation.
;
;	This program is distributed in the hope that it will be useful,
;	but WITHOUT ANY WARRANTY; without even the implied warranty of
;	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
;	GNU General Public License for more details.
;
;	You should have received a copy of the GNU General Public License 
;	along with this program; if not, write to the Free Software 
;	Foundation, Inc., 59 Temple Place, Suite 330, Boston, 
;	MA 02111-1307 USA
;
;-------------------------------------------------------------------------

; device states

USB_STATE_DETACHED	= 0x00			; outside the scope of the spec
USB_STATE_ATTACHED	= 0x01
USB_STATE_POWERED	= 0x02
USB_STATE_DEFAULT	= 0x03
USB_STATE_ADDRESS	= 0x04
USB_STATE_CONFIGURED	= 0x05
USB_STATE_SUSPENDED	= 0x80			; to be OR'ed with the other states

;****************************************************************

; bmRequestType bits

USB_REQ_DIR_MASK    = 0b10000000
USB_REQ_DIR_OUT     = 0b00000000
USB_REQ_DIR_IN      = 0b10000000
USB_REQ_TYPE_MASK   = 0b01100000
USB_REQ_TYPE_STD    = 0b00000000
USB_REQ_TYPE_CLASS  = 0b00100000
USB_REQ_TYPE_VENDOR = 0b01000000
USB_REQ_TYPE_RSVD   = 0b01100000
USB_REQ_RECP_MASK   = 0b00011111
USB_REQ_RECP_DEV    = 0b00000000
USB_REQ_RECP_IF     = 0b00000001
USB_REQ_RECP_EP     = 0b00000010
USB_REQ_RECP_OTHER  = 0b00000011

;*************************************************************

; bmRequestType values used by EP0 standard requests

USB_REQ_STD_IN_DEV = (USB_REQ_DIR_IN | USB_REQ_TYPE_STD | USB_REQ_RECP_DEV)
USB_REQ_STD_OUT_DEV = (USB_REQ_DIR_OUT | USB_REQ_TYPE_STD | USB_REQ_RECP_DEV)
USB_REQ_STD_IN_IF = (USB_REQ_DIR_IN | USB_REQ_TYPE_STD | USB_REQ_RECP_IF)
USB_REQ_STD_OUT_IF = (USB_REQ_DIR_OUT | USB_REQ_TYPE_STD | USB_REQ_RECP_IF)
USB_REQ_STD_IN_EP = (USB_REQ_DIR_IN | USB_REQ_TYPE_STD | USB_REQ_RECP_EP)
USB_REQ_STD_OUT_EP = (USB_REQ_DIR_OUT | USB_REQ_TYPE_STD | USB_REQ_RECP_EP)

;*************************************************************

; bRequest values for standard usb requests

USB_RQ_GET_STATUS		= 0x00
USB_RQ_CLEAR_FEATURE		= 0x01
; 2: reserved
USB_RQ_SET_FEATURE		= 0x03
; 4: reserved
USB_RQ_SET_ADDRESS		= 0x05
USB_RQ_GET_DESCRIPTOR		= 0x06
USB_RQ_SET_DESCRIPTOR		= 0x07
USB_RQ_GET_CONFIGURATION	= 0x08
USB_RQ_SET_CONFIGURATION	= 0x09
USB_RQ_GET_INTERFACE		= 0x0a
USB_RQ_SET_INTERFCAE		= 0x0b
USB_RQ_SYNCH_FRAME		= 0x0c

;*************************************************************

; features addresses used by Get/SetFeature

USB_FEAT_ENDPOINT_HALT		= 0x00
USB_FEAT_DEVICE_REMOTE_WAKEUP	= 0x01
USB_FEAT_TEST_MODE		= 0x02

;*************************************************************

; descriptor types used by Get/SetDescriptor

USB_DESCR_TYPE_DEVICE		= 0x01
USB_DESCR_TYPE_CONFIGURATION	= 0x02
USB_DESCR_TYPE_STRING		= 0x03

;*************************************************************

; useful values for building descriptors

USB_BCD_VER_1_0		= 0x0100
USB_BCD_VER_1_1		= 0x0110
USB_BCD_VER_2_0		= 0x0200

USB_DT_DEVICE		= 0x01
USB_DT_CONFIG		= 0x02
USB_DT_STRING		= 0x03
USB_DT_INTERFACE	= 0x04
USB_DT_ENDPOINT		= 0x05

USB_DT_DEVICE_LEN	= 0x12
USB_DT_CONFIG_LEN	= 0x09
USB_DT_INTERFACE_LEN	= 0x09
USB_DT_ENDPOINT_LEN	= 0x07

USB_ENDPOINT_CONTROL	= 0x00
USB_ENDPOINT_ISO	= 0x01
USB_ENDPOINT_BULK	= 0x02
USB_ENDPOINT_INT	= 0x03

USB_CLASS_PER_INTERFACE	= 0x00
USB_CLASS_VENDOR_SPEC	= 0xff

USB_PROTO_NONE		= 0x00
USB_PROTO_VENDOR_SPEC	= 0xff

USB_ATTRIB_SELFPOWER	= (0b10000000 | 0b01000000)
USB_ATTRIB_REM_WAKEUP	= (0b10000000 | 0b00100000)

USB_MAXPOWER_100	= (100/2)
USB_MAXPOWER_500	= (500/2)

;*************************************************************
