#!/bin/sh

# TestSuite General Variables
export MMCSDP_POSTFIX=`date "+%Y%m%d-%H%M%S"`
export MMCSDP_ROOT=`pwd`

export MMCSDP_DIR_BINARIES=${MMCSDP_ROOT}/../bin
export MMCSDP_DIR_HELPER=${MMCSDP_ROOT}/helper
export MMCSDP_DIR_TMP=${MMCSDP_ROOT}/tmp
export MMCSDP_DIR_TEST=${MMCSDP_ROOT}/test
export MMCSDP_DIR_SCENARIOS="${MMCSDP_ROOT}/scenarios"

export MMCSDP_FILE_OUTPUT=${MMCSDP_ROOT}/output.$MMCSDP_POSTFIX
export MMCSDP_FILE_LOG=${MMCSDP_ROOT}/log.$MMCSDP_POSTFIX
export MMCSDP_FILE_TMP=${MMCSDP_DIR_TMP}/tmp.$MMCSDP_POSTFIX
export MMCSDP_FILE_CMD=cmd.$MMCSDP_POSTFIX

export MMCSDP_DURATION=""
export MMCSDP_PRETTY_PRT=""
export MMCSDP_VERBOSE=""
export MMCSDP_SCENARIO_NAMES=""
export MMCSDP_STRESS=""
export MMCSDP_BLOCK_FOLDER=/dev/block
export MMCSDP_AUTO_PARTITION=0

export PATH="$PATH:$MMCSDP_ROOT:$MMCSDP_DIR_HELPER:MMCSDP_DIR_BINARIES"

cd ${MMCSDP_ROOT}/../../../mmc/scripts/
. ${MMCSDP_ROOT}/../../../mmc/scripts/conf.sh

# Utils General Variables
export UTILS_DIR=$MMCSDP_ROOT/../../utils/
export UTILS_DIR_BIN=$UTILS_DIR/bin
export UTILS_DIR_HANDLERS=$UTILS_DIR/handlers
export UTILS_DIR_SCRIPTS=$UTILS_DIR/scripts

. $UTILS_DIR/configuration/general.configuration

export PATH="$PATH:$UTILS_DIR_BIN:$UTILS_DIR_HANDLERS:$UTILS_DIR_SCRIPTS"

# MMC/SD General Variables
if [ "$FORMAT" = "" ]
then
	echo "FATAL: Please specify the format of your device"
	echo "FATAL: Available values: dos, vfat, ext2, ext3"
	echo "e.g. export FORMAT=ext4"
	exit 1
fi

# Remove any error file
handlerError.sh "clean"

# End of file

