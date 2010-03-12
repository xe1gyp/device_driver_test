#!/bin/sh

LOCAL_OPERATION=$1

test -f /proc/1/sched
if [ $? -eq 1 ]
then
	echo "Fatal: missing /proc/<process>/sched, cannot continue, directory looks like"
	ls /proc/1/
	return 1
fi

if [ "$LOCAL_OPERATION" = "run" ]; then

	LOCAL_COMMAND_LINE=$2
	$LOCAL_COMMAND_LINE

elif [ "$LOCAL_OPERATION" = "add" ]; then
	LOCAL_COMMAND_NUMBER=$2
	LOCAL_COMMAND_LINE=$3
	LOCAL_COMMAND_PRIORITY=$4

	echo $LOCAL_COMMAND_LINE > $HPP_COMMAND_LINE.$LOCAL_COMMAND_NUMBER
	echo $LOCAL_COMMAND_PRIORITY > $HPP_COMMAND_PRIORITY_VALUE.$LOCAL_COMMAND_NUMBER

elif [ "$LOCAL_OPERATION" = "execute" ]; then

	LOCAL_TIMES=$2

	tempPriorityValue1=`cat $HPP_COMMAND_PRIORITY_VALUE.1`
	tempPriorityValue2=`cat $HPP_COMMAND_PRIORITY_VALUE.2`

	if [ $tempPriorityValue1 -lt $tempPriorityValue2 ]; then
    echo 1 > $HPP_COMMAND_PRIORITY_HIGHER
	elif [ $tempPriorityValue1 -gt $tempPriorityValue2 ]; then
    echo 2 > $HPP_COMMAND_PRIORITY_HIGHER
  elif [ $tempPriorityValue1 -eq $tempPriorityValue2 ]; then
    echo equal > $HPP_COMMAND_PRIORITY_HIGHER
	fi

  $UTILS_DIR_HANDLERS/handlerProcessPriorityExecutor.sh "1" "`cat $HPP_COMMAND_LINE.1`" "$LOCAL_TIMES" "`cat $HPP_COMMAND_PRIORITY_VALUE.1`" &
  $UTILS_DIR_HANDLERS/handlerProcessPriorityExecutor.sh "2" "`cat $HPP_COMMAND_LINE.2`" "$LOCAL_TIMES" "`cat $HPP_COMMAND_PRIORITY_VALUE.2`" &

  wait

elif [ "$LOCAL_OPERATION" = "verify" ]; then

	echo -e "\nInfo: Program 1 | Initial | Final"
	echo -e "`cat $HPP_PROCFS_PID_SCHED_START.1`\n`cat $HPP_PROCFS_PID_SCHED_CURRENT.1`\n"
	echo -e "\nInfo: Program 2 | Initial | Final"
	echo -e "`cat $HPP_PROCFS_PID_SCHED_START.2`\n`cat $HPP_PROCFS_PID_SCHED_CURRENT.2`\n"

	val1=`cat $HPP_PROCFS_PID_SCHED_START.1 | awk '{print $3}'`
	val2=`cat $HPP_PROCFS_PID_SCHED_CURRENT.1  | awk '{print $3}'`
	fvalue1=`echo "$val2-$val1" | bc`
	fvalue1=`echo $fvalue1 | awk '{printf "%.0f\n", $1}'`
	echo "Info: Program 1 | The final computed value for process 1 is $fvalue1"

	val1=`cat $HPP_PROCFS_PID_SCHED_START.2 | awk '{print $3}'`
	val2=`cat $HPP_PROCFS_PID_SCHED_CURRENT.2  | awk '{print $3}'`
	fvalue2=`echo "$val2-$val1" | bc`
	fvalue2=`echo $fvalue2 | awk '{printf "%.0f\n", $1}'`
	echo "Info: Program 2 | The final computed value for process 2 is $fvalue2"

	LOCAL_PROCESS_TO_FINISH_FIRST=`cat $HPP_COMMAND_PRIORITY_HIGHER`

  if [ "$LOCAL_PROCESS_TO_FINISH_FIRST" = "equal" ]; then
    echo -e "\nInfo: We don't know what process should have finished first\n"
    return 0
  else
    echo -e "\nInfo: Process with higher priority is $LOCAL_PROCESS_TO_FINISH_FIRST"
	  if [ "$LOCAL_PROCESS_TO_FINISH_FIRST" = "1" ]; then
		  if [ "$fvalue1" -lt "$fvalue2" ]; then
			echo -e "Info: Passed\n"
		else
			echo -e "Info: Failed!\n"
			return 1
		fi
	  elif [ "$LOCAL_PROCESS_TO_FINISH_FIRST" = "2" ]; then
		if [ "$fvalue2" -lt "$fvalue1" ]; then
			echo -e "Info: Passed!\n"
		else
			echo -e "Info: Failed!\n"
			return 1
		fi
	fi
	fi

fi

# End of file
