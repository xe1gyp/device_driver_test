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

; used for switching the register banks

PSW_BANK0 = 0b00000000
PSW_BANK1 = 0b00001000
PSW_BANK2 = 0b00010000
PSW_BANK3 = 0b00011000

;********************************************************************

; nonstandard SFR registers

DPS		= 0x86
CKCON		= 0x8e
EXIF		= 0x91
MPAGE		= 0x92
EICON		= 0xd8
EIE		= 0xe8
EIP		= 0xf8

EXIF_INT2_MASK	= 0b11101111		; used to clear vectored USB IRQ

EXUSB		= EIE+0			; EIE.0 - INT2(USBIRQ) enable
PXUSB		= EIP+0			; EIP.0 - INT2(USBIRQ) priority

;********************************************************************

.if (TARGET_MCU ^ EZUSB_TYPE_AN21XX)	; it's a FX or FX2
INT2CLR		= 0xa1
INT4CLR		= 0xa2
.endif

;********************************************************************

.if (TARGET_MCU & EZUSB_TYPE_AN21XX)	; it's an AN21XX
.error (TARGET_MCU ^ EZUSB_TYPE_AN21XX)	; ambigous target controller setup
.include "ezusb_an21xx.inc"
.endif

.if (TARGET_MCU & EZUSB_TYPE_FX)	; it's a FX
.error (TARGET_MCU ^ EZUSB_TYPE_FX)	; ambigous target controller setup
.include "ezusb_fx.inc"
.endif

.if (TARGET_MCU & EZUSB_TYPE_FX2)	; it's a FX2
.error (TARGET_MCU ^ EZUSB_TYPE_FX2)	; ambigous target controller setup
.include "ezusb_fx2.inc"
USBIEN = USBIE				; different names fx2 vs. non-fx2
.endif

;********************************************************************
