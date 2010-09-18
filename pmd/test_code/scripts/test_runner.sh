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
	[ -d $PMD_DIR_SCENARIOS ] ||
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
	local PP=` if [ -z "$PMD_PRETTY_PRT" ]; then echo "off"; else echo "on"; fi`
	local VV=` if [ -z "$PMD_VERBOSE" ]; then echo "off"; else echo "on"; fi`

	# Give the gyan
	cat <<-EOF >&2
	usage: ./${0##*/} [-z] [-h] [-v] [-d PMD_DIR_TEST] [-o PMD_FILE_OUTPUT] [-l PMD_FILE_LOG]
	[-n PMD_DURATION ] [-t TMPDIR] [PMD_SCENARIO_NAMES..]

	-d PMD_DIR_TEST      Run LTP to test the filesystem mounted here. [Current - $PMD_DIR_TEST]
			At the end of test, the testdir gets cleaned out
	-s PMD_DIR_SCENARIOS  Test scenarios are located here. [Current - $PMD_DIR_SCENARIOS]
	-o PMD_FILE_OUTPUT   Redirect test output to a file. [Current- $PMD_FILE_OUTPUT {psid}]
	-p              Human readable(dont laugh too much) format logfiles. [Current - ($PP)]
	-z              Dont Merge the Scenario Name with tcid to create final tc id
	-E              Use Extended Test cases also - these are painful and can take real long time
	-l PMD_FILE_LOG      Log results of test in a logfile. [Current- $PMD_FILE_LOG {psid}]
	-t TMPDIR       Run LTP using tmp dir [Current - $PMD_DIR_TMP]
	-n PMD_DURATION     Execute the testsuite for given duration. Examples:
			-n 60s = 60 seconds
			-n 45m = 45 minutes
			-n 24h = 24 hours
			-n 2d  = 2 days
			[Current - $PMD_DURATION]

	-v              Print more verbose output to screen.[Current - ($VV)]
	-q              No messages from this script. no info too - Brave eh??
	-h              This screen [Current - guess...]
	-x INSTANCES    Run multiple instances of this testsuite.(think well...)
	-r PRE_DEF      Run predefined set of scenarios[Not Implemented yet]
			List to appear here
	-S              Run in Stress mode

	PMD_SCENARIO_NAMES  List of scenarios to test.. else, take all scenarios
			[Current - These are all filenames from $PMD_DIR_SCENARIOS]

	Good News: Ctrl+c stops and cleans up for you :)
	More help: Read the $PMD_ROOT/README

	EOF
	exit 0
}

# Insane world.. insane set of human mistakes!!
sanity_check()
{
    # Check the current values...
    # Just ensure that pan can run with a bit of peace of mind...

    [ ! -d "$PMD_DIR_TMP" -o ! -w "$PMD_DIR_TMP" ] && die "$PMD_DIR_TMP - cannot work as temporary directory"
    [ ! -d "$PMD_DIR_TEST" -o ! -w "$PMD_DIR_TEST" ] && die "$PMD_DIR_TEST - cannot work as test directory"
    [ ! -d "$PMD_DIR_SCENARIOS" ] && die "$PMD_DIR_SCENARIOS - No such directories"
    [ -z "$PMD_SCENARIO_NAMES" ] && die "No Scenarios"
		[ ! -z "$PMD_VERBOSE" -a ! -z "$PMD_QUIET_MODE" ] && die "Make up your mind - verbose or quiet??"

    export PMD_FILE_CMD=$PMD_DIR_TMP/$PMD_FILE_CMD
    rm -f $PMD_FILE_CMD

		for SCEN in $PMD_SCENARIO_NAMES
    do
		  [ ! -f "$PMD_DIR_SCENARIOS/$SCEN" -o ! -r "$PMD_DIR_SCENARIOS/$SCEN" ] && die "$PMD_DIR_SCENARIOS/$SCEN - not a scenario file"
			cat $PMD_DIR_SCENARIOS/$SCEN|grep -v "#"|sed -e "s/^[  ]*$//g"|sed -e "/^$/d">$PMD_FILE_TMP|| die "Count not create tmp file $PMD_FILE_TMP"
			if [ -z "$DONT" ]; then
				cat $PMD_FILE_TMP|sed -e "s/^/$SCEN-/g"|sed -e "s/-/_/" >>$PMD_FILE_CMD || die "Count not create command file $PMD_FILE_CMD"
				else
				cat $PMD_FILE_TMP>>$PMD_FILE_CMD || die "Count not create command file $PMD_FILE_CMD"
			fi

			# Remove the extended test cases
			if [ -z "$EXTENDED_TEST" ]; then

				cat $PMD_FILE_CMD|grep -v "^[_A-Za-z0-9]*_EXT ">$PMD_FILE_TMP || die "intermediate file gen failed"
				cat $PMD_FILE_TMP>$PMD_FILE_CMD || die "Second intermediate creation failed"
			fi

			rm -f $PMD_FILE_TMP

    done

		local PP=` if [ -z "$PMD_PRETTY_PRT" ]; then echo "off"; else echo "on"; fi`
    local VV=` if [ -z "$PMD_VERBOSE" ]; then echo "off"; else echo "on"; fi`
    export TMPDIR=${PMD_DIR_TEST}

		# Print some nice info
    if [ ! -z "$PMD_VERBOSE" ]; then
        debug "PMD_POSTFIX        $PMD_POSTFIX       "
        info  "PMD_ROOT       $PMD_ROOT      "
        info  "PMD_DIR_TMP        $PMD_DIR_TMP       "
        info  "PMD_FILE_TMP        $PMD_FILE_TMP       "
        debug "PMD_FILE_CMD        $PMD_FILE_CMD       "
        info  "PMD_DIR_TEST        $PMD_DIR_TEST       "
        info  "PMD_PRETTY_PRT     $PP            "
        info  "PMD_VERBOSE        $VV            "
        info  "PMD_FILE_OUTPUT     $PMD_FILE_OUTPUT    "
        info  "PMD_FILE_LOG        $PMD_FILE_LOG       "
        info  "PMD_DURATION       $PMD_DURATION      "
        debug "PATH           $PATH          "
        info  "PMD_DIR_SCENARIOS    $PMD_DIR_SCENARIOS   "
        info  "TMPDIR         $TMPDIR        "
        info  "PMD_SCENARIO_NAMES $PMD_SCENARIO_NAMES"
    fi
}

