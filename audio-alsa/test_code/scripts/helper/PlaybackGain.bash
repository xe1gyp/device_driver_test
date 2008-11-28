#!/bin/bash

function Usage
{
	echo "PlaybackGain.bash"
}

function Parser
{
	if [ $# != 0 ]; then
		Usage
		exit 1
	fi
}

function UpdateErrorCount
{
	errors=`echo $errors+1|bc`
	echo "Error found @ $TESTSCRIPT/PlaybackGain.bash"
}

function PrintHeader
{
	echo -e "\n********************************************************************************"
	if [ $# -eq 5 ]; then
		echo " Gain during playback "
		echo " Output Source: $1 "
		echo " Output Volume: $2 "
		echo " Channels: $3 "
		echo " Sample Rate: $4 "
		echo " Audio File: $5 "
	else
		echo "Incorrect number of arguments"
		exit 1
	fi
	echo -e  "********************************************************************************"
}

function WaitEndOfAudioFile
{
	local Match="Yes"
	local VAL_0=30
	local VAL=60
	echo "Waiting for audio file end..."
	while [ "$Match" != "No" ]; do
		ps > $TMPBASE/ps_aplay
		cat $TMPBASE/ps_aplay | grep aplay || Match="No"
		rm $TMPBASE/ps_aplay
		if [ $VAL == 100 ]; then
			VAL=$VAL_0;
		else
			VAL=$(echo $VAL+10|bc);
		fi
		echo "**************************** Changing gain to: $VAL ****************************"
		$TESTBIN/test_gain 0 $VAL
		sleep 1
	done
	echo "Audio file have ended!"
}

function PlayFile
{
	local PlayFile=$1

	aplay $PlayFile &
	WaitEndOfAudioFile
}

# ==================
# Main
# ==================

export errors=0

OUTPUT_SOURCE=$DEFAULT_OUTPUT_SOURCE
OUTPUT_VOLUME=$DEFAULT_OUTPUT_VOLUME
PLAYBACK_CHANNELS=$DEFAULT_PLAYBACK_CHANNELS

MONO_AUDIO_FILE=$GENERIC_MONO_AUDIO_FILE
PLAYBACK_MONO_SAMPLE_RATE=$GENERIC_MONO_AUDIO_FILE

STEREO_AUDIO_FILE=$GENERIC_STEREO_AUDIO_FILE
PLAYBACK_STEREO_SAMPLE_RATE=$GENERIC_STEREO_SAMPLE_RATE

# Mono test
PrintHeader $OUTPUT_SOURCE $OUTPUT_VOLUME "Mono" $PLAYBACK_MONO_SAMPLE_RATE $MONO_AUDIO_FILE
$TESTSCRIPT/ChangeOutputVolume.bash $OUTPUT_SOURCE $OUTPUT_VOLUME && \
PlayFile $GENERIC_MONO_AUDIO_FILE || \
UpdateErrorCount

# Stereo test
PrintHeader $OUTPUT_SOURCE $OUTPUT_VOLUME "Stereo" $PLAYBACK_STEREO_SAMPLE_RATE $STEREO_AUDIO_FILE
$TESTSCRIPT/ChangeOutputVolume.bash $OUTPUT_SOURCE $OUTPUT_VOLUME && \
PlayFile $GENERIC_STEREO_AUDIO_FILE || \
UpdateErrorCount

exit $errors
