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

.module device

.sbttl general device handling

.include "firmware.inc"

;**************************************************+

.area	CODE	(REL,CON)

;--------------------------------------------

usb_disconnect::
	clr	a			; disable all USB interrupts
	mov	dptr, #USBIEN
	movx	@dptr, a
.if (TARGET_MCU & EZUSB_TYPE_FX2)	; it's a FX2
	mov	dptr, #EPIE
	movx	@dptr, a
.else					; it's an AN21XX or FX
	mov	dptr, #IN07IEN
	movx	@dptr, a
	mov	dptr, #OUT07IEN
	movx	@dptr, a
.endif

	mov	dptr, #USBCS
	movx	a, @dptr
	anl	a, #0b11110101		; mask DISCON/RENUM
	orl	a, #0b00001000		; set DISCON=1, RENUM=0

.if (TARGET_MCU ^ EZUSB_TYPE_FX2)	; it's an AN21XX or FX

; FX Rev A errata: we need to drive the DISCON# pin low to overrule
; the soft-pullup. It's not 100% correct wrt. what our controlling
; hub expect's to see but it should work. With the usual transistor
; between the DISCON# pin and D+ it doesn't matter at all
; Note we do it for the AN21xx as well, because it needs to modify
; the DISCOE-bit too - whereas the FX2 doesn't have this bit.

	orl	a, #0b00000100		; DISCOE=1 - drive DISCON# (L in this case)
	movx	@dptr, a

; Drive DISCON# pin L for about 5 usec. Should be sufficient because
; the controlling hub's downstream port is expected to detect the
; disconnect within T_DDIS = 2.5 usec at max.

	mov	r2, #18
22$:	djnz	r2, 22$			; 3x18+2 cycles @ 48/4MHz = 4.7 usec

	anl	a, #~0b00000100		; clear DISCOE so DISCON# pin floats
.endif

	movx	@dptr, a

	; 10ms delay should make sure the parent hub port has
	; detected the disconnect and doesn't race with us

	mov	r3, #157
3$:	mov	r2, #0
4$:	djnz	r2, 4$			; 157x256x3 cycles = 10ms (48MHz)
	djnz	r3, 3$

	mov	DEVICE_STATE, #USB_STATE_DETACHED

	mov	a, #0xff		; w/c all pending USB interrupts
	mov	dptr, #USBIRQ
	movx	@dptr, a
.if (TARGET_MCU & EZUSB_TYPE_FX2)	; it's a FX2
	mov	dptr, #EPIRQ
	movx	@dptr, a
.else					; it's an AN21XX or FX
	mov	dptr, #IN07IRQ
	movx	@dptr, a
	mov	dptr, #OUT07IRQ
	movx	@dptr, a
.endif
	anl	EXIF, #EXIF_INT2_MASK

	lcall	usb_disable_endpoints	; invalidate all EP's
	mov	ALTSETTING, #0

	ret

;--------------------------------------------

usb_connect::
	mov	dptr, #USBCS
	movx	a, @dptr
	anl	a, #0b11110111		; clear DISCON
.if (TARGET_FLAGS & FEAT_RENUM)
	orl	a, #0b00000010		; set RENUM
.endif
.if (TARGET_MCU ^ EZUSB_TYPE_FX2)	; it's an AN21XX or FX
	orl	a, #0b00000100		; set DISCOE to drive DISCON# pin
.endif
	movx	@dptr, a
	
	mov	DEVICE_STATE, #USB_STATE_ATTACHED

	; The new connection is now signaled to the parent hub port.
	; We don't probe here (how?) for stable power at our upstream port.
	; Instead we do some wait for debounce and power stabilization
	; (100ms are granted, but we take only 43ms) and then we look for the
	; onset of the RESET condition, which implies we are in the POWERED state.

	mov	r2, #0			; wait for debounce, power settling