main()
{
	count=0
	while getopts zx:Sd:qt:po:l:vn:hs:E:I arg
	do  case $arg in
		d)
			PMD_DIR_TEST=${PMD_OPTARG} ;;
		t)
			PMD_DIR_TMP=${PMD_OPTARG} ;;
		E)
			EXTENDED_TEST=y ;;
	        q)
			PMD_QUIET_MODE=" -q " ;;
	        z)
			DONT=" " ;;
		p)
			PMD_PRETTY_PRT=" -p " ;;
		o)
			PMD_FILE_OUTPUT=${PMD_OPTARG};OO_LOG=1 ;;
		l)
			PMD_FILE_LOG=${PMD_OPTARG} ;;
		v)
			PMD_VERBOSE="-v" ;;
		n)
			PMD_DURATION=" -t ${PMD_OPTARG}" ;;
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
			PMD_INSTANCES="-x $PMD_OPTARG -O ${TMP}" ;;
		s)
			PMD_DIR_SCENARIOS=${PMD_OPTARG} ;;
		S)
			PMD_STRESS=y
			PMD_STRESSARG="-S";;

		\?) # Handle illegals
			usage ;;

	esac

	if [ ! -z "${PMD_OPTARG}" ]; then
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

	PMD_SCENARIO_NAMES=$@

	sanity_check

	# Test start

	[ -z "$PMD_QUIET_MODE" ] && { info "Test start time: $(date)" ; }

	# Usage: pan -n name [ -SyAehp ] [ -s starts ] [-t time[s|m|h|d] [ -x nactive ] [-l logfile ]
	# [ -a active-file ] [ -f command-file ] [ -d debug-level ]
	# [-o output-file] [-O output-buffer-directory] [cmd]

	cd $PMD_DIR_TEST
	PAN_COMMAND="${UTILS_DIR_BIN}/pan $PMD_QUIET_MODE -e -S $PMD_INSTANCES $PMD_DURATION -a $$ -n $$ $PMD_PRETTY_PRT -f ${PMD_FILE_CMD} -l $PMD_FILE_LOG"

	[ ! -z "$PMD_VERBOSE" ] && { info "PAN_COMMAND=$PAN_COMMAND"; }

	if [ -z "$OO_LOG" ]; then
		$PAN_COMMAND
	else
		$PAN_COMMAND|tee $PMD_FILE_OUTPUT
	fi

	if [ $? -eq 0 ]; then
		echo "INFO: pan reported all tests PASS"
		VALUE=0
	else
		echo "INFO: pan reported some tests FAIL"
		VALUE=1
	fi

	# Test end
	[ -z "$PMD_QUIET_MODE" ] && { info "Test end time: $(date)" ; }
	[ -z "$PMD_QUIET_MODE" ] && {

	cat <<-EOF >&1

	###############################################################"
		Done executing testcases."
		Result log is in the $PMD_FILE_LOG "
	###############################################################"

	EOF
	cat $PMD_FILE_LOG

	}
	cleanup
	exit $VALUE
}


cleanup()
{
	[  -z "$PMD_QUIET_MODE" ] && echo -n "INFO: Cleaning up..."
	if [ -n "${PMD_FILE_TMP}" -a -n "${PMD_FILE_CMD}" -a -n "${PMD_DIR_TEST}" -a -n "${PMD_DIR_TMP}" ]; then
		rm -rf ${PMD_FILE_TMP} ${PMD_FILE_CMD} ${PMD_DIR_TEST}/* ${PMD_DIR_TMP}/*
	else
		echo "INFO: Clean up process won't be executed because variables for directories to be removed are not set..."
	fi

	[  -z "$PMD_QUIET_MODE" ] && echo "done."
}


trap "cleanup" 0
setup
main "$@"

# End of file
