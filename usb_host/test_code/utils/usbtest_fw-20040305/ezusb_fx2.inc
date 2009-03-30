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

; endpoint config

EP1OUTCFG	= 0xe610
EP1INCFG	= 0xe611
EP2CFG		= 0xe612
EP4CFG		= 0xe613
EP6CFG		= 0xe614
EP8CFG		= 0xe615

; endpoint control/status

EP0CS		= 0xe6a0
EP0BCH		= 0xe68a
EP0BCL		= 0xe68b
EP2CS		= 0xe6a3
EP2BCH		= 0xe690
EP2BCL		= 0xe691
EP6CS		= 0xe6a5
EP6BCH		= 0xe698
EP6BCL		= 0xe699

; general usb

USBCS		= 0xe680
TOGCTL		= 0xe683
USBFRAMEH	= 0xe684
USBFRAMEL	= 0xe685
MICROFRAME	= 0xe686
FNADDR		= 0xe687
EP0CS		= 0xe6a0
SUDPTRH		= 0xe6b3
SUDPTRL		= 0xe6b4

; setup data (8bytes)

SETUPBUF 	= 0xe6b8

; interrupts

INTSETUP	= 0xe668
USBIE		= 0xe65c
USBIRQ		= 0xe65d
EPIE		= 0xe65e
EPIRQ		= 0xe65f

; breakpoint

BREAKPT		= 0xe605
BPADDRH		= 0xe606
BPADDRL		= 0xe607

; endpoint buffer

EP0BUF		= 0xe740
EP2FIFOBUF	= 0xf000
EP6FIFOBUF	= 0xf800

