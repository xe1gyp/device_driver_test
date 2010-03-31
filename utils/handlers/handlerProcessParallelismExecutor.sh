#!/bin/sh

# =============================================================================
# Variables
# =============================================================================

LOCAL_SCRIPT_PID=`echo $$`

LOCAL_COMMAND_INSTANCE=$1
LOCAL_COMMAND_LINE=$2
LOCAL_COMMAND_DELAY=$3

# =============================================================================
# Functions
# =============================================================================

# None

# =============================================================================
# Main
# =============================================================================

sleep $LOCAL_COMMAND_DELAY

echo -e "Info: Started! > Instance $LOCAL_COMMAND_INSTANCE | PID $LOCAL_SCRIPT_PID | Command $LOCAL_COMMAND_LINE"
echo $LOCAL_SCRIPT_PID >> $HPPA_LIST_PIDS_TOTALS

$LOCAL_COMMAND_LINE

if [ $? -ne 0 ]
then
	echo "$LOCAL_COMMAND_INSTANCE:$LOCAL_SCRIPT_PID:$LOCAL_COMMAND_LINE" >> $HPPA_LIST_CMDS_FAILED
	exit 1
fi

sed -i "/${LOCAL_SCRIPT_PID}/d" $HPPA_LIST_PIDS_TOTALS

echo "Info: Finished! > Instance $LOCAL_COMMAND_INSTANCE | PID $LOCAL_SCRIPT_PID | Command $LOCAL_COMMAND_LINE"
echo "$LOCAL_COMMAND_INSTANCE:$LOCAL_SCRIPT_PID:$LOCAL_COMMAND_LINE" >> $HPPA_LIST_CMDS_PASSED

# End of file
