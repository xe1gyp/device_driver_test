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

checkRetentionOpenSwitch() {
	LOCAL_POWER_DOMAIN=$1
	LOCAL_COMMAND_LINE=$2

	handlerPowerTransitionStats.sh "log" $LOCAL_POWER_DOMAIN "RET" "1"
	handlerPowerTransitionStats.sh "log" $LOCAL_POWER_DOMAIN "RET-LOGIC-OFF" "1"

	echo "Info: Suspending for $PM_WAKEUP_TIMER_DEFAULT seconds..."
	handlerSuspendResume.sh "suspend" $PM_WAKEUP_TIMER_DEFAULT

	handlerPowerTransitionStats.sh "log" $LOCAL_POWER_DOMAIN "RET" "2"
	handlerPowerTransitionStats.sh "log" $LOCAL_POWER_DOMAIN "RET-LOGIC-OFF" "2"

	handlerPowerTransitionStats.sh "compare" "RET" "1" "2"
	if [ $? -eq 1 ]; then
		LOCAL_ERROR=1
	fi

	handlerPowerTransitionStats.sh "compare" "RET-LOGIC-OFF" "1" "2"
	if [ $? -eq 1 ]; then
		LOCAL_ERROR=1
	fi

	if [ -n "$LOCAL_COMMAND_LINE" ]; then

		echo "Info: Running command -> $LOCAL_COMMAND_LINE"
		$LOCAL_COMMAND_LINE

		echo "Info: Suspending for $PM_WAKEUP_TIMER_DEFAULT seconds..."
		handlerSuspendResume.sh "suspend" $PM_WAKEUP_TIMER_DEFAULT

		handlerPowerTransitionStats.sh "log" $LOCAL_POWER_DOMAIN "RET" "3"
		handlerPowerTransitionStats.sh "log" $LOCAL_POWER_DOMAIN "RET-LOGIC-OFF" "3"

		handlerPowerTransitionStats.sh "compare" "RET" "2" "3"
		if [ $? -eq 1 ]; then
			LOCAL_ERROR=1
		fi

		handlerPowerTransitionStats.sh "compare" "RET-LOGIC-OFF" "2" "3"
		if [ $? -eq 1 ]; then
			LOCAL_ERROR=1
		fi
	fi
}

# =============================================================================
# Main
# =============================================================================

handlerError.sh "test"
if [ $? -eq 1 ]; then
	exit 1
fi

if [ $LOCAL_ERROR -eq 1 ]; then
	exit $LOCAL_ERROR
fi

if [ "$LOCAL_OPERATION" = "check" ]; then

	LOCAL_POWER_DOMAIN=$2

	checkRetentionOpenSwitch $LOCAL_POWER_DOMAIN

elif [ "$LOCAL_OPERATION" = "run" ]; then

	LOCAL_POWER_DOMAIN=$2
	LOCAL_COMMAND_LINE=$3

	checkRetentionOpenSwitch $LOCAL_POWER_DOMAIN "$LOCAL_COMMAND_LINE"

else

  echo "Fatal: Operation in handlerRetentionOpenSwitch is not supported!"
  LOCAL_ERROR=1

fi

exit $LOCAL_ERROR

# End of file
