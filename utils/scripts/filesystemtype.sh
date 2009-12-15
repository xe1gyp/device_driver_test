#!/bin/sh

if [ -f /usr/lib/opkg/alternatives/dmesg ]; then
        alias dmesg='/usr/lib/opkg/alternatives/dmesg'

elif [ -f /var/log/dmesg ]; then
        alias dmesg='cat /var/log/dmesg'
fi

dmesg > $TMPBASE/result.tmp
