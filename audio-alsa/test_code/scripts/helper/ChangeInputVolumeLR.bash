#!/bin/bash

function Parser
{
	if [ $# -eq 3 ]; then
		export INPUTSOURCE=$1
		export VOLUME_L=$2
		export VOLUME_R=$3
	else
		echo "Usage: ChangeInputVolumeLR.bash INPUTSOURCE VOLUME_LEFT VOLUME_RIGHT"
		echo "INPUTSOURCE:"
		echo " Master"
		echo " Stereo_Headset"
		echo " Hands-Free"
		echo " USB-CarKit"
		echo "VOLUME_LEFT:"
		echo " [0 ... 100]"
		echo "VOLUME_RIGHT:"
		echo " [0 ... 100]"
		exit 1
	fi
}

function UpdateErrorCount
{
	errors=`echo $errors+1|bc`
	echo "Error found @ $TESTSCRIPT/ChangeInputVolumeLR.bash"
}

function ChangeInputVolumeLR
{
	local InputSource=$1
	local VolumeL=$2
	local VolumeR=$3

	case $InputSource in
		'Master')		$TESTSCRIPT/SetInputSource.bash 'Mic_Main-Mic_Sub' && \
					amixer cset name='Master Capture Volume' $VolumeL,$VolumeR && \
					amixer cget name='Master Capture Volume' | grep "values=$VolumeL,$VolumeR" || \
					UpdateErrorCount
					;;
		'Aux/FM')		$TESTSCRIPT/SetInputSource.bash 'Aux/FM' && \
					amixer cset name='Aux/FM Capture Volume' $VolumeL,$VolumeR && \
					amixer cget name='Aux/FM Capture Volume' | grep "values=$VolumeL,$VolumeR" || \
					UpdateErrorCount
					;;
		*)			echo "Incorrect name for output source"
					;;
	esac
}

# ==================
# Main
# ==================

export errors=0

Parser $@
ChangeInputVolumeLR $INPUTSOURCE $VOLUME_L $VOLUME_R
exit $errors
