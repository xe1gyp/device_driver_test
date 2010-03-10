#!/bin/sh

MOUNT_POINT=$1

if [ "$MOUNT_POINT" != "" ]; then
	#waiting for sync
	sleep 3
	mount | grep $MOUNT_POINT && umount $MOUNT_POINT
	#waiting for sync
	sleep 3
fi
