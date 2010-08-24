#!/bin/sh

DEVICE=$1
SIZE=$2

if [ "$SIZE" = "2592 1944" ]; then
  FNAME="${TMPBASE}/5MPsi.raw"
fi

if [ "$SIZE" = "3280 2464" ]; then
  FNAME="${TMPBASE}/8MPsi.raw"
fi

$TESTBIN/burst_mode $DEVICE SRGGB10 $SIZE 1 $FNAME
RESULT=$?
echo "Test returned $RESULT"

if [ $RESULT -eq 255 ]; then
  ERR=1
elif [ -z "$STRESS" ]; then
  echo "";echo "Was a $SIZE RAW image correctly stored in $FNAME?";echo ""
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
