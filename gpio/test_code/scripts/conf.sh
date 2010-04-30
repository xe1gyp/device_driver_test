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
export COUNT=0

# Utilities
export UTILBIN=${TESTROOT}/../../utils/bin
export WAITKEY=$UTILBIN/akey
export UTILSCRIPTS=${TESTROOT}/../../utils/scripts
export UTILS_DIR_HANDLERS=${TESTROOT}/../../utils/handlers

. ${TESTROOT}/../../utils/configuration/general.configuration

export PATH="$PATH:$UTILBIN:$UTILS_DIR_HANDLERS:$UTILSCRIPTS"

# Driver specific
export MAX_GPIO_LINES=209
# 
if [ `cat /proc/cpuinfo| grep -ic OMAP4` ]; then
	export MAX_GPIO_LINES=191
fi
#
export FIRST_GPIO_LINE=0
export INITIAL_INVALID_RANGE=-5
export FINAL_INVALID_RANGE=-1

export GPIO_TEST=gpio_test.ko
export TEST_MODULE=$MODDIR/$GPIO_TEST

# export GPIO_PREEMPT=gpio.ko
# export PREEMPT_MODULE=$MODDIR/$GPIO_PREEMPT

export DEV_GPIOTEST=/dev/gpiotest
export MAIN_TEST=$TESTBIN/main_test

# cp $TESTBIN/app1 /bin/
# cp $TESTBIN/app2 /bin/

# if [ -f  "$DEV_GPIOTEST" ]
# then
#	 mknod /dev/gpiotest c 241 0
# fi

# Remove any error file
handlerError.sh "clean"

# End of file
