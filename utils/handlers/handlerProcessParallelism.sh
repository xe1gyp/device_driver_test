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
  return 1
fi

if [ "$LOCAL_OPERATION" = "clean" ]; then

	test -f $HPPA_LIST_CMDS_TOTALS && rm $HPPA_LIST_CMDS_TOTALS
	test -f $HPPA_LIST_CMDS_PASSED && rm $HPPA_LIST_CMDS_PASSED
	test -f $HPPA_LIST_PIDS_TOTALS && rm $HPPA_LIST_PIDS_TOTALS

	touch $HPPA_LIST_CMDS_TOTALS
	touch $HPPA_LIST_CMDS_PASSED
	touch $HPPA_LIST_PIDS_TOTALS

	touch $HPPA_LIST_CMDS_FAILED
	test -f $HPPA_LIST_CMDS_FAILED && rm $HPPA_LIST_CMDS_FAILED

elif [ "$LOCAL_OPERATION" = "add" ]; then

	LOCAL_COMMAND_LINE=$2
	echo $LOCAL_COMMAND_LINE >> $HPPA_LIST_CMDS_TOTALS

elif [ "$LOCAL_OPERATION" = "execute" ]; then

	LOCAL_WITH_RANDOM_DELAY=$2

	LOCAL_COMMAND_INSTANCE=0
	while read LOCAL_COMMAND_LINE
	do
		LOCAL_COMMAND_INSTANCE=`expr $LOCAL_COMMAND_INSTANCE + 1`
		LOCAL_COMMAND_DELAY="0"

		if [ "$LOCAL_WITH_RANDOM_DELAY" = "withrandomdelay" ]
		then

			LOCAL_COMMAND_DELAY_TEMP=`dd if=/dev/urandom count=1 2> /dev/null | cksum | cut -f1 -d" "`
			LOCAL_COMMAND_DELAY=`echo "$LOCAL_COMMAND_DELAY_TEMP%5" | bc`

		fi

		handlerProcessParallelismExecutor.sh "$LOCAL_COMMAND_INSTANCE" "$LOCAL_COMMAND_LINE" "$LOCAL_COMMAND_DELAY" &

	done < $HPPA_LIST_CMDS_TOTALS

	wait

	echo -e "\nInfo: Passed! > Instance | PID | Command\n"
	cat $HPPA_LIST_CMDS_PASSED
	echo

	if [ -f $HPPA_LIST_CMDS_FAILED ]
	then
		echo -e "\nInfo: Failed! > Instance | PID | Command\n"
		cat $HPPA_LIST_CMDS_FAILED
		echo
		exit 1
	fi

fi

# End of file
