#!/bin/bash

###############################################################################
#                                                                             #
#  HandlerCmd.sh is intended to provide generic operation that can be used    #
#  across multiples testcases                                                 #
#                                                                             #
###############################################################################

# =============================================================================
# Variables
# =============================================================================

LOCAL_OPERATION=$1
LOCAL_OPERAND2=$2
LOCAL_OPERAND3=$3
LOCAL_OPERAND4=$4
LOCAL_COUNTER=0
LOCAL_ERROR=0

# =============================================================================
# Functions
# =============================================================================

# Prints a message with a specific format
# @ Function  : showInfo
# @ Parameters: <message to display>
# @ Return    : None
showInfo() {
	echo -e "\n\n----------------------- handlerCmd ---------------------------\n"
	messages=( "$@" )
	for index in ${!messages[@]}; do
		echo -e "\t${messages[$index]}"
	done
	echo -e "\n--------------------------------------------------------------\n\n"
}

# Verify LOCAL_ERROR flag
# if flag is set to '1' exit the script and register the failure
# The message parameter helps to debug the script
# @ Function  : verifyErrorFlag
# @ Parameters: <debug message>
# @ Return    : None
verifyErrorFlag() {
	debug_message=$1
	if [ $LOCAL_ERROR -eq 1 ]; then
		handlerError.sh "log" "1" "halt" "handlerCmd.sh"
		showInfo "\tDEBUG: LOCAL ERROR DETECTED:" "$debug_message"  1>&2
		exit $LOCAL_ERROR
	fi
}

# Verify is the parameter is a valid number (integer)
# @ Function  : isPositiveInteger
# @ Parameters: <number>
# @ Return    : Error flag value
isPositiveInteger() {
	num=$1
	if ! [[ $num =~ ^[0-9]+$ ]]; then
		showInfo "\tERROR: [ $num ] is not a number" 1>&2
		LOCAL_ERROR=1
	fi
}

# Display the script usage
# @ Function  : generalUsage
# @ parameters: None
# @ Return    : Error flag value
generalUsage() {
	cat <<-EOF >&1

	################################################################

	 Script Usage:

	$ handlerCmd.sh run {commands}
	  $ handlerCmd.sh iterate {commands} {iterations} {delay}

	Where:
	  @ commands: commands to be executed. Multiple commands can be
		      given by wraping the list of commands with single
		      quotes and separate each command with a semicolon
	  @ iterations: number of iterations
	  @ delay: delay between iteration (seconds)

	  Example: Iterate with a single command
	  A) iterate "echo A" 10 times with a delay of 2s

	  $ handlerCmd.sh iterate'echo A' 10 2

	  Example: Iterate with multiple commands:
	  B) iterates "echo A" and "echo B" 10 times with a delay of 2s

	  $ handlerCmd.sh iterate 'echo A;echo B' 10 2

	################################################################

	EOF
	LOCAL_ERROR=1
}

# Create an array of different commands and then
# iterates the array to execute each command
# @ Function  : executeCommands
# @ Parameters: <commands to execute>
# @ Return    : Error flag value
executeCommands() {
	toexecute=( "$@" )
	for index in ${!toexecute[*]}; do
		execute=`echo "${toexecute[$index]}" | tr "," " "`
		eval $execute
		if [ $? -gt 0 ]; then
			showInfo "ERROR: [ $execute ] is not a valid command" 2>&1
			LOCAL_ERROR=1
		fi
	done
}

# =============================================================================
# Main
# =============================================================================

handlerError.sh  "test"
if [ $? -eq 1 ]; then
	exit 1
fi

# Evaluate parameters and script usage

if [ "$LOCAL_OPERATION" = "run" ]; then
	if [ $# -ne 2 ]; then
		showInfo "ERROR: Number of parameters is invalid" 2>&1
		generalUsage
		verifyErrorFlag "generaUsage(): Number of parameters is invalid"
	fi
elif [ "$LOCAL_OPERATION" = "iterate" ]; then
	if [ $# -ne 4  ]; then
		showInfo "ERROR: Number of parameters is invalid" 2>&1
		generalUsage
		verifyErrorFlag "generaUsage(): Number of parameters is invalid"
	else
		isPositiveInteger $LOCAL_OPERAND3
		verifyErrorFlag "generaUsage(): second parameter is incorrect"
		isPositiveInteger $LOCAL_OPERAND4
		verifyErrorFlag "generaUsage(): third parameter is incorrect"
	fi
else
	generalUsage
	verifyErrorFlag "generaUsage(): operation introduced is incorrect"
fi

# Execute scripts operations

case $LOCAL_OPERATION in
"run")
	LOCAL_COMMANDS=$LOCAL_OPERAND2
	commands=`echo -e $LOCAL_COMMANDS | tr " " "," | tr ';' ' '`
	executeCommands $commands
	verifyErrorFlag "executeCommands(): command not valid"
	;;
"iterate")
	# set the required variables
	LOCAL_COMMANDS=$LOCAL_OPERAND2
	LOCAL_ITERATIONS=$LOCAL_OPERAND3
	LOCAL_DELAY=$LOCAL_OPERAND4

	while [ $LOCAL_COUNTER -lt $LOCAL_ITERATIONS ]; do
		# print iteration number in a non zero based indexing
		let LOCAL_ITERATION=$LOCAL_COUNTER+1
		showInfo "\t      ITERATION: $LOCAL_ITERATION"
		commands=`echo -e $LOCAL_COMMANDS | tr " " "," | tr ';' ' '`
		executeCommands $commands
		verifyErrorFlag "executeCommands(): command not valid"
		sleep $LOCAL_DELAY
		let LOCAL_COUNTER=LOCAL_COUNTER+1
	done
	;;
*)
	generalUsage
	verifyErrorFlag "generalUsage(): invalid operation"
	;;
esac

exit $LOCAL_ERROR
# End of file

