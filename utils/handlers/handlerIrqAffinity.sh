#!/bin/sh

LOCAL_OPERATION=$1
LOCAL_COMMAND_LINE=$2
LOCAL_EXECUTION_TIMES=$3
LOCAL_TIME_TO_WAIT=$4
LOCAL_IRQ_NUMBER=$5

echo -e "\nInfo: Command > $LOCAL_COMMAND_LINE\n"

test -f /proc/irq/$LOCAL_IRQ_NUMBER/smp_affinity

if [ $? -eq 1 ]
then
	echo "Error: Cannot set affinity for irq $LOCAL_IRQ_NUMBER!"
	#return 1
fi

if [ "$LOCAL_OPERATION" = "switch" ]; then

  $LOCAL_COMMAND_LINE &
	LOCAL_COMMAND_PID=`echo $!`

	count=1
	process=
	while [ $count -le $LOCAL_EXECUTION_TIMES ]
	do

		echo -e "\nInfo: PID $LOCAL_COMMAND_PID | Irq Number $LOCAL_IRQ_NUMBER | Count $count of $LOCAL_EXECUTION_TIMES"
		if [ ! -d "/proc/$LOCAL_COMMAND_PID" ]
		then
			break
		fi

		rem=$(( $count % 2 ))

		if [ $rem -eq 0 ]
		then

			initial_value_p1=`cat /proc/interrupts | grep $LOCAL_IRQ_NUMBER | awk '{print $2}'`
			initial_value_p2=`cat /proc/interrupts | grep $LOCAL_IRQ_NUMBER | awk '{print $3}'`
			echo 1 > /proc/irq/$LOCAL_IRQ_NUMBER/smp_affinity
			sleep $LOCAL_TIME_TO_WAIT
			final_value_p1=`cat /proc/interrupts | grep $LOCAL_IRQ_NUMBER | awk '{print $2}'`
			final_value_p2=`cat /proc/interrupts | grep $LOCAL_IRQ_NUMBER | awk '{print $3}'`
			echo $initial_value_p1 $final_value_p1 $initial_value_p2 $final_value_p2
			if [ "$initial_value_p1" -lt "$final_value_p1" ]
			then
				continue
			else
				echo "Error: Number of interrupts were not increased in Processor 1"
				#return 1
			fi

		else

			initial_value_p1=`cat /proc/interrupts | grep $LOCAL_IRQ_NUMBER | awk '{print $2}'`
			initial_value_p2=`cat /proc/interrupts | grep $LOCAL_IRQ_NUMBER | awk '{print $3}'`
			echo 2 > /proc/irq/$LOCAL_IRQ_NUMBER/smp_affinity
			sleep $LOCAL_TIME_TO_WAIT
			final_value_p1=`cat /proc/interrupts | grep $LOCAL_IRQ_NUMBER | awk '{print $2}'`
			final_value_p2=`cat /proc/interrupts | grep $LOCAL_IRQ_NUMBER | awk '{print $3}'`
			echo $initial_value_p1 $final_value_p1 $initial_value_p2 $final_value_p2
			if [ "$initial_value_p2" -lt "$final_value_p2" ]
			then
				continue
			else
				echo "Error: Number of interrupts were not increased in Processor 2"
				#return 1
			fi

		fi

		count=`expr $count + 1`
		sleep $LOCAL_TIME_TO_WAIT

	done

  echo -e "\nInfo: Command > $LOCAL_COMMAND_LINE"
  echo -e "Info: Waiting for it to finish..."
	wait
	echo -e "Info: Done!\n"

fi

# End of file