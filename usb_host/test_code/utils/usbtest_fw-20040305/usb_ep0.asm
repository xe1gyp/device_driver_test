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

.module usb_ep0

.sbttl implementation of USB standard requests on EP0

.include "firmware.inc"

;****************************************************************

.include "descr.inc"

;****************************************************************

.area	CODE

;*************************************************************

usb_ep0_request::
	push	B
	push	MPAGE
	mov	PSW, #PSW_BANK1

	mov	MPAGE, #>SETUPBUF
	mov	r0, #<SETUPBUF
	movx	a, @r0				; bmRequestType
	mov	r2, a
	anl	a, #USB_REQ_TYPE_MASK
	mov	r3, a
	inc	r0
	movx	a, @r0				; bRequest
	xch	a, r3
	cjne	a, #USB_REQ_TYPE_STD, usb_request_other
	mov	a, #NUM_STANDARD_REQ
	setb	c
	subb	a, r3
	jc	usb_request_other
	mov	a, r3
	rl	a
	add	a, r3
	mov	dptr, #usb_request_jumptable
	jmp	@a+dptr

usb_request_jumptable:
	ljmp	usb_request_GetStatus
	ljmp	usb_request_ClearFeature
	ljmp	usb_request_error		; reserved
	ljmp	usb_request_SetFeature
	ljmp	usb_request_error		; reserved
	ljmp	usb_request_error		; SetAddress (always handled by the usbcore)
	ljmp	usb_request_GetDescriptor
	ljmp	usb_request_SetDescriptor
	ljmp	usb_request_GetConfiguration
	ljmp	usb_request_SetConfiguration
	ljmp	usb_request_GetInterface
	ljmp	usb_request_SetInterface
	ljmp	usb_request_SynchFrame
NUM_STANDARD_REQ = ((. - usb_request_jumptable) / 3)

usb_request_other:
	cjne	r2, #USB_REQ_VENDOR_OUT_DEV, 1$
	cjne	r3, #VENDOR_REQUEST_INTEL_OUT, usb_request_error
	lcall	intel_ctrl_out
	jnc	usb_request_ok
	sjmp	usb_request_error

1$:	cjne	r2, #USB_REQ_VENDOR_IN_DEV, usb_request_error
	cjne	r3, #VENDOR_REQUEST_INTEL_IN, 2$
	lcall	intel_ctrl_in
	jnc	usb_request_ok
	sjmp	usb_request_error

2$:	cjne	r3, #VENDOR_REQUEST_SET_MODE, usb_request_error
	lcall	set_mode_request
	jnc	usb_request_ok

usb_request_error:
	mov	dptr, #EP0CS
	mov	a, #0b00000001			; protocol-STALL
	movx	@dptr, a
	mov	a, #0b00000011			; STALL status stage
	sjmp	request_end

usb_request_ok:
	mov	dptr, #EP0CS
	mov	a, #0b00000010			; acknowledge status stage

request_end:
	movx	@dptr, a
	pop	MPAGE
	pop	B
	ret

;*************************************************************

; [9.4.5]

usb_request_GetStatus:
	mov	r0, #(SETUPBUF+6)
	movx	a, @r0				; wLengthL
	xrl	a, #2				; check requested length == 2
	jnz	usb_request_error
	inc	r0
	movx	a, @r0				; wLengthH
	jnz	usb_request_error

	cjne	r2, #USB_REQ_STD_IN_DEV, 1$
	; GetStatus(device)			; bit0: selfpower / bit1: remote wakeup
	clr	ACC.0				; assume buspowered
	mov	c, WAKEUP_ENABLED
	mov	ACC.1, c
	sjmp	6$

1$:	cjne	r2, #USB_REQ_STD_IN_IF, 2$
	; GetStatus(interface)
	clr	a				; 0
	sjmp	6$

2$:	cjne	r2, #USB_REQ_STD_IN_EP, usb_request_error
	; GetStatus(endpoint)
	mov	r0, #(SETUPBUF+4)
	movx	a, @r0				; wIndexL = endpoint
	cjne	a, #SOURCE_EP, 3$
	mov	dptr, #SOURCE_CS
	sjmp	5$
