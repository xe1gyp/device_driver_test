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

# General Variables
export TESTING_DRIVER=""

export KERNEL_MESSAGES=/var/log/messages

# Power Specific Variables
export ENABLE=1
export DISABLE=0
export WAKEUP_TIMER_DEFAULT_VALUE=1
export MEM="mem"

# All sysfs entries for Power
export SYSFS_POWER_ENTRY=/sys/power/
export STATE=$SYSFS_POWER_ENTRY/state
export SLEEP_WHILE_IDLE=$SYSFS_POWER_ENTRY/sleep_while_idle
export ENABLED_OFF_MODE=$SYSFS_POWER_ENTRY/enable_off_mode
export VOLTAGE_OFF_WHILE_IDLE=$SYSFS_POWER_ENTRY/voltage_off_while_idle
export WAKEUP_TIMER_SECONDS=$SYSFS_POWER_ENTRY/wakeup_timer_seconds

# SmartReflex
export SR_VDD1_AUTOCOMP=$SYSFS_POWER_ENTRY/sr_vdd1_autocomp
export SR_VDD2_AUTOCOMP=$SYSFS_POWER_ENTRY/sr_vdd2_autocomp

# OPP
export VDD1_OPP=$SYSFS_POWER_ENTRY/vdd1_opp
export VDD2_OPP=$SYSFS_POWER_ENTRY/vdd2_opp
export VDD1_LOCK=$SYSFS_POWER_ENTRY/vdd1_lock
export VDD2_LOCK=$SYSFS_POWER_ENTRY/vdd2_lock

# All sysfs entries for cpufreq
export CPU=cpu0
export SYSFS_CPUFREQ=/sys/devices/system/cpu/$CPU/cpufreq/

export SCALING_AVAILABLE_GOVERNORS=$SYSFS_CPUFREQ/scaling_available_governors
export SCALING_GOVERNOR=$SYSFS_CPUFREQ/scaling_governor

export SCALING_AVAILABLE_FREQUENCIES=$SYSFS_CPUFREQ/scaling_available_frequencies
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

# debugFileSystem.sh Script
export DEBUGFS_DIRECTORY=/debug
export DEBUGFS_PMCOUNT=$DEBUGFS_DIRECTORY/pm_debug/count

if [ ! `echo 1+1 | bc` ]; then
	echo "FATAL: BC is unavailable, cannot continue"
	return 1
fi