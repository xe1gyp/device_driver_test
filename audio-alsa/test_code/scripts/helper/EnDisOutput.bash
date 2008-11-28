#!/bin/bash

function Usage
{
	echo "EnDisOutput.bash OUTPUT_SOURCE PLAYBACK"
	echo "OUTPUT_SOURCE:"
	echo " 0 - Stereo_Headset"
	echo " 1 - Hands-Free"
	echo " 2 - Mono_Handset"
	echo " 3 - USB-CarKit"
	echo "PLAYBACK:"
	echo " Playback - Play an audio file"
	echo " NoPlayback - Only change the output source"
}

function Parser
{
	if [ $# -eq 2 ]; then
		OUTPUT_SOURCE="$1"
		PLAYBACK=$2
	else
		Usage
		exit 1
	fi
}

function UpdateErrorCount
{
	errors=`echo $errors+1|bc`
	echo "Error found @ $TESTSCRIPT/EnDisOutput.bash"
}

function PrintHeader
{
        echo -e "\n********************************************************************************"
        if [ $# -eq 2 ]; then
                echo " Enable/Disable Output Source "
                echo " Output Source: $1 "
                echo " Audio file: $2 "
        else
                echo "Incorrect number of arguments"
                exit 1
        fi
        echo -e  "********************************************************************************"
}


function PlayFile
{
	local PlayFile=$1
	if [ "$PLAYBACK" == "Playback" ]; then
	        aplay $PlayFile || UpdateErrorCount
	fi
}


# ==================
#       Main
# ==================

export errors=0
export PLAYBACK=""

export OUTPUT_VOLUME=$DEFAULT_OUTPUT_VOLUME

# Parse input arguments
Parser $*

# Set Output Source/Playback Switch
PrintHeader $OUTPUT_SOURCE "$MONO_AUDIO_FILE"
$TESTSCRIPT/ChangeOutputVolume.bash $OUTPUT_SOURCE $OUTPUT_VOLUME && \
$TESTSCRIPT/SetSwitch.bash $OUTPUT_SOURCE && \
PlayFile $MONO_AUDIO_FILE || \
UpdateErrorCount

exit $errors
