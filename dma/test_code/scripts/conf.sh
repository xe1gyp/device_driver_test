#!/bin/sh

# Testsuite variables
export POSTFIX=`date "+%Y%m%d-%H%M%S"`
export TESTROOT=${PWD}
export TESTBIN=${PWD}/../bin
export UTILBIN=${PWD}/../../utils/bin
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

# Full array values
export CHANNELS_ARRAY="2 3 4 5 6 7 8 9 10 11 12 13 14 15 16"
export BUFFER_SIZE_ARRAY="16384 32768 65536 131072 262144 524288"

# Notice that not every buffer size can run with every channels number,
# for memory limitations for example.
# Adjust them as you need.
export CHANNELS_ARRAY="2 3 4 5 6 7"
export BUFFER_SIZE_ARRAY="16384 32768 65536 131072"

# End of file