3$:	cjne	a, #SINK_EP, 4$
	mov	dptr, #SINK_CS
	sjmp	5$
4$:	clr	ACC.7
	jnz	usb_request_error
	sjmp	6$				; EP0 IN/OUT is never halted

5$:	movx	a, @dptr
	anl	a, #0b00000001			; mask HALT bit

6$:
.if (TARGET_MCU & EZUSB_TYPE_FX2)		; it's a FX2
	mov	dptr, #EP0BUF
.else
	mov	dptr, #IN0BUF
.endif
	movx	@dptr, a			; L-byte
	inc	dptr
	clr	a				; H-byte = 0
	movx	@dptr, a
.if (TARGET_MCU & EZUSB_TYPE_FX2)		; it's a FX2
	mov	dptr, #EP0BCH
	movx	@dptr, a
	mov	dptr, #EP0BCL
.else
	mov	dptr, #IN0BC
.endif
	mov	a, #2
	movx	@dptr, a
	sjmp	usb_request_ok

;*************************************************************

; [9.4.1]

usb_request_ClearFeature:
	inc	r0
	movx	a, @r0				; wValueL: feature selector
	cjne	r2, #USB_REQ_STD_OUT_DEV, 1$
	cjne	a, #USB_FEAT_DEVICE_REMOTE_WAKEUP, usb_request_error
	clr	WAKEUP_ENABLED
	sjmp	usb_request_ok

1$:	cjne	r2, #USB_REQ_STD_OUT_EP, usb_request_error
	cjne	a, #USB_FEAT_ENDPOINT_HALT, usb_request_error
	inc	r0
	inc	r0
	movx	a, @r0				; wIndexL: endpoint
	lcall	usb_reset_endpoint
	jc	usb_request_error
	sjmp	usb_request_ok

;*************************************************************

; [9.4.9]

usb_request_SetFeature:
	inc	r0
	movx	a, @r0				; wValueL: feature selector
	cjne	r2, #USB_REQ_STD_OUT_DEV, 2$
.if (TARGET_MCU & EZUSB_TYPE_FX2)		; it's a FX2
	cjne	a, #USB_FEAT_TEST_MODE, 1$
	; ??? - done by usbcore?
	ljmp	usb_request_error
1$:
.endif
	cjne	a, #USB_FEAT_DEVICE_REMOTE_WAKEUP, 5$
	setb	WAKEUP_ENABLED
	ljmp	usb_request_ok

2$:	cjne	r2, #USB_REQ_STD_OUT_EP, 5$
	cjne	a, #USB_FEAT_ENDPOINT_HALT, 5$
	inc	r0
	inc	r0
	movx	a, @r0				; wIndexL: endpoint
	cjne	a, #SOURCE_EP, 3$
	mov	dptr, #SOURCE_CS
	sjmp	4$
3$:	cjne	a, #SINK_EP, 5$
	mov	dptr, #SINK_CS
4$:	mov	a, #0b00000001			; set HALT
	movx	@dptr, a
	ljmp	usb_request_ok
5$:	ljmp	usb_request_error

;*************************************************************

; [9.4.3]

string_table:
	.dw	descr_string0
	.dw	descr_string1
	.dw	descr_string2
	.dw	descr_string3
NUM_STRING_DESCR = ((. - string_table)/2)

usb_request_GetDescriptor:
	cjne	r2, #USB_REQ_STD_IN_DEV, 4$
	inc	r0
	movx	a, @r0				; wValueL: descr index
	mov	r4, a
	inc	r0
	movx	a, @r0				; wValueH: descr type

	cjne	a, #USB_DESCR_TYPE_DEVICE, 1$
	mov	dptr, #descr_device
	sjmp	3$

1$:	cjne	a, #USB_DESCR_TYPE_CONFIGURATION, 2$
	cjne	r4, #0, 4$			; only config-index 0 is valid
	mov	dptr, #descr_config
	sjmp	3$

