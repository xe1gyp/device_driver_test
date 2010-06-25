#!/bin/sh

set -x

# =============================================================================
# Variables
# =============================================================================

LOCAL_OPERATION=$1
LOCAL_TEST=$2
LOCAL_TEST=$2

LOCAL_ERROR=0
LOCAL_COUNT=$GPIO_FIRST_LINE

# =============================================================================
# Functions
# =============================================================================

module() {

	LOCAL_EXTRA_ARGUMENTS=$1

	insmod $GPIO_TEST_MODULE test=$LOCAL_TEST $LOCAL_EXTRA_ARGUMENTS
	cat $GPIO_TEST_MODULE_PROCFS_RESULT | grep FAILED
	if [ $? -eq 0 ]
	then
		LOCAL_ERROR=`expr $LOCAL_ERROR + 1`
	fi
	rmmod $GPIO_TEST_MODULE
}


# =============================================================================
# Main
# =============================================================================

if [ "$LOCAL_OPERATION" = "run" ]
then
	if [ "$LOCAL_TEST" = "2" ] || [ "$LOCAL_TEST" = "4" ]
	then
		LOCAL_GPIO_TEMP_VALUE=152
	else
		LOCAL_GPIO_TEMP_VALUE=-1
	fi

	while [ $LOCAL_COUNT -le $GPIO_MAX_LINES ]
	do
		if [ $LOCAL_COUNT -ne $LOCAL_GPIO_TEMP_VALUE ]
		then
			if [ "$LOCAL_TEST" = "6" ] || [ "$LOCAL_TEST" = "7" ]
			then
				module "gpio=$LOCAL_COUNT value=1 iterations=$GPIO_SMP_TEST_ITERATIONS"
			elif [ "$LOCAL_TEST" = "9" ]
			then
				module
				LOCAL_COUNT=`expr $GPIO_MAX_LINES + 1`
			else
				module "gpio=$LOCAL_COUNT value=1"
			fi
		fi

		LOCAL_COUNT=`expr $LOCAL_COUNT + 1`
	done

	if [ $LOCAL_ERROR -gt 0 ]
	then
		echo " The number of errors = $LOCAL_ERROR, Please refer the log for more details "
		exit 1
	else
		exit 0
	fi
fi

# End of file
