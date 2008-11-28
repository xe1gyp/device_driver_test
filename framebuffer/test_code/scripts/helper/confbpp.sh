 	#!/bin/sh


if [ "$1" ]; then
  BPP_MODE=$1
  # Usage: fbmode <bits-per-pixel>
  $TESTBIN/fbmode $BPP_MODE
  sleep 2
else
  BPP_MODE="default"
fi

$FBTEST

$TESTBIN/fbmode 16

if [ -z "$STRESS" ]; then
  echo "";echo "Was framebuffer performing all funtions fine in $BPP_MODE bits mode";echo ""
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
