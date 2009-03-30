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

; 8051 control

CPUCS 			= 0x7f92

; I2C

I2CS 			= 0x7fa5
I2DAT 			= 0x7fa6
I2CMODE 		= 0x7fa7

; endpoint buffer

OUT0BUF		= 0x7ec0
IN0BUF		= 0x7f00
OUT2BUF		= 0x7dc0
IN2BUF		= 0x7e00

; interrupts

IN07IRQ		= 0x7fa9
OUT07IRQ	= 0x7faa
USBIRQ		= 0x7fab
IN07IEN		= 0x7fac
OUT07IEN	= 0x7fad
USBIEN		= 0x7fae
USBBAV		= 0x7faf

; breakpoint

BREAKPT		= USBBAV
BPADDRH		= 0x7fb2
BPADDRL		= 0x7fb3

; ep control/status

EP0CS		= 0x7fb4
IN0BC		= 0x7fb5
OUT0BC		= 0x7fc5
IN2CS		= 0x7fb8
IN2BC		= 0x7fb9
OUT2CS		= 0x7fc8
OUT2BC		= 0x7fc9

; global usb register

SUDPTRH		= 0x7fd4
SUDPTRL		= 0x7fd5
USBCS		= 0x7fd6
TOGCTL		= 0x7fd7
USBFRAMEL	= 0x7fd8
USBFRAMEH	= 0x7fd9
FNADDR		= 0x7fdb
USBPAIR		= 0x7fdd
IN07VAL		= 0x7fde
OUT07VAL	= 0x7fdf
INISOVAL	= 0x7fe0
OUTISOVAL	= 0x7fe1

; autoptr

AUTOPTRH	= 0x7fe3
AUTOPTRL	= 0x7fe4
AUTODATA	= 0x7fe5

; setup data (8bytes)

SETUPBUF 		= 0x7fe8

; iso-related stuff

OUT8DATA	= 0x7f60
IN8DATA		= 0x7f68
OUT8BCH		= 0x7f70
OUT8BCL		= 0x7f71
ISOERR		= 0x7fa0
ISOCTL		= 0x7fa1
ZBCOUT		= 0x7fa2
OUT8ADDR	= 0x7ff0
OUT9ADDR	= 0x7ff1
OUT10ADDR	= 0x7ff2
OUT11ADDR	= 0x7ff3
OUT12ADDR	= 0x7ff4
OUT13ADDR	= 0x7ff5
OUT14ADDR	= 0x7ff6
OUT15ADDR	= 0x7ff7
IN8ADDR		= 0x7ff8
IN9ADDR		= 0x7ff9
IN10ADDR	= 0x7ffa
IN11ADDR	= 0x7ffb
IN12ADDR	= 0x7ffc
IN13ADDR	= 0x7ffd
IN14ADDR	= 0x7ffe
IN15ADDR	= 0x7fff

