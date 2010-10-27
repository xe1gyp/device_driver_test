#!/bin/sh

LOCAL_DRIVER=$1

ms_perf.sh "/dev/sda1" "150" "file_strg1" "ramdrv1" &
bgpid1=$!
ms_perf.sh "/dev/sdb1" "150" "file_strg2" "ramdrv2" &
bgpid2=$!
ms_perf.sh "/dev/sdc1" "150" "file_strg3" "ramdrv3" &
bgpid3=$!
testusb_test.sh &
bgpid4=$!
wait $bgpid1 $bgpid2 $bgpid3 $bgpid4
rmdir /testsuites/usb_host/scripts/misc/file_strg1
rmdir /testsuites/usb_host/scripts/misc/ramdrv1
rmdir /testsuites/usb_host/scripts/misc/file_strg2
rmdir /testsuites/usb_host/scripts/misc/ramdrv2
rmdir /testsuites/usb_host/scripts/misc/file_strg3
rmdir /testsuites/usb_host/scripts/misc/ramdrv3








	
