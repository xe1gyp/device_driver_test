#!/bin/sh

  $TESTBIN/fbmode 16
  
  #Usage: fbflipbut
  $TESTBIN/fbflipbut

if [ -z "$STRESS" ]; then
  echo "";echo "Are the second and third buffers filled with 2 different colors and the FB screen color changed fully and finally the screen flips back to the 1st FB, is the FB showing whatever it was displaying before running the test?"; echo ""
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

