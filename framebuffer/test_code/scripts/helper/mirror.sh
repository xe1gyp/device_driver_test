#!/bin/sh

  MIRROR="/sys/class/graphics/fb0/mirror"

  $TESTBIN/fbmode 16
  
  #Usage: fbmirror <0-1> 1 to enable, 0 to disable
  echo 1 > $MIRROR
  echo "Frame Buffer horizontal mirroring set"
  sleep 2
  $FBTEST
  echo 0 > $MIRROR
  echo "Frame Buffer horizontal mirroring unset"
  sleep 2
  $FBTEST


if [ -z "$STRESS" ]; then
  echo "";echo "Are all tests from the 'fbtest' suite passed with proper (mirrored) display and after that FB display retored to its unmirrored?"; echo ""
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

