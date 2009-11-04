#!/bin/sh

SIZE=$1
FORMAT=$2
DEVICE=$3
FRNUM=$4
FPS=$5
TEST=$6

if [ -z "$5" ]; then
FPS=30
fi

if [ -z "$6" ]; then
FNAME="${TMPBASE}/${SIZE}bm.yuv"
elif [ "$FPS" = "120" ];then
FNAME="${TMPBASE}/${SIZE}_120fps.yuv"
else
FNAME="${TMPBASE}/${SIZE}-${TEST}bm.yuv"
fi

if [ "$SIZE" = "3280 2464" ]; then
  FNAME="${TMPBASE}/8MPbm.yuv"
fi

if [ "$SIZE" = "2592 1944" ]; then
  FNAME="${TMPBASE}/5MPbm.yuv"
fi

if [ "$SIZE" = "864 656" ]; then
  FNAME="${TMPBASE}/864_656bm.yuv"
fi

if [ "$SIZE" = "982 560" ]; then
  FNAME="${TMPBASE}/982_560vid.yuv"
fi

if [ -z "$6" ]; then
  # Usage: burst_mode <camDevice><pixelFmt><size><num_of_images><file_name>
  $TESTBIN/burst_mode $DEVICE $FORMAT $SIZE $FRNUM "$FNAME" $FPS
  RESULT=$?
  echo "Test returned $RESULT"
elif [ "$TEST" = "BW" ]; then
    # Usage: busrt_mode <camDevice><pixelFmt><size><num_of_images><file_name><effect>
   $TESTBIN/burst_mode $DEVICE $FORMAT $SIZE $FRNUM "$FNAME" $FPS $TEST
   RESULT=$?
   echo "Test returned $RESULT"
elif [ "$TEST" = "SEPIA" ]; then
      # Usage: busrt_mode <camDevice><pixelFmt><size><num_of_images><file_name><effect>
      $TESTBIN/burst_mode $DEVICE $FORMAT $SIZE $FRNUM "$FNAME" $FPS $TEST
      RESULT=$?
      echo "Test returned $RESULT"
elif [ "$FPS" = "120" ]; then
      #Usage: burst_mode_120 <pixelFmt><size><file_name>
      $TESTBIN/burst_mode_120 $FORMAT $SIZE "$FNAME"
      RESULT=$?
      echo "Test returned $RESULT"
else
      # Usage: busrt_zoom <camDevice><pixelFmt><size><num_of_images><file_name><zoomFactor>
      #echo "$TESTBIN/burst_zoom $DEVICE $FORMAT $SIZE $FRNUM $FNAME $EFFECT"
      $TESTBIN/burst_zoom $DEVICE $FORMAT $SIZE $FRNUM "$FNAME" $TEST
      RESULT=$?
      echo "Test returned $RESULT"
fi

  chmod 744 $FNAME

if [ $RESULT -eq 255 ]; then
  ERR=1
elif [ -z "$STRESS" ]; then
  echo "";echo "Was camera able to capture $FRNUM frame(s) in $FORMAT format of $SIZE size and saved into $FNAME?";echo ""
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
