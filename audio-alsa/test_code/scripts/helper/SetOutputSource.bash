#!/bin/bash

function Parser
{
	if [ $# -eq 1 ]; then
		export OUTPUTSOURCE=$1
	else
		echo "Usage: SetOutput.bash OUTPUTSOURCE"
		echo "OUTPUTSOURCE:"
		echo " Stereo_Headset"
		echo " Hands-Free"
		echo " Mono_Handset"
		echo " USB-CarKit"
		exit 1
	fi
}

function UpdateErrorCount
{
        errors=`echo $errors+1|bc`
        echo "Error found @ $TESTSCRIPT/SetOutputSource.bash"
}

function SetOutputSource
{
	local Source=$1
	case "$Source" in
		'Stereo_Headset')	SourceNum=0
					;;
		'Hands-Free')		SourceNum=1
					;;
		'Mono_Handset')		SourceNum=2
					;;
		'USB-CarKit')		SourceNum=3
					;;
		*)			SourceNum=-1;
					;;
	esac

	amixer cset name='Playback Source' $SourceNum && \
	amixer cget name='Playback Source' | grep "values=$SourceNum" || UpdateErrorCount
}

# ==================
# Main
# ==================

export errors=0

Parser $@
SetOutputSource $OUTPUTSOURCE
exit $errors
