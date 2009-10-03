#!/bin/sh

# Testsuites variables
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

# General variables

export LMBENCH_SRC=${TESTROOT}/../utils/lmbench-3.0-a9/src/
export LMBENCH_BIN=${TESTROOT}/../utils/lmbench-3.0-a9/bin/
export LMBENCH_RESULTS=${TESTROOT}/../utils/lmbench-3.0-a9/results/
export LMBENCH_SCRIPTS=${TESTROOT}/../utils/lmbench-3.0-a9/scripts/
export LMBENCH_DIR='cd ../utils/lmbench-3.0-a9/scripts'

# End of file
