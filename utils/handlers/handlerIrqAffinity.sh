#!/bin/sh

# =============================================================================
# Variables
# =============================================================================

LOCAL_OPERATION=$1
LOCAL_COMMAND_LINE=$2
LOCAL_EXECUTION_TIMES=$3
LOCAL_TIME_TO_WAIT=$4
LOCAL_IRQ_NUMBER=$5

export LOCAL_IRQ_GPIO=0
export LOCAL_PROC_IRQ_NUMBER=0

# =============================================================================
# Functions
# =============================================================================

getIrqBank() {

	LOCAL_VIRTUAL_IRQ=$1
	LOCAL_GPIO_LINE=`echo "$LOCAL_VIRTUAL_IRQ-160" | bc`

	if [ "$LOCAL_GPIO_LINE" -ge 0 ] && [ "$LOCAL_GPIO_LINE" -le 31 ]; then
		export LOCAL_IRQ_GPIO=$INT_GPIO_BANK1
	elif [ "$LOCAL_GPIO_LINE" -ge 32 ] && [ "$LOCAL_GPIO_LINE" -le 63 ]; then
		export LOCAL_IRQ_GPIO=$INT_GPIO_BANK2
	elif [ "$LOCAL_GPIO_LINE" -ge 64 ] && [ "$LOCAL_GPIO_LINE" -le 95 ]]; then
		export LOCAL_IRQ_GPIO=$INT_GPIO_BANK3
	elif [ "$LOCAL_GPIO_LINE" -ge 96 ] && [ "$LOCAL_GPIO_LINE" -le 127 ]; then
		export LOCAL_IRQ_GPIO=$INT_GPIO_BANK4
	elif [ "$LOCAL_GPIO_LINE" -ge 128 ] && [ "$LOCAL_GPIO_LINE" -le 159 ]; then
		export LOCAL_IRQ_GPIO=$INT_GPIO_BANK5
	elif [ "$LOCAL_GPIO_LINE" -ge 160 ] && [ "$LOCAL_GPIO_LINE" -le 191 ]; then
		export LOCAL_IRQ_GPIO=$INT_GPIO_BANK6
	fi

}

# =============================================================================
# Main
# =============================================================================

# Make sure GPIO irqs are handled in a different way
# We need to change the affinity for the irq of the GPIO bank it belongs to

if [ "$LOCAL_IRQ_NUMBER" -gt "160" ]; then
	getIrqBank $LOCAL_IRQ_NUMBER
	LOCAL_PROC_IRQ_NUMBER=$LOCAL_IRQ_GPIO
else
	LOCAL_PROC_IRQ_NUMBER=$LOCAL_IRQ_NUMBER
fi

test -f /proc/irq/$LOCAL_PROC_IRQ_NUMBER/smp_affinity
if [ $? -eq 1 ]
then
	echo "Error: Cannot set affinity for irq $LOCAL_IRQ_NUMBER!"
	exit 1
fi

handlerError.sh "test"
if [ $? -eq 1 ]; then
	exit 1
fi

handlerSysFs.sh "compare" $SYSFS_CPU_ONLINE  "0-1"
if [ $? -eq 0 ]; then
	LOCAL_MULTICORE=1
else
	echo -e "\n\nINFO: Only CPU 0 is online, will set up affinity automatically\n\n"
fi

if [ "$LOCAL_OPERATION" = "switch" ]; then

	$LOCAL_COMMAND_LINE &
	LOCAL_COMMAND_PID=`echo $!`

	count=1
	while [ $count -le $LOCAL_EXECUTION_TIMES ]
	do

		echo -e "\nInfo: PID $LOCAL_COMMAND_PID | Irq Number $LOCAL_IRQ_NUMBER | Count $count of $LOCAL_EXECUTION_TIMES"
		if [ ! -d "/proc/$LOCAL_COMMAND_PID" ]
		then
			break
		fi

		LOCAL_PROCESSOR=1

		rem=$(( $count % 2 ))
		if [ $rem -eq 1 ]
		then
			if [ $LOCAL_MULTICORE ]; then
				LOCAL_PROCESSOR=2
			fi
		fi

		LOCAL_CPU=`expr $LOCAL_PROCESSOR - 1`

		LOCAL_VALUE_INITIAL=`handlerIrq.sh get "cpu$LOCAL_CPU" $LOCAL_IRQ_NUMBER`

		echo $LOCAL_PROCESSOR > /proc/irq/$LOCAL_PROC_IRQ_NUMBER/smp_affinity
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

	done

	echo -e "\nInfo: Command > $LOCAL_COMMAND_LINE"
	echo -e "Info: Waiting for it to finish..."
	wait
	echo -e "Info: Done!\n"

fi

# End of file

