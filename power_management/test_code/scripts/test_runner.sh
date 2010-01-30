#!/bin/sh
#-----------------------
# Based on runltp script  from LTP 
# much of the functions are copied over from there - copyright remains
#-----------------------

# Give standard error message and die
die() 
{
        echo "FATAL: $*"
        usage
        exit 1
}

info()
{
	echo "INFO: $*"
}

debug_a()
{
	echo "DEBUG: $*"
}
# comment out and suit urself..Debug help
alias debug=/bin/true
#alias debug=debug_a

# Defaults and sanity stuff
setup()
{
    cd `dirname $0` || \
    {
        die "unable to change directory to $(dirname $0)"
    }
    # Load config file if found
    if [ -f "./conf.sh" ]; then
        . ./conf.sh
    else
	  echo "conf.sh file was not found! Using defaults..."
	# Load defaults..
	# if we have a working rtc, this is good.. else use next line
    export POSTFIX=`date "+%Y%m%d-%H%M%S"`
    #export POSTFIX=$$
    # Load up the defaults
    export TESTROOT=${PWD}
	  export TESTBIN=${PWD}/../bin
	  export TESTSCRIPT=${PWD}/scripts
    export TMPBASE=${TESTROOT}/tmp
	  export TMPFILE=${TMPBASE}/tmp.$POSTFIX
    export CMDFILE=cmd.$POSTFIX
    export TESTDIR=${TESTROOT}/test
    export PRETTY_PRT=""
    export VERBOSE=""
    export OUTPUTFILE=${TESTROOT}/output.$POSTFIX
    export LOGFILE=${TESTROOT}/log.$POSTFIX
    export DURATION="1h"
	  export PATH="${PATH}:${TESTROOT}:${TESTBIN}:${TESTSCRIPT}"
    export TC_SCENARIO="${TESTROOT}/scenarios"
    export SCENARIO_NAMES=""
    export UTILBIN=../../utils/bin
    fi
    
    # scenario less tests?? have the user organize it properly at least..
    [ -d $TC_SCENARIO ] ||
    {
        die "Test suite not installed correctly - no scenarios"
    }

    # we'd need the reporting tool ofcourse..
    echo $UTILBIN 	
    [ -e $UTILBIN/pan ] ||
    {
        die "FATAL: Test suite driver 'pan' not found"
    }
}

usage()
{
    # Human redable please
    local PP=` if [ -z "$PRETTY_PRT" ]; then echo "off"; else echo "on"; fi`
    local VV=` if [ -z "$VERBOSE" ]; then echo "off"; else echo "on"; fi`
    # Give the gyan
    cat <<-EOF >&2

    usage: ./${0##*/} [-z] [-h] [-v] [-d TESTDIR] [-o OUTPUTFILE] [-l LOGFILE] 
    [-n DURATION ] [-t TMPDIR] [SCENARIO_NAMES..]
                
    -d TESTDIR      Run LTP to test the filesystem mounted here. [Current - $TESTDIR]
                    At the end of test, the testdir gets cleaned out
    -s TC_SCENARIO  Test scenarios are located here. [Current - $TC_SCENARIO]
    -o OUTPUTFILE   Redirect test output to a file. [Current- $OUTPUTFILE {psid}]
    -p              Human readable(dont laugh too much) format logfiles. [Current - ($PP)]
    -z              Dont Merge the Scenario Name with tcid to create final tc id
    -E              Use Extended Test cases also - these are painful and can take real long time
    -l LOGFILE      Log results of test in a logfile. [Current- $LOGFILE {psid}]
    -t TMPDIR       Run LTP using tmp dir [Current - $TMPBASE]
    -n DURATION     Execute the testsuite for given duration. Examples:
                      -n 60s = 60 seconds
                      -n 45m = 45 minutes
                      -n 24h = 24 hours
                      -n 2d  = 2 days
                    [Current - $DURATION]
    -v              Print more verbose output to screen.[Current - ($VV)]
    -q              No messages from this script. no info too - Brave eh??
    -h              This screen [Current - guess...]
    -x INSTANCES    Run multiple instances of this testsuite.(think well...)
    -r PRE_DEF      Run predefined set of scenarios[Not Implemented yet]
                     List to appear here
    -I              Run test cases which require user interaction
    -S              Special Mode - Stress Testing
    SCENARIO_NAMES  List of scenarios to test.. else, take all scenarios 
                    [Current - These are all filenames from $TC_SCENARIO]
    
    Good News: Ctrl+c stops and cleans up for you :)
    More help: Read the $TESTROOT/README


	EOF
exit 0
}

