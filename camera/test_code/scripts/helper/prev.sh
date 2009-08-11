#!/bin/sh

ROT=$1
SIZE=$2
FORMAT=$3
OUT=$4

if [ $OUT = "TV" ]; then
  echo tv > $SYSVID 
else
  echo lcd > $SYSVID
fi
if [OUT = "TV"]; then
# Usage: ioctl <size> <format>
$TESTBIN/ioctl $SIZE $FORMAT
sleep 1 

$TESTBIN/overlay "rot$ROT" 

echo lcd > $SYSVID

if [ -z "$STRESS" ]; then
  echo "";echo "Was camera able to preview video in $FORMAT format with $SIZE size and $ROT rotation on $OUT?";echo ""
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
