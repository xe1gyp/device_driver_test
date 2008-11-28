#!/bin/sh

# Usage ioctl <size> <format>
$TESTBIN/ioctl VGA UYVY

# Usage ioctl2 <size> <format>
$TESTBIN/ioctl2 VGA UYVY

$TESTBIN/streaming_read YUYV YUYV 4 4 160 120 320 240 80 60 400 300 user



if [ -z "$STRESS" ]; then
  echo "";echo "In first place, Did you observed a cropped VGA video with width=320 and height=240?";echo ""
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
