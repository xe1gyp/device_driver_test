#!/bin/sh

set -x

LOCAL_COMMAND=$1
LOCAL_IRQ=$2
LOCAL_TIMES=$3

if [ "$LOCAL_COMMAND" = "switch" ]; then

	count=0

	while [ $count -lt $LOCAL_TIMES ]
	do	
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
		sleep 10
	done

fi