# Insane world.. insane set of human mistakes!!
sanity_check()
{
    # Check the current values...
    # Just ensure that pan can run with a bit of peace of mind...
    [ ! -d "$TMPBASE" -o ! -w "$TMPBASE" ] && die "$TMPBASE - cannot work as temporary directory"

    [ ! -d "$TESTBIN" ] && die "$TESTBIN - cannot find test binary directory"
    [ ! -d "$TESTDIR" -o ! -w "$TESTDIR" ] && die "$TESTDIR - cannot work as test directory"
    [ ! -d "$TC_SCENARIO" ] && die "$TC_SCENARIO - No such directories"
    [ -z "$SCENARIO_NAMES" ] && die "No Scenarios"
    #[ ! -z "$VERBOSE" -a ! -z "$QUIET_MODE" ] && die "Make up your mind - verbose or quiet??"
    export CMDFILE=$TMPBASE/$CMDFILE
    rm -f $CMDFILE
    for SCEN in $SCENARIO_NAMES
    do
        [ ! -f "$TC_SCENARIO/$SCEN" -o ! -r "$TC_SCENARIO/$SCEN" ] && die "$TC_SCENARIO/$SCEN - not a scenario file"
	cat $TC_SCENARIO/$SCEN|grep -v "#"|sed -e "s/^[  ]*$//g"|sed -e "/^$/d">$TMPFILE|| die "Count not create tmp file $TMPFILE"
	if [ -z "$DONT" ]; then
	cat $TMPFILE|sed -e "s/^/$SCEN-/g"|sed -e "s/-/_/" >>$CMDFILE || die "Count not create command file $CMDFILE"
	else
	cat $TMPFILE>>$CMDFILE || die "Count not create command file $CMDFILE"
	fi
	if [ -z "$EXTENDED_TEST" ]; then
		#Remove the extended test cases
		cat $CMDFILE|grep -v "^[_A-Za-z0-9]*_EXT ">$TMPFILE || die "intermediate file gen failed"
		cat $TMPFILE>$CMDFILE || die "Second intermediate creation failed"
	fi
	if [ -z "$INTERACTIVE" ]; then
		#Remove the test cases which require user interaction
		cat $CMDFILE|grep -v "^[_A-Za-z0-9]*_INT ">$TMPFILE || die "intermediate file gen failed"
		cat $TMPFILE>$CMDFILE || die "Second intermediate creation failed"
	fi
	
	rm -f $TMPFILE
    done
    local PP=` if [ -z "$PRETTY_PRT" ]; then echo "off"; else echo "on"; fi`
    local VV=` if [ -z "$VERBOSE" ]; then echo "off"; else echo "on"; fi`
    export TMPDIR=${TESTDIR}
    #print some nice info
    if [ ! -z "$VERBOSE" ]; then
        debug "POSTFIX        $POSTFIX       "
        info  "TESTROOT       $TESTROOT      "
        info  "TMPBASE        $TMPBASE       "
        info  "TMPFILE        $TMPFILE       "
        debug "CMDFILE        $CMDFILE       "
        info  "TESTDIR        $TESTDIR       "
        info  "PRETTY_PRT     $PP            "
        info  "VERBOSE        $VV            "
        info  "OUTPUTFILE     $OUTPUTFILE    "
        info  "LOGFILE        $LOGFILE       "
        info  "DURATION       $DURATION      "
        debug "PATH           $PATH          "
        info  "TC_SCENARIO    $TC_SCENARIO   "
        info  "TMPDIR         $TMPDIR        "
        info  "SCENARIO_NAMES $SCENARIO_NAMES"
    fi
} 

