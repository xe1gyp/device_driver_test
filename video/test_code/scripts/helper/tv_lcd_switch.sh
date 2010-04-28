#!/bin/sh
DEV=$1
PIPELINE=$2
IP_SZ=$3
OP_SZ=$4
POS=$5

if [ -z $PIPELINE ]; then
	echo "Error pipeline not found"
	echo "Usage ./tv_lcd_changer.sh <vid>"
	exit 1
fi

OVERLAY=`echo /sys/devices/platform/omapdss/overlay$PIPELINE/enabled`
FB_OVERLAY=`echo /sys/class/graphics/fb$PIPELINE/overlays`
MANAGER=`echo /sys/devices/platform/omapdss/overlay$PIPELINE/manager`
POSITION=`echo /sys/devices/platform/omapdss/overlay$PIPELINE/position`
INPUT_SIZE=`echo /sys/devices/platform/omapdss/overlay$PIPELINE/input_size`
OUTPUT_SIZE=`echo /sys/devices/platform/omapdss/overlay$PIPELINE/output_size`
DISP_DEVICE=`echo /sys/devices/platform/omapdss/display$DEV/enabled`
DISP_STATUS=`cat /sys/devices/platform/omapdss/display$DEV/enabled`

echo 0 > $OVERLAY

if [ $PIPELINE -eq 0 ] ; then
echo 0 > $FB_OVERLAY
echo "$POS" > $POSITION
echo "$IP_SZ" > $INPUT_SIZE
echo "$OP_SZ" > $OUTPUT_SIZE
fi

if [ $DEV -eq 1 ] ; then
echo "tv" > $MANAGER
elif [ $DEV -eq 0 ] ; then
echo "lcd" > $MANAGER
fi

#Required for Over0 to be enabled and is not required for Over1 & Over2
if [ $PIPELINE -eq 0 ] ; then
echo 1 > $OVERLAY
fi

if [ $DISP_STATUS -eq 0 ] ; then
echo 1 > $DISP_DEVICE
fi
