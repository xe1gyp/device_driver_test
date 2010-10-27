#!/bin/sh
################################################################################
# usbDevice_SelectiveSuspend.sh
# Author  : vamsi
# Date    : october 22th, 2010
# Description: USB Device Gadget Selective Suspend Resume
################################################################################

# Identify USB Device Gadget Node on host
# In loop does selective suspend and resume


for (( i=2; i<=10; i++ ))
do
      USB_DEVICE_NODE=/sys/bus/usb/devices/usb1/1-$i
      SUCCESS=`ssh ${HOSTIPADDR} cat $USB_DEVICE_NODE/product | grep "Gadget" `
      if [ "$SUCCESS" != ""  ]; then
               j=1
               while [ $j -lt 5 ]; do
               ssh ${HOSTIPADDR} sudo chmod 777 $USB_DEVICE_NODE/power/level
               ssh ${HOSTIPADDR} "echo suspend > $USB_DEVICE_NODE/power/level"
               dmesg | grep "suspend"
               dmesg -c
               ssh ${HOSTIPADDR} "echo on > $USB_DEVICE_NODE/power/level"
               dmesg | grep "RESUME"
               dmesg -c
               j=`expr $j + 1`
               done
               break;
      fi
done
