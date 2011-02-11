#!/bin/bash

###############################################################################
#                                                                             #
#  HandlerCmd.sh is intended to provide generic operation that can be used    #
#  across multiples testcases                                                 #
#                                                                             #
###############################################################################

#  Copyright (c) 2010 Texas Instruments
#
#  Author: Leed Aguilar <leed.aguilar@ti.com>
#  Author: Arce Abraham <abraham.arce.moreno@gmail.com>
#
#  This program is free software; you can redistribute it and/or
#  modify it under the terms of the GNU General Public License as
#  published by the Free Software Foundation; either version 2 of the
#  License, or (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
#  USA
#

# =============================================================================
# Variables
# =============================================================================

operation=$1
operand2=$2
operand3=$3
operand4=$4
counter=0
error_val=0

# =============================================================================
# Functions
# =============================================================================

# Prints a message with a specific format
# @ Function: showInfo
# @ Parameters: <message to display>
# @ Return: None
showInfo() {
	messages=( "$@" )
	for index in ${!messages[@]}; do
		echo  "[ handlerCmd ]: ${messages[$index]}"
	done
}

# Verify error_val flag
# if flag is set to '1' exit the script and register the failure
# The message parameter helps to debug the script
# @ Function: verifyErrorFlag
# @ Parameters: <debug message>
# @ Return: None
verifyErrorFlag() {
	debug_message=$1
	if [ $error_val -eq 1 ]; then
		handlerError.sh "log" "1" "halt" "handlerCmd.sh"
		showInfo "ERROR: $debug_message"  1>&2
		exit $error_val
	fi
}

# Verify is the parameter is a valid number (integer)
# @ Function: isPositiveInteger
# @ Parameters: <number>
# @ Return: Error flag value
isPositiveInteger() {
	num=$1
	if ! [[ $num =~ ^[0-9]+$ ]]; then
		showInfo "ERROR: ( $num ) is not a number" 1>&2
		error_val=1
	fi
}

# Display the script usage
# @ Function: usage
# @ parameters: None
# @ Return: Error flag value
usage() {
	cat <<-EOF >&1

	####################### handlerCmd #######################

	 Script Usage:

	  $ handlerCmd.sh run {commands}
	  $ handlerCmd.sh iterate {commands} {iterations} {delay}

	Where:
	  @ commands: commands to be executed. Multiple commands
		      can be given by wraping the list of commands
		      with single quotes and separate each command
		      with a semicolon
	  @ iterations: number of iterations
	  @ delay: delay between iteration (seconds)

	  Example: Iterate with a single command
	  A) iterate "echo A" 10 times with a delay of 2s

	  $ handlerCmd.sh iterate'echo A' 10 2

	  Example: Iterate with multiple commands:
	  B) iterates "echo A" and "echo B" 10 times with a delay
	     of 2 seconds

	  $ handlerCmd.sh iterate 'echo A;echo B' 10 2

	####################### handlerCmd #######################

	EOF
	error_val=1
}

# Create an array of different commands and then
# iterates the array to execute each command
# @ Function: executeCommands
# @ Parameters: <commands to execute>
# @ Return: Error flag value
executeCommands() {
	toexecute=( "$@" )
	for index in ${!toexecute[*]}; do
		execute=`echo "${toexecute[$index]}" | tr "," " "`
		eval $execute
		if [ $? -gt 0 ]; then
			showInfo "ERROR: < $execute > execution failed" 2>&1
			error_val=1
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

if [ "$operation" = "run" ]; then
	if [ $# -ne 2 ]; then
		usage
		verifyErrorFlag "Number of parameters is invalid"
	fi
elif [ "$operation" = "iterate" ]; then
	if [ $# -ne 4  ]; then
		usage
		verifyErrorFlag "Number of parameters is invalid"
	else
		isPositiveInteger $operand3
		verifyErrorFlag "Second parameter is incorrect"
		isPositiveInteger $operand4
		verifyErrorFlag "Third parameter is incorrect"
	fi
else
	usage
	verifyErrorFlag "Operation introduced is incorrect"
fi

# Execute scripts operations

case $operation in
"run")
	commands=`echo -e $operand2 | tr " " "," | tr ';' ' '`
	executeCommands $commands
	verifyErrorFlag "executeCommands reported an error"
	;;
"iterate")
	# set the required variables
	iterations=$operand3
	delay=$operand4

	while [ $counter -lt $iterations ]; do
		# print iteration number in a non zero based indexing
		let report_iteration=$counter+1
		showInfo "ITERATION::::::::::::::: $report_iteration"
		commands=`echo -e $operand2 | tr " " "," | tr ';' ' '`
		executeCommands $commands
		verifyErrorFlag "executeCommands reported an error"
		sleep $delay
		let counter=counter+1
	done
	;;
*)
	usage
	verifyErrorFlag "usage(): invalid operation"
	;;
esac

exit $error_val
# End of file

