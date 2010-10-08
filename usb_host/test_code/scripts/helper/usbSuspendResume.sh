#!/bin/sh
p= ls /sys/bus/usb/drivers/usb-storage/ | grep :
echo p > /sys/bus/usb/drivers/usb-storage/unbind 
q= ls /sys/bus/usb/devices/ | grep usb
s= ls /sys/bus/usb/devices/$q/ | grep -
echo "P = $p"
echo "Q = $q"
echo "S = $s"
echo auto > /sys/bus/usb/devices/"$q"/"$s"/power/control
dmesg | grep usb auto-suspend 
echo on > /sys/bus/usb/devices/"$q"/"$s"/power/control
dmesg | grep finish resume
echo  $p > /sys/bus/usb/drivers/usb-storage/bind


	
