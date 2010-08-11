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
	exit 1
fi

if [ "$LOCAL_OPERATION" = "clean" ]; then

	test -f $HPPR_LIST_CMDS_TOTALS && rm $HPPR_LIST_CMDS_TOTALS
	test -f $HPPR_LIST_CMDS_PASSED && rm $HPPR_LIST_CMDS_PASSED
	test -f $HPPR_LIST_PIDS_TOTALS && rm $HPPR_LIST_PIDS_TOTALS

	touch $HPPR_LIST_CMDS_TOTALS
	touch $HPPR_LIST_CMDS_PASSED
	touch $HPPR_LIST_PIDS_TOTALS

	touch $HPPR_LIST_CMDS_FAILED
	test -f $HPPR_LIST_CMDS_FAILED && rm $HPPR_LIST_CMDS_FAILED

elif [ "$LOCAL_OPERATION" = "add" ]; then

	LOCAL_COMMAND_NUMBER=$2
	LOCAL_COMMAND_LINE=$3
	LOCAL_COMMAND_PRIORITY=$4

	test -f /proc/1/sched

	if [ $? -eq 1 ]
	then
		handlerError.sh "log" "1" "halt" "handlerProcessPriority.sh"
		echo "Fatal: missing /proc/<process>/sched, cannot continue, directory looks like"
		echo "Info : please enable in kernel menuconfig the macro CONFIG_SCHED_DEBUG"
		echo "Info : Kernel Hacking -> Kernel Debugging -> Collect scheduler debugging info"
		exit 1
	fi

	echo $LOCAL_COMMAND_LINE > $HPPR_COMMAND_LINE.$LOCAL_COMMAND_NUMBER
	echo $LOCAL_COMMAND_PRIORITY > $HPPR_COMMAND_PRIORITY.$LOCAL_COMMAND_NUMBER

	echo "add" > $HPPR_OPERATION_CURRENT

elif [ "$LOCAL_OPERATION" = "assign" ]; then

	LOCAL_COMMAND_LINE=$2
	LOCAL_COMMAND_PRIORITY=$3

	echo "$LOCAL_COMMAND_LINE:$LOCAL_COMMAND_PRIORITY" >> $HPPR_LIST_CMDS_TOTALS

	echo "assign" > $HPPR_OPERATION_CURRENT

elif [ "$LOCAL_OPERATION" = "execute" ]; then

	LOCAL_CURRENT_OPERATION=`cat $HPPR_OPERATION_CURRENT`
	LOCAL_COMMAND_DELAY="0"

	if [ "$LOCAL_CURRENT_OPERATION" = "add" ]; then

		LOCAL_REPEATABILITY=$2

		tempPriorityValue1=`cat $HPPR_COMMAND_PRIORITY.1`
		tempPriorityValue2=`cat $HPPR_COMMAND_PRIORITY.2`

		FIRST_CMD="1"
		SECOND_CMD="2"

		if [ $tempPriorityValue1 -lt $tempPriorityValue2 ]; then
			echo 1 > $HPPR_COMMAND_PRIORITY_HIGHER
		elif [ $tempPriorityValue1 -gt $tempPriorityValue2 ]; then
			echo 2 > $HPPR_COMMAND_PRIORITY_HIGHER
			FIRST_CMD="2"
			SECOND_CMD="1"
		elif [ $tempPriorityValue1 -eq $tempPriorityValue2 ]; then
			echo equal > $HPPR_COMMAND_PRIORITY_HIGHER
		fi

		handlerProcessPriorityExecutor.sh "$LOCAL_CURRENT_OPERATION" "$FIRST_CMD" "`cat $HPPR_COMMAND_LINE.$FIRST_CMD`" "$LOCAL_REPEATABILITY" "`cat $HPPR_COMMAND_PRIORITY.$FIRST_CMD`" $LOCAL_COMMAND_DELAY &
		handlerProcessPriorityExecutor.sh "$LOCAL_CURRENT_OPERATION" "$SECOND_CMD" "`cat $HPPR_COMMAND_LINE.$SECOND_CMD`" "$LOCAL_REPEATABILITY" "`cat $HPPR_COMMAND_PRIORITY.$SECOND_CMD`" $LOCAL_COMMAND_DELAY &

		wait

	elif [ "$LOCAL_CURRENT_OPERATION" = "assign" ]; then

		LOCAL_WITH_RANDOM_DELAY=$2

		LOCAL_INSTANCE=0

		while read LOCAL_LINE
		do
			LOCAL_INSTANCE=`expr $LOCAL_INSTANCE + 1`
			LOCAL_COMMAND_LINE=`echo $LOCAL_LINE | cut -d: -f1`
			LOCAL_COMMAND_PRIORITY=`echo $LOCAL_LINE | cut -d: -f2`
			LOCAL_COMMAND_DELAY="0"

			if [ "$LOCAL_WITH_RANDOM_DELAY" = "withrandomdelay" ]
			then

				LOCAL_COMMAND_DELAY_TEMP=`dd if=/dev/urandom count=1 2> /dev/null | cksum | cut -f1 -d" "`
				LOCAL_COMMAND_DELAY=`echo "$LOCAL_COMMAND_DELAY%5" | bc`

			fi

			handlerProcessPriorityExecutor.sh "$LOCAL_CURRENT_OPERATION" "$LOCAL_INSTANCE" "$LOCAL_COMMAND_LINE" "1" "$LOCAL_COMMAND_PRIORITY" "$LOCAL_COMMAND_DELAY" &

		done < $HPPR_LIST_CMDS_TOTALS

		wait

		echo -e "\nInfo: Passed! > Instance | PID | Command\n"
		cat $HPPR_LIST_CMDS_PASSED
		echo

		if [ -f $HPPR_LIST_CMDS_FAILED ]; then
			echo -e "\nInfo: Failed! > Instance | PID | Command\n"
			cat $HPPR_LIST_CMDS_FAILED
			echo
			exit 1
		fi
	fi

