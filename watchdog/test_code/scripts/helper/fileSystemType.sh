#!/bin/sh

if [ -f /usr/lib/opkg/alternatives/dmesg ]; then
	echo "Working in a Poky Filesystem"
	cat /usr/lib/opkg/alternatives/dmesg > $DMESG_FILE

elif [ -f /var/log/dmesg ]; then
	echo "Working in a Minimal Busybox Filesystem"
	cat /var/log/dmesg > $DMESG_FILE
fi

# End of file

