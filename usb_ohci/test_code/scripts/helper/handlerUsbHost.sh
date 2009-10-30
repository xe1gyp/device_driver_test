#!/bin/sh
set -x
# =============================================================================
# Variables
# =============================================================================

LOCAL_COMMAND=$1
LOCAL_DRIVER=$2


# =============================================================================
# Functions
# =============================================================================


# =============================================================================
# Main
# =============================================================================

if [ "$LOCAL_COMMAND" = "add" ]; then

		if [ "$LOCAL_DRIVER" = "mass_storage" ]; then
	
		LOCAL_NODE=$3
		LOCAL_MOUNTPOINT=$4
		
		sleep 10
		mount | grep /dev/$LOCAL_NODE && umount /dev/$LOCAL_NODE
		test -d $LOCAL_MOUNTPOINT/$LOCAL_NODE || mkdir -p $LOCAL_MOUNTPOINT/$LOCAL_NODE
		ls /dev/sd*
		mount /dev/${LOCAL_NODE}  $LOCAL_MOUNTPOINT/$LOCAL_NODE && sync
					
	elif [ "$LOCAL_DRIVER" = "mouse" ]; then
		
		cat /proc/bus/usb/devices | grep 'HID'
		mknod ${USBHOST_HID_NODE} c 13 64
		ls 
	fi
	
elif [ "$LOCAL_COMMAND" = "remove" ]; then
	
	if [ "$LOCAL_DRIVER" = "mass_storage" ]; then
		
		LOCAL_NODE=$3
		LOCAL_MOUNTPOINT=$4
	
		umount $LOCAL_MOUNTPOINT/$LOCAL_NODE
		test -d $LOCAL_MOUNTPOINT/$LOCAL_NODE && rm -r $LOCAL_MOUNTPOINT/$LOCAL_NODE
		
	elif [ "$LOCAL_DRIVER" = "mouse" ]; then
	
		test -e $USBHOST_HID_NODE && rm $USBHOST_HID_NODE
		
		ls $USBHOST_HID_NODE
		mknod ${USBHOST_HID_NODE} c 13 64 
		
	fi
	
fi

	
