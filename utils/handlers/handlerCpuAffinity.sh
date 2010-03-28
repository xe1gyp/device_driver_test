#!/bin/sh

# =============================================================================
# Variables
# =============================================================================

LOCAL_OPERATION=$1

# =============================================================================
# Functions
# =============================================================================

# None

# =============================================================================
# Main
# =============================================================================

handlerError.sh "test"
if [ $? -eq 1 ]; then
  return 1
fi

echo "\nInfo: Command > $LOCAL_COMMAND_LINE\n"

if [ "$LOCAL_OPERATION" = "switch" ]; then

  LOCAL_COMMAND_LINE=$2
  LOCAL_EXECUTION_TIMES=$3
  LOCAL_TIME_TO_WAIT=$4

  $LOCAL_COMMAND_LINE &
	LOCAL_COMMAND_PID=`echo $!`
	
	count=1
	processor=1
	while [ $count -le $LOCAL_EXECUTION_TIMES ]
	do

		echo -e "\nInfo: PID $LOCAL_COMMAND_PID | Processor $processor | Count $count of $LOCAL_EXECUTION_TIMES"
		if [ ! -d "/proc/$LOCAL_COMMAND_PID" ]
		then
			break
		fi

		rem=$(( $count % 2 ))

		if [ $rem -eq 0 ]
		then
			processor=1
			$UTILS_DIR_BIN/taskset -p $processor $LOCAL_COMMAND_PID
		else
			processor=2
			$UTILS_DIR_BIN/taskset -p $processor $LOCAL_COMMAND_PID
		fi

		if [ $? -ne 0 ]
		then
			echo -e "Error: Could not set cpu affinity for processor $processor!"
			# FixMe
			# return 1
		fi

		count=`expr $count + 1`
		sleep $LOCAL_TIME_TO_WAIT

	done

  echo -e "\nInfo: Command > $LOCAL_COMMAND_LINE\n"
  echo -e "Info: Waiting for it to finish..."
	wait
	echo -e "Info: Done!\n"

elif [ "$LOCAL_OPERATION" = "clean" ]; then

  test -f $HCA_LIST_OF_COMMANDS && rm $HCA_LIST_OF_COMMANDS
  touch $HCA_LIST_OF_COMMANDS
  test -f $HCA_LIST_OF_PIDS_RUNNING && rm $HCA_LIST_OF_PIDS_RUNNING
  touch $HCA_LIST_OF_PIDS_RUNNING
  test -f $HCA_LIST_OF_COMMANDS_PASSED && rm $HCA_LIST_OF_COMMANDS_PASSED
  touch $HCA_LIST_OF_COMMANDS_PASSED

  touch $HCA_LIST_OF_PIDS_FAILED
  test -f $HCA_LIST_OF_PIDS_FAILED && rm $HCA_LIST_OF_PIDS_FAILED

elif [ "$LOCAL_OPERATION" = "assign" ]; then

  LOCAL_COMMAND=$2
  LOCAL_PROCESSOR=$3
  echo "$LOCAL_COMMAND:$LOCAL_PROCESSOR" >> $HCA_LIST_OF_COMMANDS

elif [ "$LOCAL_OPERATION" = "random" ]; then

  LOCAL_COMMAND=$2

  LOCAL_PROCESSOR=0
  while [  $LOCAL_PROCESSOR -eq 0 ]; do
    LOCAL_NUMBER=`dd if=/dev/urandom count=1 2> /dev/null | cksum | cut -f1 -d" "`
    LOCAL_PROCESSOR=`echo "$LOCAL_NUMBER%4" | bc`
  done

  echo "$LOCAL_COMMAND:$LOCAL_PROCESSOR" >> $HCA_LIST_OF_COMMANDS

elif [ "$LOCAL_OPERATION" = "execute" ]; then

  LOCAL_INSTANCE=0
  while read LOCAL_LINE
	do
	  LOCAL_INSTANCE=`expr $LOCAL_INSTANCE + 1`
	  LOCAL_COMMAND=`echo $LOCAL_LINE | cut -d: -f1`
	  LOCAL_PROCESSOR=`echo $LOCAL_LINE | cut -d: -f2`
	  echo "$LOCAL_INSTANCE $LOCAL_COMMAND $LOCAL_PROCESSOR"
    handlerCpuAffinityExecutor.sh "$LOCAL_INSTANCE" "$LOCAL_COMMAND" "$LOCAL_PROCESSOR" &
  done < $HCA_LIST_OF_COMMANDS

  wait

  echo -e "\nInfo: The following instance | pid | command passed\n"
  cat $HCA_LIST_OF_COMMANDS_PASSED
  echo

  if [ -f $HCA_LIST_OF_PIDS_FAILED ]
  then
    echo -e "\nInfo: The following instance | pid | command failed!\n"
    cat $HCA_LIST_OF_PIDS_FAILED
    echo
    exit 1
  fi

fi

# End of file