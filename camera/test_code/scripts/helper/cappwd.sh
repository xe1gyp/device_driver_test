#!/bin/sh

FBS=$1
SIZE=$2
FORMAT=$3
DEVICE=$4

# Usage: framerate <fbs>
#$TESTBIN/framerate $FBS

# Usage: ioctl <size> <format>
#$TESTBIN/ioctl $SIZE $FORMAT

#Usage: streaming <video> <frames>
$TESTBIN/streaming_frame $DEVICE $FBS $FORMAT $SIZE "1" 500&

sleep 7
echo "Suspend camera driver"
echo "Touch screen or press key to return"
echo -n standby > $SYSCAMPWD
#echo -n 3 > $SYSCAMPWD
#echo "You must be watching video streaming stopped"
#sleep 4
echo "Resume camera driver"
#echo -n 0 > $SYSCAMPWD
echo "You must be watching video streaming resumed"
sleep 7
echo "Suspend camera driver"
echo "Touch screen or press key to return"
echo -n standby > $SYSCAMPWD
#echo -n 3 > $SYSCAMPWD
#echo "You must be watching video streaming stopped"
#sleep 4
echo "Resume camera driver"
#echo -n 0 > $SYSCAMPWD
echo "You must be watching video streaming resumed"
sleep 7
echo "Suspend camera driver"
echo "Touch screen or press key to return"
echo -n standby > $SYSCAMPWD
#echo -n 3 > $SYSCAMPWD
#echo "You must be watching video streaming stopped"
#sleep 4
echo "Resume camera driver"
#echo -n 0 > $SYSCAMPWD
echo "You must be watching video streaming resumed"

sleep 13

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

