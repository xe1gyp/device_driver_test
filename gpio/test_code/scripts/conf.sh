#!/bin/sh

# Testsuite variables
export POSTFIX=`date "+%Y%m%d-%H%M%S"`
export TESTROOT=${PWD}
export TESTBIN=${PWD}/../bin
export MODDIR=${PWD}/../mods
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

# Utilities
export UTILBIN=${TESTROOT}/../../utils/bin
export WAITKEY=$UTILBIN/akey

# Driver specific
export OMAP_MAX_GPIO_LINES=192
export GPIO_VALID_LINE=19
export INITIAL_INVALID_RANGE=-5
export FINAL_INVALID_RANGE=-1

export GPIO_TEST=gpio_test.ko
export TEST_MODULE=$MODDIR/$GPIO_TEST

export GPIO_PREEMPT=gpio.ko
export PREEMPT_MODULE=$MODDIR/$GPIO_PREEMPT

export DEV_GPIOTEST=/dev/gpiotest
export MAIN_TEST=$TESTBIN/main_test

cp $TESTBIN/app1 /bin/
cp $TESTBIN/app2 /bin/

if [ -f  "$DEV_GPIOTEST" ]
then
	mknod /dev/gpiotest c 241 0
fi

# End of file
