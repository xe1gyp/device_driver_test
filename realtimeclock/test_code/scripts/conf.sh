#!/bin/sh


# Clock applications variables
export USER_INPUT='0'
export DEFAULT_INPUT='1'
export VALID_TIME='121212'		# HHMMSS
export VALID_DATE='112906'		# MMDDYY
export RTC_VALID_TIME='12:12:12'
export RTC_VALID_DATE='29-11-2006'
export DEF_RTC_VALID_TIME='23:59:45'
export DEF_RTC_VALID_DATE='31-12-2004'
export DEF_PROC_VALID_TIME='23:59:45'
export DEF_PROC_VALID_DATE='2004-12-31'
export INVALID_DATE1='133206'
export INVALID_DATE2='022906'
export INVALID_TIME1='257570'
export INT_SEC='1'
export ALARM_SEC='1'
export ALARM_VALUE=15

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

