#!/bin/bash
# common info
export HOMEDIR=`dirname $0`
. $HOMEDIR/include.common

if [ -z "$SAMPLE_RATES" ]; then
	SAMPLE_RATES=$ALL_SAMPLE_RATES
fi
if [ -z "$BIT_SIZE" ]; then
	BIT_SIZE=$ALL_BIT_SIZE
fi
if [ -z "$MONO_SUPPORT" ]; then
	MONO_SUPPORT=$ALL_MONO_SUPPORT
fi
if [ -z "$STEREO_DIR_SUPPORT" ]; then
	STEREO_DIR_SUPPORT=$ALL_STEREO_DIR_SUPPORT
fi
if [ -z "$OUTPUT_DEVICES" ]; then
	OUTPUT_DEVICES=$ALL_OUTPUT_DEVICES
fi
if [ -z "$POWER_MANAGEMENT" ]; then
	POWER_MANAGEMENT=$ALL_POWER_MANAGEMENT
fi
if [ -z "$BUFFER_SIZES" ]; then
	BUFFER_SIZES=$ALL_BUFFER_SIZES
fi
echo "TEST PARAMS:"
echo "SAMPLE_RATES=$SAMPLE_RATES"
echo "BIT_SIZE=$BIT_SIZE"
echo "MONO_SUPPORT=$MONO_SUPPORT"
echo "STEREO_DIR_SUPPORT=$STEREO_DIR_SUPPORT"
echo "OUTPUT_DEVICES=$OUTPUT_DEVICES"
COUNT=0
# $1 is rate
# $2 bit support
# $3 mono/stereo
# $4 left/right
function run_myoptions
{
	#FILE_NAME=`echo "sin-$SIN_FREQ-$1-$2-$3-$4"|tr - _`
	FILE_NAME=`echo "sin_1000_$SIN_FREQ_$1_$2_$3_$4"`
	OPT="-f $1"
	if [ "$2" = "24" ]; then
		OPT="$OPT -s 32"
	else
		OPT="$OPT -s 16"
	fi
	if [ "$3" = "mono" ]; then
		OPT="$OPT -m 1"
	else
		OPT="$OPT -m 2"
	fi

	export DPM_FILE="$TMPBASE/TMP_DPM_FILE.$$"
	if [ "$POWER_MANAGEMENT" = "sleep" ]; then
		OPT_DPM="$DPM_SLEEP_OPTION"
	elif [ "$POWER_MANAGEMENT" = "scale" ]; then
		OPT_DPM="$DPM_SCALE_OPTION"
	fi
	for buffer in $BUFFER_SIZES
	do
		if [ ! -z "$OPT_DPM" ]; then
			echo "DPM_POWER.. sleep 2 sec and then start"
			(touch $DPM_FILE;sleep 2;echo -e "$OPT_DPM\n$DPM_QUIT_OPTION\n"|$DPM_TEST;rm $DPM_FILE) &
		fi
		echo "$PLAYER $PLAYER_GENERIC_OPTION $OPT -i $RAWDIR/$FILE_NAME -b $buffer"
		$PLAYER $PLAYER_GENERIC_OPTION $OPT -i $RAWDIR/$FILE_NAME -b $buffer
		COUNT=`expr $COUNT + 1`
		ERR=$?
		if [ $ERR -ne 0 ]; then
			echo "Return Error - $ERR-Check log args for params"
			rm -f $DPM_FILE
			exit $ERR
		fi
		# check out the dpm file
		if [ ! -z "$OPT_DPM" ]; then
			while [ -f "$DPM_FILE" ]
			do
				echo "Waiting for DPM Test completion"
				sleep 1
			done
		fi
	done
}

for source in $OUTPUT_DEVICES
do
	$CHANGER $CHANGER_GENERIC_OPTION -o $source
	for rate in $SAMPLE_RATES 
	do
		for bit in $BIT_SIZE
		do
			#for mono in stereo mono
			for mono in $MONO_SUPPORT
			do
				if [ "$mono" = "mono" ]; then
					run_myoptions $rate $bit $mono m
				else
					for dir in $STEREO_DIR_SUPPORT
					do
						run_myoptions $rate $bit $mono $dir
						
					done
				fi
			done
		done
	done
done
echo RAN-$COUNT Instances - all pass
exit 0
