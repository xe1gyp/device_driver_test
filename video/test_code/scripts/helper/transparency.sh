 #!/bin/sh

VIDEO_PIPELINE=$1
SETIMG_PARAMETERS=$2
SETTRANSCK_PARAMETERS=$3
ENABLECOLKEY_PARAMETERS=$4
STREAMING_PARAMETERS=$5
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

# Usage: enablecolkey < o/p device [1:LCD 2:TV]> <enable/disable [1:0]>
$TESTBIN/enablecolkey $ENABLECOLKEY_PARAMETERS
RESULT=`command_tracking.sh $RESULT $?`

# Usage: streaming <vid> <inputfile> [<n>]
$TESTBIN/streaming $VIDEO_PIPELINE $STREAMING_PARAMETERS
RESULT=`command_tracking.sh $RESULT $?`

if [ -z "$STRESS" ]; then
	stress_message.sh
fi

exit $RESULT
