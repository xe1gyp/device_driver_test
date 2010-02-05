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
	[ -d $ETHERNET_DIR_SCENARIOS ] ||
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
	local PP=` if [ -z "$ETHERNET_PRETTY_PRT" ]; then echo "off"; else echo "on"; fi`
	local VV=` if [ -z "$ETHERNET_VERBOSE" ]; then echo "off"; else echo "on"; fi`

	# Give the gyan
	cat <<-EOF >&2
	usage: ./${0##*/} [-z] [-h] [-v] [-d ETHERNET_DIR_TEST] [-o ETHERNET_FILE_OUTPUT] [-l ETHERNET_FILE_LOG]
	[-n ETHERNET_DURATION ] [-t TMPDIR] [ETHERNET_SCENARIO_NAMES..]

	-d ETHERNET_DIR_TEST      Run LTP to test the filesystem mounted here. [Current - $ETHERNET_DIR_TEST]
			At the end of test, the testdir gets cleaned out
	-s ETHERNET_DIR_SCENARIOS  Test scenarios are located here. [Current - $ETHERNET_DIR_SCENARIOS]
	-o ETHERNET_FILE_OUTPUT   Redirect test output to a file. [Current- $ETHERNET_FILE_OUTPUT {psid}]
	-p              Human readable(dont laugh too much) format logfiles. [Current - ($PP)]
	-z              Dont Merge the Scenario Name with tcid to create final tc id
	-E              Use Extended Test cases also - these are painful and can take real long time
	-l ETHERNET_FILE_LOG      Log results of test in a logfile. [Current- $ETHERNET_FILE_LOG {psid}]
	-t TMPDIR       Run LTP using tmp dir [Current - $ETHERNET_DIR_TMP]
	-n ETHERNET_DURATION     Execute the testsuite for given duration. Examples:
			-n 60s = 60 seconds
			-n 45m = 45 minutes
			-n 24h = 24 hours
			-n 2d  = 2 days
			[Current - $ETHERNET_DURATION]

	-v              Print more verbose output to screen.[Current - ($VV)]
	-q              No messages from this script. no info too - Brave eh??
	-h              This screen [Current - guess...]
	-x INSTANCES    Run multiple instances of this testsuite.(think well...)
	-r PRE_DEF      Run predefined set of scenarios[Not Implemented yet]
			List to appear here
	-S              Run in Stress mode

	ETHERNET_SCENARIO_NAMES  List of scenarios to test.. else, take all scenarios
			[Current - These are all filenames from $ETHERNET_DIR_SCENARIOS]

	Good News: Ctrl+c stops and cleans up for you :)
	More help: Read the $ETHERNET_ROOT/README

	EOF
	exit 0
}

# Insane world.. insane set of human mistakes!!
sanity_check()
{
    # Check the current values...
    # Just ensure that pan can run with a bit of peace of mind...

    [ ! -d "$ETHERNET_DIR_TMP" -o ! -w "$ETHERNET_DIR_TMP" ] && die "$ETHERNET_DIR_TMP - cannot work as temporary directory"
    [ ! -d "$ETHERNET_DIR_TEST" -o ! -w "$ETHERNET_DIR_TEST" ] && die "$ETHERNET_DIR_TEST - cannot work as test directory"
    [ ! -d "$ETHERNET_DIR_SCENARIOS" ] && die "$ETHERNET_DIR_SCENARIOS - No such directories"
    [ -z "$ETHERNET_SCENARIO_NAMES" ] && die "No Scenarios"
		[ ! -z "$ETHERNET_VERBOSE" -a ! -z "$ETHERNET_QUIET_MODE" ] && die "Make up your mind - verbose or quiet??"

    export ETHERNET_FILE_CMD=$ETHERNET_DIR_TMP/$ETHERNET_FILE_CMD
    rm -f $ETHERNET_FILE_CMD

		for SCEN in $ETHERNET_SCENARIO_NAMES
    do
		  [ ! -f "$ETHERNET_DIR_SCENARIOS/$SCEN" -o ! -r "$ETHERNET_DIR_SCENARIOS/$SCEN" ] && die "$ETHERNET_DIR_SCENARIOS/$SCEN - not a scenario file"
			cat $ETHERNET_DIR_SCENARIOS/$SCEN|grep -v "#"|sed -e "s/^[  ]*$//g"|sed -e "/^$/d">$ETHERNET_FILE_TMP|| die "Count not create tmp file $ETHERNET_FILE_TMP"
			if [ -z "$DONT" ]; then
				cat $ETHERNET_FILE_TMP|sed -e "s/^/$SCEN-/g"|sed -e "s/-/_/" >>$ETHERNET_FILE_CMD || die "Count not create command file $ETHERNET_FILE_CMD"
				else
				cat $ETHERNET_FILE_TMP>>$ETHERNET_FILE_CMD || die "Count not create command file $ETHERNET_FILE_CMD"
			fi

			# Remove the extended test cases
			if [ -z "$EXTENDED_TEST" ]; then

				cat $ETHERNET_FILE_CMD|grep -v "^[_A-Za-z0-9]*_EXT ">$ETHERNET_FILE_TMP || die "intermediate file gen failed"
				cat $ETHERNET_FILE_TMP>$ETHERNET_FILE_CMD || die "Second intermediate creation failed"
			fi

			rm -f $ETHERNET_FILE_TMP

    done

		local PP=` if [ -z "$ETHERNET_PRETTY_PRT" ]; then echo "off"; else echo "on"; fi`
    local VV=` if [ -z "$ETHERNET_VERBOSE" ]; then echo "off"; else echo "on"; fi`
    export TMPDIR=${ETHERNET_DIR_TEST}

		# Print some nice info
    if [ ! -z "$ETHERNET_VERBOSE" ]; then
        debug "ETHERNET_POSTFIX        $ETHERNET_POSTFIX       "
        info  "ETHERNET_ROOT       $ETHERNET_ROOT      "
        info  "ETHERNET_DIR_TMP        $ETHERNET_DIR_TMP       "
        info  "ETHERNET_FILE_TMP        $ETHERNET_FILE_TMP       "
        debug "ETHERNET_FILE_CMD        $ETHERNET_FILE_CMD       "
        info  "ETHERNET_DIR_TEST        $ETHERNET_DIR_TEST       "
        info  "ETHERNET_PRETTY_PRT     $PP            "
        info  "ETHERNET_VERBOSE        $VV            "
        info  "ETHERNET_FILE_OUTPUT     $ETHERNET_FILE_OUTPUT    "
        info  "ETHERNET_FILE_LOG        $ETHERNET_FILE_LOG       "
        info  "ETHERNET_DURATION       $ETHERNET_DURATION      "
        debug "PATH           $PATH          "
        info  "ETHERNET_DIR_SCENARIOS    $ETHERNET_DIR_SCENARIOS   "
        info  "TMPDIR         $TMPDIR        "
        info  "ETHERNET_SCENARIO_NAMES $ETHERNET_SCENARIO_NAMES"
    fi
}

