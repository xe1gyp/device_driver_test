#!/bin/sh

VIDEO_PIPELINE=$1
SETIMG_PARAMETERS=$2
SETCROP_PARAMETERS=$3
STREAMING_PARAMETERS=$4
ACTUAL_SIZE=30
ACTUAL_WIDTH=0
ACTUAL_HEIGHT=0
INCREMENT=20
EXIT_WHILE=1
RESULT=0


$TESTBIN/setimg $VIDEO_PIPELINE $SETIMG_PARAMETERS > $TMPBASE\temp_setimg.tmp
var=`cat $TMPBASE\temp_setimg.tmp | grep window`
var=`echo $var | sed -e 's/[a-zA-Z-]*//g' | sed -e 's/=//g'`

RESULT=`command_tracking.sh $RESULT $?`

set $var
WIDTH=`echo $SETIMG_PARAMETERS | awk '{print $2}'`
HEIGHT=`echo $SETIMG_PARAMETERS | awk '{print $3}'`
ACTUAL_WIDTH=$((WIDTH/2))
ACTUAL_HEIGHT=$((HEIGHT/2))

while [ $EXIT_WHILE -gt 0 ];
do
	# Usage: setcrop <vid> <left> <top> <width> <height>
	$TESTBIN/setcrop $VIDEO_PIPELINE $SETCROP_PARAMETERS
	RESULT=`command_tracking.sh $RESULT $?`

	# Usage: setwin <vid> <left> <top> <width> <height>
	$TESTBIN/setwin $VIDEO_PIPELINE 0 0 $ACTUAL_WIDTH $ACTUAL_HEIGHT
	RESULT=`command_tracking.sh $RESULT $?`

	# Usage: streaming <vid> <inputfile> [<n>]
	$TESTBIN/streaming $VIDEO_PIPELINE $STREAMING_PARAMETERS
	RESULT=`command_tracking.sh $RESULT $?`

	if [ -z "$STRESS" ]; then
		stress_message.sh
	fi

	sleep 1
	ACTUAL_WIDTH=`expr "$ACTUAL_WIDTH" "+" "$INCREMENT"`
	ACTUAL_HEIGHT=`expr "$ACTUAL_HEIGHT" "+" "$INCREMENT"`
	EXIT_WHILE=`expr "$WIDTH" "-" "$ACTUAL_WIDTH"`
	
	if [ $EXIT_WHILE -gt 0 ];then
		EXIT_WHILE=`expr "$HEIGHT" "-" "$ACTUAL_HEIGHT"`
	fi

done

exit $RESULT
