#!/bin/sh

# =============================================================================
# Variables
# =============================================================================

LOCAL_OPERATION=$1
LOCAL_COMMAND_LINE=$2
LOCAL_EXECUTION_TIMES=$3
LOCAL_TIME_TO_WAIT=$4

# =============================================================================
# Functions
# =============================================================================

# None

# =============================================================================
# Main
# =============================================================================

handlerError.sh "test"
if [ $? -eq 1 ]; then
  return 1
fi

echo "\nInfo: Command > $LOCAL_COMMAND_LINE\n"

if [ "$LOCAL_OPERATION" = "switch" ]; then

  $LOCAL_COMMAND_LINE &
	LOCAL_COMMAND_PID=`echo $!`
	
	count=1
	processor=1
	while [ $count -le $LOCAL_EXECUTION_TIMES ]
	do

		echo -e "\nInfo: PID $LOCAL_COMMAND_PID | Processor $processor | Count $count of $LOCAL_EXECUTION_TIMES"
		if [ ! -d "/proc/$LOCAL_COMMAND_PID" ]
		then
			break
		fi

		rem=$(( $count % 2 ))

		if [ $rem -eq 0 ]
		then
			processor=1
			$UTILS_DIR_BIN/taskset -p $processor $LOCAL_COMMAND_PID
		else
			processor=2
			$UTILS_DIR_BIN/taskset -p $processor $LOCAL_COMMAND_PID
		fi

		if [ $? -ne 0 ]
		then
			echo -e "Error: Could not set cpu affinity for processor $processor!"
			# FixMe
			# return 1
		fi

		count=`expr $count + 1`
		sleep $LOCAL_TIME_TO_WAIT

	done

  echo -e "\nInfo: Command > $LOCAL_COMMAND_LINE\n"
  echo -e "Info: Waiting for it to finish..."
	wait
	echo -e "Info: Done!\n"

fi

# End of file