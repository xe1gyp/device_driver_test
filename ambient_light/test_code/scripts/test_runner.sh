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
	[ -d $AMBIENT_LIGHT_DIR_SCENARIOS ] ||
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
	local PP=` if [ -z "$AMBIENT_LIGHT_PRETTY_PRT" ]; then echo "off"; else echo "on"; fi`
	local VV=` if [ -z "$AMBIENT_LIGHT_VERBOSE" ]; then echo "off"; else echo "on"; fi`

	# Give the gyan
	cat <<-EOF >&2
	usage: ./${0##*/} [-z] [-h] [-v] [-d AMBIENT_LIGHT_DIR_TEST] [-o AMBIENT_LIGHT_FILE_OUTPUT] [-l AMBIENT_LIGHT_FILE_LOG]
	[-n AMBIENT_LIGHT_DURATION ] [-t TMPDIR] [AMBIENT_LIGHT_SCENARIO_NAMES..]

	-d AMBIENT_LIGHT_DIR_TEST      Run LTP to test the filesystem mounted here. [Current - $AMBIENT_LIGHT_DIR_TEST]
			At the end of test, the testdir gets cleaned out
	-s AMBIENT_LIGHT_DIR_SCENARIOS  Test scenarios are located here. [Current - $AMBIENT_LIGHT_DIR_SCENARIOS]
	-o AMBIENT_LIGHT_FILE_OUTPUT   Redirect test output to a file. [Current- $AMBIENT_LIGHT_FILE_OUTPUT {psid}]
	-p              Human readable(dont laugh too much) format logfiles. [Current - ($PP)]
	-z              Dont Merge the Scenario Name with tcid to create final tc id
	-E              Use Extended Test cases also - these are painful and can take real long time
	-l AMBIENT_LIGHT_FILE_LOG      Log results of test in a logfile. [Current- $AMBIENT_LIGHT_FILE_LOG {psid}]
	-t TMPDIR       Run LTP using tmp dir [Current - $AMBIENT_LIGHT_DIR_TMP]
	-n AMBIENT_LIGHT_DURATION     Execute the testsuite for given duration. Examples:
			-n 60s = 60 seconds
			-n 45m = 45 minutes
			-n 24h = 24 hours
			-n 2d  = 2 days
			[Current - $AMBIENT_LIGHT_DURATION]

	-v              Print more verbose output to screen.[Current - ($VV)]
	-q              No messages from this script. no info too - Brave eh??
	-h              This screen [Current - guess...]
	-x INSTANCES    Run multiple instances of this testsuite.(think well...)
	-r PRE_DEF      Run predefined set of scenarios[Not Implemented yet]
			List to appear here
	-S              Run in Stress mode

	AMBIENT_LIGHT_SCENARIO_NAMES  List of scenarios to test.. else, take all scenarios
			[Current - These are all filenames from $AMBIENT_LIGHT_DIR_SCENARIOS]

	Good News: Ctrl+c stops and cleans up for you :)
	More help: Read the $AMBIENT_LIGHT_ROOT/README

	EOF
	exit 0
}

