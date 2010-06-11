#!/bin/sh

echo "TO PERFORM THIS TEST, IT IS NECESSARY TO HAVE 2 FRAMEBUFFERS,"
echo "IF NOT PLEASE ADD omapfb.numfb=2 in bootargs"
sleep 5
$TESTBIN/fbmode 16
$TESTBIN/fbmode 16 -f /dev/fb1
echo "Sending FB to Primary LCD"
sleep 2
#echo tv > $TVOUT
echo "1" > /sys/devices/platform/omapdss/display0/enabled
echo "1" > /sys/devices/platform/omapdss/display1/enabled
echo "1" > /sys/devices/platform/omapdss/display2/enabled
sleep 2

echo "0" > /sys/devices/platform/omapdss/overlay0/enabled
echo "lcd" > /sys/devices/platform/omapdss/overlay0/manager
echo "1" > /sys/devices/platform/omapdss/overlay0/enabled
echo "0" > /sys/devices/platform/omapdss/overlay0/enabled
echo "lcd" > /sys/devices/platform/omapdss/overlay0/manager
echo "1" > /sys/devices/platform/omapdss/overlay0/enabled


echo "0" > /sys/devices/platform/omapdss/overlay1/enabled
echo "2lcd" > /sys/devices/platform/omapdss/overlay1/manager
echo "1" > /sys/devices/platform/omapdss/overlay1/enabled
echo "1" > /sys/devices/platform/omapdss/display1/enabled
echo "0" > /sys/devices/platform/omapdss/overlay1/enabled
echo "2lcd" > /sys/devices/platform/omapdss/overlay1/manager
echo "1" > /sys/devices/platform/omapdss/overlay1/enabled

echo "Sending FB0 to Primary LCD"
echo "Sending FB1 to Secondary LCD"

sleep 2

$TESTBIN/arm-none-linux-gnueabi-fbtest -f /dev/fb0 &
$TESTBIN/arm-none-linux-gnueabi-fbtest -f /dev/fb1

if [ -z "$STRESS" ]; then
  echo "";echo "Was framebuffer performing all funtions fine";echo
 ""
  $WAIT_ANSWER
  ERR=$?
  if [ $ERR -eq 1 ]; then
    echo "FAIL"
    exit 1
  else
    echo "PASS"
    exit 0
  fi
fi

