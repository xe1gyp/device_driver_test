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

; uncomment exactly one of them to set the TARGET_MCU

TARGET_MCU = EZUSB_TYPE_AN21XX
; TARGET_MCU = EZUSB_TYPE_FX
; TARGET_MCU = EZUSB_TYPE_FX2

;********************************************************************

; DEBUG = 1
DEBUG = 0

;********************************************************************

; selecteable firmware features

FEAT_RENUM		= 0x0001	; re-enumerate

; special features (development board, MCU flavours)

DEV_BOARD_MONITOR	= 0x0101	; the green LED (breakpoint)
DEV_BOARD_I2C_LED	= 0x0102	; 7-seg LED on I2C bus
DEV_BOARD_TTY0		= 0x0104	; serial port (SIO0: 9600 8N1, 3-wire)
MCU_XMEM_8K		= 0x0201	; assume device has 8KB onchip XMEM
MCU_RECLAIM_ISOBUF	= 0x0202	; assume device has 2KB reclaimable ISO-buffer

; !!! DONT ENABLE ANY DEVBOARD OR MCU FEATURE IF THE FIRMWARE
; !!! IS EXPECTED TO RUN ON REUSED STOCK DEVICES!

; TARGET_FLAGS = DEV_BOARD_MONITOR | DEV_BOARD_I2C_LED | FEAT_RENUM

TARGET_FLAGS = FEAT_RENUM

;********************************************************************
