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
	[ -d $USBHOST_DIR_SCENARIOS ] ||
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
	local PP=` if [ -z "$USBHOST_PRETTY_PRT" ]; then echo "off"; else echo "on"; fi`
	local VV=` if [ -z "$USBHOST_VERBOSE" ]; then echo "off"; else echo "on"; fi`
	
	# Give the gyan
	cat <<-EOF >&2
	usage: ./${0##*/} [-z] [-h] [-v] [-d USBHOST_DIR_TEST] [-o USBHOST_FILE_OUTPUT] [-l USBHOST_FILE_LOG] 
	[-n USBHOST_DURATION ] [-t TMPDIR] [USBHOST_SCENARIO_NAMES..]

	-d USBHOST_DIR_TEST      Run LTP to test the filesystem mounted here. [Current - $USBHOST_DIR_TEST]
			At the end of test, the testdir gets cleaned out
	-s USBHOST_DIR_SCENARIOS  Test scenarios are located here. [Current - $USBHOST_DIR_SCENARIOS]
	-o USBHOST_FILE_OUTPUT   Redirect test output to a file. [Current- $USBHOST_FILE_OUTPUT {psid}]
	-p              Human readable(dont laugh too much) format logfiles. [Current - ($PP)]
	-z              Dont Merge the Scenario Name with tcid to create final tc id
	-E              Use Extended Test cases also - these are painful and can take real long time
	-l USBHOST_FILE_LOG      Log results of test in a logfile. [Current- $USBHOST_FILE_LOG {psid}]
	-t TMPDIR       Run LTP using tmp dir [Current - $USBHOST_DIR_TMP]
	-n USBHOST_DURATION     Execute the testsuite for given duration. Examples:
			-n 60s = 60 seconds
			-n 45m = 45 minutes
			-n 24h = 24 hours
			-n 2d  = 2 days
			[Current - $USBHOST_DURATION]

	-v              Print more verbose output to screen.[Current - ($VV)]
	-q              No messages from this script. no info too - Brave eh??
	-h              This screen [Current - guess...]
	-x INSTANCES    Run multiple instances of this testsuite.(think well...)
	-r PRE_DEF      Run predefined set of scenarios[Not Implemented yet]
			List to appear here
	-S              Run in Stress mode
	
	USBHOST_SCENARIO_NAMES  List of scenarios to test.. else, take all scenarios 
			[Current - These are all filenames from $USBHOST_DIR_SCENARIOS]
    
	Good News: Ctrl+c stops and cleans up for you :)
	More help: Read the $USBHOST_ROOT/README
		
	EOF
	exit 0
}

# Insane world.. insane set of human mistakes!!
sanity_check()
{
    # Check the current values...
    # Just ensure that pan can run with a bit of peace of mind...
    
    [ ! -d "$USBHOST_DIR_TMP" -o ! -w "$USBHOST_DIR_TMP" ] && die "$USBHOST_DIR_TMP - cannot work as temporary directory"
    [ ! -d "$USBHOST_DIR_TEST" -o ! -w "$USBHOST_DIR_TEST" ] && die "$USBHOST_DIR_TEST - cannot work as test directory"
    [ ! -d "$USBHOST_DIR_SCENARIOS" ] && die "$USBHOST_DIR_SCENARIOS - No such directories"
    [ -z "$USBHOST_SCENARIO_NAMES" ] && die "No Scenarios"
		[ ! -z "$USBHOST_VERBOSE" -a ! -z "$USBHOST_QUIET_MODE" ] && die "Make up your mind - verbose or quiet??"
		
    export USBHOST_FILE_CMD=$USBHOST_DIR_TMP/$USBHOST_FILE_CMD
    rm -f $USBHOST_FILE_CMD
    
		for SCEN in $USBHOST_SCENARIO_NAMES
    do
		  [ ! -f "$USBHOST_DIR_SCENARIOS/$SCEN" -o ! -r "$USBHOST_DIR_SCENARIOS/$SCEN" ] && die "$USBHOST_DIR_SCENARIOS/$SCEN - not a scenario file"
			cat $USBHOST_DIR_SCENARIOS/$SCEN|grep -v "#"|sed -e "s/^[  ]*$//g"|sed -e "/^$/d">$USBHOST_FILE_TMP|| die "Count not create tmp file $USBHOST_FILE_TMP"
			if [ -z "$DONT" ]; then
				cat $USBHOST_FILE_TMP|sed -e "s/^/$SCEN-/g"|sed -e "s/-/_/" >>$USBHOST_FILE_CMD || die "Count not create command file $USBHOST_FILE_CMD"
				else
				cat $USBHOST_FILE_TMP>>$USBHOST_FILE_CMD || die "Count not create command file $USBHOST_FILE_CMD"
			fi

			# Remove the extended test cases
			if [ -z "$EXTENDED_TEST" ]; then
				
				cat $USBHOST_FILE_CMD|grep -v "^[_A-Za-z0-9]*_EXT ">$USBHOST_FILE_TMP || die "intermediate file gen failed"
				cat $USBHOST_FILE_TMP>$USBHOST_FILE_CMD || die "Second intermediate creation failed"
			fi
	
			rm -f $USBHOST_FILE_TMP
			
    done
    
		local PP=` if [ -z "$USBHOST_PRETTY_PRT" ]; then echo "off"; else echo "on"; fi`
    local VV=` if [ -z "$USBHOST_VERBOSE" ]; then echo "off"; else echo "on"; fi`
    export TMPDIR=${USBHOST_DIR_TEST}
		
		# Print some nice info
    if [ ! -z "$USBHOST_VERBOSE" ]; then
        debug "USBHOST_POSTFIX        $USBHOST_POSTFIX       "
        info  "USBHOST_ROOT       $USBHOST_ROOT      "
        info  "USBHOST_DIR_TMP        $USBHOST_DIR_TMP       "
        info  "USBHOST_FILE_TMP        $USBHOST_FILE_TMP       "
        debug "USBHOST_FILE_CMD        $USBHOST_FILE_CMD       "
        info  "USBHOST_DIR_TEST        $USBHOST_DIR_TEST       "
        info  "USBHOST_PRETTY_PRT     $PP            "
        info  "USBHOST_VERBOSE        $VV            "
        info  "USBHOST_FILE_OUTPUT     $USBHOST_FILE_OUTPUT    "
        info  "USBHOST_FILE_LOG        $USBHOST_FILE_LOG       "
        info  "USBHOST_DURATION       $USBHOST_DURATION      "
        debug "PATH           $PATH          "
        info  "USBHOST_DIR_SCENARIOS    $USBHOST_DIR_SCENARIOS   "
        info  "TMPDIR         $TMPDIR        "
        info  "USBHOST_SCENARIO_NAMES $USBHOST_SCENARIO_NAMES"
    fi
} 

