###############
# HDQ Configuration Script
###############
# if we have a working rtc, this is good.. else use next line
export POSTFIX=`date "+%Y%m%d-%H%M%S"`

# Load up the defaults
export TESTROOT=${PWD}
export TESTBIN=${PWD}/../bin
export TESTLIB=${PWD}/../lib
export TESTSCRIPT=${PWD}/helper
export MODDIR=${PWD}/../mods
export TMPBASE=${TESTROOT}/tmp
export TMPFILE=${TMPBASE}/tmp.$POSTFIX
export CMDFILE=cmd.$POSTFIX
export TESTDIR=${TESTROOT}/test
export PRETTY_PRT=""
export VERBOSE=""
export OUTPUTFILE=${TESTROOT}/output.$POSTFIX
export LOGFILE=${TESTROOT}/log.$POSTFIX
export PATH="${TESTROOT}:${TESTBIN}:${TESTSCRIPT}:${PATH}"
export TC_SCENARIO="${TESTROOT}/scenarios"
export SCENARIO_NAMES=""
export INTERACTIVE=""
export STRESS=""
export UTILBIN=${TESTROOT}/../../utils/bin

# Utilities
export WAIT_ANSWER=$UTILBIN/akey

# Logs
export LOG_OUTPUT=$TESTROOT/log.custom.hdq

# Modules
export HDQ_MODULE=$MODDIR/omap-hdq_test.ko

# Specific Variables
export DELAY=5

# Kernel version
export KERNEL_RELEASE_LABEL_DIR=${TESTROOT}/../../utils/kernel_release_labels
export KERNEL_RELEASE_LABEL=`uname -r`
source $KERNEL_RELEASE_LABEL_DIR/$KERNEL_RELEASE_LABEL "hdq"
