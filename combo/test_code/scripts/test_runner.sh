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
	[ -d $COMBO_DIR_SCENARIOS ] ||
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
	local PP=` if [ -z "$COMBO_PRETTY_PRT" ]; then echo "off"; else echo "on"; fi`
	local VV=` if [ -z "$COMBO_VERBOSE" ]; then echo "off"; else echo "on"; fi`

	# Give the gyan
	cat <<-EOF >&2
	usage: ./${0##*/} [-z] [-h] [-v] [-d COMBO_DIR_TEST] [-o COMBO_FILE_OUTPUT] [-l COMBO_FILE_LOG]
	[-n COMBO_DURATION ] [-t TMPDIR] [COMBO_SCENARIO_NAMES..]

	-d COMBO_DIR_TEST      Run LTP to test the filesystem mounted here. [Current - $COMBO_DIR_TEST]
			At the end of test, the testdir gets cleaned out
	-s COMBO_DIR_SCENARIOS  Test scenarios are located here. [Current - $COMBO_DIR_SCENARIOS]
	-o COMBO_FILE_OUTPUT   Redirect test output to a file. [Current- $COMBO_FILE_OUTPUT {psid}]
	-p              Human readable(dont laugh too much) format logfiles. [Current - ($PP)]
	-z              Dont Merge the Scenario Name with tcid to create final tc id
	-E              Use Extended Test cases also - these are painful and can take real long time
	-l COMBO_FILE_LOG      Log results of test in a logfile. [Current- $COMBO_FILE_LOG {psid}]
	-t TMPDIR       Run LTP using tmp dir [Current - $COMBO_DIR_TMP]
	-n COMBO_DURATION     Execute the testsuite for given duration. Examples:
			-n 60s = 60 seconds
			-n 45m = 45 minutes
			-n 24h = 24 hours
			-n 2d  = 2 days
			[Current - $COMBO_DURATION]

	-v              Print more verbose output to screen.[Current - ($VV)]
	-q              No messages from this script. no info too - Brave eh??
	-h              This screen [Current - guess...]
	-x INSTANCES    Run multiple instances of this testsuite.(think well...)
	-r PRE_DEF      Run predefined set of scenarios[Not Implemented yet]
			List to appear here
	-S              Run in Stress mode

	COMBO_SCENARIO_NAMES  List of scenarios to test.. else, take all scenarios
			[Current - These are all filenames from $COMBO_DIR_SCENARIOS]

	Good News: Ctrl+c stops and cleans up for you :)
	More help: Read the $COMBO_ROOT/README

	EOF
	exit 0
}

# Insane world.. insane set of human mistakes!!
sanity_check()
{
    # Check the current values...
    # Just ensure that pan can run with a bit of peace of mind...

    [ ! -d "$COMBO_DIR_TMP" -o ! -w "$COMBO_DIR_TMP" ] && die "$COMBO_DIR_TMP - cannot work as temporary directory"
    [ ! -d "$COMBO_DIR_TEST" -o ! -w "$COMBO_DIR_TEST" ] && die "$COMBO_DIR_TEST - cannot work as test directory"
    [ ! -d "$COMBO_DIR_SCENARIOS" ] && die "$COMBO_DIR_SCENARIOS - No such directories"
    [ -z "$COMBO_SCENARIO_NAMES" ] && die "No Scenarios"
		[ ! -z "$COMBO_VERBOSE" -a ! -z "$COMBO_QUIET_MODE" ] && die "Make up your mind - verbose or quiet??"

    export COMBO_FILE_CMD=$COMBO_DIR_TMP/$COMBO_FILE_CMD
    rm -f $COMBO_FILE_CMD

		for SCEN in $COMBO_SCENARIO_NAMES
    do
		  [ ! -f "$COMBO_DIR_SCENARIOS/$SCEN" -o ! -r "$COMBO_DIR_SCENARIOS/$SCEN" ] && die "$COMBO_DIR_SCENARIOS/$SCEN - not a scenario file"
			cat $COMBO_DIR_SCENARIOS/$SCEN|grep -v "#"|sed -e "s/^[  ]*$//g"|sed -e "/^$/d">$COMBO_FILE_TMP|| die "Count not create tmp file $COMBO_FILE_TMP"
			if [ -z "$DONT" ]; then
				cat $COMBO_FILE_TMP|sed -e "s/^/$SCEN-/g"|sed -e "s/-/_/" >>$COMBO_FILE_CMD || die "Count not create command file $COMBO_FILE_CMD"
				else
				cat $COMBO_FILE_TMP>>$COMBO_FILE_CMD || die "Count not create command file $COMBO_FILE_CMD"
			fi

			# Remove the extended test cases
			if [ -z "$EXTENDED_TEST" ]; then

				cat $COMBO_FILE_CMD|grep -v "^[_A-Za-z0-9]*_EXT ">$COMBO_FILE_TMP || die "intermediate file gen failed"
				cat $COMBO_FILE_TMP>$COMBO_FILE_CMD || die "Second intermediate creation failed"
			fi

			rm -f $COMBO_FILE_TMP

    done

		local PP=` if [ -z "$COMBO_PRETTY_PRT" ]; then echo "off"; else echo "on"; fi`
    local VV=` if [ -z "$COMBO_VERBOSE" ]; then echo "off"; else echo "on"; fi`
    export TMPDIR=${COMBO_DIR_TEST}

		# Print some nice info
    if [ ! -z "$COMBO_VERBOSE" ]; then
        debug "COMBO_POSTFIX        $COMBO_POSTFIX       "
        info  "COMBO_ROOT       $COMBO_ROOT      "
        info  "COMBO_DIR_TMP        $COMBO_DIR_TMP       "
        info  "COMBO_FILE_TMP        $COMBO_FILE_TMP       "
        debug "COMBO_FILE_CMD        $COMBO_FILE_CMD       "
        info  "COMBO_DIR_TEST        $COMBO_DIR_TEST       "
        info  "COMBO_PRETTY_PRT     $PP            "
        info  "COMBO_VERBOSE        $VV            "
        info  "COMBO_FILE_OUTPUT     $COMBO_FILE_OUTPUT    "
        info  "COMBO_FILE_LOG        $COMBO_FILE_LOG       "
        info  "COMBO_DURATION       $COMBO_DURATION      "
        debug "PATH           $PATH          "
        info  "COMBO_DIR_SCENARIOS    $COMBO_DIR_SCENARIOS   "
        info  "TMPDIR         $TMPDIR        "
        info  "COMBO_SCENARIO_NAMES $COMBO_SCENARIO_NAMES"
    fi
}

