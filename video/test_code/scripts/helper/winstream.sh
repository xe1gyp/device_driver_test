#!/bin/sh

VIDEO_PIPELINE=$1
SETIMG_PARAMETERS=$2
SETCROP_PARAMETERS=$3
STREAMING_PARAMETERS=$3
ACTUAL_SIZE=30
INCREMENT=20
EXIT_WHILE=0
RESULT=0


$TESTBIN/setimg $VIDEO_PIPELINE $SETIMG_PARAMETERS > $TMPBASE\temp_setimg.tmp
var=`cat $TMPBASE\temp_setimg.tmp | grep window | sed -e 's/[a-zA-Z-]*//g' | sed -e 's/=//g'`
RESULT=`command_tracking.sh $RESULT $?`

set $var
WIDTH=$3
HEIGTH=$4

while [ $EXIT_WHILE != 1 ];
do
	# Usage: setcrop <vid> <left> <top> <width> <height>
	#$TESTBIN/setcrop $VIDEO_PIPELINE $SETCROP_PARAMETERS
	#RESULT=`command_tracking.sh $RESULT $?`

	# Usage: setwin <vid> <left> <top> <width> <height>
	$TESTBIN/setwin $VIDEO_PIPELINE 0 0 $ACTUAL_SIZE $ACTUAL_SIZE
	RESULT=`command_tracking.sh $RESULT $?`

	# Usage: streaming <vid> <inputfile> [<n>]
	$TESTBIN/streaming $VIDEO_PIPELINE $STREAMING_PARAMETERS 0
	RESULT=`command_tracking.sh $RESULT $?`

	if [ -z "$STRESS" ]; then
		stress_message.sh
	fi

	sleep 1
	ACTUAL_SIZE=`expr "$ACTUAL_SIZE" "+" "$INCREMENT"`

	if [ "$WIDTH" -lt "$ACTUAL_SIZE" ]; then
		if [ "$HEIGTH" -lt "$ACTUAL_SIZE" ]; then
			EXIT_WHILE=`expr "$EXIT_WHILE" "+" 1`
		fi
	fi

done

exit $RESULT
