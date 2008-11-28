#!/bin/bash

function Usage
{
	echo "RecordGain.bash"
}

function Parser
{
	if [ $# -eq 1 ]; then
		PM_OPTION="$1"
	else
		Usage
		exit 1
	fi
}

function UpdateErrorCount
{
	errors=`echo $errors+1|bc`
	echo "Error found @ $TESTSCRIPT/CaptureGain.bash"
}

function PrintHeader
{
	echo -e "\n******************************************************************************"
	if [ $# -eq 7 ]; then
		echo " Gain during capture "
		echo " Input Source: $1 "
		echo " Input Volume: $2 "
		echo " Output Source: $3 "
		echo " Output Volume: $4 "
		echo " Channels: $5 "
		echo " Sample Rate: $6 "
		echo " Audio File: $7 "
	else
		echo "Incorrect number of arguments"
		exit 1
	fi
	echo -e  "******************************************************************************"
}

function WaitEndOfAudioFile
{
        local Match="Yes"
        local VAL_0=20
        local VAL=100
        echo "Waiting for audio file end..."
        while [ "$Match" != "No" ]; do
                ps > $TMPBASE/ps_arecord
                cat $TMPBASE/ps_arecord | grep arecord || Match="No"
                rm $TMPBASE/ps_arecord
                if [ $VAL == 100 ]; then
                        VAL=$VAL_0;
                else
                        VAL=$(echo $VAL+20|bc);
                fi
                echo "**************************** Changing gain to: $VAL ****************************"
                $TESTBIN/test_gain 30 $VAL
                sleep 1
        done
        echo "Audio file have ended!"
}

function RecordFile
{
	local Channels=$1
	local Rate=$2
	local Time=$3
	local RecordFile=$4

	case $Channels in
		'Mono')		Chan=1;;
                'Stereo')	Chan=2;;
                *)              echo "Incorrect type of audio file";;
        esac
	
	arecord -f S16_LE -c $Chan -r $Rate -d $Time $RecordFile &
	WaitEndOfAudioFile
}

function PlayFile
{
	local PlayFile=$1
	aplay $PlayFile
}


# ==================
# Main
# ==================

export errors=0

INPUT_SOURCE=$DEFAULT_INPUT_SOURCE
INPUT_VOLUME=$DEFAULT_INPUT_VOLUME
OUTPUT_SOURCE=$DEFAULT_OUTPUT_SOURCE
OUTPUT_VOLUME=$DEFAULT_OUTPUT_VOLUME

CAPTURE_TIME=$DEFAULT_CAPTURE_TIME

RECORD_MONO_FILE=$TEST_RECORD_MONO_FILE
RECORD_MONO_SAMPLE_RATE=48000

RECORD_STEREO_FILE=$TEST_RECORD_STEREO_FILE
RECORD_STEREO_SAMPLE_RATE=48000

# Mono test
PrintHeader $INPUT_SOURCE $INPUT_VOLUME $OUTPUT_SOURCE $OUTPUT_VOLUME "Mono" $RECORD_MONO_SAMPLE_RATE $TEST_RECORD_MONO_FILE
$TESTSCRIPT/ChangeInputVolume.bash $INPUT_SOURCE $INPUT_VOLUME && \
$TESTSCRIPT/ChangeOutputVolume.bash $OUTPUT_SOURCE $OUTPUT_VOLUME && \
RecordFile "Mono" $RECORD_MONO_SAMPLE_RATE $CAPTURE_TIME $RECORD_MONO_FILE && \
PlayFile $RECORD_MONO_FILE || \
UpdateErrorCount

# Stereo test
PrintHeader $INPUT_SOURCE $INPUT_VOLUME $OUTPUT_SOURCE $OUTPUT_VOLUME "Stereo" $RECORD_STEREO_SAMPLE_RATE $TEST_RECORD_STEREO_FILE
$TESTSCRIPT/ChangeInputVolume.bash $INPUT_SOURCE $INPUT_VOLUME && \
$TESTSCRIPT/ChangeOutputVolume.bash $OUTPUT_SOURCE $OUTPUT_VOLUME && \
RecordFile "Stereo" $RECORD_STEREO_SAMPLE_RATE $CAPTURE_TIME $RECORD_STEREO_FILE && \
PlayFile $RECORD_STEREO_FILE || \
UpdateErrorCount

exit $errors
