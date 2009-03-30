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

.module ctrlout

.sbttl vendor control R/W request like INTEL's compliance test device

.include "firmware.inc"

;****************************************************************

.area	CODE

;****************************************************************

CTRL_BUF_ADDR = 0x0c00
CTRL_BUF_SIZE = 0x0400


;
; everything called from ep0-handler, i.e. SUDAV-context!
;

intel_setup_ctrl:
	inc	r0
	inc	r0
	inc	r0
	inc	r0
	inc	r0
	movx	a, @r0				; wLengthL
	mov	INTEL_LEFT_L, a
	inc	r0
	movx	a, @r0				; wLengthH
	mov	INTEL_LEFT_H, a
	orl	a, INTEL_LEFT_L
	clr	c
	jz	1$				; accept if wLength == 0

	mov	a, INTEL_LEFT_L
	subb	a, #1
	mov	a, INTEL_LEFT_H
	subb	a, #0
	anl	a, #~(CTRL_BUF_SIZE / 256 - 1)	; test (wLength-1) < 1024
	jz	1$
	setb	c				; reject: too long
	ret

1$:	mov	dptr, #AUTOPTRL
	mov	a, #<CTRL_BUF_ADDR
	movx	@dptr, a
	mov	dptr, #AUTOPTRH
	mov	a, #>CTRL_BUF_ADDR
	movx	@dptr, a
	mov	dptr, #AUTODATA
	mov	MPAGE, #>EP0CS
	clr	c
	ret

;----------------------------------------------

intel_do_out:
	mov	r0, #<EP0CS
1$:	movx	a, @r0
	jb	ACC.3, 1$			; make sure OUTBSY is cleared

; what about zero-packet if wLength==0?
; linux-usb ehci/ohci don't send one so we don't expect it here!
; IMHO not clear from specs (5.5.3)

loop_out:
	mov	a, INTEL_LEFT_L
	orl	a, INTEL_LEFT_H
	jz	5$				; exit if done
	mov	r1, #<OUT0BC
	movx	@r1, a				; arm out-ep
2$:	movx	a, @r0
	jb	ACC.3, 2$			; wait until data packet received
	movx	a, @r1				; get packetlen
	jz	4$				; zerolen packet received
	mov	r2, a
	mov	a, INTEL_LEFT_L
	clr	c
	subb	a, r2
	mov	INTEL_LEFT_L, a
	mov	a, INTEL_LEFT_H
	subb	a, #0
	mov	INTEL_LEFT_H, a			; update remaining byte counter
	jc	6$				; overrun - too much data
	mov	r1, #<OUT0BUF
	dec	MPAGE				; OUT0BUF is in 0x7e-segment!
3$:	movx	a, @r1
	movx	@dptr, a
	inc	r1
	djnz	r2, 3$				; copy data from ep
	inc	MPAGE
	sjmp	loop_out			; loop until done

4$:	mov	a, INTEL_LEFT_L
	orl	a, INTEL_LEFT_H
	jnz	6$
5$:	clr	c
	ret
6$:	setb	c
	ret

;----------------------------------------------

intel_do_in:
	mov	r0, #<EP0CS
1$:	movx	a, @r0
	jb	ACC.2, 1$			; make sure INBSY is cleared
loop_in:
	mov	a, INTEL_LEFT_L
	clr	c
	subb	a, #64				; EP0-maxpacket
	mov	INTEL_LEFT_L, a
	mov	a, INTEL_LEFT_H
	subb	a, #0
	mov	INTEL_LEFT_H, a			; update remaining byte counter
	mov	a, #64				; EP0-maxpacket
	jnc	1$				; had >= 1 full packet left
	add	a, INTEL_LEFT_L			; restore partial final packet length
1$:	mov	r3, a				; save len
	jz	3$				; skip write if len==0
	mov	r2, a
	mov	r1, #<IN0BUF
2$:	movx	a, @dptr
	movx	@r1, a
	inc	r1
	djnz	r2, 2$				; copy data to ep
3$:	mov	r1, #<IN0BC
	mov	a, r3
	movx	@r1, a				; arm ep to commit data
4$:	movx	a, @r0
	jb	ACC.2, 4$			; wait until INBSY is cleared
	cjne	r3, #64, 5$			; partial packet indicates we're done
	mov	a, INTEL_LEFT_L
	orl	a, INTEL_LEFT_H
	jnz	loop_in				; loop until nothing is left
	; data was exact multiple of EP0-maxpacket
; nope, device shouldn't append zerolen packets on ctrl-in (except if wLength==0 above)
;	clr	a
;	movx	@r1, a				; append zerolen packet

5$:	clr	c
	ret

;----------------------------------------------

intel_ctrl_in::
	lcall	intel_setup_ctrl
	jc	1$
	lcall	intel_do_in
1$:	ret

intel_ctrl_out::
	lcall	intel_setup_ctrl
	jc	1$
	lcall	intel_do_out
1$:	ret

;*************************************************************
