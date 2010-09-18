#!/bin/sh

# =============================================================================
# Variables
# =============================================================================

LOCAL_COMMAND=$1

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

if [ "$LOCAL_COMMAND" = "mount" ]; then

	if [ -d $PM_DEBUGFS_DIRECTORY ]
	then
		echo "Debug directory is already created"
	else
		mkdir $PM_DEBUGFS_DIRECTORY
	fi

	mount | grep debugfs | grep "debugfs on $PM_DEBUGFS_DIRECTORY"

	if [ $? != 0 ]; then
		mount -t debugfs debugfs $PM_DEBUGFS_DIRECTORY
	fi

elif [ "$LOCAL_COMMAND" = "umount" ]; then

	umount $PM_DEBUGFS_DIRECTORY
	rmdir $PM_DEBUGFS_DIRECTORY

elif [ "$LOCAL_COMMAND" = "cat" ]; then

	cat $PM_COUNT

elif [ "$LOCAL_COMMAND" = "log" ]; then

	LOCAL_POWER_DOMAIN=$2
	LOCAL_HIT=$3
	LOCAL_VALUE=$4
	HIT_PLACE=0
  
	if [ "$LOCAL_HIT" = "OFF" ]; then
		HIT_PLACE=2
	elif [ "$LOCAL_HIT" = "RET" ]; then
		HIT_PLACE=3
	elif [ "$LOCAL_HIT" = "INA" ]; then
		HIT_PLACE=4
	elif [ "$LOCAL_HIT" = "ON" ]; then
		HIT_PLACE=5
	fi
  
	TEMP=`cat $PM_COUNT | grep -v always_on_core_pwrdm | grep $LOCAL_POWER_DOMAIN | grep $LOCAL_HIT`
	set $TEMP
	RESULT=`echo $2 | cut -d "," -f $HIT_PLACE`
	echo $RESULT > $TMPBASE.$LOCAL_VALUE

elif [ "$LOCAL_COMMAND" = "compare" ]; then

	LOCAL_VALUE_ONE=$2
	LOCAL_VALUE_TWO=$3
	FIRST=`cat $TMPBASE.$LOCAL_VALUE_ONE`
	SECOND=`cat $TMPBASE.$LOCAL_VALUE_TWO`

	echo "Info: Initial Value -> `cat $TMPBASE.$LOCAL_VALUE_ONE`"
	echo "Info: Initial Value -> `cat $TMPBASE.$LOCAL_VALUE_TWO`"

	sleep 5
  
	diff $TMPBASE.$LOCAL_VALUE_ONE $TMPBASE.$LOCAL_VALUE_TWO
  
	if [ $? -eq 0 ]; then
		echo "Error: values does match"
		LOCAL_ERROR=1
	else
		echo "TEST PASSED"
		LOCAL_ERROR=0
	fi

else
	echo "Command in debugFileSystem is not supported"
	LOCAL_ERROR=1
fi

handlerDebugFileSystem.sh "umount"

exit $LOCAL_ERROR

# End of file
