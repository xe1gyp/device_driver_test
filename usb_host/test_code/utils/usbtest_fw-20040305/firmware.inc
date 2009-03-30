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

; supported members of the EZ-USB MCU family - don't modify
; selection of appropriate type is done in config.inc

EZUSB_TYPE_AN21XX = 0x01
EZUSB_TYPE_FX = 0x02
EZUSB_TYPE_FX2 = 0x04

.include "config.inc"
.include "ezusb.inc"
.include "usb.inc"

; -----------------------------------------------

; different source and sink modes

SOURCE_MODE_PRELOAD = 0		; whatever was preloaded into INBUF, len=maxpacket
SOURCE_MODE_COUNTER = 1		; 32-bit packet counter in first 4 bytes, little endian
SOURCE_MODE_FRAME = 2		; (frame# << 8 | mircoframe) in first 4 bytes, little endian
SOURCE_MODE_ZERO8 = 3		; every 8th packet has len=0, all other len=maxpacket
SOURCE_MODE_SHORT = 4		; len=maxpacket, ..., 2, 1, maxpacket,... sequence

SINK_MODE_ANY = 0		; sink whatever the host sent, any len

; -----------------------------------------------

.globl	source_table, sink_table
.globl	jump_table_gate
.globl	sudav_isr, sof_isr, reset_isr
.globl	usb_ep0_request
.globl	usb_disconnect, usb_connect, usb_disable_endpoints
.globl	usb_apply_configuration, usb_reset_interface, usb_reset_endpoint
.globl	source_isr, sink_isr
.globl	usb_setup_device
.globl	set_mode_request
.globl	iso_setup, iso_handle_sof
.globl	intel_ctrl_out, intel_ctrl_in

.if (TARGET_MCU & EZUSB_TYPE_FX2)	; it's a FX2
.globl	hispeed_isr
.endif

.if DEBUG
.globl	debug_set_hotspot, debug_show_LED
.endif

.area	IVEC (ABS)
.area	ISR (REL,CON)
ISR_BASE == 0x0200
.area	CODE (REL,CON)
.area	USB_DESCR (REL,CON)
USB_DESCR_BASE == 0x0a00

; control r/w buffer starts at 0x0c00!

; registers used to store our variables

COUNTER_1 = 0x30
COUNTER_2 = 0x31
COUNTER_3 = 0x32
COUNTER_4 = 0x33

SOURCE_LEN_L = 0x34
SOURCE_LEN_H = 0x35

SOURCE_DATA_1 = 0x36
SOURCE_DATA_2 = 0x37

MAXLEN_L = 0x38
MAXLEN_H = 0x39

DEVICE_STATE = 0x40
ALTSETTING = 0x41
SOURCE_MODE = 0x42
SINK_MODE = 0x43
INTEL_LEFT_L = 0x44
INTEL_LEFT_H = 0x45

HS_NOT_FS = 0x00
WAKEUP_ENABLED = 0x01

VENDOR_REQUEST_SET_MODE = 0xb0
VENDOR_REQUEST_INTEL_OUT = 0x5b
VENDOR_REQUEST_INTEL_IN = 0x5c
USB_REQ_VENDOR_IN_DEV = (USB_REQ_DIR_IN | USB_REQ_TYPE_VENDOR | USB_REQ_RECP_DEV)
USB_REQ_VENDOR_OUT_DEV = (USB_REQ_DIR_OUT | USB_REQ_TYPE_VENDOR | USB_REQ_RECP_DEV)

; -----------------------------------------------

; provide common mappings to different an21xx/fx/fx2 registers

.if (TARGET_MCU & EZUSB_TYPE_FX2)	; it's a FX2

SINK_EP = 0x02
SINK_EP_BUF = EP2FIFOBUF
SINK_CS = EP2CS
SINK_BUSY_MASK = 0b00001000		; ep full bit
SINK_BC2 = EP2BCH
SINK_BC = EP2BCL
SINK_INT_MASK = 0b00010000

SOURCE_EP = 0x86
SOURCE_EP_BUF = EP6FIFOBUF
SOURCE_CS = EP6CS
SOURCE_BUSY_MASK = 0b00001000		; ep full bit
SOURCE_BC2 = EP6BCH
SOURCE_BC = EP6BCL
SOURCE_INT_MASK = 0b01000000

INT_SETUP_REG = INTSETUP
INT_SETUP_VALUE = 0b00010000		; enable INT2 autovector

.else					; must be AN21XX or FX

SINK_EP = 0x02
SINK_EP_BUF = OUT2BUF
SINK_CS = OUT2CS
SINK_BUSY_MASK = 0b00000010		; ep busy bit
SINK_BC = OUT2BC
SINK_INT_MASK = 0b00000100

SOURCE_EP = 0x82
SOURCE_EP_BUF = IN2BUF
SOURCE_CS = IN2CS
SOURCE_BUSY_MASK = 0b00000010		; ep busy bit
SOURCE_BC = IN2BC
SOURCE_INT_MASK = 0b00000100

INT_SETUP_REG = USBBAV
.if (TARGET_MCU & EZUSB_TYPE_AN21XX)	; it's an AN21XX
INT_SETUP_VALUE = 0b00000001		; enable INT2 autovector
.else					; it's a FX
INT_SETUP_VALUE = 0b00010001		; enable INT2CLR and INT2 autovector
.endif

.endif
