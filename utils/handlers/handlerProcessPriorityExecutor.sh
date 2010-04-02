#!/bin/sh

# =============================================================================
# Variables
# =============================================================================

LOCAL_SCRIPT_PID=`echo $$`

LOCAL_COMMAND_INSTANCE=$1
LOCAL_COMMAND_LINE=$2
LOCAL_COMMAND_REPEATABILITY=$3
LOCAL_COMMAND_PRIORITY=$4
LOCAL_COMMAND_DELAY=$5

# =============================================================================
# Functions
# =============================================================================

# None

# =============================================================================
# Main
# =============================================================================

sleep $LOCAL_COMMAND_DELAY

echo "Info: Started! > Instance $LOCAL_COMMAND_INSTANCE | PID $LOCAL_SCRIPT_PID | Command $LOCAL_COMMAND_LINE"
echo $LOCAL_SCRIPT_PID >> $HPPR_LIST_PIDS_TOTALS

cat /proc/$LOCAL_SCRIPT_PID/sched | grep se.sum_exec_runtime > $HPPR_PROCFS_PID_SCHED_START.$LOCAL_COMMAND_INSTANCE

scheduler_priority_set $LOCAL_SCRIPT_PID $LOCAL_COMMAND_PRIORITY

LOCAL_COUNT=0
while [ $LOCAL_COUNT -lt $LOCAL_COMMAND_REPEATABILITY ]
do
	eval $LOCAL_COMMAND_LINE
	if [ $? -ne 0 ]
	then
		echo "$LOCAL_COMMAND_INSTANCE:$LOCAL_SCRIPT_PID:$LOCAL_COMMAND_LINE" >> $HPPR_LIST_CMDS_FAILED
		exit 1
	fi

	LOCAL_COUNT=`expr $LOCAL_COUNT + 1`
	echo "Info: Instance $LOCAL_COMMAND_INSTANCE | PID $LOCAL_SCRIPT_PID | Priority $LOCAL_COMMAND_PRIORITY | Count $LOCAL_COUNT of $LOCAL_COMMAND_REPEATABILITY"
done

cat /proc/$LOCAL_SCRIPT_PID/sched | grep se.sum_exec_runtime > $HPPR_PROCFS_PID_SCHED_CURRENT.$LOCAL_COMMAND_INSTANCE

sed -i "/${LOCAL_SCRIPT_PID}/d" $HPPR_LIST_PIDS_TOTALS

echo "Info: Finished! > Instance $LOCAL_COMMAND_INSTANCE | PID $LOCAL_SCRIPT_PID | Command $LOCAL_COMMAND_LINE"
echo "$LOCAL_COMMAND_INSTANCE:$LOCAL_SCRIPT_PID:$LOCAL_COMMAND_LINE" >> $HPPR_LIST_CMDS_PASSED

# End of file
