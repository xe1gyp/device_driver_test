#!/bin/sh
lcd2ovl1.sh
echo "TO PERFORM THIS TEST,"
echo "IT IS NECESSARY TO ADD omapfb.tiler=y in the bootargs"
echo "1" > /sys/devices/platform/omapdss/display0/enabled
sleep 2

echo "0" > /sys/devices/platform/omapdss/overlay0/enabled
echo "lcd" > /sys/devices/platform/omapdss/overlay0/manager
echo "1" > /sys/devices/platform/omapdss/overlay0/enabled
echo "0" > /sys/devices/platform/omapdss/overlay0/enabled
echo "lcd" > /sys/devices/platform/omapdss/overlay0/manager
echo "1" > /sys/devices/platform/omapdss/overlay0/enabled

echo "1" > /sys/class/graphics/fb0/mirror
sleep 2
echo "";echo "Scenario L_DD_FB_0002";echo ""
echo "Sending FB0 with horizontal mirroring to Primary LCD"
confbpp.sh 24

sleep 5

echo "0" > /sys/class/graphics/fb0/mirror
sleep 2
echo "";echo "Scenario L_DD_FB_0002";echo ""
echo "Sending FB0 without horizontal mirroring to Primary LCD"
confbpp.sh 24


########################################## fb1 ####################

echo "1" > /sys/devices/platform/omapdss/display1/enabled
sleep 2
echo "0" > /sys/devices/platform/omapdss/overlay1/enabled
echo "2lcd" > /sys/devices/platform/omapdss/overlay1/manager
echo "1" > /sys/devices/platform/omapdss/overlay1/enabled
echo "0" > /sys/devices/platform/omapdss/overlay1/enabled
echo "2lcd" > /sys/devices/platform/omapdss/overlay1/manager
echo "1" > /sys/devices/platform/omapdss/overlay1/enabled

echo "1" > /sys/class/graphics/fb1/mirror
sleep 2
echo "";echo "Scenario L_DD_FB_0002";echo ""
echo "Sending FB1 with horizontal mirroring to Primary LCD"
confbpp.sh 24 /dev/fb1

sleep 5

echo "0" > /sys/class/graphics/fb1/mirror
sleep 2
echo "";echo "Scenario L_DD_FB_0002";echo ""
echo "Sending FB1 without horizontal mirroring to Primary LCD"
confbpp.sh 24 /dev/fb1

if [ -z "$STRESS" ]; then
  echo "";echo "Was framebuffer performing all funtions fine in TV output";echo
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