# Insane world.. insane set of human mistakes!!
sanity_check()
{
    # Check the current values...
    # Just ensure that pan can run with a bit of peace of mind...

    [ ! -d "$AMBIENT_LIGHT_DIR_TMP" -o ! -w "$AMBIENT_LIGHT_DIR_TMP" ] && die "$AMBIENT_LIGHT_DIR_TMP - cannot work as temporary directory"
    [ ! -d "$AMBIENT_LIGHT_DIR_TEST" -o ! -w "$AMBIENT_LIGHT_DIR_TEST" ] && die "$AMBIENT_LIGHT_DIR_TEST - cannot work as test directory"
    [ ! -d "$AMBIENT_LIGHT_DIR_SCENARIOS" ] && die "$AMBIENT_LIGHT_DIR_SCENARIOS - No such directories"
    [ -z "$AMBIENT_LIGHT_SCENARIO_NAMES" ] && die "No Scenarios"
		[ ! -z "$AMBIENT_LIGHT_VERBOSE" -a ! -z "$AMBIENT_LIGHT_QUIET_MODE" ] && die "Make up your mind - verbose or quiet??"

    export AMBIENT_LIGHT_FILE_CMD=$AMBIENT_LIGHT_DIR_TMP/$AMBIENT_LIGHT_FILE_CMD
    rm -f $AMBIENT_LIGHT_FILE_CMD

		for SCEN in $AMBIENT_LIGHT_SCENARIO_NAMES
    do
		  [ ! -f "$AMBIENT_LIGHT_DIR_SCENARIOS/$SCEN" -o ! -r "$AMBIENT_LIGHT_DIR_SCENARIOS/$SCEN" ] && die "$AMBIENT_LIGHT_DIR_SCENARIOS/$SCEN - not a scenario file"
			cat $AMBIENT_LIGHT_DIR_SCENARIOS/$SCEN|grep -v "#"|sed -e "s/^[  ]*$//g"|sed -e "/^$/d">$AMBIENT_LIGHT_FILE_TMP|| die "Count not create tmp file $AMBIENT_LIGHT_FILE_TMP"
			if [ -z "$DONT" ]; then
				cat $AMBIENT_LIGHT_FILE_TMP|sed -e "s/^/$SCEN-/g"|sed -e "s/-/_/" >>$AMBIENT_LIGHT_FILE_CMD || die "Count not create command file $AMBIENT_LIGHT_FILE_CMD"
				else
				cat $AMBIENT_LIGHT_FILE_TMP>>$AMBIENT_LIGHT_FILE_CMD || die "Count not create command file $AMBIENT_LIGHT_FILE_CMD"
			fi

			# Remove the extended test cases
			if [ -z "$EXTENDED_TEST" ]; then

				cat $AMBIENT_LIGHT_FILE_CMD|grep -v "^[_A-Za-z0-9]*_EXT ">$AMBIENT_LIGHT_FILE_TMP || die "intermediate file gen failed"
				cat $AMBIENT_LIGHT_FILE_TMP>$AMBIENT_LIGHT_FILE_CMD || die "Second intermediate creation failed"
			fi

			rm -f $AMBIENT_LIGHT_FILE_TMP

    done

		local PP=` if [ -z "$AMBIENT_LIGHT_PRETTY_PRT" ]; then echo "off"; else echo "on"; fi`
    local VV=` if [ -z "$AMBIENT_LIGHT_VERBOSE" ]; then echo "off"; else echo "on"; fi`
    export TMPDIR=${AMBIENT_LIGHT_DIR_TEST}

		# Print some nice info
    if [ ! -z "$AMBIENT_LIGHT_VERBOSE" ]; then
        debug "AMBIENT_LIGHT_POSTFIX        $AMBIENT_LIGHT_POSTFIX       "
        info  "AMBIENT_LIGHT_ROOT       $AMBIENT_LIGHT_ROOT      "
        info  "AMBIENT_LIGHT_DIR_TMP        $AMBIENT_LIGHT_DIR_TMP       "
        info  "AMBIENT_LIGHT_FILE_TMP        $AMBIENT_LIGHT_FILE_TMP       "
        debug "AMBIENT_LIGHT_FILE_CMD        $AMBIENT_LIGHT_FILE_CMD       "
        info  "AMBIENT_LIGHT_DIR_TEST        $AMBIENT_LIGHT_DIR_TEST       "
        info  "AMBIENT_LIGHT_PRETTY_PRT     $PP            "
        info  "AMBIENT_LIGHT_VERBOSE        $VV            "
        info  "AMBIENT_LIGHT_FILE_OUTPUT     $AMBIENT_LIGHT_FILE_OUTPUT    "
        info  "AMBIENT_LIGHT_FILE_LOG        $AMBIENT_LIGHT_FILE_LOG       "
        info  "AMBIENT_LIGHT_DURATION       $AMBIENT_LIGHT_DURATION      "
        debug "PATH           $PATH          "
        info  "AMBIENT_LIGHT_DIR_SCENARIOS    $AMBIENT_LIGHT_DIR_SCENARIOS   "
        info  "TMPDIR         $TMPDIR        "
        info  "AMBIENT_LIGHT_SCENARIO_NAMES $AMBIENT_LIGHT_SCENARIO_NAMES"
    fi
}

