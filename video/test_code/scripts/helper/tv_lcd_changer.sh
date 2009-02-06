#!/bin/sh
PIPELINE=$1

if [ -z $PIPELINE ]; then
	echo "Error pipeline not found"
	echo "Usage ./tv_lcd_changer.sh <vid>"
	exit 1
fi

DISPOSITIVE=`echo /sys/class/display_control/omap_disp_control/video$PIPELINE`

while true
do
	echo tv > $DISPOSITIVE
	sleep 5
	echo lcd > $DISPOSITIVE
	sleep 3
done


