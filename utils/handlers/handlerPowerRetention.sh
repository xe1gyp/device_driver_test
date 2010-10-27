#!/bin/sh

# =============================================================================
# Variables
# =============================================================================

LOCAL_OPERATION=$1
LOCAL_ERROR=0
LOCAL_LCD=1

# =============================================================================
# Functions
# =============================================================================

enableRetention() {

	if [ ! $LOCAL_LCD ]; then
		handlerSysFs.sh  "set"  $SYSFS_FB_BLANK  1
	fi

	handlerSysFs.sh  "set"  $SYSFS_CPU1_ONLINE  0
}

disableRetention() {

	handlerSysFs.sh  "set"  $SYSFS_CPU1_ONLINE  1

	if [ ! $LOCAL_LCD ]; then
		handlerSysFs.sh  "set"  $SYSFS_FB_BLANK  0
	fi
}

checkRetention() {
	LOCAL_POWER_DOMAIN=$1
	LOCAL_COMMAND_LINE=$2

	handlerPowerTransitionStats.sh "log" $LOCAL_POWER_DOMAIN "RET" "1"

	if [ -n "$LOCAL_COMMAND_LINE" ]; then
		echo "Info: Running command -> $LOCAL_COMMAND_LINE"
		$LOCAL_COMMAND_LINE
	fi

	echo "Info: Sleeping for $PM_RETENTION_TIMER_DEFAULT seconds..."
  sleep $PM_RETENTION_TIMER_DEFAULT

	handlerPowerTransitionStats.sh "log" $LOCAL_POWER_DOMAIN "RET" "2"
	handlerPowerTransitionStats.sh "compare" "1" "2"
  if [ $? -eq 1 ]; then
    LOCAL_ERROR=1
  fi

}

# =============================================================================
# Main
# =============================================================================

handlerError.sh "test"
if [ $? -eq 1 ]; then
	exit 1
fi

if [ ! -f $SYSFS_FB_BLANK ]; then
	echo "Fatal: $SYSFS_FB_BLANK cannot be found!"
	handlerError.sh "log" "1" "continue" "handlerPowerRetention.sh"
	LOCAL_ERROR=0
	LOCAL_LCD=0
fi

if [ ! -f $SYSFS_CPU1_ONLINE ]; then
	echo "Fatal: $SYSFS_CPU1_ONLINE cannot be found!"
	handlerError.sh "log" "1" "halt" "handlerPowerRetention.sh"
	LOCAL_ERROR=1
fi

if [ $LOCAL_ERROR -eq 1 ]; then
	exit $LOCAL_ERROR
fi

if [ "$LOCAL_OPERATION" = "enable" ]; then

	enableRetention

elif [ "$LOCAL_OPERATION" = "check" ]; then

	LOCAL_POWER_DOMAIN=$2

	checkRetention $LOCAL_POWER_DOMAIN

elif [ "$LOCAL_OPERATION" = "disable" ]; then

	disableRetention

elif [ "$LOCAL_OPERATION" = "run" ]; then

	LOCAL_POWER_DOMAIN=$2
	LOCAL_COMMAND_LINE=$3

	checkRetention $LOCAL_POWER_DOMAIN "$LOCAL_COMMAND_LINE"

else

  echo "Fatal: Operation in smartReflexAutoComp is not supported!"
  LOCAL_ERROR=1

fi

exit $LOCAL_ERROR

# End of file