main()
{
	count=0
	while getopts zx:Sd:qt:po:l:vn:hs:E:I arg
	do  case $arg in
		d)
			ETHERNET_DIR_TEST=${ETHERNET_OPTARG} ;;
		t)
			ETHERNET_DIR_TMP=${ETHERNET_OPTARG} ;;
		E)
			EXTENDED_TEST=y ;;
	        q)
			ETHERNET_QUIET_MODE=" -q " ;;
	        z)
			DONT=" " ;;
		p)
			ETHERNET_PRETTY_PRT=" -p " ;;
		o)
			ETHERNET_FILE_OUTPUT=${ETHERNET_OPTARG};OO_LOG=1 ;;
		l)
			ETHERNET_FILE_LOG=${ETHERNET_OPTARG} ;;
		v)
			ETHERNET_VERBOSE="-v" ;;
		n)
			ETHERNET_DURATION=" -t ${ETHERNET_OPTARG}" ;;
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
			ETHERNET_INSTANCES="-x $ETHERNET_OPTARG -O ${TMP}" ;;
		s)
			ETHERNET_DIR_SCENARIOS=${ETHERNET_OPTARG} ;;
		S)
			ETHERNET_STRESS=y
			ETHERNET_STRESSARG="-S";;

		\?) # Handle illegals
			usage ;;

	esac

	if [ ! -z "${ETHERNET_OPTARG}" ]; then
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

	ETHERNET_SCENARIO_NAMES=$@

	sanity_check

	# Test start

	[ -z "$ETHERNET_QUIET_MODE" ] && { info "Test start time: $(date)" ; }

	# Usage: pan -n name [ -SyAehp ] [ -s starts ] [-t time[s|m|h|d] [ -x nactive ] [-l logfile ]
	# [ -a active-file ] [ -f command-file ] [ -d debug-level ]
	# [-o output-file] [-O output-buffer-directory] [cmd]

	cd $ETHERNET_DIR_TEST
	PAN_COMMAND="${UTILS_DIR_BIN}/pan $ETHERNET_QUIET_MODE -e -S $ETHERNET_INSTANCES $ETHERNET_DURATION -a $$ -n $$ $ETHERNET_PRETTY_PRT -f ${ETHERNET_FILE_CMD} -l $ETHERNET_FILE_LOG"

	[ ! -z "$ETHERNET_VERBOSE" ] && { info "PAN_COMMAND=$PAN_COMMAND"; }

	if [ -z "$OO_LOG" ]; then
		$PAN_COMMAND
	else
		$PAN_COMMAND|tee $ETHERNET_FILE_OUTPUT
	fi

	if [ $? -eq 0 ]; then
		echo "INFO: pan reported all tests PASS"
		VALUE=0
	else
		echo "INFO: pan reported some tests FAIL"
		VALUE=1
	fi

	# Test end
	[ -z "$ETHERNET_QUIET_MODE" ] && { info "Test end time: $(date)" ; }
	[ -z "$ETHERNET_QUIET_MODE" ] && {

	cat <<-EOF >&1

	###############################################################"
		Done executing testcases."
		Result log is in the $ETHERNET_FILE_LOG "
	###############################################################"

	EOF
	cat $ETHERNET_FILE_LOG

	}
	cleanup
	exit $VALUE
}


cleanup()
{
	[  -z "$ETHERNET_QUIET_MODE" ] && echo -n "INFO: Cleaning up..."
	if [ -n "${ETHERNET_FILE_TMP}" -a -n "${ETHERNET_FILE_CMD}" -a -n "${ETHERNET_DIR_TEST}" -a -n "${ETHERNET_DIR_TMP}" ]; then
		rm -rf ${ETHERNET_FILE_TMP} ${ETHERNET_FILE_CMD} ${ETHERNET_DIR_TEST}/* ${ETHERNET_DIR_TMP}/*
	else
		echo "INFO: Clean up process won't be executed because variables for directories to be removed are not set..."
	fi

	[  -z "$ETHERNET_QUIET_MODE" ] && echo "done."
}


trap "cleanup" 0
setup
main "$@"

# End of file
