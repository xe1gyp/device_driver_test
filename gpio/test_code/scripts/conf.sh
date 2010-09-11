#!/bin/sh

# TestSuite General Variables
export GPIO_POSTFIX=`date "+%Y%m%d-%H%M%S"`
export GPIO_ROOT=`pwd`

export GPIO_DIR_BINARIES=${GPIO_ROOT}/../bin
export GPIO_DIR_MODULES=${GPIO_ROOT}/../modules
export GPIO_DIR_HELPER=${GPIO_ROOT}/helper
export GPIO_DIR_TMP=${GPIO_ROOT}/tmp
export GPIO_DIR_TEST=${GPIO_ROOT}/test
export GPIO_DIR_SCENARIOS="${GPIO_ROOT}/scenarios"

export GPIO_FILE_OUTPUT=${GPIO_ROOT}/output.$GPIO_POSTFIX
export GPIO_FILE_LOG=${GPIO_ROOT}/log.$GPIO_POSTFIX
export GPIO_FILE_TMP=${GPIO_DIR_TMP}/tmp.$GPIO_POSTFIX
export GPIO_FILE_CMD=cmd.$GPIO_POSTFIX

export GPIO_DURATION=""
export GPIO_PRETTY_PRT=""
export GPIO_VERBOSE=""
export GPIO_SCENARIO_NAMES=""
export GPIO_STRESS=""

export PATH="$PATH:$GPIO_ROOT:$GPIO_DIR_BINARIES:$GPIO_DIR_HELPER"

# Utils General Variables
export UTILS_DIR=$GPIO_ROOT/../../utils/
export UTILS_DIR_BIN=$UTILS_DIR/bin
export UTILS_DIR_HANDLERS=$UTILS_DIR/handlers
export UTILS_DIR_SCRIPTS=$UTILS_DIR/scripts

. $UTILS_DIR/configuration/general.configuration

export PATH="$PATH:$UTILS_DIR_BIN:$UTILS_DIR_HANDLERS:$UTILS_DIR_SCRIPTS"

# GPIO Driver Specific
if [ `cat /proc/cpuinfo| grep -ic OMAP4` ]; then
	export GPIO_MAX_LINES=191
else
	export GPIO_MAX_LINES=209
fi

# GPIO Testsuite Specific
export GPIO_FIRST_LINE=0
export GPIO_INVALID_RANGE_INITIAL=-5
export GPIO_INVALID_RANGE_FINAL=-1
export GPIO_SMP_TEST_ITERATIONS=20

export GPIO_TEST_MODULE_NAME=gpio_test.ko
export GPIO_TEST_MODULE=$GPIO_DIR_MODULES/$GPIO_TEST_MODULE_NAME
export GPIO_TEST_MODULE_PROCFS_RESULT=/proc/driver/gpio_test_result

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
