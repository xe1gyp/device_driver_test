#!/bin/sh
################################################################################
# g_zero_enum_stress.sh
# Author  : vamsi
# Date    : october 22th, 2010
# Description: enumerating g_zero stress
################################################################################

# Install module Gadget file Zero.
# check for host side mount point
# Mount to drive on hostpc to temp location

i=1
while [ $i -lt 5 ]; do
ssh ${HOSTIPADDR} lsusb > $USBDEVICE_LSUSB_BEFORE
modprobe g_zero
ssh ${HOSTIPADDR} lsusb > $USBDEVICE_LSUSB_AFTER
grep 0525 $USBDEVICE_LSUSB_AFTER | grep a4a0
rmmod g_zero
i=`expr $i + 1`
done
