#!/bin/sh -x

# TestSuite General Variables
export VIBRATOR_POSTFIX=`date "+%Y%m%d-%H%M%S"`
export VIBRATOR_ROOT=`pwd`

export VIBRATOR_DIR_BINARIES=${VIBRATOR_ROOT}/../bin
export VIBRATOR_DIR_HELPER=${VIBRATOR_ROOT}/helper
export VIBRATOR_DIR_TMP=${VIBRATOR_ROOT}/tmp
export VIBRATOR_DIR_TEST=${VIBRATOR_ROOT}/test
export VIBRATOR_DIR_SCENARIOS="${VIBRATOR_ROOT}/scenarios"

export VIBRATOR_FILE_OUTPUT=${VIBRATOR_ROOT}/output.$VIBRATOR_POSTFIX
export VIBRATOR_FILE_LOG=${VIBRATOR_ROOT}/log.$VIBRATOR_POSTFIX
export VIBRATOR_FILE_TMP=${VIBRATOR_DIR_TMP}/tmp.$VIBRATOR_POSTFIX
export VIBRATOR_FILE_CMD=cmd.$VIBRATOR_POSTFIX

export VIBRATOR_DURATION=""
export VIBRATOR_PRETTY_PRT=""
export VIBRATOR_VERBOSE=""
export VIBRATOR_SCENARIO_NAMES=""
export VIBRATOR_STRESS=""


export VIBRATOR_SYSFS_PATH="/sys/class/timed_output/vibrator"

export PATH="${PATH}:${VIBRATOR_ROOT}:${VIBRATOR_DIR_BINARIES}:${VIBRATOR_DIR_HELPER}"

# Utils General Variables
export UTILS_DIR=$VIBRATOR_ROOT/../../utils/
export UTILS_DIR_BIN=$UTILS_DIR/bin
export UTILS_DIR_HANDLERS=$UTILS_DIR/handlers
export UTILS_DIR_SCRIPTS=$UTILS_DIR/scripts

. $UTILS_DIR/configuration/general.configuration

export PATH="$PATH:$UTILS_DIR_BIN:$UTILS_DIR_HANDLERS:$UTILS_DIR_SCRIPTS"

# General variables
export DMESG_FILE=/var/log/dmesg


# End of file
