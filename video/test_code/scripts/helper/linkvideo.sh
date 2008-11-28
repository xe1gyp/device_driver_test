#!/bin/sh

#TAG=$1
#SETIMG_PARAMETERS=$1
#SETWIN_PARAMETERS=$1
#SETWIN_PARAMETERS_2=$2
OUTPUT1=$1
OUTPUT2=$2
#SETMIRROR_PARAMETERS=$5
#STREAMING_PARAMETERS=$5
#SETLINK_PARAMETERS=$7
#MESSAGE=$8

print()
{
  msg=$1
  echo $msg
  echo $msg >> $LOG_OUTPUT
}

#set $SETIMG_PARAMETERS
#print ""
#print "---------------------- L_DD_VIDEO_$TAG ----------------------"
#print "Verify the pipelines can be enabled or configured individually for mirroring, rotation, cropping, while the pipelines are linked"
#print "Setting image parameters"
#print "<vid:$1> <fmt:$2> <width:$3> <height:$4>"
#sleep $MESSAGE_DELAY

#set $SETWIN_PARAMETERS
#print "Setting win parameters"
#print "<vid:$1> <left:$2> <top:$3> <width:$4> <height:$5>"
#sleep $MESSAGE_DELAY

#set $SETWIN_PARAMETERS_2
#print "Setting win parameters"
#print "<vid:$1> <left:$2> <top:$3> <width:$4> <height:$5>"
#sleep $MESSAGE_DELAY

#set $SETMIRROR_PARAMETERS
#print "Setting mirror parameters <mirror $SETMIRROR_PARAMETERS>"
#print "<vid:$1> <enable[1]/disable[0]><$2>"
#sleep $MESSAGE_DELAY

#set $SETLINK_PARAMETERS
#print "Setting link parameters"
#print "<link to:$1>"
#sleep $MESSAGE_DELAY

echo "lcd" > /sys/class/display_control/omap_disp_control/video1
echo "lcd" > /sys/class/display_control/omap_disp_control/video2
sleep 1

echo SETTING IMG PARAMETERS
$TESTBIN/setimg 1 "RGB565" "160" "120"
$TESTBIN/setimg 2 "RGB565" "160" "120"

echo SETTING WIN PARAMETERS
$TESTBIN/setwin "1" "0" "0" "160" "120"
$TESTBIN/setwin "2" "0" "160" "160" "120"

#$TESTBIN/setmirror $SETMIRROR_PARAMETERS | grep "fault"
$TESTBIN/setlink 2& $TESTBIN/streaming "1" "$VIDEOFILES/video_qqvga_rgb_30";echo ^C

echo "lcd" > /sys/class/display_control/omap_disp_control/video1
echo "tv" > /sys/class/display_control/omap_disp_control/video2
sleep 1

echo SETTING WIN PARAMETERS
$TESTBIN/setwin "1" "0" "0" "160" "120"
$TESTBIN/setwin "2" "0" "160" "160" "120"

$TESTBIN/setlink 2& $TESTBIN/streaming "1" "$VIDEOFILES/video_qqvga_rgb_30";

echo "tv" > /sys/class/display_control/omap_disp_control/video1
echo "tv" > /sys/class/display_control/omap_disp_control/video2
sleep 1

echo SETTING WIN PARAMETERS
$TESTBIN/setwin "1" "0" "0" "160" "120"
$TESTBIN/setwin "2" "0" "160" "160" "120"

$TESTBIN/streaming "1" "$VIDEOFILES/video_qqvga_rgb_30";

echo "lcd" > /sys/class/display_control/omap_disp_control/video1
echo "lcd" > /sys/class/display_control/omap_disp_control/video2

if [ -z "$STRESS" ]; then
  print ""
  print "Were the images displayed correctly on both videopipelines??"
  $WAIT_ANSWER
  ERR=$?
  if [ $ERR -eq 1 ]; then
    print "FAIL"
    exit 1
  else
    print "PASS"
    exit 0
  fi
fi

