#!/bin/sh
#-----------------------
# Based on runltp script from LTP
# Much of the functions are copied over from there
# Copyright remains
#-----------------------

# Give standard error message and die
die()
{
	echo "FATAL: $*"
	usage
	exit 1
}

# Give information message
info()
{
	echo "INFO: $*"
}

# Give debug message

debug_a()
{
	echo "DEBUG: $*"
}

# Comment out and suit urself..Debug help
alias debug=/bin/true
# alias debug=debug_a

# Defaults and sanity stuff
setup()
{

	cd `dirname $0` || \
	{
		die "unable to change directory to $(dirname $0)"
	}

	# Load config file
	if [ -f "./conf.sh" ]; then
		. ./conf.sh
		if [ $? -eq 0 ]; then
			echo "INFO: Requested tests will be started"
		else
			echo "FATAL: Configuration file with errors"
		fi
	else
		die "FATAL: Configuration file not found"
	fi

	# scenario less tests?? have the user organize it properly at least..
	[ -d $SENSOR_DIR_SCENARIOS ] ||
	{
		die "Test suite not installed correctly - no scenarios"
	}

	# we'd need the reporting tool ofcourse..
	[ -e $UTILS_DIR_BIN/pan ] ||
	{
		die "FATAL: Test suite driver 'pan' not found"
	}
}

usage()
{
	# Human redable please
	local PP=` if [ -z "$SENSOR_PRETTY_PRT" ]; then echo "off"; else echo "on"; fi`
	local VV=` if [ -z "$SENSOR_VERBOSE" ]; then echo "off"; else echo "on"; fi`

	# Give the gyan
	cat <<-EOF >&2
	usage: ./${0##*/} [-z] [-h] [-v] [-d SENSOR_DIR_TEST] [-o SENSOR_FILE_OUTPUT] [-l SENSOR_FILE_LOG]
	[-n SENSOR_DURATION ] [-t TMPDIR] [SENSOR_SCENARIO_NAMES..]

	-d SENSOR_DIR_TEST      Run LTP to test the filesystem mounted here. [Current - $SENSOR_DIR_TEST]
			At the end of test, the testdir gets cleaned out
	-s SENSOR_DIR_SCENARIOS  Test scenarios are located here. [Current - $SENSOR_DIR_SCENARIOS]
	-o SENSOR_FILE_OUTPUT   Redirect test output to a file. [Current- $SENSOR_FILE_OUTPUT {psid}]
	-p              Human readable(dont laugh too much) format logfiles. [Current - ($PP)]
	-z              Dont Merge the Scenario Name with tcid to create final tc id
	-E              Use Extended Test cases also - these are painful and can take real long time
	-l SENSOR_FILE_LOG      Log results of test in a logfile. [Current- $SENSOR_FILE_LOG {psid}]
	-t TMPDIR       Run LTP using tmp dir [Current - $SENSOR_DIR_TMP]
	-n SENSOR_DURATION     Execute the testsuite for given duration. Examples:
			-n 60s = 60 seconds
			-n 45m = 45 minutes
			-n 24h = 24 hours
			-n 2d  = 2 days
			[Current - $SENSOR_DURATION]

	-v              Print more verbose output to screen.[Current - ($VV)]
	-q              No messages from this script. no info too - Brave eh??
	-h              This screen [Current - guess...]
	-x INSTANCES    Run multiple instances of this testsuite.(think well...)
	-r PRE_DEF      Run predefined set of scenarios[Not Implemented yet]
			List to appear here
	-S              Run in Stress mode

	SENSOR_SCENARIO_NAMES  List of scenarios to test.. else, take all scenarios
			[Current - These are all filenames from $SENSOR_DIR_SCENARIOS]

	Good News: Ctrl+c stops and cleans up for you :)
	More help: Read the $SENSOR_ROOT/README

	EOF
	exit 0
}

