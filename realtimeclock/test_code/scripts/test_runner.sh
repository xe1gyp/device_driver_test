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
	[ -d $RTC_DIR_SCENARIOS ] ||
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
	local PP=` if [ -z "$RTC_PRETTY_PRT" ]; then echo "off"; else echo "on"; fi`
	local VV=` if [ -z "$RTC_VERBOSE" ]; then echo "off"; else echo "on"; fi`

	# Give the gyan
	cat <<-EOF >&2
	usage: ./${0##*/} [-z] [-h] [-v] [-d RTC_DIR_TEST] [-o RTC_FILE_OUTPUT] [-l RTC_FILE_LOG]
	[-n RTC_DURATION ] [-t TMPDIR] [RTC_SCENARIO_NAMES..]

	-d RTC_DIR_TEST      Run LTP to test the filesystem mounted here. [Current - $RTC_DIR_TEST]
			At the end of test, the testdir gets cleaned out
	-s RTC_DIR_SCENARIOS  Test scenarios are located here. [Current - $RTC_DIR_SCENARIOS]
	-o RTC_FILE_OUTPUT   Redirect test output to a file. [Current- $RTC_FILE_OUTPUT {psid}]
	-p              Human readable(dont laugh too much) format logfiles. [Current - ($PP)]
	-z              Dont Merge the Scenario Name with tcid to create final tc id
	-E              Use Extended Test cases also - these are painful and can take real long time
	-l RTC_FILE_LOG      Log results of test in a logfile. [Current- $RTC_FILE_LOG {psid}]
	-t TMPDIR       Run LTP using tmp dir [Current - $RTC_DIR_TMP]
	-n RTC_DURATION     Execute the testsuite for given duration. Examples:
			-n 60s = 60 seconds
			-n 45m = 45 minutes
			-n 24h = 24 hours
			-n 2d  = 2 days
			[Current - $RTC_DURATION]

	-v              Print more verbose output to screen.[Current - ($VV)]
	-q              No messages from this script. no info too - Brave eh??
	-h              This screen [Current - guess...]
	-x INSTANCES    Run multiple instances of this testsuite.(think well...)
	-r PRE_DEF      Run predefined set of scenarios[Not Implemented yet]
			List to appear here
	-S              Run in Stress mode

	RTC_SCENARIO_NAMES  List of scenarios to test.. else, take all scenarios
			[Current - These are all filenames from $RTC_DIR_SCENARIOS]

	Good News: Ctrl+c stops and cleans up for you :)
	More help: Read the $RTC_ROOT/README

	EOF
	exit 0
}

# Insane world.. insane set of human mistakes!!
sanity_check()
{
    # Check the current values...
    # Just ensure that pan can run with a bit of peace of mind...

    [ ! -d "$RTC_DIR_TMP" -o ! -w "$RTC_DIR_TMP" ] && die "$RTC_DIR_TMP - cannot work as temporary directory"
    [ ! -d "$RTC_DIR_TEST" -o ! -w "$RTC_DIR_TEST" ] && die "$RTC_DIR_TEST - cannot work as test directory"
    [ ! -d "$RTC_DIR_SCENARIOS" ] && die "$RTC_DIR_SCENARIOS - No such directories"
    [ -z "$RTC_SCENARIO_NAMES" ] && die "No Scenarios"
		[ ! -z "$RTC_VERBOSE" -a ! -z "$RTC_QUIET_MODE" ] && die "Make up your mind - verbose or quiet??"

    export RTC_FILE_CMD=$RTC_DIR_TMP/$RTC_FILE_CMD
    rm -f $RTC_FILE_CMD

		for SCEN in $RTC_SCENARIO_NAMES
    do
		  [ ! -f "$RTC_DIR_SCENARIOS/$SCEN" -o ! -r "$RTC_DIR_SCENARIOS/$SCEN" ] && die "$RTC_DIR_SCENARIOS/$SCEN - not a scenario file"
			cat $RTC_DIR_SCENARIOS/$SCEN|grep -v "#"|sed -e "s/^[  ]*$//g"|sed -e "/^$/d">$RTC_FILE_TMP|| die "Count not create tmp file $RTC_FILE_TMP"
			if [ -z "$DONT" ]; then
				cat $RTC_FILE_TMP|sed -e "s/^/$SCEN-/g"|sed -e "s/-/_/" >>$RTC_FILE_CMD || die "Count not create command file $RTC_FILE_CMD"
				else
				cat $RTC_FILE_TMP>>$RTC_FILE_CMD || die "Count not create command file $RTC_FILE_CMD"
			fi

			# Remove the extended test cases
			if [ -z "$EXTENDED_TEST" ]; then

				cat $RTC_FILE_CMD|grep -v "^[_A-Za-z0-9]*_EXT ">$RTC_FILE_TMP || die "intermediate file gen failed"
				cat $RTC_FILE_TMP>$RTC_FILE_CMD || die "Second intermediate creation failed"
			fi

			rm -f $RTC_FILE_TMP

    done

		local PP=` if [ -z "$RTC_PRETTY_PRT" ]; then echo "off"; else echo "on"; fi`
    local VV=` if [ -z "$RTC_VERBOSE" ]; then echo "off"; else echo "on"; fi`
    export TMPDIR=${RTC_DIR_TEST}

		# Print some nice info
    if [ ! -z "$RTC_VERBOSE" ]; then
        debug "RTC_POSTFIX        $RTC_POSTFIX       "
        info  "RTC_ROOT       $RTC_ROOT      "
        info  "RTC_DIR_TMP        $RTC_DIR_TMP       "
        info  "RTC_FILE_TMP        $RTC_FILE_TMP       "
        debug "RTC_FILE_CMD        $RTC_FILE_CMD       "
        info  "RTC_DIR_TEST        $RTC_DIR_TEST       "
        info  "RTC_PRETTY_PRT     $PP            "
        info  "RTC_VERBOSE        $VV            "
        info  "RTC_FILE_OUTPUT     $RTC_FILE_OUTPUT    "
        info  "RTC_FILE_LOG        $RTC_FILE_LOG       "
        info  "RTC_DURATION       $RTC_DURATION      "
        debug "PATH           $PATH          "
        info  "RTC_DIR_SCENARIOS    $RTC_DIR_SCENARIOS   "
        info  "TMPDIR         $TMPDIR        "
        info  "RTC_SCENARIO_NAMES $RTC_SCENARIO_NAMES"
    fi
}

