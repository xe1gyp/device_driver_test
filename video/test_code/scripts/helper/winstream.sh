#!/bin/sh

VIDEO_PIPELINE=$1
SETIMG_PARAMETERS=$2
SETCROP_PARAMETERS=$3
SETWIN_POSITION=$4
STREAMING_PARAMETERS=$5
INCREMENT=20
EXIT_WHILE=0
RESULT=0
NUM_BUFF=6

$TESTBIN/setimg $VIDEO_PIPELINE $SETIMG_PARAMETERS > $TMPBASE\temp_setimg.tmp
var=`cat $TMPBASE\temp_setimg.tmp | grep window`
var=`echo $var | sed -e 's/[a-zA-Z-]*//g' | sed -e 's/=//g'`

RESULT=`command_tracking.sh $RESULT $?`

set $var
WIDTH=`echo $SETIMG_PARAMETERS | awk '{print $2}'`
HEIGTH=`echo $SETIMG_PARAMETERS | awk '{print $3}'`
X_POS=`echo $SETWIN_POSITION | awk '{print $1}'`
Y_POS=`echo $SETWIN_POSITION | awk '{print $2}'`
ACTUAL_SIZE=$HEIGTH

while [ $EXIT_WHILE != 1 ];
do
	# Usage: setcrop <vid> <left> <top> <width> <height>
	$TESTBIN/setcrop $VIDEO_PIPELINE $SETCROP_PARAMETERS
	RESULT=`command_tracking.sh $RESULT $?`

	# Usage: setwin <vid> <left> <top> <width> <height>
	$TESTBIN/setwin $VIDEO_PIPELINE  $X_POS $Y_POS $ACTUAL_SIZE $ACTUAL_SIZE
	RESULT=`command_tracking.sh $RESULT $?`

	# Usage: streaming <vid> <inputfile> [<n>]
	$TESTBIN/streaming $VIDEO_PIPELINE $NUM_BUFF $STREAMING_PARAMETERS
	RESULT=`command_tracking.sh $RESULT $?`

	if [ -z "$STRESS" ]; then
		stress_message.sh
	fi

	sleep 2
	ACTUAL_SIZE=`expr "$ACTUAL_SIZE" "+" "$INCREMENT"`

	if [ "$WIDTH" -lt "$ACTUAL_SIZE" ]; then
		if [ "$HEIGTH" -lt "$ACTUAL_SIZE" ]; then
			EXIT_WHILE=`expr "$EXIT_WHILE" "+" 1`
		fi
	fi

done

exit $RESULT
