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

export PATH="$PATH:$COMBO_ROOT:$COMBO_DIR_HELPER"

# Utils General Variables
export UTILS_DIR=$COMBO_ROOT/../../utils/
export UTILS_DIR_BIN=$UTILS_DIR/bin
export UTILS_DIR_HANDLERS=$UTILS_DIR/handlers
export UTILS_DIR_SCRIPTS=$UTILS_DIR/scripts

. $UTILS_DIR/configuration/general.configuration

export PATH="$PATH:$UTILS_DIR_BIN:$UTILS_DIR_HANDLERS:$UTILS_DIR_SCRIPTS"


# Extra Applications
$UTILS_DIR_BIN/top2 -h
if [ $? -ne 0 ]; then
  echo -e "FATAL: top2 application is not found, cannot continue!"
  exit 1
fi

$UTILS_DIR_BIN/time --version
if [ $? -ne 0 ]; then
  echo -e "FATAL: GNU time is not found, cannot continue!"
  exit 1
fi

arecord --version
if [ $? -ne 0 ]; then
  echo -e "FATAL: arecord application is not found, cannot continue!"
  exit 1
fi

aplay --version
if [ $? -ne 0 ]; then
  echo -e "FATAL: aplay application is not found, cannot continue!"
  exit 1
fi

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
#export MTD_TESTSUITE=$COMBO_ROOT/../../mtd/scripts/
#if [ ! -d "$MTD_TESTSUITE" ]
#then
#	echo "FATAL: MTD TestSuite cannot be found!"
#	exit 1
#else
#  cd $MTD_TESTSUITE
#  . $MTD_TESTSUITE/conf.sh
#  cd $COMBO_ROOT
#fi

# End of file