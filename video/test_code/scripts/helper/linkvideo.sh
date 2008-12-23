#!/bin/sh

OUTPUT1=$1
OUTPUT2=$2

echo $OUTPUT1 > /sys/class/display_control/omap_disp_control/video1
if [ "$OUTPUT1" = "tv" ];then
        echo "Sending pipeline 1 to tv"
        sleep 3
        echo "Done"
fi

echo $OUTPUT2 > /sys/class/display_control/omap_disp_control/video2
if [ "$OUTPUT2" = "tv" ];then
        echo "Sending pipeline 2 to tv"
        sleep 3
        echo "Done"
fi


#echo SETTING IMG PARAMETERS
$TESTBIN/setimg 1 "RGB565" "160" "120"
$TESTBIN/setimg 2 "RGB565" "160" "120"

#echo SETTING WIN PARAMETERS
$TESTBIN/setwin "1" "0" "0" "160" "120"
$TESTBIN/setwin "2" "0" "160" "160" "120"

$TESTBIN/setlink 2& $TESTBIN/streaming "1" "$VIDEOFILES/video_qqvga_rgb_30"
killall setlink

# reseting the previous values.
echo "lcd" > /sys/class/display_control/omap_disp_control/video1
echo "lcd" > /sys/class/display_control/omap_disp_control/video2

if [ -z "$STRESS" ]; then
        strees_messages.sh
fi

