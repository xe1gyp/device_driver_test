#!/bin/sh

# TestSuite General Variables
export TIMER_POSTFIX=`date "+%Y%m%d-%H%M%S"`
export TIMER_ROOT=`pwd`

export TIMER_DIR_BINARIES=${TIMER_ROOT}/../bin
export TIMER_DIR_MODULES=${TIMER_ROOT}/../modules
export TIMER_DIR_HELPER=${TIMER_ROOT}/helper
export TIMER_DIR_TMP=${TIMER_ROOT}/tmp
export TIMER_DIR_TEST=${TIMER_ROOT}/test
export TIMER_DIR_SCENARIOS="${TIMER_ROOT}/scenarios"

export TIMER_FILE_OUTPUT=${TIMER_ROOT}/output.$TIMER_POSTFIX
export TIMER_FILE_LOG=${TIMER_ROOT}/log.$TIMER_POSTFIX
export TIMER_FILE_TMP=${TIMER_DIR_TMP}/tmp.$TIMER_POSTFIX
export TIMER_FILE_CMD=cmd.$TIMER_POSTFIX

export TIMER_DURATION=""
export TIMER_PRETTY_PRT=""
export TIMER_VERBOSE=""
export TIMER_SCENARIO_NAMES=""
export TIMER_STRESS=""

export PATH="$PATH:$TIMER_ROOT:$TIMER_DIR_BINARIES:$TIMER_DIR_HELPER"

# Utils General Variables
export UTILS_DIR=$TIMER_ROOT/../../utils/
export UTILS_DIR_BIN=$UTILS_DIR/bin
export UTILS_DIR_HANDLERS=$UTILS_DIR/handlers
export UTILS_DIR_SCRIPTS=$UTILS_DIR/scripts

. $UTILS_DIR/configuration/general.configuration

export PATH="$PATH:$UTILS_DIR_BIN:$UTILS_DIR_HANDLERS:$UTILS_DIR_SCRIPTS"

#GP Timer Variables

if [ -f /var/log/dmesg ]; then
        alias dmesg='cat /var/log/dmesg'
fi

GPTIMER=`dmesg | grep -i gptimer`
export RESERVED_GPTIMER=`echo $GPTIMER | awk '{print $4}'| cut -c '8'`
export MAXGPTIMERS=12

# End of file

