#!/bin/sh
######################################################################
# Configuration script
# Author  : Misael Lopez Cruz
# Date    : 04 Jul 07
# Description: Configuration script with user specified value for some
# environment variables
######################################################################

# These variables dont' need to be modified
export POSTFIX=`date "+%Y%m%d-%H%M%S"`
export TESTROOT=${PWD}
export TESTBIN=${PWD}/../bin
export UTILBIN=${PWD}/../../utils/bin
export TESTMODS=${PWD}/../modules
export TESTSCRIPT=${PWD}/helper
export TMPBASE=${TESTROOT}/tmp
export TMPFILE=${TMPBASE}/tmp.$POSTFIX
export CMDFILE=cmd.$POSTFIX
export TESTDIR=${TESTROOT}/test
export PRETTY_PRT=""
export VERBOSE=""
export OUTPUTFILE=${TESTROOT}/output.$POSTFIX
export LOGFILE=${TESTROOT}/log.$POSTFIX
export DURATION="1h"
export PATH="${PATH}:${TESTROOT}:${TESTBIN}:${TESTSCRIPT}"
export TC_SCENARIO="${TESTROOT}/scenarios"
export SCENARIO_NAMES=""

# Utilities
export WAIT_ANSWER=${UTILBIN}/akey

# Logs
export LOG_OUTPUT="$TESTROOT/log.custom.powermanagement"

# Specific Variables
export NAME=""
export MESSAGE_DELAY=5
export DESCRIPTION=""

export MIN_INT_PER_SECOND=0
export MAX_INT_PER_SECOND=1500

# Power Management
#export DPM_SCRIPT=$TESTSCRIPT/dpm.bash
export DPM_SCRIPT=$TESTSCRIPT/dpm_test_8x.bash
export PARAMETERS=""
export TIME_TO_REPEAT=0
export QUIT_OPTION=0
export SCALE_OPTION=6
export SLEEP_OPTION=4
export SLOW_OPERATING_POINT=s
export FAST_OPERATING_POINT=f
export STRESS_OPERATING_POINTS=7
export STRESS_FREQUENCIES=6

export UTILOMAP3=${TESTROOT}/../utils/omap3

# General Variables
export TESTING_DRIVER=""

#export KERNEL_MESSAGES=/var/log/messages
export KERNEL_MESSAGES=dmesg


# Power Specific Variables
export ENABLE=1
export DISABLE=0
export WAKEUP_TIMER_DEFAULT_VALUE=1
export MEM="mem"

# debugFileSystem.sh Script
export DEBUGFS_DIRECTORY=/debug
export DEBUGFS_PMCOUNT=$DEBUGFS_DIRECTORY/pm_debug/count

# All sysfs entries for Power
export SYSFS_POWER_ENTRY=/sys/power/
export DEBUGFS_POWER_ENTRY=$DEBUGFS_DIRECTORY/pm_debug
export STATE=$SYSFS_POWER_ENTRY/state
export SLEEP_WHILE_IDLE=$DEBUGFS_POWER_ENTRY/sleep_while_idle
export ENABLED_OFF_MODE=$DEBUGFS_POWER_ENTRY/enable_off_mode
export VOLTAGE_OFF_WHILE_IDLE=$DEBUGFS_POWER_ENTRY/voltage_off_while_idle
export WAKEUP_TIMER_SECONDS=$DEBUGFS_POWER_ENTRY/wakeup_timer_seconds

# SmartReflex
export SR_VDD1_AUTOCOMP=$DEBUGFS_POWER_ENTRY/sr1_autocomp
export SR_VDD2_AUTOCOMP=$DEBUGFS_POWER_ENTRY/sr2_autocomp


# OPP
export VDD1_OPP=$SYSFS_POWER_ENTRY/vdd1_opp
export VDD2_OPP=$SYSFS_POWER_ENTRY/vdd2_opp
export VDD1_LOCK=$SYSFS_POWER_ENTRY/vdd1_lock
export VDD2_LOCK=$SYSFS_POWER_ENTRY/vdd2_lock

# All sysfs entries for cpufreq
export CPU=cpu0
export SYSFS_CPUFREQ=/sys/devices/system/cpu/$CPU/cpufreq
export DSP_FREQ_TMP=$DEBUGFS_DIRECTORY/clock/virt_26m_ck/osc_sys_ck/sys_ck
export DSP_FREQ=$DSP_FREQ_TMP/dpll2_ck/dpll2_m2_ck/iva2_ck/rate

export SCALING_AVAILABLE_GOVERNORS=$SYSFS_CPUFREQ/scaling_available_governors
export SCALING_GOVERNOR=$SYSFS_CPUFREQ/scaling_governor

export SCALING_AVAILABLE_FREQUENCIES=$SYSFS_CPUFREQ/\
scaling_available_frequencies
export SCALING_CUR_FREQ=$SYSFS_CPUFREQ/scaling_cur_freq
export SCALING_SET_SPEED=$SYSFS_CPUFREQ/scaling_setspeed

export STATS_TOTAL_TRANS=$SYSFS_CPUFREQ/stats/total_trans
export STATS_TIME_IN_STATE=$SYSFS_CPUFREQ/stats/time_in_state


export GOVERNORS_LIST_OK=$TMPBASE/governors.list.ok
export GOVERNORS_LIST_ERROR=$TMPBASE/governors.list.error

export GOVERNOR_CONSERVATIVE=conservative
export GOVERNOR_ONDEMAND=ondemand
export GOVERNOR_POWERSAVE=powersave
export GOVERNOR_USERSPACE=userspace
export GOVERNOR_PERFORMANCE=performance

export VDD1_OPP1_FREQ=300000
export VDD1_OPP2_FREQ=600000
export VDD1_OPP3_FREQ=800000
export VDD1_OPP4_FREQ=1000000

export AUDIO_SAMPLE=/testsuites/audio-alsa/scripts/helper/audio-samples

if [ ! -e "$AUDIO_SAMPLE" ]; then
	echo "FATAL: Audio Samples cannot be found -> $AUDIO_SAMPLE"
	return 1
fi

if [ ! `echo 1+1 | bc` ]; then
	echo "FATAL: BC is unavailable, cannot continue"
	return 1
fi

