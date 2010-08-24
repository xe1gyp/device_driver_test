#!/bin/sh

COMB=$1
DEVICE=$2

if [ -z "$DEVICE" ]; then
  echo "Defaulting to use Device 1"
  DEVICE=1
fi

# Usage: multi_open <combination> <fps> <out dev> <capture dev>
$TESTBIN/multi_open $COMB 30 1 $DEVICE
RESULT=$?
echo "Test returned $RESULT"

if [ $RESULT -eq 255 ]; then
  ERR=1
elif [ -z "$STRESS" ]; then
  echo "";echo "Was the video captured and displayed on the LCD with brigthness change?";echo ""
  $WAIT_ANSWER
  ERR=$?
fi
if [ $ERR -eq 1 ]; then
  echo "FAIL"
  exit 1
else
  echo "PASS"
  exit 0
fi
