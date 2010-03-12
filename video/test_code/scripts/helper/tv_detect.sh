# !/bin/sh

TV_DETECT=$1
SETIMG_PARAMETERS=$2
STREAMING_PARAMETERS=$3
SETWIN_PARAMETERS=$4

check_device_status()
{
	COUNT=6
	EXPECTED_STATUS=$1
	MESSAGE=$2

	echo -e "\n"$MESSAGE COMPOSITE OUTPUT OF THE BOARD
	echo PRESS ANY KEY WHEN DONE

	if [ -z "$STRESS" ]; then
		$WAIT_KEY
	else
		sleep 2
	fi

	while [ $COUNT -gt 0 ]; do
		DEVICE_STATUS=`cat $TV/device_connected`

		if [ "$DEVICE_STATUS" = "$EXPECTED_STATUS" ]; then
			return 0
		fi
		sleep 3
		echo $MESSAGE COMPOSITE OUTPUT OF THE BOARD
		let COUNT=COUNT-1
	done

	return 1
}

RESULT=0

if [ "$TV_DETECT" = "ENABLED" ];then

	# Usage: setimg <vid> <fmt> <width> <height>
	$TESTBIN/setimg 1 $SETIMG_PARAMETERS
	RESULT=`command_tracking.sh $RESULT $?`

	echo 1 > $TV/device_detect_enabled

	check_device_status 0 "DISCONNECT TV FROM "
	if [ "$?" -eq "1" ]; then
		echo "TV was not disconnected"
		exit 1
	fi

	check_device_status 1 "CONNECT TV TO "
	if [ "$?" -eq "1" ]; then
		echo "TV was not connected"
		exit 1
	fi

	echo "0" > $OVL1/enabled
	echo "tv" > $OVL1/manager
	echo "1" > $TV/enabled
	$TESTBIN/streaming 1 $STREAMING_PARAMETERS

	sleep 3

	check_device_status 0 "DISCONNECT TV FROM "

	if [ "$?" -eq "1" ]; then
		echo "TV was not disconnected"
		exit 1
	fi

	if [ -z "$STRESS" ]; then
		stress_message.sh
	fi

else
	echo 0 > $TV/device_detect_enabled
	DEVICE_STATUS=`cat $TV/device_connected`

	if [ -z $DEVICE_STATUS ]; then
		RESULT=0
	else
		RESULT=1
	fi

fi

sleep 3


exit $RESULT
