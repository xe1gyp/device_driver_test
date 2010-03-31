#!/bin/sh

# =============================================================================
# Variables
# =============================================================================

LOCAL_SCRIPT_PID=`echo $$`

LOCAL_COMMAND_INSTANCE=$1
LOCAL_COMMAND_LINE=$2
LOCAL_COMMAND_PROCESSOR=$3
LOCAL_COMMAND_DELAY=$4
# =============================================================================
# Functions
# =============================================================================

# None

# =============================================================================
# Main
# =============================================================================

sleep $LOCAL_COMMAND_DELAY

echo "Info: Started! > Instance $LOCAL_COMMAND_INSTANCE | PID $LOCAL_SCRIPT_PID | Mask Processor $LOCAL_COMMAND_PROCESSOR | Command $LOCAL_COMMAND_LINE"
echo $LOCAL_SCRIPT_PID >> $HCA_LIST_PIDS_TOTALS

taskset $LOCAL_COMMAND_PROCESSOR $LOCAL_COMMAND_LINE

if [ $? -ne 0 ]
then
	echo "$LOCAL_COMMAND_INSTANCE | $LOCAL_SCRIPT_PID | $LOCAL_COMMAND_LINE" >> $HCA_LIST_CMDS_FAILED
	exit 1
fi

wait

sed -i "/${LOCAL_SCRIPT_PID}/d" $HCA_LIST_PIDS_TOTALS

echo "Info: Finished! > Instance $LOCAL_COMMAND_INSTANCE | PID $LOCAL_SCRIPT_PID | Command $LOCAL_COMMAND_LINE"
echo "$LOCAL_COMMAND_INSTANCE | $LOCAL_SCRIPT_PID | $LOCAL_COMMAND_LINE" >> $HCA_LIST_CMDS_PASSED

# End of file
