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
	[ -d $HSUART_DIR_SCENARIOS ] ||
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
	local PP=` if [ -z "$HSUART_PRETTY_PRT" ]; then echo "off"; else echo "on"; fi`
	local VV=` if [ -z "$HSUART_VERBOSE" ]; then echo "off"; else echo "on"; fi`

	# Give the gyan
	cat <<-EOF >&2
	usage: ./${0##*/} [-z] [-h] [-v] [-d HSUART_DIR_TEST] [-o HSUART_FILE_OUTPUT] [-l HSUART_FILE_LOG]
	[-n HSUART_DURATION ] [-t TMPDIR] [HSUART_SCENARIO_NAMES..]

	-d HSUART_DIR_TEST      Run LTP to test the filesystem mounted here. [Current - $HSUART_DIR_TEST]
			At the end of test, the testdir gets cleaned out
	-s HSUART_DIR_SCENARIOS  Test scenarios are located here. [Current - $HSUART_DIR_SCENARIOS]
	-o HSUART_FILE_OUTPUT   Redirect test output to a file. [Current- $HSUART_FILE_OUTPUT {psid}]
	-p              Human readable(dont laugh too much) format logfiles. [Current - ($PP)]
	-z              Dont Merge the Scenario Name with tcid to create final tc id
	-E              Use Extended Test cases also - these are painful and can take real long time
	-l HSUART_FILE_LOG      Log results of test in a logfile. [Current- $HSUART_FILE_LOG {psid}]
	-t TMPDIR       Run LTP using tmp dir [Current - $HSUART_DIR_TMP]
	-n HSUART_DURATION     Execute the testsuite for given duration. Examples:
			-n 60s = 60 seconds
			-n 45m = 45 minutes
			-n 24h = 24 hours
			-n 2d  = 2 days
			[Current - $HSUART_DURATION]

	-v              Print more verbose output to screen.[Current - ($VV)]
	-q              No messages from this script. no info too - Brave eh??
	-h              This screen [Current - guess...]
	-x INSTANCES    Run multiple instances of this testsuite.(think well...)
	-r PRE_DEF      Run predefined set of scenarios[Not Implemented yet]
			List to appear here
	-S              Run in Stress mode

	HSUART_SCENARIO_NAMES  List of scenarios to test.. else, take all scenarios
			[Current - These are all filenames from $HSUART_DIR_SCENARIOS]

	Good News: Ctrl+c stops and cleans up for you :)
	More help: Read the $HSUART_ROOT/README

	EOF
	exit 0
}

# Insane world.. insane set of human mistakes!!
sanity_check()
{
    # Check the current values...
    # Just ensure that pan can run with a bit of peace of mind...

    [ ! -d "$HSUART_DIR_TMP" -o ! -w "$HSUART_DIR_TMP" ] && die "$HSUART_DIR_TMP - cannot work as temporary directory"
    [ ! -d "$HSUART_DIR_TEST" -o ! -w "$HSUART_DIR_TEST" ] && die "$HSUART_DIR_TEST - cannot work as test directory"
    [ ! -d "$HSUART_DIR_SCENARIOS" ] && die "$HSUART_DIR_SCENARIOS - No such directories"
    [ -z "$HSUART_SCENARIO_NAMES" ] && die "No Scenarios"
		[ ! -z "$HSUART_VERBOSE" -a ! -z "$HSUART_QUIET_MODE" ] && die "Make up your mind - verbose or quiet??"

    export HSUART_FILE_CMD=$HSUART_DIR_TMP/$HSUART_FILE_CMD
    rm -f $HSUART_FILE_CMD

		for SCEN in $HSUART_SCENARIO_NAMES
    do
		  [ ! -f "$HSUART_DIR_SCENARIOS/$SCEN" -o ! -r "$HSUART_DIR_SCENARIOS/$SCEN" ] && die "$HSUART_DIR_SCENARIOS/$SCEN - not a scenario file"
			cat $HSUART_DIR_SCENARIOS/$SCEN|grep -v "#"|sed -e "s/^[  ]*$//g"|sed -e "/^$/d">$HSUART_FILE_TMP|| die "Count not create tmp file $HSUART_FILE_TMP"
			if [ -z "$DONT" ]; then
				cat $HSUART_FILE_TMP|sed -e "s/^/$SCEN-/g"|sed -e "s/-/_/" >>$HSUART_FILE_CMD || die "Count not create command file $HSUART_FILE_CMD"
				else
				cat $HSUART_FILE_TMP>>$HSUART_FILE_CMD || die "Count not create command file $HSUART_FILE_CMD"
			fi

			# Remove the extended test cases
			if [ -z "$EXTENDED_TEST" ]; then

				cat $HSUART_FILE_CMD|grep -v "^[_A-Za-z0-9]*_EXT ">$HSUART_FILE_TMP || die "intermediate file gen failed"
				cat $HSUART_FILE_TMP>$HSUART_FILE_CMD || die "Second intermediate creation failed"
			fi

			rm -f $HSUART_FILE_TMP

    done

		local PP=` if [ -z "$HSUART_PRETTY_PRT" ]; then echo "off"; else echo "on"; fi`
    local VV=` if [ -z "$HSUART_VERBOSE" ]; then echo "off"; else echo "on"; fi`
    export TMPDIR=${HSUART_DIR_TEST}

		# Print some nice info
    if [ ! -z "$HSUART_VERBOSE" ]; then
        debug "HSUART_POSTFIX        $HSUART_POSTFIX       "
        info  "HSUART_ROOT       $HSUART_ROOT      "
        info  "HSUART_DIR_TMP        $HSUART_DIR_TMP       "
        info  "HSUART_FILE_TMP        $HSUART_FILE_TMP       "
        debug "HSUART_FILE_CMD        $HSUART_FILE_CMD       "
        info  "HSUART_DIR_TEST        $HSUART_DIR_TEST       "
        info  "HSUART_PRETTY_PRT     $PP            "
        info  "HSUART_VERBOSE        $VV            "
        info  "HSUART_FILE_OUTPUT     $HSUART_FILE_OUTPUT    "
        info  "HSUART_FILE_LOG        $HSUART_FILE_LOG       "
        info  "HSUART_DURATION       $HSUART_DURATION      "
        debug "PATH           $PATH          "
        info  "HSUART_DIR_SCENARIOS    $HSUART_DIR_SCENARIOS   "
        info  "TMPDIR         $TMPDIR        "
        info  "HSUART_SCENARIO_NAMES $HSUART_SCENARIO_NAMES"
    fi
}

