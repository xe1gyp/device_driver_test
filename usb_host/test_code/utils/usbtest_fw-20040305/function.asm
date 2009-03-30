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

.module function

.sbttl implementation of the USB test function

.include "firmware.inc"

;**************************************************+

.area	ISR

;--------------------------------------------

sudav_isr::
	push	PSW
	push	ACC
	push	DPH
	push	DPL

	lcall	usb_ep0_request

	anl	EXIF, #EXIF_INT2_MASK
.if (TARGET_MCU & EZUSB_TYPE_AN21XX)	; it's an AN21XX
	mov	dptr, #USBIRQ		; (3)
	mov	a, #0b00000001		; (2)
	movx	@dptr, a		; (2+)
.else					; it's a FX or FX2
	mov	INT2CLR, a		; (2)
.endif

	pop	DPL
	pop	DPH
	pop	ACC
	pop	PSW
	reti

;--------------------------------------------

sof_isr::
	push	PSW
	push	ACC
	push	DPH
	push	DPL

	anl	EXIF, #EXIF_INT2_MASK
.if (TARGET_MCU & EZUSB_TYPE_AN21XX)	; it's an AN21XX
	mov	dptr, #USBIRQ		; (3)
	mov	a, #0b00000010		; (2)
	movx	@dptr, a		; (2+)
.else					; it's a FX or FX2
	mov	INT2CLR, a		; (2)
.endif

	lcall	iso_handle_sof

	pop	DPL
	pop	DPH
	pop	ACC
	pop	PSW
	reti

;--------------------------------------------

; this is not invoked during the initial reset after connect because
; interrupts are still disabled there. This is exactly what we want in
; order to prevent interfering with highspeed-negotiation during this
; initial reset.

reset_isr::
	push	PSW
	push	ACC
	push	DPL
	push	DPH

	anl	EXIF, #EXIF_INT2_MASK
.if (TARGET_MCU & EZUSB_TYPE_AN21XX)	; it's an AN21XX
	mov	dptr, #USBIRQ		; (3)
	mov	a, #0b00010000		; (2)
	movx	@dptr, a		; (2+)
.else					; it's a FX or FX2
	mov	INT2CLR, a		; (2)
.endif

	mov	MAXLEN_L, #<64
	mov	MAXLEN_H, #>0
	clr	HS_NOT_FS

	clr	a
	lcall	usb_apply_configuration

	mov	DEVICE_STATE, #USB_STATE_DEFAULT

	pop	DPH
	pop	DPL
	pop	ACC
	pop	PSW
	reti

;--------------------------------------------

.if (TARGET_MCU & EZUSB_TYPE_FX2)	; it's a FX2

; not sure if this could happen at all after the speed negotiation
; during initial bus reset - maybe resume after highspeed-suspend?

hispeed_isr::
	push	PSW
	push	ACC
	push	DPL
	push	DPH

	anl	EXIF, #EXIF_INT2_MASK
	mov	INT2CLR, a

	setb	HS_NOT_FS
	mov	SOURCE_LEN_H, #>512
	mov	SOURCE_LEN_L, #<512

	pop	DPH
	pop	DPL
	pop	ACC
	pop	PSW
	reti

.endif

;**************************************************+

source_isr::
	push	PSW
	push	ACC
	push	DPL
	push	DPH

	anl	EXIF, #EXIF_INT2_MASK
.if (TARGET_MCU & EZUSB_TYPE_AN21XX)		; it's an AN21XX
	mov	dptr, #IN07IRQ
	mov	a, #0b00000100
	movx	@dptr, a
.else						; it's a FX or FX2
	mov	INT2CLR, a
.endif

	mov	PSW, #PSW_BANK3

	mov	r2, SOURCE_LEN_H		; default len=maxpacket
	mov	r3, SOURCE_LEN_L
	mov	a, SOURCE_MODE
	mov	dptr, #source_table
	lcall	jump_table_gate

.if (TARGET_MCU & EZUSB_TYPE_FX2)		; it's a FX2
	mov	dptr, #SOURCE_BC2
	mov	a, r2
	movx	@dptr, a
.endif
	mov	dptr, #SOURCE_BC
	mov	a, r3
	movx	@dptr, a

	pop	DPH
	pop	DPL
	pop	ACC
	pop	PSW
	reti

;--------------------------------------------

sink_isr::
	push	PSW
	push	ACC
	push	DPL
	push	DPH

	anl	EXIF, #EXIF_INT2_MASK
.if (TARGET_MCU & EZUSB_TYPE_AN21XX)		; it's an AN21XX
	mov	dptr, #OUT07IRQ
	mov	a, #0b00000100
	movx	@dptr, a
