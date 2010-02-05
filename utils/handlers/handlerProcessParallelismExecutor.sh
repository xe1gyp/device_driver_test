#!/bin/sh

LOCAL_PID=`echo $$`
echo $LOCAL_PID >> $HPP_LIST_OF_PIDS_RUNNING

LOCAL_INSTANCE=$1
LOCAL_COMMAND=$2

echo -e "Info: Instance $LOCAL_INSTANCE | PID: $LOCAL_PID | Command: $LOCAL_COMMAND"

$LOCAL_COMMAND

if [ $? -ne 0 ]
then
  echo "$LOCAL_INSTANCE | $LOCAL_PID | $LOCAL_COMMAND" >> $HPP_LIST_OF_PIDS_FAILED
fi

sed -i "/${LOCAL_PID}/d" $HPP_LIST_OF_PIDS_RUNNING
echo -e "Info: Instance $LOCAL_INSTANCE | PID $LOCAL_PID | Finished!"

# End of file