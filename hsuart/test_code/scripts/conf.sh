#!/bin/sh

# TestSuite General Variables
export HSUART_POSTFIX=`date "+%Y%m%d-%H%M%S"`
export HSUART_ROOT=`pwd`

export HSUART_DIR_BINARIES=${HSUART_ROOT}/../bin
export HSUART_DIR_MODULES=${HSUART_ROOT}/../modules
export HSUART_DIR_HELPER=${HSUART_ROOT}/helper
export HSUART_DIR_TMP=${HSUART_ROOT}/tmp
export HSUART_DIR_TEST=${HSUART_ROOT}/test
export HSUART_DIR_SCENARIOS="${HSUART_ROOT}/scenarios"

export HSUART_FILE_OUTPUT=${HSUART_ROOT}/output.$HSUART_POSTFIX
export HSUART_FILE_LOG=${HSUART_ROOT}/log.$HSUART_POSTFIX
export HSUART_FILE_TMP=${HSUART_DIR_TMP}/tmp.$HSUART_POSTFIX
export HSUART_FILE_CMD=cmd.$HSUART_POSTFIX

export HSUART_DURATION=""
export HSUART_PRETTY_PRT=""
export HSUART_VERBOSE=""
export HSUART_SCENARIO_NAMES=""
export HSUART_STRESS=""

export PATH="$PATH:$HSUART_ROOT:$HSUART_DIR_BINARIES:$HSUART_DIR_HELPER"

# Utils General Variables
export UTILS_DIR=$HSUART_ROOT/../../utils/
export UTILS_DIR_BIN=$UTILS_DIR/bin
export UTILS_DIR_HANDLERS=$UTILS_DIR/handlers
export UTILS_DIR_SCRIPTS=$UTILS_DIR/scripts

. $UTILS_DIR/configuration/general.configuration

export PATH="$PATH:$UTILS_DIR_BIN:$UTILS_DIR_HANDLERS:$UTILS_DIR_SCRIPTS"

if [ "$UART_TEST_PORT1" = "" ]
then
	echo "FATAL: Please specify the PORT to send the data"
	echo "e.g. export UART_TEST_PORT1=ttyO0"
	exit 1
fi

# End of file