main()
{
	count=0
	while getopts zx:Sd:qt:po:l:vn:hs:E:I arg
	do  case $arg in
		d)
			COMBO_DIR_TEST=${COMBO_OPTARG} ;;
		t)
			COMBO_DIR_TMP=${COMBO_OPTARG} ;;
		E)
			EXTENDED_TEST=y ;;
	        q)
			COMBO_QUIET_MODE=" -q " ;;
	        z)
			DONT=" " ;;
		p)
			COMBO_PRETTY_PRT=" -p " ;;
		o)
			COMBO_FILE_OUTPUT=${COMBO_OPTARG};OO_LOG=1 ;;
		l)
			COMBO_FILE_LOG=${COMBO_OPTARG} ;;
		v)
			COMBO_VERBOSE="-v" ;;
		n)
			COMBO_DURATION=" -t ${COMBO_OPTARG}" ;;
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
			COMBO_INSTANCES="-x $COMBO_OPTARG -O ${TMP}" ;;
		s)
			COMBO_DIR_SCENARIOS=${COMBO_OPTARG} ;;
		S)
			COMBO_STRESS=y
			COMBO_STRESSARG="-S";;

		\?) # Handle illegals
			usage ;;

	esac

	if [ ! -z "${COMBO_OPTARG}" ]; then
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

	COMBO_SCENARIO_NAMES=$@

	sanity_check

	# Test start

	[ -z "$COMBO_QUIET_MODE" ] && { info "Test start time: $(date)" ; }

	# Usage: pan -n name [ -SyAehp ] [ -s starts ] [-t time[s|m|h|d] [ -x nactive ] [-l logfile ]
	# [ -a active-file ] [ -f command-file ] [ -d debug-level ]
	# [-o output-file] [-O output-buffer-directory] [cmd]

	cd $COMBO_DIR_TEST
	PAN_COMMAND="${UTILS_DIR_BIN}/pan $COMBO_QUIET_MODE -e -S $COMBO_INSTANCES $COMBO_DURATION -a $$ -n $$ $COMBO_PRETTY_PRT -f ${COMBO_FILE_CMD} -l $COMBO_FILE_LOG"

	[ ! -z "$COMBO_VERBOSE" ] && { info "PAN_COMMAND=$PAN_COMMAND"; }

	if [ -z "$OO_LOG" ]; then
		$PAN_COMMAND
	else
		$PAN_COMMAND|tee $COMBO_FILE_OUTPUT
	fi

	if [ $? -eq 0 ]; then
		echo "INFO: pan reported all tests PASS"
		VALUE=0
	else
		echo "INFO: pan reported some tests FAIL"
		VALUE=1
	fi

	# Test end
	[ -z "$COMBO_QUIET_MODE" ] && { info "Test end time: $(date)" ; }
	[ -z "$COMBO_QUIET_MODE" ] && {

	cat <<-EOF >&1

	###############################################################"
		Done executing testcases."
		Result log is in the $COMBO_FILE_LOG "
	###############################################################"

	EOF
	cat $COMBO_FILE_LOG

	}
	cleanup
	exit $VALUE
}


cleanup()
{
	[  -z "$COMBO_QUIET_MODE" ] && echo -n "INFO: Cleaning up..."
	if [ -n "${COMBO_FILE_TMP}" -a -n "${COMBO_FILE_CMD}" -a -n "${COMBO_DIR_TEST}" -a -n "${COMBO_DIR_TMP}" ]; then
		rm -rf ${COMBO_FILE_TMP} ${COMBO_FILE_CMD} ${COMBO_DIR_TEST}/* ${COMBO_DIR_TMP}/*
	else
		echo "INFO: Clean up process won't be executed because variables for directories to be removed are not set..."
	fi

	[  -z "$COMBO_QUIET_MODE" ] && echo "done."
}


trap "cleanup" 0
setup
main "$@"

# End of file