main()
{
	count=0
	while getopts zx:Sd:qt:po:l:vn:hs:E:I arg
	do  case $arg in
		d)
			AMBIENT_LIGHT_DIR_TEST=${AMBIENT_LIGHT_OPTARG} ;;
		t)
			AMBIENT_LIGHT_DIR_TMP=${AMBIENT_LIGHT_OPTARG} ;;
		E)
			EXTENDED_TEST=y ;;
	        q)
			AMBIENT_LIGHT_QUIET_MODE=" -q " ;;
	        z)
			DONT=" " ;;
		p)
			AMBIENT_LIGHT_PRETTY_PRT=" -p " ;;
		o)
			AMBIENT_LIGHT_FILE_OUTPUT=${AMBIENT_LIGHT_OPTARG};OO_LOG=1 ;;
		l)
			AMBIENT_LIGHT_FILE_LOG=${AMBIENT_LIGHT_OPTARG} ;;
		v)
			AMBIENT_LIGHT_VERBOSE="-v" ;;
		n)
			AMBIENT_LIGHT_DURATION=" -t ${AMBIENT_LIGHT_OPTARG}" ;;
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
			AMBIENT_LIGHT_INSTANCES="-x $AMBIENT_LIGHT_OPTARG -O ${TMP}" ;;
		s)
			AMBIENT_LIGHT_DIR_SCENARIOS=${AMBIENT_LIGHT_OPTARG} ;;
		S)
			AMBIENT_LIGHT_STRESS=y
			AMBIENT_LIGHT_STRESSARG="-S";;

		\?) # Handle illegals
			usage ;;

	esac

	if [ ! -z "${AMBIENT_LIGHT_OPTARG}" ]; then
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

	AMBIENT_LIGHT_SCENARIO_NAMES=$@

	sanity_check

	# Test start

	[ -z "$AMBIENT_LIGHT_QUIET_MODE" ] && { info "Test start time: $(date)" ; }

	# Usage: pan -n name [ -SyAehp ] [ -s starts ] [-t time[s|m|h|d] [ -x nactive ] [-l logfile ]
	# [ -a active-file ] [ -f command-file ] [ -d debug-level ]
	# [-o output-file] [-O output-buffer-directory] [cmd]

	cd $AMBIENT_LIGHT_DIR_TEST
	PAN_COMMAND="${UTILS_DIR_BIN}/pan $AMBIENT_LIGHT_QUIET_MODE -e -S $AMBIENT_LIGHT_INSTANCES $AMBIENT_LIGHT_DURATION -a $$ -n $$ $AMBIENT_LIGHT_PRETTY_PRT -f ${AMBIENT_LIGHT_FILE_CMD} -l $AMBIENT_LIGHT_FILE_LOG"

	[ ! -z "$AMBIENT_LIGHT_VERBOSE" ] && { info "PAN_COMMAND=$PAN_COMMAND"; }

	if [ -z "$OO_LOG" ]; then
		$PAN_COMMAND
	else
		$PAN_COMMAND|tee $AMBIENT_LIGHT_FILE_OUTPUT
	fi

	if [ $? -eq 0 ]; then
		echo "INFO: pan reported all tests PASS"
		VALUE=0
	else
		echo "INFO: pan reported some tests FAIL"
		VALUE=1
	fi

	# Test end
	[ -z "$AMBIENT_LIGHT_QUIET_MODE" ] && { info "Test end time: $(date)" ; }
	[ -z "$AMBIENT_LIGHT_QUIET_MODE" ] && {

	cat <<-EOF >&1

	###############################################################"
		Done executing testcases."
		Result log is in the $AMBIENT_LIGHT_FILE_LOG "
	###############################################################"

	EOF
	cat $AMBIENT_LIGHT_FILE_LOG

	}
	cleanup
	exit $VALUE
}


cleanup()
{
	[  -z "$AMBIENT_LIGHT_QUIET_MODE" ] && echo -n "INFO: Cleaning up..."
	if [ -n "${AMBIENT_LIGHT_FILE_TMP}" -a -n "${AMBIENT_LIGHT_FILE_CMD}" -a -n "${AMBIENT_LIGHT_DIR_TEST}" -a -n "${AMBIENT_LIGHT_DIR_TMP}" ]; then
		rm -rf ${AMBIENT_LIGHT_FILE_TMP} ${AMBIENT_LIGHT_FILE_CMD} ${AMBIENT_LIGHT_DIR_TEST}/* ${AMBIENT_LIGHT_DIR_TMP}/*
	else
		echo "INFO: Clean up process won't be executed because variables for directories to be removed are not set..."
	fi

	[  -z "$AMBIENT_LIGHT_QUIET_MODE" ] && echo "done."
}


trap "cleanup" 0
setup
main "$@"

# End of file
