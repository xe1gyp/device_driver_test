#!/bin/sh

GAIN=0

aplay $STEREO_AUDIO_FILE"11025.wav" &
while [ $GAIN -le 18 ]
do
	amixer cset numid=7 $GAIN
	sleep 1
	GAIN=`expr $GAIN + 1`
done
