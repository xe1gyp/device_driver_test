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
export MONO_AUDIO_FILE="$TESTSCRIPT/audio-samples/rainbow_mono.wav"
export STEREO_AUDIO_FILE="$TESTSCRIPT/audio-samples/beethoven_"
export GENERIC_MONO_AUDIO_FILE="$TESTSCRIPT/audio-samples/rainbow_mono.wav"
export TEST_RECORD_STEREO_FILE="$TMPBASE/record_stereo_file.wav"
export RECORD_FILE="$TMPBASE/filename_rec.wav"

export AMIXER=alsa_amixer
export APLAY=alsa_aplay
export ARECORD=arecord

# End of file
