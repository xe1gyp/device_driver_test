#!/bin/sh

# Testsuite variables
export POSTFIX=`date "+%Y%m%d-%H%M%S"`
export TESTROOT=${PWD}
export TESTBIN=${PWD}/../bin
export TESTMODS=${PWD}/../mods
export TESTSCRIPT=${PWD}/helper
export TMPBASE=${TESTROOT}/tmp
export TMPFILE=${TMPBASE}/tmp.$POSTFIX
export CMDFILE=cmd.$POSTFIX
export TESTDIR=${TESTROOT}/test
export PRETTY_PRT=""
export VERBOSE=""
export OUTPUTFILE=${TESTROOT}/output.$POSTFIX
export LOGFILE=${TESTROOT}/log.$POSTFIX
export DURATION=""
export PATH="${PATH}:${TESTROOT}:${TESTBIN}:${TESTSCRIPT}"
export TC_SCENARIO="${TESTROOT}/scenarios"
export SCENARIO_NAMES=""

# Utilities
export UTILBIN=${TESTROOT}/../../utils/bin
export UTILSCRIPTS=${TESTROOT}/../../utils/scripts

# Audio files path
export DEFAULT_INPUT_SOURCE="Mic_Headset"
export DEFAULT_INPUT_VOLUME=100
export DEFAULT_CAPTURE_TIME=5
export DEFAULT_CAPTURE_RATE=48000
export DEFAULT_CAPTURE_CHANNELS="Stereo"

export DEFAULT_OUTPUT_SOURCE="Stereo_Headset"
export DEFAULT_OUTPUT_VOLUME=100
export DEFAULT_PLAYBACK_CHANNELS="Stereo"

export DEFAULT_CAPTURE="Capture"
export DEFAULT_PLAYBACK="Playback"
export DEFAULT_PLAYBACK_CAPTURE="PlaybackCapture"

export MONO_AUDIO_FILE="$TESTSCRIPT/audio-samples/rainbow_mono.wav"
export STEREO_AUDIO_FILE="$TESTSCRIPT/audio-samples/beethoven_"

export GENERIC_MONO_AUDIO_FILE="$TESTSCRIPT/audio-samples/rainbow_mono.wav"
export GENERIC_MONO_SAMPLE_RATE=22050

export GENERIC_STEREO_AUDIO_FILE="$TESTSCRIPT/audio-samples/beat_stereo.wav"
export GENERIC_STEREO_SAMPLE_RATE=44100

export TEST_RECORD_MONO_FILE="$TMPBASE/record_mono_file.wav"
export TEST_RECORD_STEREO_FILE="$TMPBASE/record_stereo_file.wav"

export AMIXER=alsa_amixer
export APLAY=alsa_aplay
export ARECORD=arecord

# End of file
