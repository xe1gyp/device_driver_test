#!/bin/sh

  $TESTBIN/fbmode 16
  
  echo "off" > /sys/class/display_control/omap_disp_control/lcdbacklight
  echo "LCD backlight turn OFF"
  sleep 3
  $FBTEST
  echo "on" > /sys/class/display_control/omap_disp_control/lcdbacklight
  echo "LCD backlight turn ON"  
  sleep 2

if [ -z "$STRESS" ]; then
  echo "";echo "The Backlight turned OFF, ran the fbtest, and it turned ON again?"; echo ""
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

