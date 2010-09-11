#!/bin/sh -x

# TestSuite General Variables
export BATTERY_POSTFIX=`date "+%Y%m%d-%H%M%S"`
export BATTERY_ROOT=`pwd`

export BATTERY_DIR_BINARIES=${BATTERY_ROOT}/../bin
export BATTERY_DIR_HELPER=${BATTERY_ROOT}/helper
export BATTERY_DIR_TMP=${BATTERY_ROOT}/tmp
export BATTERY_DIR_TEST=${BATTERY_ROOT}/test
export BATTERY_DIR_SCENARIOS="${BATTERY_ROOT}/scenarios"

export BATTERY_FILE_OUTPUT=${BATTERY_ROOT}/output.$BATTERY_POSTFIX
export BATTERY_FILE_LOG=${BATTERY_ROOT}/log.$BATTERY_POSTFIX
export BATTERY_FILE_TMP=${BATTERY_DIR_TMP}/tmp.$BATTERY_POSTFIX
export BATTERY_FILE_CMD=cmd.$BATTERY_POSTFIX

export BATTERY_DURATION=""
export BATTERY_PRETTY_PRT=""
export BATTERY_VERBOSE=""
export BATTERY_SCENARIO_NAMES=""
export BATTERY_STRESS=""

export PATH="${PATH}:${BATTERY_ROOT}:${BATTERY_DIR_BINARIES}:${BATTERY_DIR_HELPER}"

# Utils General Variables
export UTILS_DIR=$BATTERY_ROOT/../../utils/
export UTILS_DIR_BIN=$UTILS_DIR/bin
export UTILS_DIR_HANDLERS=$UTILS_DIR/handlers
export UTILS_DIR_SCRIPTS=$UTILS_DIR/scripts

. $UTILS_DIR/configuration/general.configuration

export PATH="$PATH:$UTILS_DIR_BIN:$UTILS_DIR_HANDLERS:$UTILS_DIR_SCRIPTS"

# Driver specific
# Battery driver needs the TEST_PLATFORM variable to choose the
# correct sysfs entries to probe
# Supports "DUMMY" for PC testing, "OMAP" and OMAP4"
# See scripts/helper/battery_util.sh
export TEST_PLATFORM="OMAP4"

# End of file
