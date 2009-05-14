#!/bin/sh

if [ " -e /usr/lib/opkg/alternatives/dmesg " ]
then
	echo "Working in Poky Filesystem"
	export FSNAME='/usr/lib/opkg/alternatives/dmesg'
	/usr/lib/opkg/alternatives/dmesg > $TESTSCRIPT/result.tmp
	#echo $FSNAME
else
	echo "Working in Busybox Filesystem"
	export FSNAME='cat /var/log/dmesg'
	cat $FSNAME > $TESTSCRIPT/result.tmp
	#echo $FSNAME
fi
