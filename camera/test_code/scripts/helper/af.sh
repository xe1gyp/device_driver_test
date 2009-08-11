#!/bin/sh

VID=$1
RATE=$2
LENS=$3

${TESTBIN}/af_stream $VID $LENS $RATE "2" 
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
