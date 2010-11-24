#!/bin/sh

DISPLAY=$1
VIDEO_PIPELINE=$2
RESULT=0

#Usage: ./overlay_switch.sh <DisplayDevice> <VideoPipeline>

echo "0" > /sys/devices/platform/omapdss/display$DISPLAY/enabled
sleep 2
echo "1" > /sys/devices/platform/omapdss/display$DISPLAY/enabled
sleep 1
if [ "$DISPLAY" = "0" ];then
	echo "0" > /sys/devices/platform/omapdss/overlay$VIDEO_PIPELINE/enabled
	echo "lcd" > /sys/devices/platform/omapdss/overlay$VIDEO_PIPELINE/manager
	echo "1" > /sys/devices/platform/omapdss/overlay$VIDEO_PIPELINE/enabled

elif [ "$DISPLAY" = "1" ];then
        echo "0" > /sys/devices/platform/omapdss/overlay$VIDEO_PIPELINE/enabled
        echo "2lcd" > /sys/devices/platform/omapdss/overlay$VIDEO_PIPELINE/manager
        echo "1" > /sys/devices/platform/omapdss/overlay$VIDEO_PIPELINE/enabled

elif [ "$DISPLAY" = "2" ];then
	echo "0" > /sys/devices/platform/omapdss/overlay$VIDEO_PIPELINE/enabled
	echo "tv" > /sys/devices/platform/omapdss/overlay$VIDEO_PIPELINE/manager
	echo "1" > /sys/devices/platform/omapdss/overlay$VIDEO_PIPELINE/enabled

fi
sleep 1

exit $RESULT
