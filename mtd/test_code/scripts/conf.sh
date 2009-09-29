#!/bin/sh

# TestSuite General Variables
export MTD_POSTFIX=`date "+%Y%m%d-%H%M%S"`
export MTD_ROOT=`pwd`

export MTD_DIR_HELPER=${MTD_ROOT}/helper
export MTD_DIR_TMP=${MTD_ROOT}/tmp
export MTD_DIR_TEST=${MTD_ROOT}/test
export MTD_DIR_SCENARIOS="${MTD_ROOT}/scenarios"
export MTD_DIR_BINARIES=${MTD_ROOT}/../binaries

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
. ${MTD_ROOT}/../../utils/configuration/general.configuration
export UTILS_DIR_BIN=${MTD_ROOT}/../../utils/bin
export UTILS_DIR_HANDLERS=${MTD_ROOT}/../../utils/handlers

# MTD General Variables
export MTD_JFFS_OPTIONS=${MTD_DIR_TMP}/mtd.jffs.options
export MTD_CHARACTER_DEVICE=${MTD_DIR_TMP}/mtd.character.device
export MTD_BLOCK_DEVICE=${MTD_DIR_TMP}/mtd.block.device

# End of file