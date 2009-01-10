#!/bin/sh
###################################################################################################################
# Configuration script
# Author  : Misael Lopez Cruz
# Date    : September 19, 2006
# Description: Configuration script with user specified value for some environment variables
###################################################################################################################

# These variables don't need to be modified
export POSTFIX=`date "+%Y%m%d-%H%M%S"`
export TESTROOT=${PWD}
export TESTBIN=${PWD}/../bin
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
export INTERACTIVE=""
export STRESS=""
export UTILBIN=${TESTROOT}/../../utils/bin
export STRESSARG=""

export ENABLECOL=${TESTBIN}/enablecolkey
export SETBGCOLOR=${TESTBIN}/setbgcolor
export SETCOLORENV=${TESTBIN}/setcolorconv
export SETCROP=${TESTBIN}/setcrop
export SETDEFCOLON=${TESTBIN}/setdefcolcon
export SETIMG=${TESTBIN}/setimg
export SETLINK=${TESTBIN}/setlink
export SETMIRROR=${TESTBIN}/setmirror
export SETROTATION=${TESTBIN}/setrotation
export SETTRANSCK=${TESTBIN}/settransck
export SETWIN=${TESTBIN}/setwin
export SHOWINFO=${TESTBIN}/showinfo
export STREAMING=${TESTBIN}/streaming

export VIDEOFILES=${TESTROOT}/../utils/videofiles/
export MESSAGE_DELAY=1
export WAIT_ANSWER=${UTILBIN}/akey
export WAIT_KEY=${UTILBIN}/waitkey
export HELP=""
export TESTCASE=""
