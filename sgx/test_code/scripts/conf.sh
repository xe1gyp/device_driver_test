#!/bin/sh

# TestSuite General Variables
export SGX_POSTFIX=`date "+%Y%m%d-%H%M%S"`
export SGX_ROOT=`pwd`
export SGX_DIR_BINARIES=${SGX_ROOT}/../bin
export SGX_DIR_MODULES=${SGX_ROOT}/../modules
export SGX_DIR_HELPER=${SGX_ROOT}/helper
export SGX_DIR_TMP=${SGX_ROOT}/tmp
export SGX_DIR_TEST=${SGX_ROOT}/test
export SGX_DIR_SCENARIOS="${SGX_ROOT}/scenarios"
export PATH="${SGX_ROOT}:${SGX_DIR_HELPER}:$SGX_DIR_BINARIES:${PATH}"

# Utils General Variables
export UTILS_DIR=$SGX_ROOT/../../utils/
export UTILS_DIR_BIN=$UTILS_DIR/bin
export UTILS_DIR_HANDLERS=$UTILS_DIR/handlers
export UTILS_DIR_SCRIPTS=$UTILS_DIR/scripts
export UTILS_DIR_MONKEY=$UTILS_DIR_SCRIPTS/monkey

export SGX_FILE_OUTPUT=${SGX_ROOT}/output.$SGX_POSTFIX
export SGX_FILE_LOG=${SGX_ROOT}/log.$SGX_POSTFIX
export SGX_FILE_TMP=${SGX_DIR_TMP}/tmp.$SGX_POSTFIX
export SGX_FILE_CMD=cmd.$SGX_POSTFIX

export SGX_DURATION=""
export SGX_PRETTY_PRT=""
export SGX_VERBOSE=""
export SGX_SCENARIO_NAMES=""
export SGX_STRESS=""
export SGX_PANIC=$SGX_DIR_TMP/sgx.panic

. $UTILS_DIR/configuration/general.configuration

export PATH="$PATH:$UTILS_DIR_BIN:$UTILS_DIR_HANDLERS:$UTILS_DIR_SCRIPTS:$UTILS_DIR_MONKEY"

# Specific SGX variables
export SGX_TOTAL_WALLPAPERS=10
export SGX_TOTAL_LIVE_WALLPAPERS=10
export SGX_LIVE_WALLPAPER_GALAXY=1
export SGX_LIVE_WALLPAPER_GRASS=2
export SGX_LIVE_WALLPAPER_MAGIC_SMOKE=3
export SGX_LIVE_WALLPAPER_MANY=4
export SGX_LIVE_WALLPAPER_NEXUS=5
export SGX_LIVE_WALLPAPER_POLAR_CLOCK=6
export SGX_LIVE_WALLPAPER_SPECTRUM=7
export SGX_LIVE_WALLPAPER_VU_METER=8
export SGX_LIVE_WALLPAPER_WATER=9
export SGX_LIVE_WALLPAPER_WAVE_FORM=10

# Remove any error file
handlerError.sh "clean"

# End of file
