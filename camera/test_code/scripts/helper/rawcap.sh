#!/bin/sh

DEVICE=$1

# Usage: ioctl <size> <format>
#$TESTBIN/ioctl 2592 1944 RAW10
#sleep 1 

FNAME="${TMPBASE}/5MPsi.raw"
$TESTBIN/burst_mode $DEVICE RAW10 2592 1944 1 $FNAME
RESULT=$?
echo "Test returned $RESULT"
chmod 744 $FNAME

if [ $RESULT -eq 255 ]; then
  ERR=1
elif [ -z "$STRESS" ]; then
  echo "";echo "Was capture 5MP image in $FNAME without image processing of video driver?";echo ""
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
