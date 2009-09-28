#!/bin/sh

VID=$1
RATE=$2
LENS=$3
DEVICE=$4
LENS_TYPE=$5

if [ "$LENS_TYPE" = "RELATIVE" ]; then
${TESTBIN}/af_stream_rel $DEVICE $VID $RATE
else
${TESTBIN}/af_stream $DEVICE $VID $LENS $RATE "2"
fi

RESULT=$?
echo "Test returned $RESULT"

if [ $RESULT -eq 255 ]; then
  ERR=1
elif [ -z "$STRESS" ]; then
  echo ""; echo "Was camera able to adjust lens at correct position(s)?"
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
