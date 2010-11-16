#!/bin/sh

# TestSuite General Variables
export PMD_POSTFIX=`date "+%Y%m%d-%H%M%S"`
export PMD_ROOT=`pwd`

export PMD_DIR_BINARIES=${PMD_ROOT}/../bin
export PMD_DIR_MODULES=${PMD_ROOT}/../modules
export PMD_DIR_HELPER=${PMD_ROOT}/helper
export PMD_DIR_TMP=${PMD_ROOT}/tmp
export PMD_DIR_TEST=${PMD_ROOT}/test
export PMD_DIR_SCENARIOS="${PMD_ROOT}/scenarios"

export PMD_FILE_OUTPUT=${PMD_ROOT}/output.$PMD_POSTFIX
export PMD_FILE_LOG=${PMD_ROOT}/log.$PMD_POSTFIX
export PMD_FILE_TMP=${PMD_DIR_TMP}/tmp.$PMD_POSTFIX
export PMD_FILE_CMD=cmd.$PMD_POSTFIX

export PMD_DURATION=""
export PMD_PRETTY_PRT=""
export PMD_VERBOSE=""
export PMD_SCENARIO_NAMES=""
export PMD_STRESS=""
export PMD_PANIC=$PMD_DIR_TMP/ethernet.panic

export PATH="${PMD_ROOT}:${PMD_DIR_HELPER}:$PMD_DIR_BINARIES:${PATH}"

# Utils General Variables
export UTILS_DIR=$PMD_ROOT/../../utils/
export UTILS_DIR_BIN=$UTILS_DIR/bin
export UTILS_DIR_HANDLERS=$UTILS_DIR/handlers
export UTILS_DIR_SCRIPTS=$UTILS_DIR/scripts
export UTILS_DIR_MONKEY=$UTILS_DIR/monkey

. $UTILS_DIR/configuration/general.configuration

export PATH="$PATH:$UTILS_DIR_BIN:$UTILS_DIR_HANDLERS:$UTILS_DIR_SCRIPTS"

# Specific pmd Variables
export PMD_WAKEUP_TIMER_DEFAULT=1

# oppControl.sh, need update
export VDD1_OPP1_FREQ=300000
export VDD1_OPP2_FREQ=600000
export VDD1_OPP3_FREQ=800000
export VDD1_OPP4_FREQ=1000000

export DSP_FREQ_TMP=$PM_DEBUGFS_DIRECTORY/clock/virt_26m_ck/osc_sys_ck/sys_ck
export DSP_FREQ=$DSP_FREQ_TMP/dpll2_ck/dpll2_m2_ck/iva2_ck/rate

if [ ! `echo 1+1 | bc` ]; then
	echo "FATAL: BC is unavailable, cannot continue"
	return 1
fi

# Remove any error file
handlerError.sh "clean"

# End of file
