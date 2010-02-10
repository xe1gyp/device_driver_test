#!/bin/sh

FBS=$1
SIZE=$2
FORMAT=$3
FRNUM=$4
LIMIT=$5
DEVICE=1
COUNT=0

until [ "$COUNT" = "$LIMIT" ]; do
  DEVICE=`expr $COUNT % 2 + 1`
  echo "Device: $DEVICE, Count: $COUNT" 
  #usage: $TESTBIN/streaming_frame <device><framespersecond><format><size><vid><frame_number> 
  $TESTBIN/streaming_frame $DEVICE $FBS $FORMAT $SIZE "1" $FRNUM
  RESULT=$?
  echo "Test returned $RESULT"
  if [ $RESULT -eq 255 ];then
    ERR=1
    COUNT=$LIMIT
  else
    COUNT=`expr $COUNT + 1`
  fi
done
  
if [ -z "$STRESS" ]; then
  echo "";echo " Did camera driver switch succesfully between Micron and OV sensors? ";echo ""
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
