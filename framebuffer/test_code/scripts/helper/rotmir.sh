#!/bin/sh

  ROT=$1
  $TESTBIN/fbmode 16
  

  #Usage fbrotation <rotation>
  $TESTBIN/fbrotate $ROT
  sleep 3
  #Usage: fbmirror <0-1> 1 to enable, 0 to disable
  $TESTBIN/fb_mirror 1
  echo "Frame Buffer horizontal mirroring set"
  sleep 2
  $FBTEST
  $TESTBIN/fb_mirror 0
  echo "Frame Buffer horizontal mirroring unset"
  sleep 2

  $TESTBIN/fbrotate 0

if [ -z "$STRESS" ]; then
  echo "";echo "Are all tests from the 'fbtest' suite passed with proper (mirrored) display with $ROT degrees rotated?"; echo ""
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

