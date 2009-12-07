#!/bin/sh
set -x

LOCAL_COMMAND=$1
LOCAL_WAKEUP_TIME=$2

if [ "$LOCAL_COMMAND" = "suspend" ]; then

  echo > $KERNEL_MESSAGES
  echo $LOCAL_WAKEUP_TIME > $WAKEUP_TIMER_SECONDS
  echo $MEM > $STATE
  echo $DISABLE > $WAKEUP_TIMER_SECONDS
  sleep 25
  cat $KERNEL_MESSAGES | grep "Successfully"
  
  if [ $? != 0 ]; then
    echo "Error: All powerdomains did not enter target state"
    cat $KERNEL_MESSAGES
    exit 1
  fi
  
else
  echo "Command in suspendResume is not supported"
  exit 1
fi

# End of file