.else						; it's a FX or FX2
	mov	INT2CLR, a
.endif

	mov	PSW, #PSW_BANK3

.if (TARGET_MCU & EZUSB_TYPE_FX2)		; it's a FX2
	mov	dptr, #SINK_BC2
	movx	a, @dptr
	mov	r2, a
.else						; it's an AN21XX or FX
	mov	r2, #0
.endif
	mov	dptr, #SINK_BC
	movx	a, @dptr
	mov	r3, a				; r2:r3 = packetlen

	mov	a, SINK_MODE
	mov	dptr, #sink_table
	lcall	jump_table_gate

	mov	dptr, #SINK_BC
.if (TARGET_MCU & EZUSB_TYPE_FX2)		; it's a FX2
	mov	a, #0b10000000			; set skip bit to give buffer to SIE
.endif
	movx	@dptr, a			; arm EP buffer

	pop	DPH
	pop	DPL
	pop	ACC
	pop	PSW
	reti

;**************************************************

usb_reset_endpoint::				; ACC: ep-addr to reset

; check valid endpoints

	cjne	a, #SOURCE_EP, 1$
	mov	r4, #~SOURCE_INT_MASK
	sjmp	_int_off
1$:	cjne	a, #SINK_EP, 2$
	mov	r4, #~SINK_INT_MASK
	sjmp	_int_off
2$:	clr	ACC.7
	setb	c
	jnz	3$				; no error if EP0
	clr	c
3$:	ret

; prevent EP-specific interrupt requests to propagate downwards the shared INT2

_int_off:
	mov	r5, a
	push	IE
	clr	EA
.if (TARGET_MCU & EZUSB_TYPE_FX2)		; it's a FX2
	mov	dptr, #EPIE
.else
	mov	dptr, #IN07IEN
	jb	ACC.7, 4$
	mov	dptr, #OUT07IEN
4$:
.endif
	movx	a, @dptr
	anl	a, r4
	movx	@dptr, a

_reset_toggle:
	mov	dptr, #TOGCTL
	mov	a, r5
	rlc	a
	rr	a
	mov	ACC.4, c
	anl	a, #0b00010111			; ACC: [0 0 0 dir 0 ep2 ep1 ep0]
	movx	@dptr, a			; select EP
	orl	a, #0b00100000
	movx	@dptr, a			; reset data toggle to DATA0
	jb	ACC.4, _reset_source		; jump if SOURCE EP (dir=IN)
	mov	dptr, #SINK_CS
	clr	a				; clear HALT
	movx	@dptr, a

; sink-ep: unarm (multibuffer)

_unarm_sink:
	mov	dptr, #SINK_CS
	movx	a, @dptr
	anl	a, #SINK_BUSY_MASK
	jnz	_int_on

	mov	dptr, #SINK_BC
.if (TARGET_MCU & EZUSB_TYPE_FX2)		; it's a FX2
	mov	a, #0b10000000			; set skip bit to give buffer to SIE
.endif
	movx	@dptr, a			; arm EP buffer
	sjmp	_unarm_sink

; source-ep: first arm all multibuffers, then unarm

_reset_source:

5$:	mov	dptr, #SOURCE_CS
	movx	a, @dptr
	anl	a, #SOURCE_BUSY_MASK
	jnz	_unarm_source
	mov	dptr, #SOURCE_BC
	clr	a
	movx	@dptr, a			; source dummy len=0 packet
	sjmp	5$

; hm, aren't we potentially racing with the host scheduling further IN?

_unarm_source:
.if (TARGET_MCU & EZUSB_TYPE_FX2)		; it's a FX2
.error 1	; dunno how to unarm EP-IN with FX2
.else
	mov	dptr, #(IN2CS+2)
	movx	a, @dptr
	orl	a, #0b00000010
	movx	@dptr, a
	mov	dptr, #(IN2CS)
	movx	a, @dptr
	orl	a, #0b00000010
	movx	@dptr, a
.endif

; source-ep: zerofill multibuffer, rearm ep

_fill_buffer:
	mov	dptr, #SOURCE_CS
	clr	a
	movx	@dptr, a
6$:	mov	dptr, #SOURCE_CS
	movx	a, @dptr
	anl	a, #SOURCE_BUSY_MASK
	jnz	_int_on

	clr	a
	mov	r6, #0x40
	mov	dptr, #SOURCE_EP_BUF
