#!/bin/sh

GAIN=0

$APLAY $STEREO_AUDIO_FILE"11025.wav" &
while [ $GAIN -le 18 ]
do
	$AMIXER cset numid=7 $GAIN
	sleep 1
	GAIN=`expr $GAIN + 1`
done
