#!/bin/sh

# Testsuite variables
export POSTFIX=`date "+%Y%m%d-%H%M%S"`
export TESTROOT=${PWD}
export TESTBIN=${TESTROOT}/../bin
export UTILBIN=${TESTROOT}/../../utils/bin
export TESTMODS=${TESTROOT}/../mods
export TESTSCRIPT=${TESTROOT}/helper
export TMPBASE=/tmp
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
export ROOTFILES=/sdcard/
export ANDROID_FILE=android.package
# End of file
