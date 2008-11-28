#!/bin/bash

function Usage
{
	echo "OutputVolume.bash"
	echo "Change volume settings for output controls"
}

function PrintHeader
{
        echo -e "\n********************************************************************************"
        if [ $# -eq 3 ]; then
                echo " Change Output Volume "
                echo " Output Source: $1 "
                echo " Output Volume: $2 "
                echo " Audio file: $3 "
	elif [ $# -eq 4 ]; then
		echo " Change Output Volume "
		echo " Output Source: $1 "
		echo " Left Output Volume: $2 "
		echo " Right Output Volume: $3 "
		echo " Audio file: $4 "
        else
                echo "Incorrect number of arguments"
                exit 1
        fi
        echo -e  "********************************************************************************"
}

function UpdateErrorCount
{
	errors=`echo $errors+1|bc`
	echo "Error found @ $TESTSCRIPT/OutputVolume.bash"
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
		*)		echo "Incorrect type of audio file";;
	esac

	for OutputVolume in $OutputVolumes; do
		PrintHeader $OutputSource $OutputVolume $File
		$TESTSCRIPT/ChangeOutputVolume.bash $OutputSource $OutputVolume && \
		PlayFile $File $Playback || \
		UpdateErrorCount
	done
}

function VolumeLR
{
	local OutputSource=$1
	local OutputVolumesL=$2
	local OutputVolumesR=$3
	local Channels=$4
	local Playback=$5

	case $Channels in
		'Mono')         File=$GENERIC_MONO_AUDIO_FILE;;
		'Stereo')       File=$GENERIC_STEREO_AUDIO_FILE;;
		*)              echo "Incorrect type of audio file";;
	esac

	for OutputVolumeL in $OutputVolumesL; do
		for OutputVolumeR in $OutputVolumesR; do
			PrintHeader $OutputSource $OutputVolumeL $OutputVolumeR $File
			$TESTSCRIPT/ChangeOutputVolumeLR.bash $OutputSource $OutputVolumeL $OutputVolumeR && \
			PlayFile $File $Playback || \
			UpdateErrorCount
		done
	done
}

# ==================
#       Main
# ==================

export errors=0

OUTPUT_SOURCE="Master"
OUTPUT_VOLUMES="0 100"
OUTPUT_VOLUMES_L="0 100"
OUTPUT_VOLUMES_R="0 100"
CHANNELS="Stereo"
PLAYBACK="Playback"
Volume $OUTPUT_SOURCE "$OUTPUT_VOLUMES" $CHANNELS $PLAYBACK
VolumeLR $OUTPUT_SOURCE "$OUTPUT_VOLUMES_L" "$OUTPUT_VOLUMES_R" $CHANNELS $PLAYBACK

OUTPUT_SOURCE="Stereo_Headset"
OUTPUT_VOLUMES="0 100"
OUTPUT_VOLUMES_L="0 100"
OUTPUT_VOLUMES_R="0 100"
CHANNELS="Stereo"
PLAYBACK="Playback"
Volume $OUTPUT_SOURCE "$OUTPUT_VOLUMES" $CHANNELS $PLAYBACK
VolumeLR $OUTPUT_SOURCE "$OUTPUT_VOLUMES_L" "$OUTPUT_VOLUMES_R" $CHANNELS $PLAYBACK

OUTPUT_SOURCE="Hands-Free"
OUTPUT_VOLUMES="0 100"
OUTPUT_VOLUMES_L="0 100"
OUTPUT_VOLUMES_R="0 100"
CHANNELS="Stereo"
PLAYBACK="Playback"
Volume $OUTPUT_SOURCE "$OUTPUT_VOLUMES" $CHANNELS $PLAYBACK
VolumeLR $OUTPUT_SOURCE "$OUTPUT_VOLUMES_L" "$OUTPUT_VOLUMES_R" $CHANNELS $PLAYBACK

OUTPUT_SOURCE="Mono_Handset"
OUTPUT_VOLUMES="0 100"
CHANNELS="Mono"
PLAYBACK="Playback"
Volume $OUTPUT_SOURCE "$OUTPUT_VOLUMES" $CHANNELS $PLAYBACK

OUTPUT_SOURCE="USB-CarKit"
OUTPUT_VOLUMES="0 100"
OUTPUT_VOLUMES_L="0 100"
OUTPUT_VOLUMES_R="0 100"
CHANNELS="Stereo"
PLAYBACK="NoPlayback"
Volume $OUTPUT_SOURCE "$OUTPUT_VOLUMES" $CHANNELS $PLAYBACK
VolumeLR $OUTPUT_SOURCE "$OUTPUT_VOLUMES_L" "$OUTPUT_VOLUMES_R" $CHANNELS $PLAYBACK

exit $errors
