#!/bin/bash

function Parser
{
	if [ $# -eq 2 ]; then
		export OUTPUTSOURCE=$1
		export VOLUME=$2
	else
		echo "Usage: ChangeOutputVolume.bash OUTPUTSOURCE VOLUME"
		echo "OUTPUTSOURCE:"
		echo " Master"
		echo " Stereo_Headset"
		echo " Hands-Free"
		echo " Mono_Handset"
		echo " USB-CarKit"
		echo " Sidetone"
		echo "VOLUME:"
		echo " [0 ... 100] - Master, Stereo_Headset, Hands-Free, Mono_Handset, USB-CarKit"
		echo " [0 ... 41] - Sidetone"
		exit 1
	fi
}

function UpdateErrorCount
{
	errors=`echo $errors+1|bc`
        echo "Error found @ $TESTSCRIPT/ChangeOutputVolume.bash"
}

function ChangeOutputVolume
{
	local OutputSource=$1
	local Volume=$2

	case $OutputSource in
		'Master')		$TESTSCRIPT/SetOutputSource.bash 'Hands-Free' && \
					amixer cset name='Master Playback Volume' $Volume && \
					amixer cget name='Master Playback Volume' | grep "values=$Volume,$Volume" || \
					UpdateErrorCount
					;;
		'Stereo_Headset')	$TESTSCRIPT/SetOutputSource.bash 'Stereo_Headset' && \
					amixer cset name='Headset Playback Volume' $Volume && \
					amixer cget name='Headset Playback Volume' | grep "values=$Volume,$Volume" || \
					UpdateErrorCount
					;;
		'Hands-Free')		$TESTSCRIPT/SetOutputSource.bash 'Hands-Free' && \
					amixer cset name='Master Playback Volume' $Volume && \
					amixer cget name='Master Playback Volume' | grep "values=$Volume,$Volume" || \
					UpdateErrorCount
					;;
		'Mono_Handset')		$TESTSCRIPT/SetOutputSource.bash 'Mono_Handset' && \
					amixer cset name='Handset Playback Volume' $Volume && \
					amixer cget name='Handset Playback Volume' | grep "values=$Volume" || \
					UpdateErrorCount
					;;
		'USB-CarKit')		$TESTSCRIPT/SetOutputSource.bash 'USB-CarKit' && \
					amixer cset name='USB-Carkit Playback Volume' $Volume && \
					amixer cget name='USB-Carkit Playback Volume' | grep "values=$Volume,$Volume" || \
					UpdateErrorCount
					;;
		'Sidetone')		$TESTSCRIPT/SetOutputSource.bash 'Hands-Free' && \
					amixer cset name='Sidetone Playback Volume' $Volume && \
					amixer cget name='Sidetone Playback Volume' | grep "values=$Volume" || \
					UpdateErrorCount
					;;
		*)			echo "Incorrect name for output source"
					exit 1
					;;
	esac
}

# ==================
# Main
# ==================

export errors=0

Parser $@
ChangeOutputVolume $OUTPUTSOURCE $VOLUME
exit $errors
