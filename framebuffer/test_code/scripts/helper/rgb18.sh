#!/bin/sh

echo "By default previous tests should have been with 16bit IO lines."
sleep 5
$TESTBIN/fbmode 16
echo "Changing the I/O lines to 18 bit"
sleep 3
echo '18' > /sys/class/display_control/omap_disp_control/lcd_data_lines
sleep 2
$FBTEST
sleep 2
echo '16' > /sys/class/display_control/omap_disp_control/lcd_data_lines

if [ -z "$STRESS" ]; then
  echo "";echo "Did the Linux-fbdev's 'fbtest' test suite pass all tests with proper display on the framebuffer device?";echo ""
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

