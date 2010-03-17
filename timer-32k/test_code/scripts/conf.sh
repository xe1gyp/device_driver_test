#!/bin/sh

export POSTFIX=`date "+%Y%m%d-%H%M%S"`
export TESTROOT=${PWD}
export TESTBIN=${TESTROOT}/../bin
export UTILBIN=${TESTROOT}/../../utils/bin
export TESTMODS=${TESTROOT}/../modules
export TESTSCRIPT=${TESTROOT}/helper
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

#GP Timer Variables

if [ -f /var/log/dmesg ]; then
        alias dmesg='cat /var/log/dmesg'
fi

GPTIMER=`dmesg | grep -i gptimer`
export RESERVED_GPTIMER=`echo $GPTIMER | awk '{print $4}'| cut -c '8'`
export MAXGPTIMERS=12
