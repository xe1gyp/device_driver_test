#!/bin/sh

# Testsuite variables
export POSTFIX=`date "+%Y%m%d-%H%M%S"`
export TESTROOT=${PWD}
export TESTBIN=${PWD}/../bin
export UTILBIN=${PWD}/../../utils/bin
export TESTMODS=${PWD}/../mods
export TESTSCRIPT=${PWD}/helper
export TMPBASE=${TESTROOT}/tmp
export TMPFILE=${TMPBASE}/tmp.$POSTFIX
export CMDFILE=cmd.$POSTFIX
export TESTDIR=${TESTROOT}/test
export PRETTY_PRT=""
export VERBOSE=""
export OUTPUTFILE=${TESTROOT}/output.$POSTFIX
export LOGFILE=${TESTROOT}/log.$POSTFIX
export DURATION=""
export PATH="${PATH}:${TESTROOT}:${TESTBIN}:${TESTSCRIPT}"
export TC_SCENARIO="${TESTROOT}/scenarios"
export SCENARIO_NAMES=""
export UART_APPLICATIONS=${PWD}/../utils/hsuart_applications/

# Ts application
export TS_UART=${UART_APPLICATIONS}/ts_uart

#Specific HUART variables
export SEND="s"
export RECEIVE="r"
export SAMPLE_FILE=${PWD}/../utils/sample_file
export NO_FLOW_CONTROL=0
export AUTOCTS_AUTORTS=1
export XON_XOFF=2

#Negative TC variables
export NORMAL=0
export DISCONNECT=1
export WRONG_BAUDRATE=2
export WRONG_FLOW_CONTROL=3
