#!/bin/sh

# Timer constants
export MIN_INT_PER_SECOND=10
export MAX_INT_PER_SECOND=30

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

# Test modules
export TIMER_MOD=test32ktimer.ko
export PRIORITY_MOD=priority_test.ko
export TIMER_ENTRY="32KHz timer"

# Remove modules if they already exist
lsmod | grep test32ktimer && rmmod $TIMER_MOD && echo "$TIMER_MOD was removed successfully" || echo "$TIMER_MOD is not inserted"
lsmod | grep priority_test && rmmod $PRIORITY_MOD && echo "$PRIORITY_MOD was removed successfully" || echo "$PRIORITY_MOD is not inserted"

# Kernel and Timer path
export WDT_DEV_ENTRY='/dev/watchdog'
export KDIR='kernel_org/2.6_kernel'
export TIME_PATH='arch/arm/plat-omap/dmtimer.c'