7$:	movx	@dptr, a
	inc	dptr
	djnz	r6, 7$

	mov	r2, SOURCE_LEN_H		; default len=maxpacket
	mov	r3, SOURCE_LEN_L
	mov	a, SOURCE_MODE
	mov	dptr, #source_table
	lcall	jump_table_gate

.if (TARGET_MCU & EZUSB_TYPE_FX2)		; it's a FX2
	mov	dptr, #SOURCE_BC2
	mov	a, r2
	movx	@dptr, a
.endif

	mov	dptr, #SOURCE_BC
	mov	a, r3
	movx	@dptr, a
	sjmp	6$

; clear pending bogus EP_IRQ due to unarm sequence

_int_on:
	mov	a, r5
	jb	ACC.7, 7$
.if (TARGET_MCU & EZUSB_TYPE_FX2)		; it's a FX2
	mov	dptr, #EPIRQ
.else
	mov	dptr, #OUT07IRQ
.endif
	mov	a, r4
	cpl	a
	movx	@dptr, a			; clear pending SINK EP interrupt
.if (TARGET_MCU & EZUSB_TYPE_FX2)		; it's a FX2
	mov	dptr, #EPIE
.else
	mov	dptr, #OUT07IEN
.endif
	sjmp	_done

7$:
.if (TARGET_MCU & EZUSB_TYPE_FX2)		; it's a FX2
	mov	dptr, #EPIRQ
.else
	mov	dptr, #IN07IRQ
.endif
	mov	a, r4
	cpl	a
	movx	@dptr, a			; clear pending SOURCE interrupt
.if (TARGET_MCU & EZUSB_TYPE_FX2)		; it's a FX2
	mov	dptr, #EPIE
.else
	mov	dptr, #IN07IEN
.endif

; done, reenable interrupts

_done:	movx	a, @dptr
	cpl	a
	anl	a, r4
	cpl	a
	movx	@dptr, a			; enable EP interrupt
	pop	IE
	clr	c
	ret

;--------------------------------------------

usb_disable_endpoints::
	clr	a			; invalidate all EP's
.if (TARGET_MCU & EZUSB_TYPE_FX2)	; it's a FX2
	mov	dptr, #EP1INCFG
	movx	@dptr, a
	mov	dptr, #EP1OUTCFG
	movx	@dptr, a
	mov	dptr, #EP2CFG
	movx	@dptr, a
	mov	dptr, #EP4CFG
	movx	@dptr, a
	mov	dptr, #EP6CFG
	movx	@dptr, a
	mov	dptr, #EP8CFG
	movx	@dptr, a
.else					; it's an AN21XX or FX
	mov	dptr, #INISOVAL
	movx	@dptr, a
	mov	dptr, #OUTISOVAL
	movx	@dptr, a
	inc	a			; don't disable EP0
	mov	dptr, #IN07VAL
	movx	@dptr, a
	mov	dptr, #OUT07VAL
	movx	@dptr, a
.endif
	ret


usb_reset_interface::
	mov	ALTSETTING, a
	push	IE
	clr	EA

	jnz	working_as

	acall	usb_disable_endpoints	; AS=0: disable all EP's
	clr	c
	pop	IE
	ret

working_as:
	lcall	iso_setup		; depending on AS, setup the iso-ep(s)

	; enable bulk EP's and reset them

.if (TARGET_MCU & EZUSB_TYPE_FX2)	; it's a FX2
	mov	dptr, #EP2CFG
	mov	a, #0b10100000		; EP2: OUT, BULK, 512 byte quad-buf
	movx	@dptr, a
	mov	dptr, #EP6CFG
	mov	a, #0b11100000		; EP6: IN, BULK, 512 byte quad-buf
	movx	@dptr, a
	; default setting AUTOIN/AUTOOUT=0 should be correct

.else					; it's an AN21XX or FX
	mov	a, #0b00000101		; select EP0 and EP2
	mov	dptr, #IN07VAL		; enable EP IN
	movx	@dptr, a
	mov	a, #0b00000101		; select EP0 and EP2
	mov	dptr, #OUT07VAL		; enable EP OUT
	movx	@dptr, a
.endif

	mov	a, #SINK_EP
	acall	usb_reset_endpoint
	jc	1$

	mov	a, #SOURCE_EP
	acall	usb_reset_endpoint
	jc	1$

	clr	c
1$:	pop	IE
	ret

;--------------------------------------------

usb_apply_configuration:
	clr	c
	jnz	2$
1$:	mov	DEVICE_STATE, #USB_STATE_ADDRESS
	ret
2$:	acall	usb_reset_interface
	jc	1$
	mov	DEVICE_STATE, #USB_STATE_CONFIGURED
	clr	c
	ret

;**************************************************
