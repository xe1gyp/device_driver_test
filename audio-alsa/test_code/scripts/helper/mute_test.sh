#!/bin/sh

COUNT=0
GAIN=0
$APLAY $STEREO_AUDIO_FILE"11025.wav" &
while [ $COUNT -le 9 ]
do
	if [ "$GAIN" == 0 ]; then
		GAIN=18
	else
		GAIN=0
	fi

	$AMIXER cset numid=7 $GAIN
	sleep 2
	COUNT=`expr $COUNT + 1`
done
