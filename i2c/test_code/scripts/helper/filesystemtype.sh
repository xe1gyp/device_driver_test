#!/bin/sh

if [ -f /usr/lib/opkg/alternatives/dmesg ]; then
        echo "Working in Poky Filesystem"
        export FSNAME='/usr/lib/opkg/alternatives/dmesg'
        /usr/lib/opkg/alternatives/dmesg > $TMPBASE/result.tmp

elif [ -f /var/log/dmesg ]; then
        echo "Working Busybox Filesystem"
        export FSNAME='cat /var/log/dmesg'
        cat /var/log/dmesg > $TMPBASE/result.tmp
	echo $FSNAME
fi