# Insane world.. insane set of human mistakes!!
sanity_check()
{
    # Check the current values...
    # Just ensure that pan can run with a bit of peace of mind...

    [ ! -d "$SENSOR_DIR_TMP" -o ! -w "$SENSOR_DIR_TMP" ] && die "$SENSOR_DIR_TMP - cannot work as temporary directory"
    [ ! -d "$SENSOR_DIR_TEST" -o ! -w "$SENSOR_DIR_TEST" ] && die "$SENSOR_DIR_TEST - cannot work as test directory"
    [ ! -d "$SENSOR_DIR_SCENARIOS" ] && die "$SENSOR_DIR_SCENARIOS - No such directories"
    [ -z "$SENSOR_SCENARIO_NAMES" ] && die "No Scenarios"
		[ ! -z "$SENSOR_VERBOSE" -a ! -z "$SENSOR_QUIET_MODE" ] && die "Make up your mind - verbose or quiet??"

    export SENSOR_FILE_CMD=$SENSOR_DIR_TMP/$SENSOR_FILE_CMD
    rm -f $SENSOR_FILE_CMD

		for SCEN in $SENSOR_SCENARIO_NAMES
    do
		  [ ! -f "$SENSOR_DIR_SCENARIOS/$SCEN" -o ! -r "$SENSOR_DIR_SCENARIOS/$SCEN" ] && die "$SENSOR_DIR_SCENARIOS/$SCEN - not a scenario file"
			cat $SENSOR_DIR_SCENARIOS/$SCEN|grep -v "#"|sed -e "s/^[  ]*$//g"|sed -e "/^$/d">$SENSOR_FILE_TMP|| die "Count not create tmp file $SENSOR_FILE_TMP"
			if [ -z "$DONT" ]; then
				cat $SENSOR_FILE_TMP|sed -e "s/^/$SCEN-/g"|sed -e "s/-/_/" >>$SENSOR_FILE_CMD || die "Count not create command file $SENSOR_FILE_CMD"
				else
				cat $SENSOR_FILE_TMP>>$SENSOR_FILE_CMD || die "Count not create command file $SENSOR_FILE_CMD"
			fi

			# Remove the extended test cases
			if [ -z "$EXTENDED_TEST" ]; then

				cat $SENSOR_FILE_CMD|grep -v "^[_A-Za-z0-9]*_EXT ">$SENSOR_FILE_TMP || die "intermediate file gen failed"
				cat $SENSOR_FILE_TMP>$SENSOR_FILE_CMD || die "Second intermediate creation failed"
			fi

			rm -f $SENSOR_FILE_TMP

    done

		local PP=` if [ -z "$SENSOR_PRETTY_PRT" ]; then echo "off"; else echo "on"; fi`
    local VV=` if [ -z "$SENSOR_VERBOSE" ]; then echo "off"; else echo "on"; fi`
    export TMPDIR=${SENSOR_DIR_TEST}

		# Print some nice info
    if [ ! -z "$SENSOR_VERBOSE" ]; then
        debug "SENSOR_POSTFIX        $SENSOR_POSTFIX       "
        info  "SENSOR_ROOT       $SENSOR_ROOT      "
        info  "SENSOR_DIR_TMP        $SENSOR_DIR_TMP       "
        info  "SENSOR_FILE_TMP        $SENSOR_FILE_TMP       "
        debug "SENSOR_FILE_CMD        $SENSOR_FILE_CMD       "
        info  "SENSOR_DIR_TEST        $SENSOR_DIR_TEST       "
        info  "SENSOR_PRETTY_PRT     $PP            "
        info  "SENSOR_VERBOSE        $VV            "
        info  "SENSOR_FILE_OUTPUT     $SENSOR_FILE_OUTPUT    "
        info  "SENSOR_FILE_LOG        $SENSOR_FILE_LOG       "
        info  "SENSOR_DURATION       $SENSOR_DURATION      "
        debug "PATH           $PATH          "
        info  "SENSOR_DIR_SCENARIOS    $SENSOR_DIR_SCENARIOS   "
        info  "TMPDIR         $TMPDIR        "
        info  "SENSOR_SCENARIO_NAMES $SENSOR_SCENARIO_NAMES"
    fi
}

