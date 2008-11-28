#!/bin/bash

function Usage
{
	echo "PauseResume.bash"
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
	echo "Error found @ $TESTSCRIPT/PauseResume.bash"
}

function PrintHeader
{
	echo -e "\n********************************************************************************"
	if [ $# -eq 5 ]; then
		echo " Pause/Resume "
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

function PlayFile
{
	local PlayFile=$1
	local CommandSeq="p \n r \n p \n r \n p \n r \n p \n r \n p \n r \n p \n r \n p \n r \n p \n r \n p \n r \n p \n r \n p \n r \n"

	echo -r "$CommandSeq" | $TESTBIN/aplay-pr $PlayFile
}

# ==================
# Main
# ==================

export errors=0

OUTPUT_SOURCE=$DEFAULT_OUTPUT_SOURCE
OUTPUT_VOLUME=$DEFAULT_OUTPUT_VOLUME

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