2$:	cjne	a, #USB_DESCR_TYPE_STRING, 4$
	mov	a, #NUM_STRING_DESCR
	setb	c
	subb	a, r4				; string-id
	jc	4$
	mov	a, r4
	rl	a
	mov	r1, a
	mov	dptr, #string_table
	movc	a, @a+dptr
	xch	a, r1
	inc	a
	movc	a, @a+dptr
	mov	DPL, a
	mov	DPH, r1

3$:	mov	MPAGE, #>SUDPTRH
	mov	r0, #<SUDPTRH
	mov	a, DPH
	movx	@r0, a
	mov	r0, #<SUDPTRL
	mov	a, DPL
	movx	@r0, a				; triggers USB-core support
	ljmp	usb_request_ok
4$:	ljmp	usb_request_error

;*************************************************************

; [9.4.8]

usb_request_SetDescriptor:
	ljmp	usb_request_error		; optional

;*************************************************************

; [9.4.2]

usb_request_GetConfiguration:
	cjne	r2, #USB_REQ_STD_IN_DEV, 2$
	mov	r1, #0				; unconfigured
	mov	a, DEVICE_STATE
	cjne	a, #USB_STATE_CONFIGURED, 1$
	inc	r1				; we have only ConfigValue=1

1$:
.if (TARGET_MCU & EZUSB_TYPE_FX2)		; it's a FX2
	mov	dptr, #EP0BUF
.else
	mov	dptr, #IN0BUF
.endif
	mov	a, r1
	movx	@dptr, a
.if (TARGET_MCU & EZUSB_TYPE_FX2)		; it's a FX2
	mov	dptr, #EP0BCH
	clr	a
	movx	@dptr, a
	mov	dptr, #EP0BCL
.else
	mov	dptr, #IN0BC
.endif
	mov	a, #1
	movx	@dptr, a
	ljmp	usb_request_ok
2$:	ljmp	usb_request_error

;*************************************************************

; [9.4.7]

usb_request_SetConfiguration:
	cjne	r2, #USB_REQ_STD_OUT_DEV, 2$
	inc	r0
	movx	a, @r0				; wValueL: config value
	jz	1$				; unconfigure
	cjne	a, #CONFIG_VALUE, 2$
1$:	lcall	usb_apply_configuration
	jc	2$
	ljmp	usb_request_ok
2$:	ljmp	usb_request_error

;*************************************************************

; [9.4.5]

usb_request_GetInterface:
	cjne	r2, #USB_REQ_STD_IN_IF, 1$
	mov	a, DEVICE_STATE
	cjne	a, #USB_STATE_CONFIGURED, 1$
.if (TARGET_MCU & EZUSB_TYPE_FX2)		; it's a FX2
	mov	dptr, #EP0BUF
.else
	mov	dptr, #IN0BUF
.endif
	mov	a, ALTSETTING
	movx	@dptr, a
.if (TARGET_MCU & EZUSB_TYPE_FX2)		; it's a FX2
	mov	dptr, #EP0BCH
	clr	a
	movx	@dptr, a
	mov	dptr, #EP0BCL
.else
	mov	dptr, #IN0BC
.endif
	mov	a, #1
	movx	@dptr, a
	ljmp	usb_request_ok
1$:	ljmp	usb_request_error


;*************************************************************

; [9.4.10]

usb_request_SetInterface:
	cjne	r2, #USB_REQ_STD_OUT_IF, 1$
	mov	a, DEVICE_STATE
	cjne	a, #USB_STATE_CONFIGURED, 1$
	inc	r0
	movx	a, @r0				; wValueL: altsetting
	mov	r7, a
	clr	c
	subb	a, #NUM_ALTSETTING
	jnc	1$				; invalid altsetting requested
	inc	r0
	inc	r0
	movx	a, @r0				; wIndexL: interface
	xch	a, r7
	cjne	r7, #0, 1$			; we have only interface==0

	lcall	usb_reset_interface
	jc	1$
	ljmp	usb_request_ok
1$:	ljmp	usb_request_error

;*************************************************************

; [9.4.11]

usb_request_SynchFrame:
	ljmp	usb_request_error		; no ISO - always error

;*************************************************************

