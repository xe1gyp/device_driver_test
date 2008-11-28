#!/bin/bash

function Parser
{
	if [ $# -eq 1 ]; then
		export INPUTSOURCE=$1
	else
		echo "Usage: SetInputSource.bash INPUTSOURCE"
		echo "INPUTSOURCE:"
		echo " Mic_Headset"
		echo " Mic_Main-Mic_Sub"
		echo " Aux/FM"
		echo " USB-CarKit"
		exit 1
	fi
}

function UpdateErrorCount
{
	errors=`echo $errors+1|bc`
	echo "Error found @ $TESTSCRIPT/SetInputSource.bash"
}


function SetInputSource
{
	local Source=$1
	case "$Source" in
		'Mic_Headset')		SourceNum=0
					;;
		'Mic_Main-Mic_Sub')	SourceNum=1
					;;
		'Aux/FM')		SourceNum=2
					;;
		'USB-CarKit')		SourceNum=3
					;;
		*)			SourceNum=-1;
					;;
	esac

	amixer cset name='Capture Source' $SourceNum && \
	amixer cget name='Capture Source' | grep "values=$SourceNum" || UpdateErrorCount
}

# ==================
# Main
# ==================

export errors=0

Parser $@
SetInputSource $INPUTSOURCE
exit $errors