main()
{
   count=0
    while getopts zx:Sd:qt:po:l:vn:hs:E:I arg
    do  case $arg in
        d)
           TESTDIR=${OPTARG} ;;
        t)
           TMPBASE=${OPTARG} ;;
	      E)
	         EXTENDED_TEST=y ;;
        q)
           QUIET_MODE=" -q " ;;
        z)
           DONT=" " ;;
        p)
           PRETTY_PRT=" -p " ;;
        o)
           OUTPUTFILE=${OPTARG};OO_LOG=1 ;;
        l)
           LOGFILE=${OPTARG} ;;
        v)
           VERBOSE="-v" ;;
        n) 
           DURATION=" -t ${OPTARG}" ;;
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
            INSTANCES="-x $OPTARG -O ${TMP}";;
    
        s)
           TC_SCENARIO=${OPTARG} ;;
        I)
           INTERACTIVE=y ;;
        S)
           STRESS=y ;;        

        \?) # Handle illegals
            usage ;;
        esac
     if [ ! -z "${OPTARG}" ]; then
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
    SCENARIO_NAMES=$@
    sanity_check
    # test start
    [ -z "$QUIET_MODE" ] && { info "Test start time: $(date)" ; }
    # run pan
    #$PAN_COMMAND #Duplicated code here, because otherwise if we fail, only "PAN_COMMAND" gets output
        #Usage: pan -n name [ -SyAehp ] [ -s starts ] [-t time[s|m|h|d] [ -x nactive ] [
        #-l logfile ]
        #[ -a active-file ] [ -f command-file ] [ -d debug-level ]
        #[-o output-file] [-O output-buffer-directory] [cmd]
    cd $TESTDIR
    echo $UTILBIN	
    PAN_COMMAND="${UTILBIN}/pan $QUIET_MODE -e -S $INSTANCES $DURATION -a $$ -n $$ $PRETTY_PRT -f ${CMDFILE} -l $LOGFILE "
    [ ! -z "$VERBOSE" ] && { info "PAN_COMMAND=$PAN_COMMAND"; }
    if [ -z "$OO_LOG" ]; then
    $PAN_COMMAND
    else
	    $PAN_COMMAND|tee $OUTPUTFILE
    fi
    
    if [ $? -eq 0 ]; then
      echo "INFO: pan reported all tests PASS"
      VALUE=0
    else
      echo "INFO: pan reported some tests FAIL"
      VALUE=1
    fi
    # test end
    [ -z "$QUIET_MODE" ] && { info "Test end time: $(date)" ; }
    [ -z "$QUIET_MODE" ] && { 

    cat <<-EOF >&1
        
       ###############################################################"
        
            Done executing testcases."
            result log is in the $LOGFILE "
        
       ###############################################################"
       
	EOF
	cat $LOGFILE
    }
    cleanup
    exit $VALUE
}

cleanup()
{
    [  -z "$QUIET_MODE" ] && echo -n "INFO: Cleaning up..."
    if [ -n "${TMPFILE}" -a -n "${CMDFILE}" -a -n "${TESTDIR}" -a -n "${TMPBASE}" ]; then
        rm -rf ${TMPFILE} ${CMDFILE} ${TESTDIR}/* ${TMPBASE}/*
    else
        echo "INFO: Clean up process won't be executed because variables for directories to be removed are not set..."
    fi
    [  -z "$QUIET_MODE" ] && echo "done."
}

trap "cleanup" 0
setup
main "$@"
