#!/bin/bash

if [ $# -lt 2 ]; then
	echo "Invalid arguments" 
else
	echo "The mount point=$1  File size=$2 MB"
	let t=$2+64
	mkdir -p  /testsuites/usb_host/scripts/misc/$3
	mkdir -p  /testsuites/usb_host/scripts/misc/$4
	mount $1 /testsuites/usb_host/scripts/misc/$3
	mount -t tmpfs tmpfs /testsuites/usb_host/scripts/misc/$4 -o size=$t"M"
	dd if=/dev/zero of=/testsuites/usb_host/scripts/misc/$4/temp.txt bs=1M count=$2
	echo "Write time of $2 MB"
	time  cp /testsuites/usb_host/scripts/misc/$4/temp.txt /testsuites/usb_host/scripts/misc/$3/.; time sync
	echo "$4"
	ls /testsuites/usb_host/scripts/misc/$4
	echo "$3"
	ls /testsuites/usb_host/scripts/misc/$3
	rm /testsuites/usb_host/scripts/misc/$4/temp.txt
	echo "unmounting $1 ..."	
	umount $1
	echo "mounting $1 ..."
	mount $1 /testsuites/usb_host/scripts/misc/$3
	echo "Read time of $2 MB"
	time cp /testsuites/usb_host/scripts/misc/$3/temp.txt /testsuites/usb_host/scripts/misc/$4/1.txt; time sync 
	rm -Rf /testsuites/usb_host/scripts/misc/$4/*
	rm -Rf /testsuites/usb_host/scripts/misc/$3/*
	umount /testsuites/usb_host/scripts/misc/$4
	umount $1
fi