elif [ "$LOCAL_OPERATION" = "verify" ]; then

	echo -e "\nInfo: Program 1 | Initial | Final"
	echo -e "`cat $HPPR_PROCFS_PID_SCHED_START.1`\n`cat $HPPR_PROCFS_PID_SCHED_CURRENT.1`\n"
	echo -e "\nInfo: Program 2 | Initial | Final"
	echo -e "`cat $HPPR_PROCFS_PID_SCHED_START.2`\n`cat $HPPR_PROCFS_PID_SCHED_CURRENT.2`\n"

	val1=`cat $HPPR_PROCFS_PID_SCHED_START.1 | awk '{print $3}'`
	val1=0
	val2=`cat $HPPR_PROCFS_PID_SCHED_CURRENT.1  | awk '{print $3}'`
	fvalue1=`echo "scale=6; $val2-$val1" | bc`
	echo "Info: Program 1 | Final value for process 1 is $fvalue1"

	val1=`cat $HPPR_PROCFS_PID_SCHED_START.2 | awk '{print $3}'`
	val1=0
	val2=`cat $HPPR_PROCFS_PID_SCHED_CURRENT.2  | awk '{print $3}'`
	fvalue2=`echo "scale=6; $val2-$val1" | bc`
	echo "Info: Program 2 | Final value for process 2 is $fvalue2"

	LOCAL_PROCESS_TO_FINISH_FIRST=`cat $HPPR_COMMAND_PRIORITY_HIGHER`

	if [ "$LOCAL_PROCESS_TO_FINISH_FIRST" = "equal" ]; then
		echo -e "\nInfo: We don't know what process should have finished first\n"
		exit 0
	else
		echo -e "\nInfo: Process with higher priority is $LOCAL_PROCESS_TO_FINISH_FIRST"
		if [ "$LOCAL_PROCESS_TO_FINISH_FIRST" = "1" ]; then
			LOCAL_RESULT=`echo "scale=6; $fvalue1 > $fvalue2" | bc`
			if [ "$LOCAL_RESULT" = "0" ]; then
				echo -e "Info: Failed!\n"
				exit 1
			else
				echo -e "Info: Passed\n"
				exit 0
			fi

		elif [ "$LOCAL_PROCESS_TO_FINISH_FIRST" = "2" ]; then
			LOCAL_RESULT=`echo "scale=6; $fvalue2 > $fvalue1" | bc`
			if [ "$LOCAL_RESULT" = "0" ]; then
				echo -e "Info: Failed!\n"
				exit 1
			else
				echo -e "Info: Passed\n"
				exit 0
			fi
		fi
	fi
fi

# End of file
