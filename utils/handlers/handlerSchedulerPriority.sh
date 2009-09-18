#!/bin/sh

set -x

LOCAL_COMMAND=$1

if [ "$LOCAL_COMMAND" = "program" ]; then
	LOCAL_NUMBER_PROGRAM=$2
	LOCAL_PROGRAM=$3
	LOCAL_PRIORITY=$4

	echo $LOCAL_PROGRAM > $PRIORITY_PROGRAM.$LOCAL_NUMBER_PROGRAM
	echo $LOCAL_PRIORITY > $PRIORITY_VALUE.$LOCAL_NUMBER_PROGRAM

elif [ "$LOCAL_COMMAND" = "start" ]; then
        
	LOCAL_TIMES=$2

	(($UTILHANDLERS/handlerPidExecutionTime.sh "1" "`cat $PRIORITY_PROGRAM.1`" "$LOCAL_TIMES" "`cat $PRIORITY_VALUE.1`" &) && ($UTILHANDLERS/handlerPidExecutionTime.sh "2" "`cat $PRIORITY_PROGRAM.2`" "$LOCAL_TIMES" "`cat $PRIORITY_VALUE.2`"))
	
	return 0

elif [ "$LOCAL_COMMAND" = "verify" ]; then

	set +x
	LOCAL_PROCESS_TO_FINISH_FIRST=$2
	echo "Program 1"
	cat $PROCFS_PID_SCHED_START.1
	cat $PROCFS_PID_SCHED_CURRENT.1
	val1=`cat $PROCFS_PID_SCHED_START.1 | awk '{print $3}'`
	val2=`cat $PROCFS_PID_SCHED_CURRENT.1  | awk '{print $3}'`
	fvalue1=`echo "$val2-$val1" | bc`
	fvalue1=`echo $fvalue1 | awk '{printf "%.0f\n", $1}'`
	echo "The final value for process 1 is $fvalue1"
	echo "Program 2"
	cat $PROCFS_PID_SCHED_START.2
	cat $PROCFS_PID_SCHED_CURRENT.2
	val1=`cat $PROCFS_PID_SCHED_START.2 | awk '{print $3}'`
	val2=`cat $PROCFS_PID_SCHED_CURRENT.2  | awk '{print $3}'`
	fvalue2=`echo "$val2-$val1" | bc`
	fvalue2=`echo $fvalue2 | awk '{printf "%.0f\n", $1}'`
	echo "The final value for process 2 is $fvalue2"

	if [ "$LOCAL_PROCESS_TO_FINISH_FIRST" = "1" ]; then
		if [ "$fvalue1" -lt "$fvalue2" ]; then
			echo "Succed"
			return 0
		else
			echo "Failed!"
			return 1
		fi
	elif [ "$LOCAL_PROCESS_TO_FINISH_FIRST" = "2" ]; then
		if [ "$fvalue2" -lt "$fvalue1" ]; then
			echo "Succed"
			return 0
		else
			echo "Failed!"
			return 1
		fi
	fi
	
	
	sleep 1

fi
