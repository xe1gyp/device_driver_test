#!/bin/sh

SIZE=$1
FORMAT=$2

# Usage: ioctl <size> <format>
$TESTBIN/ioctl $SIZE $FORMAT
#OJV
echo "OJV THE SIZE CHANGE IS :" $SIZE
sleep 1 

if [ "$SIZE" = "2592 1944" ]; then 
  FNAME="${TMPBASE}/5MPbm.yuv"
else
  FNAME="${TMPBASE}/${SIZE}bm.yuv"
fi

# Usage: busrt_mode <num_of_images><file_name>
$TESTBIN/burst_mode 4 "$FNAME"
chmod 744 $FNAME

if [ -z "$STRESS" ]; then
  echo "";echo "Was camera able to capture a sequence of four images in $FORMAT format of $SIZE size and saved into $FNAME?";echo ""
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
