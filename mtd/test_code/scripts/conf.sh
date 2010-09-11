#!/bin/sh

# TestSuite General Variables
export MTD_POSTFIX=`date "+%Y%m%d-%H%M%S"`
export MTD_ROOT=`pwd`

export MTD_DIR_HELPER=${MTD_ROOT}/helper
export MTD_DIR_TMP=${MTD_ROOT}/tmp
export MTD_DIR_TEST=${MTD_ROOT}/test
export MTD_DIR_SCENARIOS="${MTD_ROOT}/scenarios"
export MTD_DIR_BINARIES=${MTD_ROOT}/../bin

export MTD_FILE_OUTPUT=${MTD_ROOT}/output.$MTD_POSTFIX
export MTD_FILE_LOG=${MTD_ROOT}/log.$MTD_POSTFIX
export MTD_FILE_TMP=${MTD_DIR_TMP}/tmp.$MTD_POSTFIX
export MTD_FILE_CMD=cmd.$MTD_POSTFIX

export MTD_DURATION=""
export MTD_PRETTY_PRT=""
export MTD_VERBOSE=""
export MTD_SCENARIO_NAMES=""
export MTD_STRESS=""

export PATH="${MTD_ROOT}:${MTD_DIR_HELPER}:${PATH}"

# Utils General Variables
export UTILS_DIR=$MTD_ROOT/../../utils/
export UTILS_DIR_BIN=$UTILS_DIR/bin
export UTILS_DIR_HANDLERS=$UTILS_DIR/handlers
export UTILS_DIR_SCRIPTS=$UTILS_DIR/scripts

. $UTILS_DIR/configuration/general.configuration

export PATH="$PATH:$UTILS_DIR_BIN:$UTILS_DIR_HANDLERS:$UTILS_DIR_SCRIPTS"

# MTD General Variables
export MTD_JFFS_OPTIONS=${MTD_DIR_TMP}/mtd.jffs.options
export MTD_CHARACTER_DEVICE=${MTD_DIR_TMP}/mtd.character.device
export MTD_BLOCK_DEVICE=${MTD_DIR_TMP}/mtd.block.device

# End of file