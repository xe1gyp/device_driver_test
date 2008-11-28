#!/bin/bash

function Usage
{
	echo "MultiplePlayback.bash"
}

function Parser
{
	if [ $# -ne 0 ]; then
		Usage
		exit 1
	fi
}

function UpdateErrorCount
{
	errors=`echo $errors+1|bc`
	echo "Error found @ $TESTSCRIPT/MultiplePlayback.bash"
}

function PrintHeader
{
        echo -e "\n********************************************************************************"
        if [ $# -eq 4 ]; then
                echo " Playback Multiple Streams "
                echo " Output Source: $1 "
                echo " Output Volume: $2 "
                echo " Audio file 1: $3 "
                echo " Audio file 2: $4 "
        else
                echo "Incorrect number of arguments"
                exit 1
        fi
        echo -e  "********************************************************************************"
}

function WaitEndOfAudioFile
{
	local Match="Yes"
	echo "Waiting for audio file ends..."
	while [ "$Match" != "No" ]; do
		ps > $TMPBASE/ps_aplay
		cat $TMPBASE/ps_aplay | grep aplay || Match="No"
		rm $TMPBASE/ps_aplay
		sleep 1
	done
	echo "Audio file have finished!"
}

function PlayMultipleFiles
{
	local File1=$1
	local File2=$2
	local Delay=$3
	local Playback=$4

	if [ "$Playback" == "Playback" ]; then
		aplay $File1 &
	        sleep $Delay
		aplay $File2 || UpdateErrorCount
	        WaitEndOfAudioFile
	fi
}

function TestMultipleStreams
{
	local OutputSource=$1
	local OutputVolume=$2
	local File1=$3
	local File2=$4
	local Delay=$5
	local Playback=$6

	PrintHeader $OutputSource $OutputVolume $File1 $File2
	$TESTSCRIPT/ChangeOutputVolume.bash $OutputSource $OutputVolume && \
	PlayMultipleFiles $File1 $File2 $Delay $Playback || \
	UpdateErrorCount
}

# ==================
# Main
# ==================

export errors=0

AUDIO_FILE_1=""$STEREO_AUDIO_FILE"48000.wav"
AUDIO_FILE_2=""$STEREO_AUDIO_FILE"8000.wav"
OUTPUT_VOLUME=$DEFAULT_OUTPUT_VOLUME
DELAY=3

# Parse input arguments
Parser $@

OUTPUT_SOURCE="Stereo_Headset"
PLAYBACK="Playback"
TestMultipleStreams $OUTPUT_SOURCE $OUTPUT_VOLUME $AUDIO_FILE_1 $AUDIO_FILE_2 $DELAY $PLAYBACK

OUTPUT_SOURCE="Hands-Free"
PLAYBACK="Playback"
TestMultipleStreams $OUTPUT_SOURCE $OUTPUT_VOLUME $AUDIO_FILE_1 $AUDIO_FILE_2 $DELAY $PLAYBACK

OUTPUT_SOURCE="Mono_Handset"
PLAYBACK="Playback"
TestMultipleStreams $OUTPUT_SOURCE $OUTPUT_VOLUME $AUDIO_FILE_1 $AUDIO_FILE_2 $DELAY $PLAYBACK

#OUTPUT_SOURCE="USB-CarKit"
#PLAYBACK="NoPlayback"
#TestMultipleStreams $OUTPUT_SOURCE $OUTPUT_VOLUME $AUDIO_FILE_1 $AUDIO_FILE_2 $DELAY $PLAYBACK

exit $errors
