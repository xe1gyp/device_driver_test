#!/bin/sh

usage ()
{
COMMAND=$1

case $COMMAND in
	android_runner)
		echo "Usage: android_runner.sh [set | remove | start | stop]"
		;;
	set)
		echo "Usage: android_runner.sh set <PACKAGE>"
		;;
	start)
		echo "Usage: android_runner.sh start <ACTIVITY>"
		echo "Optional: FILE and TIME"
		;;
esac
}

validate_package ()
{
        if [ ! -f $ANDROID_FOLDER/$ANDROID_FILE ]; then
                echo "$ANDROID_FOLDER/$ANDROID_FILE does not exist."
                echo "First run android_runner set <PACKAGE>"
                exit 1
        fi
}

validate_activity ()
{
	PACKAGE=`cat $ANDROID_FOLDER/$ANDROID_FILE`
	PID=`/system/bin/ps | grep $PACKAGE`

	if [ "$PID" = "" ]; then
		echo "$PACKGE process is not running"
		exit 1
	fi
}

if [ "$ANDROID_FILE" = "" ]; then
	echo "Fatal: ANDROID_FILE variable is not set"
	exit 1
fi

COMMAND=$1

if [ "$COMMAND" = "set" ]; then
	PACKAGE=$2
	if [ "$PACKAGE" = "" ]; then
		usage "set"
		exit 1
	fi
	echo "$PACKAGE" > $ANDROID_FOLDER/$ANDROID_FILE
	ls $ANDROID_FOLDER/$ANDROID_FILE
elif [ "$COMMAND" = "remove" ]; then
	validate_package
        rm $ANDROID_FOLDER/$ANDROID_FILE
elif [ "$COMMAND" = "start" ]; then
	ACTIVITY=$2
	FILE=$3

        if [ "$ACTIVITY" = "" ]; then
                usage "start"
                exit 1
        fi

	validate_package
	PACKAGE=`cat  $ANDROID_FOLDER/$ANDROID_FILE`

	if [ "$FILE" = "" ]; then
		am start -n $PACKAGE/$PACKAGE.$ACTIVITY
	else
		am start -n $PACKAGE/$PACKAGE.$ACTIVITY -d $FILE
	fi
elif [ "$COMMAND" = "stop" ]; then
	TIME=${2:-0}
	PACKAGE=`cat  $ANDROID_FOLDER/$ANDROID_FILE`

	validate_package
	validate_activity

	sleep $TIME
	killall $PACKAGE
elif [ "$COMMAND" = "" ]; then
	usage "android_runner"
	exit 1
fi
