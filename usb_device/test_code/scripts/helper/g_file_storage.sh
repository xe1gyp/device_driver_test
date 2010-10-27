#!/bin/sh
################################################################################
# g_file_storage.sh
# Author  : vamsi
# Date    : october 22th, 2010
# Description: Script for gadget file storage data transfer
################################################################################

# Install module Gadget file storage.
# check for host side mount point
# Mount to drive on hostpc to temp location
# Data transfer from OMAP to hostPC
# unmount remove the module from kernel
# Install and remount the file stoarge
# Data transfer from HOSTPC to Device side.
# Compare both transfers.
# Unmount and remove the device.
# For stress test do it in a loop

i=1
while [ $i -lt $2 ]; do
ssh ${HOSTIPADDR} lsusb > $USBDEVICE_LSUSB_BEFORE
ssh ${HOSTIPADDR} ls /dev/sd* > $USBDEVICE_LSSD_BEFORE
modprobe g_file_storage file=/dev/mmcblk0 stall=n removable=y
ssh ${HOSTIPADDR} lsusb > $USBDEVICE_LSUSB_AFTER
echo "Enumeration done"
grep 0525 $USBDEVICE_LSUSB_AFTER

# Mount the device, create a big file, copy it.
echo -n "Generating 150MB file..."
ssh ${HOSTIPADDR} work/usbtest/genfile.sh $1
echo "Done"
sleep 6
ssh ${HOSTIPADDR} ls /dev/sd* > $USBDEVICE_LSSD_AFTER
HOST_MOUNT_ID=`diff -n $USBDEVICE_LSSD_BEFORE $USBDEVICE_LSSD_AFTER | grep 1`
echo $HOST_MOUNT_ID
ssh ${HOSTIPADDR} mkdir -p /tmp/media
echo -n "Mounting medium..."
ssh ${HOSTIPADDR} sudo mount $HOST_MOUNT_ID /tmp/media
echo "Done"
echo -n "Copying files..."
ssh ${HOSTIPADDR} sudo time cp /tmp/$1.txt /tmp/media/
echo "Done"
echo -n "Unmounting medium..."
ssh ${HOSTIPADDR} sudo umount /tmp/media
echo "Done"
echo -n "Disconnect medium..."
rmmod g_file_storage
echo "Done"
#echo "Done copying files. Unmounting medium"
echo -n "Reconnect medium..."
modprobe g_file_storage file=/dev/mmcblk0 stall=n removable=y
echo "Done"
sleep 6
echo -n "Mounting medium..."
ssh ${HOSTIPADDR} sudo mount $HOST_MOUNT_ID /tmp/media
echo "Done"
echo -n "Copying files back..."
ssh ${HOSTIPADDR} time cp /tmp/media/$1.txt /tmp/$1-2.txt
echo "Done"
echo -n "Unmounting medium..."
ssh ${HOSTIPADDR} sudo umount /tmp/media
echo "Done"
echo "Verifying md5sums..."
ssh ${HOSTIPADDR} md5sum /tmp/test*.txt
echo -n "Cleaning up..."
ssh ${HOSTIPADDR} rm /tmp/test*.txt

# Then unmount, rmmod, insmod, copy back
# Then compare md5sums
#
rmmod g_file_storage
echo "Done"
i=`expr $i + 1`
done
