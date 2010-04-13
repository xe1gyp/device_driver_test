#!/bin/sh

OUTPUT1=$1
OUTPUT2=$2
RESULT=0

echo $OUTPUT1 > /sys/devices/platform/omapdss/overlay1/manager

if [ "$OUTPUT1" = "tv" ];then
        echo "Sending pipeline 1 to tv"
        sleep 3
        echo "Done"
fi

echo $OUTPUT2 > /sys/devices/platform/omapdss/overlay2/manager

if [ "$OUTPUT2" = "tv" ];then
        echo "Sending pipeline 2 to tv"
        sleep 3
        echo "Done"
fi


#echo SETTING IMG PARAMETERS
$TESTBIN/setimg 1 "RGB565" "160" "120"
RESULT=`command_tracking.sh $RESULT $?`

$TESTBIN/setimg 2 "RGB565" "160" "120"
RESULT=`command_tracking.sh $RESULT $?`

#echo SETTING WIN PARAMETERS
$TESTBIN/setwin "1" "0" "0" "160" "120"
RESULT=`command_tracking.sh $RESULT $?`

$TESTBIN/setwin "2" "0" "160" "160" "120"
RESULT=`command_tracking.sh $RESULT $?`

$TESTBIN/setlink 2 1
$TESTBIN/streaming "2" "$VIDEOFILES/video_qqvga_rgb_30"
$TESTBIN/setlink 2 0
killall setlink

# reseting the previous values.
echo "lcd" > /sys/devices/platform/omapdss/overlay1/manager
echo "lcd" > /sys/devices/platform/omapdss/overlay2/manager

if [ -z "$STRESS" ]; then
        stress_message.sh
fi

exit $RESULT
