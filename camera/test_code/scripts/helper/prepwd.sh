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

# Usage: ioctl <size> <format>
$TESTBIN/ioctl $SIZE $FORMAT
sleep 1

$TESTBIN/overlay "rot$ROT" &

sleep 7
echo "Suspend camera driver"
echo -n 3 > $SYSCAMPWD
echo "You must not see video preview"
sleep 4
echo "Resume camera driver"
echo -n 0 > $SYSCAMPWD
echo "You must see video preview"
sleep 7
echo "Suspend camera driver"
echo -n 3 > $SYSCAMPWD
echo "You must not see video preview"
sleep 3
echo "Resume camera driver"
echo -n 0 > $SYSCAMPWD
echo "You must see video preview"
sleep 7
echo "Suspend camera driver"
echo -n 3 > $SYSCAMPWD
echo "You must not see video preview"
sleep 2
echo "Resume camera driver"
echo -n 0 > $SYSCAMPWD
echo "You must see video preview"

sleep 10

echo lcd > $SYSVID

if [ -z "$STRESS" ]; then
  echo "";echo "Was camera able to preview video in $FORMAT format with $SIZE size and $ROT rotation on $OUT without kernel crash during suspend-resume?";echo ""
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