main()
{
	count=0
	while getopts zx:Sd:qt:po:l:vn:hs:E:I arg
	do  case $arg in
		d)
			SENSOR_DIR_TEST=${SENSOR_OPTARG} ;;
		t)
			SENSOR_DIR_TMP=${SENSOR_OPTARG} ;;
		E)
			EXTENDED_TEST=y ;;
	        q)
			SENSOR_QUIET_MODE=" -q " ;;
	        z)
			DONT=" " ;;
		p)
			SENSOR_PRETTY_PRT=" -p " ;;
		o)
			SENSOR_FILE_OUTPUT=${SENSOR_OPTARG};OO_LOG=1 ;;
		l)
			SENSOR_FILE_LOG=${SENSOR_OPTARG} ;;
		v)
			SENSOR_VERBOSE="-v" ;;
		n)
			SENSOR_DURATION=" -t ${SENSOR_OPTARG}" ;;
		h)
			usage ;;
		x)  # number of ltp's to run

			cat <<-EOF >&1
			WARNING: The use of -x can cause unpredictable failures, as a
			result of concurrently running multiple tests designed
			to be ran exclusively.
			Pausing for 10 seconds...Last chance to hit that ctrl+c
			EOF
					sleep 10
			SENSOR_INSTANCES="-x $SENSOR_OPTARG -O ${TMP}" ;;
		s)
			SENSOR_DIR_SCENARIOS=${SENSOR_OPTARG} ;;
		S)
			SENSOR_STRESS=y
			SENSOR_STRESSARG="-S";;

		\?) # Handle illegals
			usage ;;

	esac

	if [ ! -z "${SENSOR_OPTARG}" ]; then
		count=" $count + 2"
	else
		count=" $count + 1"
	fi

	done

	count=$(( $count ))
	while [ $count -ne 0 ]
	do
		shift;
		count=$(($count - 1))
	done

	SENSOR_SCENARIO_NAMES=$@

	sanity_check

	# Test start

	[ -z "$SENSOR_QUIET_MODE" ] && { info "Test start time: $(date)" ; }

	# Usage: pan -n name [ -SyAehp ] [ -s starts ] [-t time[s|m|h|d] [ -x nactive ] [-l logfile ]
	# [ -a active-file ] [ -f command-file ] [ -d debug-level ]
	# [-o output-file] [-O output-buffer-directory] [cmd]

	cd $SENSOR_DIR_TEST
	PAN_COMMAND="${UTILS_DIR_BIN}/pan $SENSOR_QUIET_MODE -e -S $SENSOR_INSTANCES $SENSOR_DURATION -a $$ -n $$ $SENSOR_PRETTY_PRT -f ${SENSOR_FILE_CMD} -l $SENSOR_FILE_LOG"

	[ ! -z "$SENSOR_VERBOSE" ] && { info "PAN_COMMAND=$PAN_COMMAND"; }

	if [ -z "$OO_LOG" ]; then
		$PAN_COMMAND
	else
		$PAN_COMMAND|tee $SENSOR_FILE_OUTPUT
	fi

	if [ $? -eq 0 ]; then
		echo "INFO: pan reported all tests PASS"
		VALUE=0
	else
		echo "INFO: pan reported some tests FAIL"
		VALUE=1
	fi

	# Test end
	[ -z "$SENSOR_QUIET_MODE" ] && { info "Test end time: $(date)" ; }
	[ -z "$SENSOR_QUIET_MODE" ] && {

	cat <<-EOF >&1

	###############################################################"
		Done executing testcases."
		Result log is in the $SENSOR_FILE_LOG "
	###############################################################"

	EOF
	cat $SENSOR_FILE_LOG

	}
	cleanup
	exit $VALUE
}


cleanup()
{
	[  -z "$SENSOR_QUIET_MODE" ] && echo -n "INFO: Cleaning up..."
	if [ -n "${SENSOR_FILE_TMP}" -a -n "${SENSOR_FILE_CMD}" -a -n "${SENSOR_DIR_TEST}" -a -n "${SENSOR_DIR_TMP}" ]; then
		rm -rf ${SENSOR_FILE_TMP} ${SENSOR_FILE_CMD} ${SENSOR_DIR_TEST}/* ${SENSOR_DIR_TMP}/*
	else
		echo "INFO: Clean up process won't be executed because variables for directories to be removed are not set..."
	fi

	[  -z "$SENSOR_QUIET_MODE" ] && echo "done."
}


trap "cleanup" 0
setup
main "$@"

# End of file
