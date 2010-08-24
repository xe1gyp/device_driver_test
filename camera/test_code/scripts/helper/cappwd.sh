#!/bin/sh

FPS=$1
SIZE=$2
FORMAT=$3
DEVICE=$4

#Usage: streaming_frame <cam dev> <framerate> <pixformat> <size> <vout dev> <frames>
$TESTBIN/streaming_frame $DEVICE $FPS $FORMAT $SIZE "1" 500 &

sleep 1
echo "Suspend camera driver"
echo "Touch screen or press key to resume"
echo mem > /sys/power/state

if [ -z "$STRESS" ]; then
    echo "";echo " Was streaming $FORMAT video in $SIZE size displayed cleanly on LCD screen, without kernel crash during the suspend-resume?";echo ""
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

