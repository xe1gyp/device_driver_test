#!/bin/sh

PIPELINE=$1
DELAY=$2
RESULT=0

if [ -z $PIPELINE ]; then
        echo "Error pipeline not found"
        echo "Usage ./dss_streaming.sh <vid> <delay (optional)>"
        exit 1
fi

tv_lcd_changer.sh $PIPELINE&
RESULT=`command_tracking.sh $RESULT $?`
imgstream.sh $PIPELINE "RGB565 160 120" "$VIDEOFILES/video_qqvga_rgb_30 $DELAY"
RESULT=`command_tracking.sh $RESULT $?`
killall tv_lcd_changer.sh
RESULT=`command_tracking.sh $RESULT $?`

if [ -z "$STRESS" ]; then
        stress_message.sh
fi

exit $RESULT
