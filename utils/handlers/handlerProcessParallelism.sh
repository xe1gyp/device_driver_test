#!/bin/sh

LOCAL_OPERATION=$1

if [ "$LOCAL_OPERATION" = "clean" ]; then

  test -f $HPP_LIST_OF_COMMANDS && rm $HPP_LIST_OF_COMMANDS
  touch $HPP_LIST_OF_COMMANDS
  test -f $HPP_LIST_OF_PIDS_RUNNING && rm $HPP_LIST_OF_PIDS_RUNNING
  touch $HPP_LIST_OF_PIDS_RUNNING
  test -f $HPP_LIST_OF_PIDS_FAILED && rm $HPP_LIST_OF_PIDS_FAILED

  return 0

elif [ "$LOCAL_OPERATION" = "add" ]; then

  LOCAL_COMMAND=$2
  echo $LOCAL_COMMAND >> $HPP_LIST_OF_COMMANDS

elif [ "$LOCAL_OPERATION" = "execute" ]; then

  INSTANCE=0
  while read COMMAND
	do
	  INSTANCE=`expr $INSTANCE + 1`
    $UTILS_DIR_HANDLERS/handlerProcessParallelismExecutor.sh "$INSTANCE" "$COMMAND" &
  done < $HPP_LIST_OF_COMMANDS

  wait

  if [ -f $HPP_LIST_OF_PIDS_FAILED ]
  then
    echo -e "\nError: The following Instance | PID | Command failed!"
    cat $HPP_LIST_OF_PIDS_FAILED
    echo
    return 1
  fi

  echo -e "\nInfo: $INSTANCE proccesses were executed without any failure\n"

fi

# End of file