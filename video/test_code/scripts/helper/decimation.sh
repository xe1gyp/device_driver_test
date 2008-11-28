#!/bin/sh

VIDEO_PIPELINE=$1
WIDTH=$2
HEIGTH=$3
PXL_FMT=$4
VIDEO_FILE=$5
D_LEFT=$6
D_TOP=$7
D_WIDTH=$8
D_HEIGTH=$9
ROTATION=$10

if [ -z "$ROTATION" ]; then
	# Usage: setrotation <video> <rotationDegree>
	$TESTBIN/setrotation $VIDEO_PIPELINE 0
fi

# Usage: setimg <video> <pixel Fmt> <width> <heigth>
$TESTBIN/setimg $VIDEO_PIPELINE $PXL_FMT $WIDTH $HEIGTH

# Usage: setcrop <vid> <left> <top> <width> <height>
$TESTBIN/setcrop $VIDEO_PIPELINE 0 0 $WIDTH $HEIGTH

# Usage: setwin <vid> <left> <top> <width> <height>
$TESTBIN/setwin $VIDEO_PIPELINE $D_LEFT $D_TOP $D_WIDTH $D_HEIGTH

# Usage: streaming <vid> <inputfile> [<n>]
$TESTBIN/streaming $VIDEO_PIPELINE $VIDEO_FILE "1"

if [ -z "$STRESS" ]; then
  echo "Was the window changed and video streamed correctly?"
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

