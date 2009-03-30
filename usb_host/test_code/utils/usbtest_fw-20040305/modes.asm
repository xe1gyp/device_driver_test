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

.module modes

.sbttl different modes for data sourcing and sinking

.include "firmware.inc"

;**************************************************+

.area	ISR

;--------------------------------------------

source_table::
	ljmp	source_preload
	ljmp	source_counter
	ljmp	source_frame
	ljmp	source_zero8
	ljmp	source_short

sink_table::
	ljmp	sink_any

;--------------------------------------------

jump_table_gate::
	mov	r0, a
	add	a, r0
	add	a, r0
	jmp	@a+dptr

;--------------------------------------------

; helpers that implement different packet sources
; all called directly from interrupt
; on entry, r2:r3 is set to the default maxpacketlen
; on exit, r2:r3 returns length of the sourced packet

source_preload:
	ret

source_counter:
	mov	dptr, #SOURCE_EP_BUF
	mov	a, #1
	add	a, COUNTER_1
	xch	a, COUNTER_1
	movx	@dptr, a
	inc	dptr
	clr	a
	addc	a, COUNTER_2
	xch	a, COUNTER_2
	movx	@dptr, a
	inc	dptr
	clr	a
	addc	a, COUNTER_3
	xch	a, COUNTER_3
	movx	@dptr, a
	inc	dptr
	clr	a
	addc	a, COUNTER_4
	xch	a, COUNTER_4
	movx	@dptr, a
	ret

SOURCE_MODE_FRAME = 2

source_frame:
	mov	dptr, #USBFRAMEL
1$:	movx	a, @dptr
	mov	r5, a
	mov	dptr, #USBFRAMEH
	movx	a, @dptr
	mov	r6, a
	mov	dptr, #USBFRAMEL
	movx	a, @dptr
	xrl	a, r5
	jnz	1$				; repeat if we race with SOF
	mov	r4, #0				; microframe==0 if not HS
.if (TARGET_MCU & EZUSB_TYPE_FX2)		; it's a FX2
	jnb	HS_NOT_FS, 2$
	mov	dptr, #MICROFRAME
	movx	a, @dptr
	mov	r4, a
	mov	dptr, #USBFRAMEL
	movx	a, @dptr
	xrl	a, r5
	jnz	1$				; repeat if we race with SOF
2$:
.endif
	mov	dptr, #SOURCE_EP_BUF
	mov	a, r4
	movx	@dptr, a
	inc	dptr
	mov	a, r5
	movx	@dptr, a
	inc	dptr
	mov	a, r6
	movx	@dptr, a
	inc	dptr
	clr	a
	movx	@dptr, a			; clear MSB of 32-bit representation
	ret

source_zero8:
	mov	a, SOURCE_DATA_1
.if 1						; sanity check, should be obsolete later
	jz	1$
	mov	r4, a
	dec	a
	anl	a, r4
	jz	2$
1$:	mov	r4, #0b00000001
2$:	mov	a, r4
.endif
	rr	a
	mov	SOURCE_DATA_1, a
	jnb	ACC.0, 3$
	clr	a
	mov	r2, a
	mov	r3, a
3$:	ret

source_short:
	mov	r3, SOURCE_DATA_1
	mov	r2, SOURCE_DATA_2
	mov	a, r3
	clr	c
	subb	a, #1
	mov	SOURCE_DATA_1, a
	orl	a, r2
	jnz	1$
	mov	SOURCE_DATA_1, SOURCE_LEN_L
	mov	SOURCE_DATA_2, SOURCE_LEN_H
	ret
1$:	mov	a, r2
	subb	a, #0
	mov	SOURCE_DATA_2, a
	ret

;**************************************************+

; helpers that implement different packet sinks
; all called directly from interrupt
; invoked with r2:r3 indicating the length of the packet

sink_any:
	ret

;**************************************************+

set_mode_request::
	setb	c
	ret

;**************************************************+
