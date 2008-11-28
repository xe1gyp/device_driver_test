#!/bin/bash

function Parser
{
	if [ $# -eq 1 ]; then
		export SOURCE=$1
	else
		echo "Usage: SetSwitch.bash SOURCE"
		echo "SOURCE:"
		echo " Mic_Headset"
		echo " Mic_Main-Mic_Sub"
		echo " Hands-Free"
		echo " Sidetone"
		exit 1
	fi
}	

function UpdateErrorCount
{
	errors=`echo $errors+1|bc`
	echo "Error found @ $TESTSCRIPT/SetSwitch.bash"
}

function SetSwitch
{
	local Source=$1
	case $Source in
		'Mic_Headset')		$TESTSCRIPT/SetInputSource.bash 'Mic_Headset' && \
					amixer cset name='Mic Headset Capture Switch' 0 && \
					amixer cget name='Mic Headset Capture Switch' | grep "values=off" || \
					UpdateErrorCount
					amixer cset name='Mic Headset Capture Switch' 1 && \
					amixer cget name='Mic Headset Capture Switch' | grep "values=on" || \
					UpdateErrorCount
					;;
		'Mic_Main-Mic_Sub')	$TESTSCRIPT/SetInputSource.bash 'Mic_Main-Mic_Sub' && \
					amixer cset name='Mic Main Capture Switch' 0 && \
					amixer cget name='Mic Main Capture Switch' | grep "values=off" || \
					UpdateErrorCount
					amixer cset name='Mic Main Capture Switch' 1 && \
					amixer cget name='Mic Main Capture Switch' | grep "values=on" || \
					UpdateErrorCount
					amixer cset name='Mic Sub Capture Switch' 0 && \
					amixer cget name='Mic Sub Capture Switch' | grep "values=off" || \
					UpdateErrorCount && \
					amixer cset name='Mic Sub Capture Switch' 1 && \
					amixer cget name='Mic Sub Capture Switch' | grep "values=on" || \
					UpdateErrorCount
					;;
		'Hands-Free')		$TESTSCRIPT/SetOutputSource.bash 'Hands-Free' && \
					amixer cset name='Hands-free Playback Switch' 0 && \
					amixer cget name='Hands-free Playback Switch' | grep "values=off" || \
					UpdateErrorCount
					amixer cset name='Hands-free Playback Switch' 1 && \
					amixer cget name='Hands-free Playback Switch' | grep "values=on" || \
					UpdateErrorCount
					;;
		'Sidetone')		amixer cset name='Sidetone Playback Switch' 0 && \
					amixer cget name='Sidetone Playback Switch' | grep "values=off" || \
					UpdateErrorCount
					amixer cset name='Sidetone Playback Switch' 1 && \
					amixer cget name='Sidetone Playback Switch' | grep "values=on" || \
					UpdateErrorCount
					;;
		*)			echo "No switch control available for specified control"
					;;
	esac
}

# ==================
# Main
# ==================

export errors=0

Parser $@
SetSwitch $SOURCE
exit $errors

