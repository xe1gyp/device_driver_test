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
export PROC_DRIVER_DIR=/proc/driver
export MCSPI_PROC_DIR=${PROC_DRIVER_DIR}/mcspi_test
export PRETTY_PRT=""
export VERBOSE=""
export OUTPUTFILE=${TESTROOT}/output.$POSTFIX
export LOGFILE=${TESTROOT}/log.$POSTFIX
export DURATION=""
export PATH="${TESTROOT}:${TESTBIN}:${TESTSCRIPT}:${PATH}"
export TC_SCENARIO="${TESTROOT}/scenarios"
export SCENARIO_NAMES=""
export STRESS=""
export DEBUG_DIR=/debug/
export POWERSYSFS=/sys/power
if [ -f "/sys/power/wakeup_timer_seconds" ] #is this power entry in sysfs?
then #if yes, we can look in /sys/power for all entries
       export POWER_ENTRIES=$POWERSYSFS
else #if not, they must be in debugfs and it should be mounted
       if [ ! -d $DEBUG_DIR ]; then mkdir $DEBUG_DIR; mount -t debugfs debugfs /debug; fi;
       export POWER_ENTRIES=$DEBUG_DIR/pm_debug/
fi

# Utilities
export UTILBIN=${TESTROOT}/../../utils/bin
export WAITKEY=$UTILBIN/akey

# Driver specific
export GPIO_TEST=gpio_test.ko
export TEST_MODULE=$MODDIR/$GPIO_TEST

# End of file