1$:	mov	r3, #0
2$:	mul	ab
	djnz	r3, 2$			; 256x256x8 cycles = 43 msec (48MHz)
	djnz	r2, 1$

	mov	dptr, #USBIRQ		; clear ancient usb interrupt flags
	mov	a, #0xff
	movx	@dptr, a
	anl	EXIF, #EXIF_INT2_MASK

3$:	movx	a, @dptr
	jnb	ACC.4, 3$		; spin until we see the RESET
	movx	@dptr, a		; acknowledge pending events

	mov	DEVICE_STATE, #USB_STATE_POWERED

	; Now we have RESET on the bus for some time, probably followed by the
	; FS/HS negotiation protocol. When we see the first SOF token, we can
	; safely assume this has finished and we are in the default state.

4$:	movx	a, @dptr
	jnb	ACC.1, 4$		; spin until we see a SOF
	movx	@dptr, a		; acknowledge pending events

	mov	DEVICE_STATE, #USB_STATE_DEFAULT

	clr	HS_NOT_FS		; assume we are runnung at fullspeed
.if (TARGET_MCU & EZUSB_TYPE_FX2)	; it's a FX2
	mov	dptr, #USBCS
	movx	a, @dptr
	jnb	ACC.7, 5$		; check if highspeed already granted
	setb	HS_NOT_FS

5$:	mov	a, #0b00100001		; highspeed ints: HSGRANT/SUDAV
.else
	mov	a, #0b00000001		; fullspeed ints: SUDAV
.endif
	mov	dptr, #USBIEN		; the common USBIRQ (INT2) is still disabled
	movx	@dptr, a		; but enable the detailed USB interrupts now

	mov	dptr, #FNADDR		; SetAddress handled by usbcore - wait until we got one
6$:	movx	a, @dptr
	jz	6$			; spin until device address != 0

	mov	DEVICE_STATE, #USB_STATE_ADDRESS

	; When we enter the address state the specs grant us up to 2 msec
	; until we must be ready to deal with the next host request.
	; We enable the USB interrupts so the SUDAV-isr can handle it.

	setb	EXUSB

	; Before returning we wait until we are configured. Likely we
	; would also want to enable USB SUSPEND interrupts

.if (TARGET_FLAGS & FEAT_RENUM)
	mov	a, #USB_STATE_CONFIGURED
7$:	cjne	a, DEVICE_STATE, 7$
.else
	mov	a, #1
	lcall	usb_apply_configuration
.endif

	mov	dptr, #USBIEN
	movx	a, @dptr
	setb	ACC.1			; enable SOF interrupt
	setb	ACC.4			; enable RESET interrupt
	movx	@dptr, a

	ret

;-------------------------------------------------------

usb_setup_device::

	mov	dptr, #INT_SETUP_REG
	mov	a, #INT_SETUP_VALUE	; use USB INT-autovector
	movx	@dptr, a

.if (TARGET_MCU ^ EZUSB_TYPE_FX2)	; it's an AN21XX or FX
	mov	dptr, #ISOCTL
	clr	a			; make sure ISODISAB is cleared
	movx	@dptr, a

	mov	dptr, #USBPAIR
	clr	a
	setb	ACC.7			; set ISOEND0
	orl	a, #0b00001001		; pair (double buffer) EP2, both IN and OUT
	movx	@dptr, a
.endif

	mov	ALTSETTING, #0

	clr	a
	mov	COUNTER_1, a
	mov	COUNTER_2, a
	mov	COUNTER_3, a
	mov	COUNTER_4, a
	mov	SOURCE_DATA_2, a
	inc	a
	mov	SOURCE_DATA_1, a

	mov	SINK_MODE, #SINK_MODE_ANY
	mov	SOURCE_MODE, #SOURCE_MODE_PRELOAD

	mov	MAXLEN_L, #<64
	mov	MAXLEN_H, #>64
	jnb	HS_NOT_FS, 1$
	mov	MAXLEN_L, #<512
	mov	MAXLEN_H, #>512

1$:	mov	SOURCE_LEN_L, MAXLEN_L
	mov	SOURCE_LEN_H, MAXLEN_H

	ret

;-------------------------------------------------------

