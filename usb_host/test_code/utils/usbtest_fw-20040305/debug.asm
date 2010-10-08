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

.module debug

.sbttl debug helpers

.include "firmware.inc"

;**************************************************+

.area	CODE	(REL,CON)

;--------------------------------------------

.if DEBUG

debug_set_hotspot::

.if (TARGET_FLAGS & DEV_BOARD_MONITOR)
	mov	r1, DPL
	mov	a, DPH
	mov	dptr, #BPADDRH
	movx	@dptr, a
	mov	dptr, #BPADDRL
	mov	a, r1
	movx	@dptr, a

	mov	dptr, #BREAKPT
	movx	a, @dptr
	anl	a, #0b11110001
	orl	a, #0b00001110
	movx	@dptr, a
.endif
	ret

;*************************************************
;
;	debug_show_LED
;
;	Write a to LED (7-segments) connected to I2C bus

I2C_LED 	= 0b01000010	; I2C-cmd: write to PCF8574 unit 1

debug_show_LED:

.if (TARGET_FLAGS & DEV_BOARD_I2C_LED)
	cpl	a
	push	ACC

	mov	dptr, #I2CS
1$:	movx	a, @dptr
	jb	ACC.6, 1$

	mov	a, #0b10000000
	movx	@dptr, a

	mov	dptr, #I2DAT
	mov	a, #I2C_LED
	movx	@dptr, a
	mov	dptr, #I2CS
2$:	movx	a, @dptr
	jnb	ACC.0, 2$

	mov	dptr, #I2DAT
	pop	ACC
	movx	@dptr, a
	mov	dptr, #I2CS
3$:	movx	a, @dptr
	jnb	ACC.0, 3$

	mov	a, #0b01000000
	movx	@dptr, a
.endif
	ret

.endif

;**********************************

