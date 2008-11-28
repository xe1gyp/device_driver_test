#!/bin/bash

function Parser
{
	if [ $# -eq 3 ]; then
		export OUTPUTSOURCE=$1
		export VOLUME_L=$2
		export VOLUME_R=$3
	else
		echo "Usage: ChangeOutputVolumeLR.bash OUTPUTSOURCE VOLUME_LEFT VOLUME_RIGHT"
		echo "OUTPUTSOURCE:"
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
	echo "Error found @ $TESTSCRIPT/ChangeOutputVolumeLR.bash"
}

function ChangeOutputVolumeLR
{
	local OutputSource=$1
	local VolumeL=$2
	local VolumeR=$3

	case $OutputSource in
		'Master')		$TESTSCRIPT/SetOutputSource.bash 'Hands-Free' && \
					amixer cset name='Master Playback Volume' $VolumeL,$VolumeR && \
					amixer cget name='Master Playback Volume' | grep "values=$VolumeL,$VolumeR" || \
					UpdateErrorCount
					;;
                'Stereo_Headset')       $TESTSCRIPT/SetOutputSource.bash 'Stereo_Headset' && \
                                        amixer cset name='Headset Playback Volume' $VolumeL,$VolumeR && \
                                        amixer cget name='Headset Playback Volume' | grep "values=$VolumeL,$VolumeR" || \
                                        UpdateErrorCount
                                        ;;

		'Hands-Free')		$TESTSCRIPT/SetOutputSource.bash 'Hands-Free' && \
					amixer cset name='Master Playback Volume' $VolumeL,$VolumeR && \
					amixer cget name='Master Playback Volume' | grep "values=$VolumeL,$VolumeR" || \
					UpdateErrorCount
					;;
		'USB-CarKit')		$TESTSCRIPT/SetOutputSource.bash 'USB-CarKit' && \
					amixer cset name='USB-Carkit Playback Volume' $VolumeL,$VolumeR && \
					amixer cget name='USB-Carkit Playback Volume' | grep "values=$VolumeL,$VolumeR" || \
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
ChangeOutputVolumeLR $OUTPUTSOURCE $VOLUME_L $VOLUME_R
exit $errors
