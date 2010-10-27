#!/bin/sh
################################################################################
# g_file_storage_enum.sh
# Author  : vamsi
# Date    : october 22th, 2010
# Description: enumerating and mounting file storage
################################################################################

# Install module Gadget file storage.
# check for host side mount point
# Mount to drive on hostpc to temp location

ssh ${HOSTIPADDR} lsusb > $USBDEVICE_LSUSB_BEFORE
ssh ${HOSTIPADDR} ls /dev/sd* > $USBDEVICE_LSSD_BEFORE
modprobe g_file_storage file=/dev/mmcblk0 stall=n removable=y
ssh ${HOSTIPADDR} lsusb > $USBDEVICE_LSUSB_AFTER
echo "Enumeration done"
grep 0525 $USBDEVICE_LSUSB_AFTER
sleep 6
ssh ${HOSTIPADDR} ls /dev/sd* > $USBDEVICE_LSSD_AFTER
HOST_MOUNT_ID=`diff -n $USBDEVICE_LSSD_BEFORE $USBDEVICE_LSSD_AFTER | grep 1`
echo $HOST_MOUNT_ID
ssh ${HOSTIPADDR} mkdir -p /tmp/media
echo -n "Mounting medium..."
ssh ${HOSTIPADDR} sudo mount $HOST_MOUNT_ID /tmp/media
echo "Done"
