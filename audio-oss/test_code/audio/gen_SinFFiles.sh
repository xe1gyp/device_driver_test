#!/bin/sh
SAMPLE_RATES="48000 44100 32000 24000 22050 16000 12000 11025 8000"
SINGEN=../bin/sineGen_i386
SIN_FREQ=1000
DURATION=5
GEN_OPTS="-s $SIN_FREQ -t $DURATION"
RAW_FILE_LOC=../scripts/raw_sin/
# $1 is rate
# $2 bit support
# $3 mono/stereo
# $4 left/right
get_myoptions() {
	FILE_NAME=`echo "sin-$SIN_FREQ-$1-$2-$3-$4"|tr - _`
	OPT="-f $1"
	if [ "$2" = "24" ]; then
		OPT="$OPT -24"
	fi
	if [ "$3" = "mono" ]; then
		OPT="$OPT -m"
	else
		if [ "$4" = "l" ]; then
			OPT="$OPT -z 1"
		elif [ "$4" = "r" ]; then
			OPT="$OPT -z 0"
		fi
	fi
	echo "Generating $FILE_NAME with opts $OPT"
	$SINGEN $GEN_OPTS $OPT -o $RAW_FILE_LOC/$FILE_NAME
}
for rate in $SAMPLE_RATES 
do
	for bit in 24 16
	do
		for mono in stereo mono
		do
			if [ "$mono" = "mono" ]; then
				get_myoptions $rate $bit $mono m
			else
				for dir in s l r
				do
					get_myoptions $rate $bit $mono $dir
				done
			fi
		done
	done
done

