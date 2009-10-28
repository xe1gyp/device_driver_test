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

.area	USB_DESCR

USB_VERSION		= USB_BCD_VER_1_1

VENDOR_ID		= 0xfff0		; as used by usb-skeleton
PRODUCT_ID		= 0xfff0
DEVICE_BCD_ID		= 0x0001

DEFAULT_MAXLEN		= 0x40
SOURCE_MAXLEN		= 0x40
SINK_MAXLEN		= 0x40

NUM_CONFIG		= 0x01
CONFIG_VALUE		= 0x01
NUM_INTERFACE		= 0x01
INTERFACE_ID		= 0x00
NUM_ALTSETTING		= 0x02
ALTSETTING_DEFAULT	= 0x00
ALTSETTING_ID		= 0x01			; our altsetting

ISO_IN_EP		= 0x88
ISO_OUT_EP		= 0x08
ISO_MAXLEN		= 512
ISO_EP_ATTRIB		= USB_ENDPOINT_ISO	; iso, data-ep, no synch.
ISO_INTERVAL		= 1			; 2^(interval-1) ms

NUM_ENDPOINTS		= 4

; cannot use .dw here: the i8051 is big-endian, believe it or not!

descr_device::
	.db	USB_DT_DEVICE_LEN	; bLength
	.db	USB_DT_DEVICE		; bDescriptorType
	.db	<USB_VERSION		; bcdUSB(L)
	.db	>USB_VERSION		; bcdUSB(H)
	.db	USB_CLASS_VENDOR_SPEC	; bDeviceClass
	.db	0x00			; bDeviceSubClass
	.db	USB_PROTO_VENDOR_SPEC	; bDeviceProtocoll
	.db	DEFAULT_MAXLEN		; bMaxPacketSize
	.db	<VENDOR_ID		; idVendor(L)
	.db	>VENDOR_ID		; idVendor(H)
	.db	<PRODUCT_ID		; idProduct(L)
	.db	>PRODUCT_ID		; idProduct(H)
	.db	<DEVICE_BCD_ID		; bcdDevice(L)
	.db	>DEVICE_BCD_ID		; bcdDevice(H)
	.db	0x01			; iManufacturer
	.db	0x02			; iProduct
	.db	0x03			; iSerialNumber
	.db	NUM_CONFIG		; bNumConfigurations

descr_config::
	.db	USB_DT_CONFIG_LEN	; bLength
	.db	USB_DT_CONFIG		; bDescrType
	.db	<descr_config_len	; wTotalLength(L)
	.db	>descr_config_len	; wTotalLength(H)
	.db	NUM_INTERFACE		; bNumInterfaces
	.db	CONFIG_VALUE		; bConfigurationValue
	.db	0x00			; iConfiguration
	.db	USB_ATTRIB_REM_WAKEUP	; bmAttributes
	.db	USB_MAXPOWER_100	; MaxPower

descr_interface:
descr_altset0:
	.db	USB_DT_INTERFACE_LEN	; bLength
	.db	USB_DT_INTERFACE	; bDescType
	.db	INTERFACE_ID		; bInterfaceNumber
	.db	ALTSETTING_DEFAULT	; bAlternateSetting
	.db	0			; bNumEndpoints
	.db	USB_CLASS_VENDOR_SPEC	; bInterfaceClass
	.db	0x00			; bInterfaceSubClass
	.db	USB_PROTO_NONE		; bInterfaceProtocol
	.db	0x00			; iInterface

descr_altset1:
	.db	USB_DT_INTERFACE_LEN	; bLength
	.db	USB_DT_INTERFACE	; bDescType
	.db	INTERFACE_ID		; bInterfaceNumber
	.db	ALTSETTING_ID		; bAlternateSetting
	.db	NUM_ENDPOINTS		; bNumEndpoints
	.db	USB_CLASS_VENDOR_SPEC	; bInterfaceClass
	.db	0x01			; bInterfaceSubClass
	.db	USB_PROTO_NONE		; bInterfaceProtocol
	.db	0x00			; iInterface

descr_ep_source:
	.db	USB_DT_ENDPOINT_LEN	; bLength
	.db	USB_DT_ENDPOINT		; bDescriptorType
	.db	SOURCE_EP		; bEndpointAddress
	.db	USB_ENDPOINT_BULK	; bmAttributes (BULK)
	.db	<SOURCE_MAXLEN		; wMaxPacketSize(L)
	.db	>SOURCE_MAXLEN		; wMaxPacketSize(H)
	.db	0x00			; bInterval

descr_ep_sink:
	.db	USB_DT_ENDPOINT_LEN	; bLength
	.db	USB_DT_ENDPOINT		; bDescriptorType
	.db	SINK_EP			; bEndpointAddress
	.db	USB_ENDPOINT_BULK	; bmAttributes (BULK)
	.db	<SINK_MAXLEN		; wMaxPacketSize(L)
	.db	>SINK_MAXLEN		; wMaxPacketSize(H)
	.db	0x00			; bInterval

descr_ep_isoin:
	.db	USB_DT_ENDPOINT_LEN	; bLength
	.db	USB_DT_ENDPOINT		; bDescriptorType
	.db	ISO_IN_EP		; bEndpointAddress
	.db	ISO_EP_ATTRIB		; bmAttributes (BULK)
	.db	<ISO_MAXLEN		; wMaxPacketSize(L)
	.db	>ISO_MAXLEN		; wMaxPacketSize(H)
	.db	ISO_INTERVAL		; bInterval

descr_ep_isoout:
	.db	USB_DT_ENDPOINT_LEN	; bLength
	.db	USB_DT_ENDPOINT		; bDescriptorType
	.db	ISO_OUT_EP		; bEndpointAddress
	.db	ISO_EP_ATTRIB		; bmAttributes (BULK)
	.db	<ISO_MAXLEN		; wMaxPacketSize(L)
	.db	>ISO_MAXLEN		; wMaxPacketSize(H)
	.db	ISO_INTERVAL		; bInterval

descr_config_len = (. - descr_config)


descr_string0::
	.db	(descr_string0_len)	; bLength
	.db	USB_DT_STRING		; bDescriptorType (string)
	.db	0, 0			; some language code
descr_string0_len = (. - descr_string0)

descr_string1::
	.db	(descr_string1_len)	; bLength
	.db	USB_DT_STRING		; bDescriptorType (string)
	.db	'M, 0, 'a, 0, 'r, 0, 't, 0, 'i, 0, 'n, 0, ' , 0
	.db	'D, 0, 'i, 0, 'e, 0, 'h, 0, 'l, 0
descr_string1_len = (. - descr_string1)

descr_string2::
	.db	(descr_string2_len)	; bLength
	.db	USB_DT_STRING		; bDescriptorType (string)
	.db	'U, 0, 'S, 0, 'B, 0, '-, 0, 'T, 0, 'e, 0, 's, 0, 't, 0
descr_string2_len = (. - descr_string2)

descr_string3::
	.db	(descr_string3_len)	; bLength
	.db	USB_DT_STRING		; bDescriptorType (string)
	.db	'2, 0, '0, 0, '0, 0, '4, 0, '-, 0, '0, 0, '3, 0, '-, 0, '0, 0, '5, 0
descr_string3_len = (. - descr_string3)

;	.ascii	"Martin Diehl Test"

;---------------------------------------------------------
