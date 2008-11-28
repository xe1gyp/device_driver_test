#!/bin/sh

DEVICE=$1

# Usage: ioctl <size> <format>
#$TESTBIN/ioctl 2592 1944 RAW10
#sleep 1 

FNAME="${TMPBASE}/5MPsi.raw"
$TESTBIN/burst_mode $DEVICE RAW10 2592 1944 1 $FNAME
chmod 744 $FNAME

if [ -z "$STRESS" ]; then
  echo "";echo "Was capture 5MP image in $FNAME without image processing of video driver?";echo ""
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
