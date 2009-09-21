#!/bin/sh

set -x

LOCAL_COMMAND=$1
LOCAL_PROGRAM=$2
LOCAL_IRQ=$3
LOCAL_TIMES=$4

test -d /proc/irq/$LOCAL_IRQ/smp_affinity

if [ $? -eq 1 ]
then
	echo "Cannot set affinity for irq " $LOCAL_IRQ
	return 1
fi

if [ "$LOCAL_COMMAND" = "switch" ]; then

	eval $LOCAL_PROGRAM &
	LOCAL_PID=`echo $!`

	count=0

	while [ $count -lt $LOCAL_TIMES ]
	do

		if [ ! -d "/proc/$LOCAL_PID" ]
		then
			break
		fi

		rem=$(( $count % 2 ))
		if [ $rem -eq 0 ]
		then

			initial_value_p1=`cat /proc/interrupts | grep $LOCAL_IRQ | awk '{print $2}'`
			initial_value_p2=`cat /proc/interrupts | grep $LOCAL_IRQ | awk '{print $3}'`
			echo 1 > /proc/irq/$LOCAL_IRQ/smp_affinity
			sleep 10
			final_value_p1=`cat /proc/interrupts | grep $LOCAL_IRQ | awk '{print $2}'`
			final_value_p2=`cat /proc/interrupts | grep $LOCAL_IRQ | awk '{print $3}'`
			echo $initial_value_p1 $final_value_p1 $initial_value_p2 $final_value_p2

		else
			initial_value_p1=`cat /proc/interrupts | grep $LOCAL_IRQ | awk '{print $2}'`
			initial_value_p2=`cat /proc/interrupts | grep $LOCAL_IRQ | awk '{print $3}'`
			echo 2 > /proc/irq/$LOCAL_IRQ/smp_affinity
			sleep 10
			final_value_p1=`cat /proc/interrupts | grep $LOCAL_IRQ | awk '{print $2}'`
			final_value_p2=`cat /proc/interrupts | grep $LOCAL_IRQ | awk '{print $3}'`
			echo $initial_value_p1 $final_value_p1 $initial_value_p2 $final_value_p2
		fi
	
		count=`expr $count + 1`
		echo "$LOCAL_PID | $count"
		sleep 5

	done

	continue

fi
