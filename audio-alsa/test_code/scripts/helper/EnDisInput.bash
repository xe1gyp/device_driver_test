#!/bin/bash

function Usage
{
	echo "EnDisInput.bash INPUT_SOURCE"
	echo "INPUT_SOURCE:"
	echo " Mic_Headset"
	echo " Mic_Main-Mic_Sub"
	echo " Aux/FM"
	echo " USB-CarKit"
	echo "CAPTURE:"
	echo " Capture - Capture an audio file"
	echo " NoCapture - Only change the input control"
}

function Parser
{
	if [ $# -eq 2 ]; then
		INPUT_SOURCE="$1"
		CAPTURE="$2"
	else
		Usage
		exit 1
	fi
}

function UpdateErrorCount
{
	errors=`echo $errors+1|bc`
	echo "Error found @ $TESTSCRIPT/EnDisInput.bash"
}

function PrintHeader
{
        echo -e "\n********************************************************************************"
        if [ $# -eq 5 ]; then
                echo " Enable/Disable Input Source "
                echo " Input Source: $1 "
		echo " Input Volume: $2 "
		echo " Output Source: $3 "
		echo " Output Volume: $4 "
                echo " Audio file: $5 "
        else
                echo "Incorrect number of arguments"
                exit 1
        fi
        echo -e  "********************************************************************************"
}

function RecordFile
{
        local Channels=$1
        local Rate=$2
        local Time=$3
	local File=$4

        case $Channels in
                'Mono')         File=$TEST_RECORD_MONO_FILE; Chan=1;;
                'Stereo')       File=$TEST_RECORD_STEREO_FILE; Chan=2;;
                *)              echo "Incorrect type of audio file";;
        esac

	if [ "$CAPTURE" == "Capture" ]; then
	        arecord -f S16_LE -c $Chan -r $Rate -d $Time $File || UpdateErrorCount
	fi
}

function PlayFile
{
	local PlayFile=$1
        if [ "$CAPTURE" == "Capture" ]; then
                aplay $PlayFile || UpdateErrorCount
        fi
}

# ==================
#       Main
# ==================

errors=0
export CAPTURE=""

export INPUT_VOLUME=$DEFAULT_INPUT_VOLUME
CAPTURE_TIME=$DEFAULT_CAPTURE_TIME
CAPTURE_RATE=$DEFAULT_CAPTURE_RATE
CAPTURE_CHANNELS=$DEFAULT_CAPTURE_CHANNELS

OUTPUT_SOURCE=$DEFAULT_OUTPUT_SOURCE
OUTPUT_VOLUME=$DEFAULT_OUTPUT_VOLUME

# Parse input arguments
Parser $@

# Set Input Source/Playback Switch
PrintHeader $INPUT_SOURCE $INPUT_VOLUME $OUTPUT_SOURCE $OUTPUT_VOLUME "$TEST_RECORD_STEREO_FILE"
$TESTSCRIPT/ChangeInputVolume.bash $INPUT_SOURCE $INPUT_VOLUME && \
$TESTSCRIPT/SetSwitch.bash $INPUT_SOURCE && \
RecordFile $CAPTURE_CHANNELS $CAPTURE_RATE $CAPTURE_TIME $TEST_RECORD_STEREO_FILE && \
$TESTSCRIPT/ChangeOutputVolume.bash $OUTPUT_SOURCE $OUTPUT_VOLUME && \
PlayFile $TEST_RECORD_STEREO_FILE && \
rm -f $TEST_RECORD_STEREO_FILE || \
UpdateErrorCount

exit $errors
