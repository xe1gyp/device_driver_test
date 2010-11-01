#!/bin/sh

# =============================================================================
# Variables
# =============================================================================

LOCAL_OPERATION=$1
LOCAL_OPTION=$2

# =============================================================================
# Functions
# =============================================================================

# None

# =============================================================================
# Main
# =============================================================================

if [ "$LOCAL_OPERATION" = "get" ]; then

	if [ "$LOCAL_OPTION" = "irq" ]; then

		LOCAL_DRIVER=$3
		LOCAL_FILE=$4

		if [ "$LOCAL_DRIVER" = "" ]; then
			echo "Error: driver value was not provided"
			exit 1
		fi

		for i in `cat /proc/interrupts | grep -i $LOCAL_DRIVER | awk '{print$1}' | cut -d: -f1`
		do
			echo $i > $LOCAL_FILE
			echo $i
		done

	elif [ "$LOCAL_OPTION" = "cpu0" ] || [ "$LOCAL_OPTION" = "cpu1" ]; then

		LOCAL_IRQ_NUMBER=$3

		test -f  $LOCAL_IRQ_NUMBER
		if [ $? = 0 ]; then
			LOCAL_IRQ_NUMBER=`cat $LOCAL_IRQ_NUMBER`
		fi

		cat /proc/interrupts | grep -i $LOCAL_OPTION 2>&1 > /dev/null
		if [ $? != 0 ]; then
			echo "FATAL: $LOCAL_OPTION irq information not available, please check!"
			cat /proc/interrupts
			exit 1
		fi

		if [ "$LOCAL_OPTION" = "cpu0" ]; then
			cat /proc/interrupts | grep $LOCAL_IRQ_NUMBER: | awk '{print $2}'
		elif [ "$LOCAL_OPTION" = "cpu1" ]; then
			cat /proc/interrupts | grep $LOCAL_IRQ_NUMBER: | awk '{print $3}'
		fi

	elif [ "$LOCAL_OPTION" = "cpu0_irq" ] || [ "$LOCAL_OPTION" = "cpu1_irq" ]; then

		LOCAL_DRIVER=$3
		LOCAL_FILE=$4

		if [ "$LOCAL_DRIVER" = "" ]; then
			echo "Error: driver value was not provided"
			exit 1
		fi

		if [ "$LOCAL_OPTION" = "cpu0_irq" ]; then
			for i in `cat /proc/interrupts | grep -i $LOCAL_DRIVER | awk '{print$2}' | cut -d: -f1`
			do
				echo $i > $LOCAL_FILE
				echo $i
			done
		elif [ "$LOCAL_OPTION" = "cpu1_irq" ]; then
			for i in `cat /proc/interrupts | grep -i $LOCAL_DRIVER | awk '{print$3}' | cut -d: -f1`
			do
				echo $i > $LOCAL_FILE
				echo $i
			done
		fi
	else

		echo "Fatal: Unsupported option"
		exit 1

	fi

else

	echo "Fatal: Unsupported operation"
	exit 1

fi

# End of file
