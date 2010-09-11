#!/bin/sh

# TestSuite General Variables
export DEMO_POSTFIX=`date "+%Y%m%d-%H%M%S"`
export DEMO_ROOT=`pwd`

export DEMO_DIR_BINARIES=${DEMO_ROOT}/../bin
export DEMO_DIR_HELPER=${DEMO_ROOT}/helper
export DEMO_DIR_TMP=${DEMO_ROOT}/tmp
export DEMO_DIR_TEST=${DEMO_ROOT}/test
export DEMO_DIR_SCENARIOS="${DEMO_ROOT}/scenarios"

export DEMO_FILE_OUTPUT=${DEMO_ROOT}/output.$DEMO_POSTFIX
export DEMO_FILE_LOG=${DEMO_ROOT}/log.$DEMO_POSTFIX
export DEMO_FILE_TMP=${DEMO_DIR_TMP}/tmp.$DEMO_POSTFIX
export DEMO_FILE_CMD=cmd.$DEMO_POSTFIX

export DEMO_DURATION=""
export DEMO_PRETTY_PRT=""
export DEMO_VERBOSE=""
export DEMO_SCENARIO_NAMES=""
export DEMO_STRESS=""

export PATH="$DEMO_ROOT:$DEMO_DIR_BINARIES:$DEMO_DIR_HELPER:$PATH"

# Utils General Variables
export UTILS_DIR=$DEMO_ROOT/../../utils/
export UTILS_DIR_BIN=$UTILS_DIR/bin
export UTILS_DIR_HANDLERS=$UTILS_DIR/handlers
export UTILS_DIR_SCRIPTS=$UTILS_DIR/scripts

. $UTILS_DIR/configuration/general.configuration

export PATH="$PATH:$UTILS_DIR_BIN:$UTILS_DIR_HANDLERS:$UTILS_DIR_SCRIPTS"

# Driver specific
export HELLO_WORLD_APP=hello.world
export APPLICATION_HELLO_WORLD=$TESTBIN/$HELLO_WORLD_APP

export HELLO_WORLD_MOD=hello.world.ko
export MODULE_HELLO_WORLD=$MODDIR/$HELLO_WORLD_MOD

export KERNEL_MESSAGES=/var/log/messages
export HELPER_KERNEL_MESSAGES=helperKernelMessages.sh

# Remove all existing references for handlerError.sh
handlerError.sh "clean"

# End of file
