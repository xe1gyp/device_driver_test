#!/bin/sh

# =============================================================================
# Variables
# =============================================================================

LOCAL_COMMAND=$1
LOCAL_DRIVER=$2
LOCAL_MOUNTPOINT=$3

# =============================================================================
# Functions
# =============================================================================


# =============================================================================
# Main
# =============================================================================

if [ "$LOCAL_COMMAND" = "create" ]; then

	$USBHOST_DIR_HELPER/handlerUsbHostSetup.sh "create" $LOCAL_DRIVER 

	test -d $LOCAL_MOUNTPOINT || mkdir $LOCAL_MOUNTPOINT
	sleep 10
	ls /dev/sd*
	echo "Command: sync && mount ${USBHOST_DEVFS_PARTITION} ${LOCAL_MOUNTPOINT} && sync"
	sleep 5
	mount ${USBHOST_DEVFS_PARTITION} $LOCAL_MOUNTPOINT && sync
	
elif [ "$LOCAL_COMMAND" = "remove" ]; then
	umount $LOCAL_MOUNTPOINT
	rm -rf $LOCAL_MOUNTPOINT
	$USBHOST_DIR_HELPER/handlerUsbHostSetup.sh "remove" $LOCAL_DRIVER 
	
	
fi

	
