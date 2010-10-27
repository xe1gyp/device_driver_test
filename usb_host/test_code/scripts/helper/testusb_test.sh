#!/bin/sh

LOCAL_DRIVER=$1

$USBHOST_BIN/testusb -A /proc/bus/usb/ -t1 -c1 -s 4096
$USBHOST_BIN/testusb -A /proc/bus/usb/ -t2 -c1 -s 4096
$USBHOST_BIN/testusb -A /proc/bus/usb/ -t1 -c1000 -s 4096
$USBHOST_BIN/testusb -A /proc/bus/usb/ -t2 -c1000 -s 4096
$USBHOST_BIN/testusb -A /proc/bus/usb/ -t1 -c1000 -s 8192
$USBHOST_BIN/testusb -A /proc/bus/usb/ -t2 -c1000 -s 8192
$USBHOST_BIN/testusb -A /proc/bus/usb/ -t1 -c1000 -s 65536
$USBHOST_BIN/testusb -A /proc/bus/usb/ -t2 -c1000 -s 65536
$USBHOST_BIN/testusb -A /proc/bus/usb/ -t1 -c1000 -s 1048576
$USBHOST_BIN/testusb -A /proc/bus/usb/ -t2 -c1000 -s 1048576







	