main()
{
	count=0
	while getopts zx:Sd:qt:po:l:vn:hs:E:I arg
	do  case $arg in
		d)
			HSUART_DIR_TEST=${HSUART_OPTARG} ;;
		t)
			HSUART_DIR_TMP=${HSUART_OPTARG} ;;
		E)
			EXTENDED_TEST=y ;;
	        q)
			HSUART_QUIET_MODE=" -q " ;;
	        z)
			DONT=" " ;;
		p)
			HSUART_PRETTY_PRT=" -p " ;;
		o)
			HSUART_FILE_OUTPUT=${HSUART_OPTARG};OO_LOG=1 ;;
		l)
			HSUART_FILE_LOG=${HSUART_OPTARG} ;;
		v)
			HSUART_VERBOSE="-v" ;;
		n)
			HSUART_DURATION=" -t ${HSUART_OPTARG}" ;;
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
			HSUART_INSTANCES="-x $HSUART_OPTARG -O ${TMP}" ;;
		s)
			HSUART_DIR_SCENARIOS=${HSUART_OPTARG} ;;
		S)
			HSUART_STRESS=y
			HSUART_STRESSARG="-S";;

		\?) # Handle illegals
			usage ;;

	esac

	if [ ! -z "${HSUART_OPTARG}" ]; then
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

	HSUART_SCENARIO_NAMES=$@

	sanity_check

	# Test start

	[ -z "$HSUART_QUIET_MODE" ] && { info "Test start time: $(date)" ; }

	# Usage: pan -n name [ -SyAehp ] [ -s starts ] [-t time[s|m|h|d] [ -x nactive ] [-l logfile ]
	# [ -a active-file ] [ -f command-file ] [ -d debug-level ]
	# [-o output-file] [-O output-buffer-directory] [cmd]

	cd $HSUART_DIR_TEST
	PAN_COMMAND="${UTILS_DIR_BIN}/pan $HSUART_QUIET_MODE -e -S $HSUART_INSTANCES $HSUART_DURATION -a $$ -n $$ $HSUART_PRETTY_PRT -f ${HSUART_FILE_CMD} -l $HSUART_FILE_LOG"

	[ ! -z "$HSUART_VERBOSE" ] && { info "PAN_COMMAND=$PAN_COMMAND"; }

	if [ -z "$OO_LOG" ]; then
		$PAN_COMMAND
	else
		$PAN_COMMAND|tee $HSUART_FILE_OUTPUT
	fi

	if [ $? -eq 0 ]; then
		echo "INFO: pan reported all tests PASS"
		VALUE=0
	else
		echo "INFO: pan reported some tests FAIL"
		VALUE=1
	fi

	# Test end
	[ -z "$HSUART_QUIET_MODE" ] && { info "Test end time: $(date)" ; }
	[ -z "$HSUART_QUIET_MODE" ] && {

	cat <<-EOF >&1

	###############################################################"
		Done executing testcases."
		Result log is in the $HSUART_FILE_LOG "
	###############################################################"

	EOF
	cat $HSUART_FILE_LOG

	}
	cleanup
	exit $VALUE
}


cleanup()
{
	[  -z "$HSUART_QUIET_MODE" ] && echo -n "INFO: Cleaning up..."
	if [ -n "${HSUART_FILE_TMP}" -a -n "${HSUART_FILE_CMD}" -a -n "${HSUART_DIR_TEST}" -a -n "${HSUART_DIR_TMP}" ]; then
		rm -rf ${HSUART_FILE_TMP} ${HSUART_FILE_CMD} ${HSUART_DIR_TEST}/* ${HSUART_DIR_TMP}/*
	else
		echo "INFO: Clean up process won't be executed because variables for directories to be removed are not set..."
	fi

	[  -z "$HSUART_QUIET_MODE" ] && echo "done."
}


trap "cleanup" 0
setup
main "$@"

# End of file
