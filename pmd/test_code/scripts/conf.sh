#!/bin/sh
######################################################################
# Configuration script
# Author  : Misael Lopez Cruz
# Date    : 04 Jul 07
# Description: Configuration script with user specified value for some
# environment variables
######################################################################

# These variables dont' need to be modified
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
export DURATION="1h"
export PATH="${PATH}:${TESTROOT}:${TESTBIN}:${TESTSCRIPT}"
export TC_SCENARIO="${TESTROOT}/scenarios"
export SCENARIO_NAMES=""

# Utilities
export WAIT_ANSWER=${UTILBIN}/akey

# Logs
export LOG_OUTPUT="$TESTROOT/log.custom.powermanagement"

# Specific Variables
export NAME=""
export MESSAGE_DELAY=5
export DESCRIPTION=""

export MIN_INT_PER_SECOND=0
export MAX_INT_PER_SECOND=1500

# Power Management
#export DPM_SCRIPT=$TESTSCRIPT/dpm.bash
export DPM_SCRIPT=$TESTSCRIPT/dpm_test_8x.bash
export PARAMETERS=""
export TIME_TO_REPEAT=0
export QUIT_OPTION=0
export SCALE_OPTION=6
export SLEEP_OPTION=4
export SLOW_OPERATING_POINT=s
export FAST_OPERATING_POINT=f
export STRESS_OPERATING_POINTS=7
export STRESS_FREQUENCIES=6
