#!/bin/sh

${TESTBIN}/process_cam 10 20 
RESULT=$?
echo "Test returned $RESULT"

if [ $RESULT -eq 255 ]; then
  ERR=1
elif [ -z "$STRESS" ]; then  
  echo "";echo "Did camera driver switch succesfully between threads?";echo ""
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

#if [ -z "$STRESS" ]; then
#  echo "";echo "Did camera driver switch succesfully between threads?";echo ""
 # $WAIT_ANSWER
  #ERR=$?
  #if [ $ERR -eq 1 ]; then
   # echo "FAIL"
   # exit 1
  #else
  #  echo "PASS"
   # exit 0
 # fi
#fi