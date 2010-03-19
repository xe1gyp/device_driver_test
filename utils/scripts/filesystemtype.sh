#!/bin/sh

if [ -f /var/log/dmesg ]; then
        alias dmesg='cat /var/log/dmesg'
fi

dmesg > /tmp/result.tmp
