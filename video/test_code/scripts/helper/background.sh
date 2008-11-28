#!/bin/sh

OUT_DEV=$1
BG_COLOR=$2

#Usage: setbgcolor <output device [1: LCD 2: TV]> <24 bit RGB value>
$TESTBIN/setbgcolor $OUT_DEV $BG_COLOR

if [ -z "$STRESS" ]; then
  echo "Did the background color in the LCD change?"
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
