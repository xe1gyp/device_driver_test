#!/bin/sh

# =============================================================================
# Variables
# =============================================================================

LOCAL_OPERATION=$1
LOCAL_ERROR=0

# =============================================================================
# Functions
# =============================================================================

# None

# =============================================================================
# Main
# =============================================================================

handlerError.sh "test"
if [ $? -eq 1 ]; then
	exit 1
fi

handlerDebugFileSystem.sh "mount"

if [ "$LOCAL_OPERATION" = "log" ]; then

	LOCAL_POWER_DOMAIN=$2
	LOCAL_STATE=$3
	LOCAL_VALUE=$4
	LOCAL_STATE_PLACE=0

	if [ "$LOCAL_STATE" = "OFF" ]; then
		LOCAL_STATE_PLACE=2
	elif [ "$LOCAL_STATE" = "RET" ]; then
		LOCAL_STATE_PLACE=3
	elif [ "$LOCAL_STATE" = "INA" ]; then
		LOCAL_STATE_PLACE=4
	elif [ "$LOCAL_STATE" = "ON" ]; then
		LOCAL_STATE_PLACE=5
	elif [ "$LOCAL_STATE" = "RET-LOGIC-OFF" ]; then
		LOCAL_STATE_PLACE=6
	fi

	LOCAL_TEMP=`cat $PM_COUNT | grep ^$LOCAL_POWER_DOMAIN | cut -d "," -f $LOCAL_STATE_PLACE`
	echo "Info: Powerdomain requested: $LOCAL_POWER_DOMAIN"
	echo "Info: Powerdomain transition statistic requested: $LOCAL_TEMP"
	LOCAL_RESULT=`echo $LOCAL_TEMP | cut -d ":" -f 2`
	echo $LOCAL_RESULT > $UTILS_DIR_TMP/pts.$LOCAL_STATE.$LOCAL_VALUE

elif [ "$LOCAL_OPERATION" = "compare" ]; then

	LOCAL_STATE=$2
	LOCAL_VALUE_ONE=$3
	LOCAL_VALUE_TWO=$4

	echo "Info: $LOCAL_POWER_DOMAIN Initial Value -> `cat $UTILS_DIR_TMP/pts.$LOCAL_STATE.$LOCAL_VALUE_ONE`"
	echo "Info: $LOCAL_POWER_DOMAIN Final Value -> `cat $UTILS_DIR_TMP/pts.$LOCAL_STATE.$LOCAL_VALUE_TWO`"

	sleep 5

	diff $UTILS_DIR_TMP/pts.$LOCAL_STATE.$LOCAL_VALUE_ONE $UTILS_DIR_TMP/pts.$LOCAL_STATE.$LOCAL_VALUE_TWO

	if [ $? -eq 0 ]; then
		echo "Error: $LOCAL_STATE values does match, counter did not increase"
		LOCAL_ERROR=1
	else
		echo "Info: $LOCAL_STATE values do not match, counter was increased"
		echo "TEST PASSED"
		LOCAL_ERROR=0
	fi

fi

handlerDebugFileSystem.sh "umount"

exit $LOCAL_ERROR

# End of file
