#!/bin/sh

FBS=$1
SIZE=$2
FORMAT=$3
DEVICE=$4
ZOOM=$5

# Usage: framerate <fbs>
#$TESTBIN/framerate $FBS

# Usage: ioctl <size> <format>
#$TESTBIN/ioctl $SIZE $FORMAT
#sleep 1 

#Usage: streaming <video> <frames>
if [ "$ZOOM" = "ZOOM" ]; then
  time $TESTBIN/streaming_zoom $DEVICE $FORMAT $SIZE "1" "300"
  echo "Streaming Zoom test"
else
  #if [ $FBS = "30" ]; then
  time $TESTBIN/streaming_frame $DEVICE $FBS $FORMAT $SIZE
  echo "Please report time that test takes to run."
  #sleep 5
  #else
  #  $TESTBIN/streaming $DEVICE $FORMAT $SIZE
  #fi
fi

#$TESTBIN/framerate 15

if [ -z "$STRESS" ]; then
  if [ $SIZE = "VGA" -o $SIZE = "D1PAL" -o $SIZE = "D1NTSC" ]; then
    echo "";echo " Was streaming $FORMAT video at $FBS fps in $SIZE size displayed cleanly on LCD screen with video chopped to screen size.?";echo ""
  else 
    echo "";echo " Was streaming $FORMAT video at $FBS fbs in $SIZE size displayed cleanly on LCD screen?";echo ""
  fi
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
