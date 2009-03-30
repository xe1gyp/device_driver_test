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

.module	main

.sbttl entry point, interrupt vectors, initialization, idle loop

.include "firmware.inc"

;**************************************************+

.area IVEC (ABS)

entry::
.org	0x0000
	ljmp	main

.org	0x0043
	ljmp	isr_vector_page

isr_vector_page = 0x0100

.org	0x0100					; USB SUDAV
	ljmp	sudav_isr

.org	0x0104					; USB SOF
	ljmp	sof_isr

.org	0x0110					; USB RESET
	ljmp	reset_isr

.if (TARGET_MCU & EZUSB_TYPE_FX2)		; it's a FX2

.org	0x0114					; USB HSGRANT
	ljmp	hispeed_isr

.org	0x0130					; EP2 (OUT)
	ljmp	sink_isr

.org	0x0138					; EP6 (IN)
	ljmp	source_isr

.else						; it's an AN21XX or FX

.org	0x0128					; EP2IN
	ljmp	source_isr

.org	0x012c					; EP2OUT
	ljmp	sink_isr

.endif

;**************************************************+

.area	CODE	(REL,CON)

;--------------------------------------------

CKCON_XMEM_WS_MASK = 0b00000111

init_8051:
	clr	a
	mov	PSW, a
	mov	IE, a
	mov	IP, a
	mov	EXIF, a
	mov	EICON, a
	mov	EIE, a
	mov	EIP, a
	mov	PCON, a
	mov	DPS, a
	mov	PSW, a
	mov	a, CKCON
	anl	a, #~CKCON_XMEM_WS_MASK		; 0-ws XMEM access
	mov	CKCON, a
.if DEBUG
	clr	a
	lcall	debug_show_LED
.endif
	ret

;--------------------------------------------

main::
	clr	EA
	mov	SP, #0x7f			; 128 byte stack space

	lcall	init_8051

	; give usbcore time to ACK the final EP0 firmware download request

	mov	r2, #0
1$:	djnz	r2, 1$				; 256x3 cycles = 64 usec (48MHz)

	lcall	usb_disconnect

	lcall	usb_setup_device

	; prevent races with the host still handling the disconnect

	mov	r2, #3
2$:	mov	r3, #0
3$:	mov	r4, #0
4$:	djnz	r4, 4$				; 3x256x256x3 cycles = 49msec (48MHz)
	djnz	r3, 3$
	djnz	r2, 2$

	setb	EA

	lcall	usb_connect

.if DEBUG
	mov	dptr, #idle
	lcall	debug_set_hotspot
.endif


schedule:
	; nothing to do for now - it's all done in interrupt
	mov	r2, #12
idle:	djnz	r2, idle			; spin 3 us here
	sjmp	schedule

;**********************************
