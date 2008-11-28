 #!/bin/sh

VIDEO_PIPELINE=$1
SETIMG_PARAMETERS=$2
SETTRANSCK_PARAMETERS=$3
ENABLECOLKEY_PARAMETERS=$4
STREAMING_PARAMETERS=$5

sleep $MESSAGE_DELAY

# Usage: setimg <vid> <fmt> <width> <height>
$TESTBIN/setimg $VIDEO_PIPELINE $SETIMG_PARAMETERS

# Usage: settransck < o/p device [1:LCD 2:TV]> <key type [0:GFX DEST 1:VID SRC]> <RGB key value>
# RGB Key value for 0xF801 -> 63489
$TESTBIN/settransck $SETTRANSCK_PARAMETERS

# Usage: enablecolkey < o/p device [1:LCD 2:TV]> <enable/disable [1:0]>
$TESTBIN/enablecolkey $ENABLECOLKEY_PARAMETERS

# Usage: streaming <vid> <inputfile> [<n>]
$TESTBIN/streaming $VIDEO_PIPELINE $STREAMING_PARAMETERS

if [ -z "$STRESS" ]; then
  echo "Were the lines displayed with transparency?"
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
