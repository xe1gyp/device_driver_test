#!/bin/bash

function Parser
{
	if [ $# -eq 2 ]; then
		export INPUTSOURCE=$1
		export VOLUME=$2
	else
		echo "Usage: ChangeInputVolume.bash INPUTSOURCE VOLUME"
		echo "INPUTSOURCE:"
		echo " Mic_Headset"
		echo " Mic_Main-Mic_Sub"
		echo " Aux/FM"
		echo " USB-CarKit"
		echo "VOLUME:"
		echo " [0 ... 100]"
		exit 1
	fi
}

function UpdateErrorCount
{
	errors=`echo $errors+1|bc`
	echo "Error found @ $TESTSCRIPT/ChangeInputVolume.bash"
}

function ChangeInputVolume
{
	local InputSource=$1
	local Volume=$2

	case $InputSource in
		'Master')		$TESTSCRIPT/SetInputSource.bash 'Mic_Main-Mic_Sub' && \
					amixer cset name='Master Capture Volume' $Volume && \
					amixer cget name='Master Capture Volume' | grep "values=$Volume,$Volume" || \
					UpdateErrorCount
					;;
		'Mic_Headset')		$TESTSCRIPT/SetInputSource.bash 'Mic_Headset' && \
					amixer cset name='Mic Headset Capture Volume' $Volume && \
					amixer cget name='Mic Headset Capture Volume' | grep "values=$Volume" || \
					UpdateErrorCount
					;;				
		'Mic_Main-Mic_Sub')	$TESTSCRIPT/SetInputSource.bash 'Mic_Main-Mic_Sub' && \
					amixer cset name='Mic Main Capture Volume' $Volume && \
					amixer cget name='Mic Main Capture Volume' | grep "values=$Volume" && \
					amixer cset name='Mic Sub Capture Volume' $Volume && \
					amixer cget name='Mic Sub Capture Volume' | grep "values=$Volume" || \
					UpdateErrorCount
					;;
		'Aux/FM')		$TESTSCRIPT/SetInputSource.bash 'Aux/FM' && \
					amixer cset name='Aux/FM Capture Volume' $Volume && \
					amixer cget name='Aux/FM Capture Volume' | grep "values=$Volume,$Volume" || \
					UpdateErrorCount
					;;
		'USB-CarKit')		$TESTSCRIPT/SetInputSource.bash 'USB-CarKit' && \
					amixer cset name='USB-Carkit Capture Volume' $Volume && \
					amixer cget name='USB-Carkit Capture Volume' | grep "values=$Volume" || \
					UpdateErrorCount
					;;
		*)			echo "Incorrect name for input source"
					exit 1
					;;
	esac
}

# ==================
# Main
# ==================

export errors=0

Parser $@
ChangeInputVolume $INPUTSOURCE $VOLUME
exit $errors
