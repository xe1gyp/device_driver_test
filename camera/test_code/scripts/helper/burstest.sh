#!/bin/sh

SIZE=$1
FORMAT=$2
DEVICE=$3
FRNUM=$4
ZOOM=$5

# Usage: ioctl <size> <format>
#$TESTBIN/ioctl $SIZE $FORMAT
#sleep 1 
#FNAME="${TMPBASE}/${SIZE}bm.yuv"
if [ -z "$5" ]; then
FNAME="${TMPBASE}/${SIZE}bm.yuv"
else
FNAME="${TMPBASE}/${SIZE}-${ZOOM}bm.yuv"
fi

if [ "$SIZE" = "2592 1944" ]; then 
  FNAME="${TMPBASE}/5MPbm.yuv"
fi

if [ "$SIZE" = "864 656" ]; then
  FNAME="${TMPBASE}/864_656bm.yuv"
fi

if [ -z "$5" ]; then
  # Usage: busrt_mode <camDevice><pixelFmt><size><num_of_images><file_name>
  $TESTBIN/burst_mode $DEVICE $FORMAT $SIZE $FRNUM "$FNAME"
else
  # Usage: busrt_zoom <camDevice><pixelFmt><size><num_of_images><file_name><zoomFactor>
  $TESTBIN/burst_zoom $DEVICE $FORMAT $SIZE $FRNUM "$FNAME" $ZOOM
fi

  chmod 744 $FNAME

if [ -z "$STRESS" ]; then
  echo "";echo "Was camera able to capture $FRNUM frame(s) in $FORMAT format of $SIZE size and saved into $FNAME?";echo ""
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
