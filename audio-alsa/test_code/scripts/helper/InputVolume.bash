#!/bin/bash

function Usage
{
	echo "InputVolume.bash"
	echo "Change volume settings for input controls"
}

function PrintHeader
{
	echo -e "\n********************************************************************************"
	if [ $# -eq 5 ]; then
		echo " Change Input Volume "
		echo " Input Source: $1 "
		echo " Input Volume: $2 "
		echo " Output Source: $3 "
		echo " Output Volume: $4 "
		echo " Audio file: $5 "
	elif [ $# -eq 6 ]; then
		echo " Change Input Volume "
		echo " Input Source: $1 "
		echo " Left Input Volume: $2 "
		echo " Right Input Volume: $3 "
		echo " Output Source: $4 "
		echo " Output Volume: $5 "
		echo " Audio file: $6 "
	else
		echo "Incorrect number of arguments"
		exit 1
	fi
	echo -e  "********************************************************************************"
}

function UpdateErrorCount
{
        errors=`echo $errors+1|bc`
        echo "Error found @ $TESTSCRIPT/InputVolume.bash"
}

function RecordFile
{
	local Channels=$1
	local Rate=$2
	local Time=$3
	local File=$4
	local Capture=$5

	if [ "$Capture" == "Capture" ]; then
		arecord -f S16_LE -c $Channels -r $Rate -d $Time $File || UpdateErrorCount
	fi
}

function PlayFile
{
	local PlayFile=$1
	local Capture=$2
	if [ "$Capture" == "Capture" ]; then
		aplay $PlayFile || UpdateErrorCount
	fi
}

function Volume
{
	local InputSource=$1
	local InputVolumes=$2
	local Channels=$3
	local Capture=$4

	case $Channels in
		'Mono')		File=$TEST_RECORD_MONO_FILE; Chan=1;;
		'Stereo')	File=$TEST_RECORD_STEREO_FILE; Chan=2;;
		*)		echo "Incorrect type of audio file";;
	esac

	for InputVolume in $InputVolumes; do
		PrintHeader $InputSource $InputVolume $OUTPUT_SOURCE $OUTPUT_VOLUME $File
		$TESTSCRIPT/ChangeInputVolume.bash $InputSource $InputVolume && \
		RecordFile $Chan $CAPTURE_RATE $CAPTURE_TIME $File $Capture && \
		PlayFile $File $Capture || \
		UpdateErrorCount
	done
}

function VolumeLR
{
	local InputSource=$1
	local InputVolumesL=$2
	local InputVolumesR=$3
	local Channels=$4
	local Playback=$5

	case $Channels in
		'Mono')		File=$TEST_RECORD_MONO_FILE; Chan=1;;
		'Stereo')	File=$TEST_RECORD_STEREO_FILE; Chan=2;;
		*)		echo "Incorrect type of audio file";;
	esac

	for InputVolumeL in $InputVolumesL; do
		for InputVolumeR in $InputVolumesR; do
			PrintHeader $InputSource $InputVolumeL $InputVolumeR $OUTPUT_SOURCE $OUTPUT_VOLUME $File
			$TESTSCRIPT/ChangeInputVolumeLR.bash $InputSource $InputVolumeL $InputVolumeR && \
			RecordFile $Chan $CAPTURE_RATE $CAPTURE_TIME $File $Capture && \
			PlayFile $File $Capture || \
			UpdateErrorCount
		done
	done
}


# ==================
# Main
# ==================

export errors=0

export CAPTURE_RATE=$DEFAULT_CAPTURE_RATE
export CAPTURE_TIME=$DEFAULT_CAPTURE_TIME

export OUTPUT_SOURCE=$DEFAULT_OUTPUT_SOURCE
export OUTPUT_VOLUME=$DEFAULT_OUTPUT_VOLUME
$TESTSCRIPT/ChangeOutputVolume.bash $OUTPUT_SOURCE $OUTPUT_VOLUME

INPUT_SOURCE="Master"
INPUT_VOLUMES="0 100"
INPUT_VOLUMES_L="100"
INPUT_VOLUMES_R="100"
CHANNELS="Stereo"
CAPTURE="Capture"
Volume $INPUT_SOURCE "$INPUT_VOLUMES" $CHANNELS $CAPTURE
VolumeLR $INPUT_SOURCE "$INPUT_VOLUMES_L" "$INPUT_VOLUMES_R" $CHANNELS $CAPTURE

INPUT_SOURCE="Mic_Headset"
INPUT_VOLUMES="0 100"
CHANNELS="Mono"
CAPTURE="Capture"
Volume $INPUT_SOURCE "$INPUT_VOLUMES" $CHANNELS $CAPTURE

INPUT_SOURCE="Mic_Main-Mic_Sub"
INPUT_VOLUMES="0 100"
CHANNELS="Mono"
CAPTURE="Capture"
Volume $INPUT_SOURCE "$INPUT_VOLUMES" $CHANNELS $CAPTURE

INPUT_SOURCE="Aux/FM"
INPUT_VOLUMES="0 100"
INPUT_VOLUMES_L="0 100"
INPUT_VOLUMES_R="0 100"
CHANNELS="Stereo"
CAPTURE="NoCapture"
Volume $INPUT_SOURCE "$INPUT_VOLUMES" $CHANNELS $CAPTURE
VolumeLR $INPUT_SOURCE "$INPUT_VOLUMES_L" "$INPUT_VOLUMES_R" $CHANNELS $CAPTURE

INPUT_SOURCE="USB-CarKit"
INPUT_VOLUMES="0 100"
CHANNELS="Mono"
CAPTURE="NoCapture"
Volume $INPUT_SOURCE "$INPUT_VOLUMES" $CHANNELS $CAPTURE

exit $errors
