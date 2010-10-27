#!/bin/sh
################################################################################
# g_file_storage_enum_stress.sh
# Author  : vamsi
# Date    : october 22th, 2010
# Description: Script for gadget file storage enumeration stress test
################################################################################

# Install module Gadget file storage.
# check for host side mount point
# remove the module from kernel in loop

i=1
while [ $i -lt 50 ]; do
ssh ${HOSTIPADDR} lsusb > $USBDEVICE_LSUSB_BEFORE
modprobe g_file_storage file=/dev/mmcblk0 stall=n removable=y
sleep 6
ssh ${HOSTIPADDR} lsusb > $USBDEVICE_LSUSB_AFTER
ssh ${HOSTIPADDR} mkdir -p /tmp/media
grep 0525 $USBDEVICE_LSUSB_AFTER | grep a4a5
rmmod g_file_storage
i=`expr $i + 1`
done
