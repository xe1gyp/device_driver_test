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
export HELLO_WORLD_APP=hello.world
export APPLICATION_HELLO_WORLD=$TESTBIN/$HELLO_WORLD_APP

export HELLO_WORLD_MOD=hello.world.ko
export MODULE_HELLO_WORLD=$MODDIR/$HELLO_WORLD_MOD

export KERNEL_MESSAGES=/var/log/messages
export HELPER_KERNEL_MESSAGES=helperKernelMessages.sh

# End of file
