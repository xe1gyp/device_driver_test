;-------------------------------------------------------------------------
;
;	Test firmware for devices using EZ-USB Microcontroller
;
;	Copyright (c) 2004 Martin Diehl
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

.module isoxfer

.sbttl ISO in/out support

.include "firmware.inc"

;**************************************************+

.area	ISR

;--------------------------------------------

; depending on AS, setup the iso-ep(s)

iso_setup::
.if (TARGET_MCU & EZUSB_TYPE_FX2)	; it's a FX2
	ret
.else

	; set iso fifo buffer ranges

	mov	dptr, #OUT8ADDR
	clr	a
	movx	@dptr, a
	add	a, #(512/4)		; iso-out size
	mov	r2, #7
2$:	inc	dptr
	movx	@dptr, a
	djnz	r2, 2$
	mov	dptr, #IN8ADDR
	movx	@dptr, a
	add	a, #(512/4)		; iso-out size
	mov	r2, #7
3$:	inc	dptr
	movx	@dptr, a
	djnz	r2, 3$

	; enable iso endpoint 8 in/out

	mov	a, #0b00000001
	mov	dptr, #OUTISOVAL
	movx	@dptr, a
	mov	dptr, #INISOVAL
	movx	@dptr, a

	ret

.endif

;--------------------------------------------

iso_handle_sof::
.if (TARGET_MCU & EZUSB_TYPE_FX2)	; it's a FX2
	ret
.else
	mov	PSW, #PSW_BANK2		; switch register bank - caller restores PSW
	mov	a, ALTSETTING
	jnz	iso_out			; only if AS!=0
	ret

iso_out:
	mov	dptr, #ZBCOUT
	movx	a, @dptr
	jb	ACC.0, iso_in		; no data arrived

	mov	dptr, #ISOERR
	movx	a, @dptr
	jb	ACC.0, iso_in		; crc-error, ignore data

	mov	dptr, #OUT8BCL
	movx	a, @dptr
	mov	r2, a
	mov	dptr, #OUT8BCH
	movx	a, @dptr
	mov	r3, a
	orl	a, r2
	jz	iso_in			; BC=0
	inc	r3
	cjne	r2, #0, 1$
	dec	r3
1$:	mov	dptr, #OUT8DATA
2$:	movx	a, @dptr
	djnz	r2, 2$
	djnz	r3, 2$

iso_in:
	mov	r2, #<512
	mov	r3, #>512
	inc	r3
	cjne	r2, #0, 1$
	dec	r3
1$:	mov	dptr, #IN8DATA
	clr	a
2$:	movx	@dptr, a
	djnz	r2, 2$
	djnz	r3, 2$
	
	ret
.endif

;**************************************************
