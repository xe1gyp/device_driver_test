#!/bin/sh

COMB=$1

# Usage: multi_opwn <combination>

$TESTBIN/multi_open $COMB

if [ -z "$STRESS" ]; then
  echo "";echo "Was the video captured and displayed on the LCD with birgthness change?";echo ""
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