main()
{
	count=0
	while getopts zx:Sd:qt:po:l:vn:hs:E:I arg
	do  case $arg in
		d)
			USBHOST_DIR_TEST=${USBHOST_OPTARG} ;;
		t)
			USBHOST_DIR_TMP=${USBHOST_OPTARG} ;;
		E)
			EXTENDED_TEST=y ;;
	        q)
			USBHOST_QUIET_MODE=" -q " ;;
	        z)
			DONT=" " ;;
		p)
			USBHOST_PRETTY_PRT=" -p " ;;
		o)
			USBHOST_FILE_OUTPUT=${USBHOST_OPTARG};OO_LOG=1 ;;
		l)
			USBHOST_FILE_LOG=${USBHOST_OPTARG} ;;
		v)
			USBHOST_VERBOSE="-v" ;;
		n) 
			USBHOST_DURATION=" -t ${USBHOST_OPTARG}" ;;
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
			USBHOST_INSTANCES="-x $USBHOST_OPTARG -O ${TMP}" ;;
		s)
			USBHOST_DIR_SCENARIOS=${USBHOST_OPTARG} ;;
		S)
			USBHOST_STRESS=y
			USBHOST_STRESSARG="-S";;
		
		\?) # Handle illegals
			usage ;;
        
	esac
	
	if [ ! -z "${USBHOST_OPTARG}" ]; then
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
	
	USBHOST_SCENARIO_NAMES=$@

	sanity_check
	
	# Test start
	
	[ -z "$USBHOST_QUIET_MODE" ] && { info "Test start time: $(date)" ; }

	# Usage: pan -n name [ -SyAehp ] [ -s starts ] [-t time[s|m|h|d] [ -x nactive ] [-l logfile ]
	# [ -a active-file ] [ -f command-file ] [ -d debug-level ]
	# [-o output-file] [-O output-buffer-directory] [cmd]

	cd $USBHOST_DIR_TEST
	PAN_COMMAND="${UTILS_DIR_BIN}/pan $USBHOST_QUIET_MODE -e -S $USBHOST_INSTANCES $USBHOST_DURATION -a $$ -n $$ $USBHOST_PRETTY_PRT -f ${USBHOST_FILE_CMD} -l $USBHOST_FILE_LOG"
    
	[ ! -z "$USBHOST_VERBOSE" ] && { info "PAN_COMMAND=$PAN_COMMAND"; }
    	
	if [ -z "$OO_LOG" ]; then
		$PAN_COMMAND
	else
		$PAN_COMMAND|tee $USBHOST_FILE_OUTPUT
	fi
    
	if [ $? -eq 0 ]; then
		echo "INFO: pan reported all tests PASS"
		VALUE=0
	else
		echo "INFO: pan reported some tests FAIL"
		VALUE=1
	fi
    
	# Test end
	[ -z "$USBHOST_QUIET_MODE" ] && { info "Test end time: $(date)" ; }
	[ -z "$USBHOST_QUIET_MODE" ] && { 

	cat <<-EOF >&1

	###############################################################"
		Done executing testcases."
		Result log is in the $USBHOST_FILE_LOG "
	###############################################################"
       
	EOF
	cat $USBHOST_FILE_LOG
	
	}
	cleanup
	exit $VALUE
}


cleanup()
{
	[  -z "$USBHOST_QUIET_MODE" ] && echo -n "INFO: Cleaning up..."
	if [ -n "${USBHOST_FILE_TMP}" -a -n "${USBHOST_FILE_CMD}" -a -n "${USBHOST_DIR_TEST}" -a -n "${USBHOST_DIR_TMP}" ]; then
		rm -rf ${USBHOST_FILE_TMP} ${USBHOST_FILE_CMD} ${USBHOST_DIR_TEST}/* ${USBHOST_DIR_TMP}/*
	else
		echo "INFO: Clean up process won't be executed because variables for directories to be removed are not set..."
	fi

	[  -z "$USBHOST_QUIET_MODE" ] && echo "done."
}


trap "cleanup" 0
setup
main "$@"

# End of file
