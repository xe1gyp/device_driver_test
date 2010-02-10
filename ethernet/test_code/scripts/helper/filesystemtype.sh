#!/bin/sh

if [ -f /usr/lib/opkg/alternatives/dmesg ]; then
        echo "Working in Poky Filesystem"
        TEMP_ETHERNET_KERNEL_MESSAGES="/usr/lib/opkg/alternatives/dmesg"
elif [ -f /var/log/dmesg ]; then
        echo "Working Busybox Filesystem"
        TEMP_ETHERNET_KERNEL_MESSAGES="/var/log/dmesg"
fi

cat $TEMP_ETHERNET_KERNEL_MESSAGES > $ETHERNET_KERNEL_MESSAGES

# End of file