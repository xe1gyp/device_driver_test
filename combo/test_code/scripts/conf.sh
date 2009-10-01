#!/bin/sh

# TestSuite General Variables
export COMBO_POSTFIX=`date "+%Y%m%d-%H%M%S"`
export COMBO_ROOT=`pwd`

export COMBO_DIR_HELPER=${COMBO_ROOT}/helper
export COMBO_DIR_TMP=${COMBO_ROOT}/tmp
export COMBO_DIR_TEST=${COMBO_ROOT}/test
export COMBO_DIR_SCENARIOS="${COMBO_ROOT}/scenarios"

export COMBO_FILE_OUTPUT=${COMBO_ROOT}/output.$COMBO_POSTFIX
export COMBO_FILE_LOG=${COMBO_ROOT}/log.$COMBO_POSTFIX
export COMBO_FILE_TMP=${COMBO_DIR_TMP}/tmp.$COMBO_POSTFIX
export COMBO_FILE_CMD=cmd.$COMBO_POSTFIX

export COMBO_DURATION=""
export COMBO_PRETTY_PRT=""
export COMBO_VERBOSE=""
export COMBO_SCENARIO_NAMES=""
export COMBO_STRESS=""

export PATH="${COMBO_ROOT}:${COMBO_DIR_HELPER}:${PATH}"

# Utils General Variables
. ${COMBO_ROOT}/../../utils/configuration/general.configuration
export UTILS_DIR_BIN=${COMBO_ROOT}/../../utils/bin
export UTILS_DIR_HANDLERS=${COMBO_ROOT}/../../utils/handlers

# MMC/SD General Testsuite
export MMCSD_TESTSUITE=$COMBO_ROOT/../../mmc/scripts/
if [ ! -d "$MMCSD_TESTSUITE" ]
then
	echo "FATAL: MMC TestSuite cannot be found!"
	exit 1
else
  cd $MMCSD_TESTSUITE
  . $MMCSD_TESTSUITE/conf.sh
  cd $COMBO_ROOT
fi

# MTD General Testsuite
export MTD_TESTSUITE=$COMBO_ROOT/../../mtd/scripts/
if [ ! -d "$MTD_TESTSUITE" ]
then
	echo "FATAL: MTD TestSuite cannot be found!"
	exit 1
else
  cd $MTD_TESTSUITE
  . $MTD_TESTSUITE/conf.sh
  cd $COMBO_ROOT
fi

# Combo Specific Variables
export COMBO_MOUNTPOINT_NAND=/mnt/nand/
export COMBO_MOUNTPOINT_MMCSD=/mnt/mmcsd/
export COMBO_MOUNTPOINT_TMPFS=/mnt/tmpfs/

export COMBO_FILE_NAND=file.nand
export COMBO_FILE_MMCSD=file.mmcsd
export COMBO_FILE_TMPFS=file.tmpfs

export COMBO_NAND_TIME_CREATION=$COMBO_DIR_TMP/nand.time.creation
export COMBO_NAND_TIME_REMOVAL=$COMBO_DIR_TMP/nand.time.removal
export COMBO_MMCSD_TIME_CREATION=$COMBO_DIR_TMP/mmcsd.time.creation
export COMBO_MMCSD_TIME_REMOVAL=$COMBO_DIR_TMP/mmcsd.time.removal

# End of file