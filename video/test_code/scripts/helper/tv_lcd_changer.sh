#!/bin/sh
PIPELINE=$1

if [ -z $PIPELINE ]; then
	echo "Error pipeline not found"
	echo "Usage ./tv_lcd_changer.sh <vid>"
	exit 1
fi

DISPOSITIVE=`echo /sys/devices/platform/omapdss/overlay$PIPELINE/manager`
OVERLAY=`echo /sys/devices/platform/omapdss/overlay$PIPELINE/enabled`

while true
do
	echo 0 > $OVERLAY
	echo tv > $DISPOSITIVE
	echo 1 > $OVERLAY
	sleep 5
	echo 0 > $OVERLAY
	echo lcd > $DISPOSITIVE
	echo 1 > $OVERLAY
	sleep 3
done
