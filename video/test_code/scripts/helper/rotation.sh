#!/bin/sh

VIDEO_PIPELINE=$1
SETIMG_PARAMETERS=$2
STREAMING_PARAMETERS=$3
SETCROP_PARAMETERS=$4
SETWIN_PARAMETERS=$5
RESULT=0

# Usage: setimg <vid> <fmt> <width> <height>
$TESTBIN/setimg $VIDEO_PIPELINE $SETIMG_PARAMETERS

# Usage: setrotation <vid> <angle>
$TESTBIN/setrotation $VIDEO_PIPELINE 90
# Usage: streaming <vid> <inputfile> [<n>]
$TESTBIN/streaming_tiler $VIDEO_PIPELINE $STREAMING_PARAMETERS 0

# Usage: setrotation <vid> <angle>
$TESTBIN/setrotation $VIDEO_PIPELINE 180
# Usage: streaming <vid> <inputfile> [<n>]
$TESTBIN/streaming_tiler $VIDEO_PIPELINE $STREAMING_PARAMETERS 0

# Usage: setrotation <vid> <angle>
$TESTBIN/setrotation $VIDEO_PIPELINE 270
# Usage: streaming <vid> <inputfile> [<n>]
$TESTBIN/streaming_tiler $VIDEO_PIPELINE $STREAMING_PARAMETERS 0

$TESTBIN/setrotation $VIDEO_PIPELINE 0

RESULT=`command_tracking.sh $RESULT $?`

if [ -z "$STRESS" ]; then
	stress_message.sh
fi

exit $RESULT
