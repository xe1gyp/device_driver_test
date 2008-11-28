#!/bin/bash

function Usage
{
	echo "Record.bash PM_OPTION"
	echo "PM_OPTION - Power management option: "
	echo " 'Sleep' - Deep sleep"
	echo " 'FreqScaling' - Frequency Scaling"
	echo " 'NoPMDPM' - No PM/DPM tests"
}

function Parser
{
	if [ $# -eq 1 ]; then
		PM_OPTION="$1"
	else
		Usage
		exit 1
	fi
}

function UpdateErrorCount
{
	errors=`echo $errors+1|bc`
	echo "Error found @ $TESTSCRIPT/Record.bash"
}

function PrintHeader
{
        echo -e "\n********************************************************************************"
        if [ $# -eq 8 ]; then
                echo " Record Support "
		echo " Input Source: $1 "
		echo " Input Volume: $2 "
		echo " Channels: $3 "
		echo " Sample Rate: $4 "
		echo " Record Time: $5 "
                echo " Output Source: $6 "
                echo " Output Volume: $7 "
                echo " Audio file: $8 "
        else
                echo "Incorrect number of arguments"
                exit 1
        fi
        echo -e  "********************************************************************************"
}

function WaitEndOfAudioFile
{
	local Match="Yes"
	echo "Waiting for audio file ends..."
	while [ "$Match" != "No" ]; do
		ps > $TMPBASE/ps_arecord
		cat $TMPBASE/ps_arecord | grep arecord || Match="No"
		rm $TMPBASE/ps_arecord
		sleep 1
	done
	echo "Audio file have finished!"
}

function RecordFile
{
	local Channels=$1
	local Rate=$2
	local Time=$3
	local File=$4
	local Capture=$5
	
	if [ "$Capture" == "Capture" ]; then
		if [ "$PM_OPTION" == "Sleep" ]; then	
			(arecord -f S16_LE -c $Channels -r $Rate -d $Time $File &) && \
			echo -e "$SLEEP_OPTION\n$QUIT_OPTION" | $DPM_SCRIPT && \
			WaitEndOfAudioFile
		elif [ "$PM_OPTION" == "FreqScaling" ]; then
			(arecord -f S16_LE -c $Channels -r $Rate -d $Time $File &) && \
			echo -e "$SCALE_OPTION\n$QUIT_OPTION" | $DPM_SCRIPT && \
			WaitEndOfAudioFile	
		elif [ "$PM_OPTION" == "NoPMDPM" ]; then
			arecord -f S16_LE -c $Channels -r $Rate -d $Time $File || \
			UpdateErrorCount
		fi
	fi
}

function PlayFile
{
	local File=$1
	local Capture=$2

	if [ "$Capture" == "Capture" ]; then
		aplay $File || UpdateErrorCount
	fi
}

function TestSampleRates
{
	local InputSource=$1
	local InputVolumes=$2
	local Channels=$3
	local SampleRates=$4
	local RecordTime=$5
	local OutputSource=$6
	local OutputVolume=$7
	local Capture=$8

	case $Channels in
		'Mono')		File=$TEST_RECORD_MONO_FILE; Chan=1;;
		'Stereo')	File=$TEST_RECORD_STEREO_FILE; Chan=2;;
		*)		echo "Incorrect type of audio file";;
        esac

	for SampleRate in $SampleRates; do
		for InputVolume in $InputVolumes; do
			PrintHeader $InputSource $InputVolume $Channels $SampleRate $RecordTime $OutputSource $OutputVolume $File
			$TESTSCRIPT/ChangeInputVolume.bash $InputSource $InputVolume && \
			$TESTSCRIPT/ChangeOutputVolume.bash $OutputSource $OutputVolume && \
			RecordFile $Chan $SampleRate $RecordTime $File $Capture && \
			PlayFile $File $Capture || \
			UpdateErrorCount
		done
	done
}


# ==================
# Main
# ==================

export errors=0

INPUT_VOLUMES="0 70"
SAMPLE_RATES="48000 44100 32000 24000 22050 16000 12000 11025 8000"
#SAMPLE_RATES="11025"
RECORD_TIME=5

OUTPUT_SOURCE=$DEFAULT_OUTPUT_SOURCE
OUTPUT_VOLUME=100

# Parse input arguments
Parser $@

INPUT_SOURCE="Mic_Headset"
CHANNELS="Stereo"
CAPTURE="Capture"
TestSampleRates $INPUT_SOURCE "$INPUT_VOLUMES" $CHANNELS "$SAMPLE_RATES" $RECORD_TIME $OUTPUT_SOURCE $OUTPUT_VOLUME $CAPTURE

INPUT_SOURCE="Mic_Main-Mic_Sub"
CHANNELS="Mono"
CAPTURE="Capture"
TestSampleRates $INPUT_SOURCE "$INPUT_VOLUMES" $CHANNELS "$SAMPLE_RATES" $RECORD_TIME $OUTPUT_SOURCE $OUTPUT_VOLUME $CAPTURE

INPUT_SOURCE="Aux/FM"
CHANNELS="Stereo"
CAPTURE="NoCapture"
TestSampleRates $INPUT_SOURCE "$INPUT_VOLUMES" $CHANNELS "$SAMPLE_RATES" $RECORD_TIME $OUTPUT_SOURCE $OUTPUT_VOLUME $CAPTURE

#INPUT_SOURCE="USB-CarKit"
#CHANNELS="Mono"
#CAPTURE="NoCapture"
#TestSampleRates $INPUT_SOURCE "$INPUT_VOLUMES" $CHANNELS "$SAMPLE_RATES" $RECORD_TIME $OUTPUT_SOURCE $OUTPUT_VOLUME $CAPTURE

exit $errors

