#!/bin/bash

function Usage
{
	echo "FullDuplex.bash PM_OPTION"
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
	echo "Error found @ $TESTSCRIPT/FullDuplex.bash"
}

function PrintHeader
{
        echo -e "\n********************************************************************************"
        if [ $# -eq 9 ]; then
                echo " Full Duplex "
		echo " Input Source: $1 "
		echo " Input Volume: $2 "
		echo " Output Source: $3 "
		echo " Output Volume: $4 "
		echo " Channels: $5 "
		echo " Sample Rate: $6 "
		echo " Time: $7 "
		echo " Playback Audio File: $8 "
		echo " Record Audio File: $9 "
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
		ps > $TMPBASE/ps_aplay
		cat $TMPBASE/ps_aplay | grep aplay || Match="No"
		rm $TMPBASE/ps_aplay
		sleep 1
	done
	echo "Audio file have finished!"
}

function PlayRecordFile
{
	local Channels=$1
	local Rate=$2
	local Time=$3
	local PlaybackFile=$4
	local RecordFile=$5
	local PlaybackCapture=$6
	
	if [ "$PlaybackCapture" == "PlaybackCapture" ]; then
		if [ "$PM_OPTION" == "Sleep" ]; then	
			(arecord -f S16_LE -c $Channels -r $Rate -d $Time $RecordFile | aplay $PlaybackFile &) && \
			echo -e "$SLEEP_OPTION\n$QUIT_OPTION" | $DPM_SCRIPT && \
			WaitEndOfAudioFile
		elif [ "$PM_OPTION" == "FreqScaling" ]; then
			(arecord -f S16_LE -c $Channels -r $Rate -d $Time $RecordFile | aplay $PlaybackFile &) && \
			echo -e "$SCALE_OPTION\n$QUIT_OPTION" | $DPM_SCRIPT && \
			WaitEndOfAudioFile	
		elif [ "$PM_OPTION" == "NoPMDPM" ]; then
			arecord -f S16_LE -c $Channels -r $Rate -d $Time $RecordFile | aplay $PlaybackFile || \
			UpdateErrorCount
		fi
	fi
}

function PlayFile
{
	local File=$1
	local PlaybackCapture=$2

	if [ "$PlaybackCapture" == "PlaybackCapture" ]; then
		aplay $File || UpdateErrorCount
	fi
}

function TestSampleRates
{
	local InputSource=$1
	local InputVolume=$2
	local OutputSource=$3
	local OutputVolume=$4
	local Channels=$5
	local SampleRates=$6
	local PlaybackCapture=$7

	for SampleRate in $SampleRates; do
		case $Channels in
			'Mono')		PlaybackFile=$GENERIC_MONO_AUDIO_FILE
					RecordFile=$TEST_RECORD_MONO_FILE
					Time=4
					Chan=1
					;;
			'Stereo')	PlaybackFile="$STEREO_AUDIO_FILE$SampleRate.wav"
					RecordFile=$TEST_RECORD_STEREO_FILE
					Time=18
					Chan=2
					;;
			*)		echo "Incorrect type of audio file";;
	        esac

		PrintHeader $InputSource $InputVolume $OutputSource $OutputVolume $Channels $SampleRate $Time $PlaybackFile $RecordFile
		$TESTSCRIPT/ChangeInputVolume.bash $InputSource $InputVolume && \
		$TESTSCRIPT/ChangeOutputVolume.bash $OutputSource $OutputVolume && \
		PlayRecordFile $Chan $SampleRate $Time $PlaybackFile $RecordFile $PlaybackCapture && \
		PlayFile $RecordFile $PlaybackCapture || \
		UpdateErrorCount
	done
}


# ==================
# Main
# ==================

export errors=0

INPUT_SOURCE=$DEFAULT_INPUT_SOURCE
INPUT_VOLUME=$DEFAULT_INPUT_VOLUME
OUTPUT_SOURCE=$DEFAULT_OUTPUT_SOURCE
OUTPUT_VOLUME=$DEFAULT_OUTPUT_VOLUME

SAMPLE_RATES="48000 44100 32000 24000 22050 16000 12000 11025 8000"
#SAMPLE_RATES="48000 16000 8000"

# Parse input arguments
Parser $@

INPUT_SOURCE="Mic_Headset"
CHANNELS="Stereo"
PLAYBACKCAPTURE="PlaybackCapture"
TestSampleRates $INPUT_SOURCE $INPUT_VOLUME $OUTPUT_SOURCE $OUTPUT_VOLUME $CHANNELS "$SAMPLE_RATES" $PLAYBACKCAPTURE

exit $errors
