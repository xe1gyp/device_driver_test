#!/bin/sh

# =============================================================================
# Variables
# =============================================================================

LOCAL_SCRIPT_PID=`echo $$`

LOCAL_COMMAND_NUMBER=$1
LOCAL_COMMAND_LINE=$2
LOCAL_EXECUTION_TIMES=$3
LOCAL_COMMAND_PRIORITY=$4

# =============================================================================
# Functions
# =============================================================================

# None

# =============================================================================
# Main
# =============================================================================

cat /proc/$LOCAL_SCRIPT_PID/sched | grep se.sum_exec_runtime > $HPP_PROCFS_PID_SCHED_START.$LOCAL_COMMAND_NUMBER

$UTILS_DIR_BIN/scheduler_priority_set $LOCAL_SCRIPT_PID $LOCAL_COMMAND_PRIORITY

count=0
while [ $count -lt $LOCAL_EXECUTION_TIMES ]
do
	eval $LOCAL_COMMAND_LINE
	count=`expr $count + 1`
  echo -e "Info: Program $LOCAL_COMMAND_NUMBER | PID $LOCAL_SCRIPT_PID | Priority $LOCAL_COMMAND_PRIORITY | Count $count of $LOCAL_EXECUTION_TIMES"
done

cat /proc/$LOCAL_SCRIPT_PID/sched | grep se.sum_exec_runtime > $HPP_PROCFS_PID_SCHED_CURRENT.$LOCAL_COMMAND_NUMBER

# End of file