main()
{
	count=0
	while getopts zx:Sd:qt:po:l:vn:hs:E:I arg
	do  case $arg in
		d)
			RTC_DIR_TEST=${RTC_OPTARG} ;;
		t)
			RTC_DIR_TMP=${RTC_OPTARG} ;;
		E)
			EXTENDED_TEST=y ;;
	        q)
			RTC_QUIET_MODE=" -q " ;;
	        z)
			DONT=" " ;;
		p)
			RTC_PRETTY_PRT=" -p " ;;
		o)
			RTC_FILE_OUTPUT=${RTC_OPTARG};OO_LOG=1 ;;
		l)
			RTC_FILE_LOG=${RTC_OPTARG} ;;
		v)
			RTC_VERBOSE="-v" ;;
		n)
			RTC_DURATION=" -t ${RTC_OPTARG}" ;;
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
			RTC_INSTANCES="-x $RTC_OPTARG -O ${TMP}" ;;
		s)
			RTC_DIR_SCENARIOS=${RTC_OPTARG} ;;
		S)
			RTC_STRESS=y
			RTC_STRESSARG="-S";;

		\?) # Handle illegals
			usage ;;

	esac

	if [ ! -z "${RTC_OPTARG}" ]; then
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

	RTC_SCENARIO_NAMES=$@

	sanity_check

	# Test start

	[ -z "$RTC_QUIET_MODE" ] && { info "Test start time: $(date)" ; }

	# Usage: pan -n name [ -SyAehp ] [ -s starts ] [-t time[s|m|h|d] [ -x nactive ] [-l logfile ]
	# [ -a active-file ] [ -f command-file ] [ -d debug-level ]
	# [-o output-file] [-O output-buffer-directory] [cmd]

	cd $RTC_DIR_TEST
	PAN_COMMAND="${UTILS_DIR_BIN}/pan $RTC_QUIET_MODE -e -S $RTC_INSTANCES $RTC_DURATION -a $$ -n $$ $RTC_PRETTY_PRT -f ${RTC_FILE_CMD} -l $RTC_FILE_LOG"

	[ ! -z "$RTC_VERBOSE" ] && { info "PAN_COMMAND=$PAN_COMMAND"; }

	if [ -z "$OO_LOG" ]; then
		$PAN_COMMAND
	else
		$PAN_COMMAND|tee $RTC_FILE_OUTPUT
	fi

	if [ $? -eq 0 ]; then
		echo "INFO: pan reported all tests PASS"
		VALUE=0
	else
		echo "INFO: pan reported some tests FAIL"
		VALUE=1
	fi

	# Test end
	[ -z "$RTC_QUIET_MODE" ] && { info "Test end time: $(date)" ; }
	[ -z "$RTC_QUIET_MODE" ] && {

	cat <<-EOF >&1

	###############################################################"
		Done executing testcases."
		Result log is in the $RTC_FILE_LOG "
	###############################################################"

	EOF
	cat $RTC_FILE_LOG

	}
	cleanup
	exit $VALUE
}


cleanup()
{
	[  -z "$RTC_QUIET_MODE" ] && echo -n "INFO: Cleaning up..."
	if [ -n "${RTC_FILE_TMP}" -a -n "${RTC_FILE_CMD}" -a -n "${RTC_DIR_TEST}" -a -n "${RTC_DIR_TMP}" ]; then
		rm -rf ${RTC_FILE_TMP} ${RTC_FILE_CMD} ${RTC_DIR_TEST}/* ${RTC_DIR_TMP}/*
	else
		echo "INFO: Clean up process won't be executed because variables for directories to be removed are not set..."
	fi

	[  -z "$RTC_QUIET_MODE" ] && echo "done."
}


trap "cleanup" 0
setup
main "$@"

# End of file
