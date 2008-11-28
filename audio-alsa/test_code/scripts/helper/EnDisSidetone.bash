#!/bin/bash

function Usage
{
	echo "EnDisSidetone.bash"
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
	echo "Error found @ $TESTSCRIPT/EnDisSidetone.bash"
}

function PrintHeader
{
        echo -e "\n********************************************************************************"
        if [ $# -eq 3 ]; then
                echo " Enable/Disable Sidetone "
		echo " Output Source: $1 "
		echo " Output Volume: $2 "
		echo " Audio file: $3 "
        else
                echo "Incorrect number of arguments"
                exit 1
        fi
        echo -e  "********************************************************************************"
}

function PlayFile
{
	local File=$1
	local Playback=$2

        if [ "$Playback" == "Playback" ]; then
                aplay $File || UpdateErrorCount
        fi
}

function Volume
{
	local OutputSource=$1
	local OutputVolumes=$2
	local Channels=$3
	local Playback=$4

        case $Channels in
                'Mono')         File=$GENERIC_MONO_AUDIO_FILE;;
                'Stereo')       File=$GENERIC_STEREO_AUDIO_FILE;;
                *)              echo "Incorrect type of audio file";;
        esac

        for OutputVolume in $OutputVolumes; do
                PrintHeader $OutputSource $OutputVolume $File
                $TESTSCRIPT/ChangeOutputVolume.bash $OutputSource $OutputVolume && \
                PlayFile $File $Playback || \
                UpdateErrorCount
        done
}

# ==================
#       Main
# ==================

export errors=0

OUTPUT_SOURCE="Sidetone"
OUTPUT_VOLUMES="0 41"
CHANNELS="Mono"
PLAYBACK='NoPlayback'

# Parse input arguments
Parser $*

# Set Output Source/Playback Switch
Volume $OUTPUT_SOURCE "$OUTPUT_VOLUMES" $CHANNELS $PLAYBACK && \
$TESTSCRIPT/SetSwitch.bash "Sidetone" || \
UpdateErrorCount

exit $errors
