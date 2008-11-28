#!/bin/bash

function Usage
{
	echo "Playback.bash PM_OPTION"
	echo "PM_OPTION - Power management option: "
	echo " 'Sleep' - Deep sleep"
	echo " 'FreqScaling' - Frequency Scaling"
	echo " 'NoPMDPM' - No PM/DPM tests"
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
	echo "Error found @ $TESTSCRIPT/Playback.bash"
}

function PrintHeader
{
        echo -e "\n********************************************************************************"
        if [ $# -eq 4 ]; then
                echo " Playback Support "
                echo " Output Source: $1 "
                echo " Output Volume: $2 "
                echo " Sample Rate: $3 "
                echo " Audio file: $4 "
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

function PlayFile
{
	local File=$1
	local Playback=$2

	# Run the PM/DPM options
	if [ "$Playback" == "Playback" ]; then
		if [ "$PM_OPTION" == "Sleep" ]; then
			(aplay $File &) && sleep 5 && echo -e "$SLEEP_OPTION\n$QUIT_OPTION" | $DPM_SCRIPT && WaitEndOfAudioFile
		elif [ "$PM_OPTION" == "FreqScaling" ]; then
			(aplay $File &) && echo -e "$SCALE_OPTION\n$QUIT_OPTION" | $DPM_SCRIPT && WaitEndOfAudioFile
		elif [ "$PM_OPTION" == "NoPMDPM" ]; then
			aplay $File || UpdateErrorCount
		fi
	fi
}

function TestSampleRates
{
	local OutputSource=$1
	local AudioPrefix=$2
	local SampleRates=$3
	local Volumes=$4
	local Playback=$5

	for SampleRate in $SampleRates; do
		AudioFile="$AudioPrefix$SampleRate.wav"
		for Volume in $Volumes; do
			PrintHeader $OutputSource $Volume $SampleRate $AudioFile
			$TESTSCRIPT/ChangeOutputVolume.bash $OutputSource $Volume && \
			PlayFile $AudioFile $Playback || \
			UpdateErrorCount
		done
	done
}


# ==================
# Main
# ==================

export errors=0

SAMPLE_RATES="48000 44100 32000 24000 22050 16000 12000 11025 8000"
#SAMPLE_RATES="48000"
OUTPUT_VOLUMES="0 100"
AUDIO_FILE_PREFIX=$STEREO_AUDIO_FILE

# Parse input arguments
Parser $@

OUTPUT_SOURCE="Stereo_Headset"
PLAYBACK="Playback"
TestSampleRates $OUTPUT_SOURCE $AUDIO_FILE_PREFIX "$SAMPLE_RATES" "$OUTPUT_VOLUMES" $PLAYBACK

OUTPUT_SOURCE="Hands-Free"
PLAYBACK="Playback"
TestSampleRates $OUTPUT_SOURCE $AUDIO_FILE_PREFIX "$SAMPLE_RATES" "$OUTPUT_VOLUMES" $PLAYBACK

OUTPUT_SOURCE="Mono_Handset"
PLAYBACK="Playback"
TestSampleRates $OUTPUT_SOURCE $AUDIO_FILE_PREFIX "$SAMPLE_RATES" "$OUTPUT_VOLUMES" $PLAYBACK

#OUTPUT_SOURCE="USB-CarKit"
#PLAYBACK="NoPlayback"
#TestSampleRates $OUTPUT_SOURCE $AUDIO_FILE_PREFIX "$SAMPLE_RATES" "$OUTPUT_VOLUMES" $PLAYBACK

exit $errors

