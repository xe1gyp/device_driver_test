#!/bin/sh

# PM.SH STATES APPLICATION
# STATES: Sequence of states to be tested. Example: "D1 D2 D3 D2 D1".
# APPLICATION: A representative application that tests the desired states sequence.

# Dx Constants (exported by scripts/conf.sh)
D0=$D0_CONSTANT
D1=$D1_CONSTANT
D2=$D2_CONSTANT
D3=$D3_CONSTANT

# Errors
ERRORS=0

update_errors(){
	ERRORS=$(echo $ERRORS+1 | bc)
}

errors_result(){
	echo "$ERRORS errors found during power management tests"
	if [ $ERRORS = 0 ]; then
		exit 0
	else
		exit 1
	fi
}

change_state(){
	DESIRED_STATE=$1
        case $DESIRED_STATE in
                "D0")
                        DX=0
                        ;;
                "D1")
                        DX=5
                        ;;
                "D2")
                        DX=6
                        ;;
                "D3")
                        DX=7
                        ;;
		*)
			echo "Invalid state"
			exit 1
			;;
        esac
       
	echo "Changing to state $DESIRED_STATE"
	DX_COMMAND="-n $DX"
	sleep 1
        echo $DX_COMMAND > $DEVICE || update_errors
}

verify_state(){
        DESIRED_STATE=$1
        case $DESIRED_STATE in
                "D0")
                        DX=0
                        ;;
                "D1")
                        DX=5
                        ;;
                "D2")
                        DX=6
                        ;;
                "D3")
                        DX=7
                        ;;
		*)
			echo "Invalid state"
			exit 1
        esac

        STATE=$(cat $DEVICE) || update_errors
	if [ $DX = $STATE ]; then
		echo "Verifying change to state $DESIRED_STATE: OK"
	else
		echo "Verifying change to state $DESIRED_STATE: FAIL"
		update_errors
	fi
}

run_application(){
	echo "Running application: $APP"
	if [ "$CURRENT_STATE" = "D0" ]; then
		($TESTBIN/$APP &) && killall $APP || update_errors
	else
		$TESTBIN/$APP && update_errors
	fi
}

# Main
# Parser
        # Get desired states (D0, D1, D2 or D3)
	DEVICE=$1
        STATES=$2
	if [ $# -lt 3 ]; then
		echo "No application to be run"
		RUN_APP="No"
	elif [ $# = 3 ]; then
		APP=$3
		echo "Application to be run: $APP"
		RUN_APP="Yes"
	else
		echo "Too many arguments"
		RUN_APP="No"
	fi

	set STATES
	for CURRENT_STATE in $STATES
	do
		echo "=== State $CURRENT_STATE: Start ==="
		change_state $CURRENT_STATE
		verify_state $CURRENT_STATE
		if [ "$RUN_APP" = "Yes" ]; then 
			run_application $CURRENT_STATE
		fi
		echo "=== State $CURRENT_STATE: End ==="
	done
	
	# Return with the correct exit value
	errors_result
