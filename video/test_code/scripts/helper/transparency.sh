 #!/bin/sh

VIDEO_PIPELINE=$1
SETIMG_PARAMETERS=$2
SETTRANSCK_PARAMETERS=$3
STREAMING_PARAMETERS=$4
RESULT=0

sleep $MESSAGE_DELAY

# Usage: setimg <vid> <fmt> <width> <height>
$TESTBIN/setimg $VIDEO_PIPELINE $SETIMG_PARAMETERS
RESULT=`command_tracking.sh $RESULT $?`

# Usage: settransck < o/p device [1:LCD 2:TV]>
# <key type [0:GFX DEST 1:VID SRC]> <RGB key value>

# RGB Key value for 0xF801 -> 63489
$TESTBIN/settransck $SETTRANSCK_PARAMETERS
RESULT=`command_tracking.sh $RESULT $?`

cat $VIDEOFILES/f801.txt > /dev/fb0

# Usage: streaming <vid> <inputfile> [<n>]
$TESTBIN/streaming $VIDEO_PIPELINE $STREAMING_PARAMETERS
RESULT=`command_tracking.sh $RESULT $?`

if [ -z "$STRESS" ]; then
	stress_message.sh
fi

exit $RESULT
