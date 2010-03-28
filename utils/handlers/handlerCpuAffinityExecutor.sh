#!/bin/sh

# =============================================================================
# Variables
# =============================================================================

LOCAL_PID=`echo $$`
echo $LOCAL_PID >> $HPP_LIST_OF_PIDS_RUNNING

LOCAL_INSTANCE=$1
LOCAL_COMMAND=$2
LOCAL_PROCESSOR=$3

# =============================================================================
# Functions
# =============================================================================

# None

# =============================================================================
# Main
# =============================================================================

echo -e "Info: Instance $LOCAL_INSTANCE | PID: $LOCAL_PID | Command: $LOCAL_COMMAND | Processor: $LOCAL_PROCESSOR"

taskset $LOCAL_PROCESSOR $LOCAL_COMMAND

if [ $? -ne 0 ]
then
  echo "$LOCAL_INSTANCE | $LOCAL_PID | $LOCAL_COMMAND" >> $HCA_LIST_OF_PIDS_FAILED
  exit 1
fi

wait

sed -i "/${LOCAL_PID}/d" $HCA_LIST_OF_PIDS_RUNNING
echo -e "Info: Instance $LOCAL_INSTANCE | PID $LOCAL_PID | Finished!"
echo "$LOCAL_INSTANCE | $LOCAL_PID | $LOCAL_COMMAND" >> $HCA_LIST_OF_COMMANDS_PASSED

# End of file