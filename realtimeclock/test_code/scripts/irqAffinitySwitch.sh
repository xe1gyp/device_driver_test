#!/bin/sh

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




if [ "$LOCAL_OPERATION" = "switch" ]; then

        LOCAL_COMMAND_PID="0"
        LOCAL_COMMAND_PID=`ps | grep "twl6030" | grep -v grep | awk '{print $1}'`  
        count=1
	process=
	while [ $count -le $LOCAL_EXECUTION_TIMES ]
	do

		echo -e "\nInfo: PID $LOCAL_COMMAND_PID | Irq Number $LOCAL_IRQ_NUMBER | Count $count of $LOCAL_EXECUTION_TIMES"
		
                rem=$(( $count % 2 ))
                if [ $rem -eq 0 ]
		then
                        
			initial_value_p1=`cat /proc/interrupts | grep $LOCAL_IRQ_NUMBER: | awk '{print $2}'`
			initial_value_p2=`cat /proc/interrupts | grep $LOCAL_IRQ_NUMBER: | awk '{print $3}'`
			taskset -p "1" $LOCAL_COMMAND_PID
                        echo -e 1 | per_int /dev/rtc0   
			sleep $LOCAL_TIME_TO_WAIT

			final_value_p1=`cat /proc/interrupts | grep $LOCAL_IRQ_NUMBER: | awk '{print $2}'`
			final_value_p2=`cat /proc/interrupts | grep $LOCAL_IRQ_NUMBER: | awk '{print $3}'`

			echo "Values IP1 FP1 IP2 FP2: $initial_value_p1 $final_value_p1 $initial_value_p2 $final_value_p2"

			if [ "$initial_value_p1" -lt "$final_value_p1" ]
			then
				echo "Number of interrupts were increased in Processor 1"
			else
				echo "Error: Number of interrupts were not increased in Processor 1"
				exit 1
			fi

		else
                        initial_value_p1=`cat /proc/interrupts | grep $LOCAL_IRQ_NUMBER: | awk '{print $2}'`
			initial_value_p2=`cat /proc/interrupts | grep $LOCAL_IRQ_NUMBER: | awk '{print $3}'`
                        taskset -p 2 $LOCAL_COMMAND_PID
                        echo -e 1 | per_int /dev/rtc0
                        sleep $LOCAL_TIME_TO_WAIT
                        final_value_p1=`cat /proc/interrupts | grep $LOCAL_IRQ_NUMBER: | awk '{print $2}'`
			final_value_p2=`cat /proc/interrupts | grep $LOCAL_IRQ_NUMBER: | awk '{print $3}'`

			echo "Values IP1 FP1 IP2 FP2: $initial_value_p1 $final_value_p1 $initial_value_p2 $final_value_p2"

			if [ "$initial_value_p2" -lt "$final_value_p2" ]
			then
				echo "Number of interrupts increased in Processor 2"
			else
				echo "Error: Number of interrupts were not increased in Processor 2"
				exit 1
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

