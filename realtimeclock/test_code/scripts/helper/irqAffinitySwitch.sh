#!/bin/sh

set -x

# =============================================================================
# Variables
# =============================================================================

LOCAL_OPERATION=$1
LOCAL_EXECUTION_TIMES=$2
LOCAL_TIME_TO_WAIT=$3
LOCAL_IRQ_NUMBER=$4

export LOCAL_IRQ_GPIO=0
export LOCAL_PROC_IRQ_NUMBER=0

# =============================================================================
# Main
# =============================================================================

handlerSysFs.sh "compare" $SYSFS_CPU_ONLINE  "0-1"
if [ $? -eq 0 ]; then
	LOCAL_MULTICORE=1
else
	echo -e "\n\nINFO: Only CPU 0 is online, will set up affinity automatically\n\n"
fi

if [ "$LOCAL_OPERATION" = "switch" ]; then

	LOCAL_COMMAND_PID="0"
	LOCAL_COMMAND_PID=`ps -A | grep "twl6030" | grep -v grep | awk '{print $1}'`

	count=1
	while [ $count -le $LOCAL_EXECUTION_TIMES ]
	do

		#echo "Info: PID $LOCAL_COMMAND_PID | Irq Number $LOCAL_IRQ_NUMBER | Count $count of $LOCAL_EXECUTION_TIMES"

		LOCAL_PROCESSOR=1

		rem=$(( $count % 2 ))
		if [ $rem -eq 0 ]
		then
			LOCAL_PROCESSOR=1
		else
			if [ $LOCAL_MULTICORE ]; then
				LOCAL_PROCESSOR=2
			fi
		fi

		LOCAL_CPU=`expr $LOCAL_PROCESSOR - 1`

		LOCAL_VALUE_INITIAL=`handlerIrq.sh get "cpu$LOCAL_CPU" $LOCAL_IRQ_NUMBER`

		taskset -p $LOCAL_PROCESSOR $LOCAL_COMMAND_PID
		echo  1 | per_int /dev/rtc0
		sleep $LOCAL_TIME_TO_WAIT

		LOCAL_VALUE_FINAL=`handlerIrq.sh get "cpu$LOCAL_CPU" $LOCAL_IRQ_NUMBER`

		echo "Values Initial | Final : $LOCAL_VALUE_INITIAL | $LOCAL_VALUE_FINAL"

		if [ "$LOCAL_VALUE_INITIAL" -lt "$LOCAL_VALUE_FINAL" ]
		then
			echo "Number of interrupts were increased in Processor $LOCAL_PROCESSOR"
		else
			echo "Error: Number of interrupts were not increased in Processor $LOCAL_PROCESSOR"
			exit 1
		fi

		count=`expr $count + 1`
		sleep $LOCAL_TIME_TO_WAIT

	done

	echo  "Info: Command > $LOCAL_COMMAND_LINE"
	echo  "Info: Waiting for it to finish..."
	wait
	echo  "Info: Done!"

fi

# End of file

