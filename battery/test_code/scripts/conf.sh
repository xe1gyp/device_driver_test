#!/bin/sh

# Testsuite variables
export POSTFIX=`date "+%Y%m%d-%H%M%S"`
export TESTROOT=${PWD}
export TESTBIN=${PWD}/../bin
export MODDIR=${PWD}/../modules
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
export PATH="${TESTROOT}:${TESTBIN}:${TESTSCRIPT}:${PATH}"
export TC_SCENARIO="${TESTROOT}/scenarios"
export SCENARIO_NAMES=""
export STRESS=""

# External Utilities
. ${TESTROOT}/../../utils/configuration/general.configuration
export UTILBIN=${TESTROOT}/../../utils/bin

# Driver specific
#Battery driver needs the TEST_PLATFORM variable to choose the
#correct sysfs entries to probe
#Supports "DUMMY" for PC testing, "OMAP" and OMAP4"
#See scripts/helper/battery_util.sh
export TEST_PLATFORM="OMAP4"

# End of file
