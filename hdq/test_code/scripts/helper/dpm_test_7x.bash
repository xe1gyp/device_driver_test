#!/bin/bash

# set -x
# dpm-test-omap24xx.bash
# 
# Test functions for DPM and Frequency and Voltage changes for the
# TI OMAP 24xx:
#
#    OMAP2420
#    OMAP2430
#
# Based on dpm-test-functions-031605 for OMAP2420 CEE 3.1 initial
# release from Echo Engineering.
#
# How to use:
# - Load this file on the target:
#   . ./dpm-test-omap24xx.bash
# 
# This will start a menu to choose specific tests. Once the file is
# loaded, many of the test-functions may also be run from the command
# line just by typing the name.
#

# Re-entry Function
# - dpm-menu (interactive menu to pick tests)

##################################################
# INITIAL VALUES
# Change this for more or less frequency repeats in the stress tests
export DPM_COUNT=10

# Set this non-zero for a delay in the stress tests
export DPM_DELAY=0

# Sleep Mode (from pm.h):
#   1=CPUMODE_IDLE (actually "on")
#   2=CPUMODE_STANDBY
#   3=CPUMODE_SLEEP
#   4=CPUMODE_RESERVED = RETENTION

S_MODE=3

# Power State Mode (from pm_prcm.h):
# 0=OFF, 1=ON, 2=RETENTION, 3=SLEEP, 4=STANDBY, 5=DORMANT
PWRST_MODE=2

# Power State Domain:
# 0=MPU, 1=DSP, 2=GFX, 3=CORE, 4=MDM
PWRST_DOMAIN=1

# AutoIdle Mode Values: 0, 1, 2, 3
AI_MODE=3

# AutoIdle Domain Values: 0, 1, 2, 3, 4
AI_DOMAIN=0 # DPLL

##################################################

# Where all these commands go
export CMD=/proc/driver/dpm/cmd

# The DPM force flag
export DPM_FORCE_FILE=/sys/dpm/control/force

# DPM Device events
export DPM_DEV_EVENTS=/sys/dpm/control/device-events

##################################################

echo "Loading DPM Testing Functions..."

echo "Mounting filesystems (ignore errors if already mounted)"
mount -t sysfs sysfs /sys

echo "Initializing DPM..."
echo init > $CMD

function dpm-create-opt
{
    if [ "${1}" = "" ] || [ "${2}" = "" ] || [ "${3}" = "" ] ; then
	echo "Usage: dpm-create-opt <rootname> <prcm> <volt> [ <dpllmult> <dplldiv> <coremult> <armdiv> <sleepmode> ]"
    else
	# set variables to arguments
	export DPMDPLLM=${4}
	export DPMDPLLN=${5}
	export DPMCORE=${6}
	export DPMARMDIV=${7}
	export DPMSLEEP=${8}
	export DPMMPUPWST=${9}
	export DPMDSPPWST=${10}
	export DPMGFXPWST=${11}
	export DPMCOREPWST=${12}
	export DPMMDMPWST=${13}
	export DPMDSPIDLE=${14}
	export DPMCOREIDLE=${15}
	export DPMWKUPIDLE=${16}
	export DPMDPLLIDLE=${17}
	export DPMMDMIDLE=${18}
	
	# override with defaults if argument was not set
	if [ "${4}" = "" ] ; then 
	    export DPMDPLLM="-1"
	fi
	if [ "${5}" = "" ] ; then 
	    export DPMDPLLN="-1"
	fi
	if [ "${6}" = "" ] ; then 
	    export DPMCORE="-1"
	fi
	if [ "${7}" = "" ] ; then 
	    export DPMARMDIV="-1"
	fi
	if [ "${8}" = "" ] ; then 
	    export DPMSLEEP="-1"
	fi
	if [ "${9}" = "" ] ; then 
	    export DPMMPUPWST="-1"
	fi
	if [ "${10}" = "" ] ; then 
	    export DPMDSPPWST="-1"
	fi
	if [ "${11}" = "" ] ; then 
	    export DPMGFXPWST="-1"
	fi
	if [ "${12}" = "" ] ; then 
	    export DPMCOREPWST="-1"
	fi
	if [ "${13}" = "" ] ; then 
	    export DPMMDMPWST="-1"
	fi
	if [ "${14}" = "" ] ; then 
	    export DPMDSPIDLE="-1"
	fi
	if [ "${15}" = "" ] ; then 
	    export DPMCOREIDLE="-1"
	fi
	if [ "${16}" = "" ] ; then 
	    export DPMWKUPIDLE="-1"
	fi
	if [ "${17}" = "" ] ; then 
	    export DPMDPLLIDLE="-1"
	fi

	if [ "${18}" = "" ] ; then 
	    export DPMMDMIDLE="-1"
	fi

   	echo "      Name      prcm volt DPLLMULT DPLLDIV COREMULT ARMDIV SLEEP"
   	echo "      ----      ---- ---- -------- ------- -------- ------ ------ -----"
	echo "  ${1}OPT    ${2}  ${3}   $DPMDPLLM       $DPMDPLLN      $DPMCORE        $DPMARMDIV     $DPMSLEEP"
	echo create_opt ${1}OPT ${2} ${3} $DPMDPLLM $DPMDPLLN $DPMCORE $DPMARMDIV $DPMSLEEP $DPMMPUPWST $DPMDSPPWST $DPMGFXPWST $DPMCOREPWST $DPMMDMPWST $DPMDSPIDLE $DPMCOREIDLE $DPMWKUPIDLE $DPMDPLLIDLE $DPMMDMIDLE > $CMD
#	grep "${1}OPT" /proc/driver/dpm/opt_stats
    fi
}

function dpm-create-class
{
    if [ "${1}" = "" ] ; then
	echo "Usage: dpm-create-class <rootname>"
    else
	echo create_class ${1}CLASS ${1}OPT > $CMD
    fi
}
function dpm-create-opt-and-class
{
    if [ "${1}" = "" ] || [ "${2}" = "" ] || [ "${3}" = "" ] ; then
	echo "Usage: dpm-create-opt-and-class <rootname> <prcm> <volt>"
    else
	dpm-create-opt ${1} ${2} ${3} ${4} ${5} ${6} ${7} ${8} ${9}
	dpm-create-class ${1}
    fi
}

function dpm-create-policy
{
    if [ "${1}" = "" ] ; then
	echo "Usage: dpm-create-policy <rootname>"
    else
	export XXX=${1}CLASS
	echo create_policy ${1}POL \
	    $XXX $XXX $XXX $XXX \
	    $XXX $XXX $XXX $XXX \
	    $XXX \
	    $XXX $XXX $XXX $XXX > $CMD
    fi
}
function dpm-set-policy
{
    if [ "${1}" = "" ] ; then
	echo "Usage: dpm-set-policy <rootname>"
    else
	echo set_policy ${1}POL > $CMD
    fi
}
function dpm-create-and-set-policy
{
    if [ "${1}" = "" ] ; then
	echo "Usage: dpm-create-and-set-policy <rootname>"
    else
	dpm-create-policy ${1}
	dpm-set-policy ${1}
    fi
}

function dpm-check-speeds
{

# Pulls messages like this from /var/log/messages (not very useful
# when using the console or when syslog is not running (like with a
# flash filesystem)
#
# Jan  1 00:13:29 10 kernel: DPM: Freq=520.00MHz, SysBus=208.00Mhz
# Jan  1 00:13:29 10 kernel: DPM: L=16, N=5, B=1, HT=0, T=1 (should be On)
# Jan  1 00:13:29 10 kernel: DPM: Turbo Mode = On
# Jan  1 00:13:29 10 kernel: DPM: Core frequency = Turbo Mode

#    tail -10 /var/log/messages | grep "DPM:"
grep Bogo /proc/cpuinfo
}

function dpm-do-all
{
    if [ "${1}" = "" ] || [ "${2}" = "" ] || [ "${3}" = "" ] ; then
	echo "Usage: dpm-do-all <rootname> <prcm> <volt> [ <dpllmult> <dplldiv> <coremult> <armdiv> <sleepmode> ]"
	echo "Create operating point from arguments; create a class and a policy"
 	echo "with new operating point configured for all operating states."
	echo "Name, prcm, and volt are required, the rest are optional, but order matters."
	echo "NOTE: you may *not* re-use names"
    else
	dpm-create-opt ${1} ${2} ${3} ${4} ${5} ${6} ${7} ${8}
	dpm-create-class ${1}
	dpm-create-and-set-policy ${1}

# Only use this function when syslog is known to be running and the
# console is not being used to run tests
	dpm-check-speeds
    fi
}

function dpm-sleep-test
{
    # Change S_MODE up top per instructions in comments
    RAND=$RANDOM
    AWAKE=awake-$RAND
    ASLEEP=asleep-$RAND
    POLICY=policy-$RAND
    
    echo "Creating operating point $AWAKE-OPT using $DefaultOp"
    eval dpm-create-opt \$AWAKE- \$$DefaultOp

    echo "Creating DPM class $AWAKE-CL"
    echo create_class $AWAKE-CL $AWAKE-OPT > $CMD
    
    echo "Creating operating point $ASLEEP-OPT"
#                             PRCM Volt DPLLM DPLLN CORE ARMDIV SLEEP
    dpm-create-opt $ASLEEP-     -1  -1  -1    -1    -1   -1     $S_MODE

    echo "Creating DPM class $ASLEEP-CL"
    echo create_class $ASLEEP-CL $ASLEEP-OPT > $CMD
    
# from dpm.h
#	relock idle-task idle sleep
#	task-4 task-3 task-2 task-1
#	task
#	task+1 task+2 task+3 task+4

    echo "Creating policy $POLICY"	
    echo create_policy $POLICY \
	$AWAKE-CL $AWAKE-CL $AWAKE-CL $ASLEEP-CL \
	$AWAKE-CL $AWAKE-CL $AWAKE-CL $AWAKE-CL \
	$AWAKE-CL \
	$AWAKE-CL $AWAKE-CL $AWAKE-CL $AWAKE-CL > $CMD
    
    echo "Setting policy $POLICY"
    echo set_policy $POLICY > $CMD

    echo "Changing task state to sleep in $DPM_DELAY seconds"
    sleep $DPM_DELAY
    
    TO_STATE="sleep"
    echo "Setting state to '$TO_STATE' (mode $S_MODE)"
    echo set_state $TO_STATE > $CMD
    
    sleep 1
    echo "The system is awake again."
}

function dpm-pwrst-test-domain
{
    DOMAIN="$1"
    PWRST_PARAMS="$2"
    RAND=$RANDOM
    PWRST=pwrst-$RAND

    echo
    echo "Old values:"
    grep "^$DOMAIN" /proc/pwromap24xx

    echo
    echo "Changing $DOMAIN Power State Mode to $PWRST_MODE"
#                        PRCM Volt DPLLM DPLLN CORE ARMDIV SLEEP MPU/DSP/GFX/CORE/MDM PWRST  DSPIDLE COREIDLE WKUPIDLE DPLLIDLE MDMIDLE
    dpm-create-opt $PWRST- -1   -1 -1    -1    -1    -1    -1    $PWRST_PARAMS                -1       -1       -1        -1    -1

    echo "Creating DPM class $PWRST-CL"
    echo create_class $PWRST-CL $PWRST-OPT > $CMD

    echo "Creating policy $PWRST-POL"	
    echo create_policy $PWRST-POL \
	$PWRST-CL $PWRST-CL $PWRST-CL $PWRST-CL \
	$PWRST-CL $PWRST-CL $PWRST-CL $PWRST-CL \
	$PWRST-CL \
	$PWRST-CL $PWRST-CL $PWRST-CL $PWRST-CL > $CMD
    
    echo "Setting policy $PWRST-POL"
    echo set_policy $PWRST-POL > $CMD

    echo
    echo "New values:"
    grep "^$DOMAIN" /proc/pwromap24xx
    echo
    echo "$DOMAIN power state mode test complete"
}


function dpm-domain-pwrst-test
{
     echo " "
     echo "Domain Power State test for Domain $PWRST_DOMAIN, Mode $PWRST_MODE"
     M="$PWRST_MODE"
     case "$PWRST_DOMAIN" in
#                                         MPU DSP GFX COR MDM
         0   ) dpm-pwrst-test-domain MPU "$M  -1  -1  -1  -1" ;;
         1   ) dpm-pwrst-test-domain DSP "-1  $M  -1  -1  -1" ;;
         2   ) dpm-pwrst-test-domain GFX "-1  -1  $M  -1  -1" ;;
         3   ) dpm-pwrst-test-domain COR "-1  -1  -1  $M  -1" ;;
         4   ) dpm-pwrst-test-domain MDM "-1  -1  -1  -1  $M" ;;
         *   ) echo "'$PWRST_DOMAIN' is invalid";;
     esac
}

function dpm-domain-autoidle-test
{
    DOMAIN="$1"
    IDLE_PARAMS="$2"
    RAND=$RANDOM
    AUTOIDLE=autoidle-$RAND

     echo " "
     echo "Changing AutoIdle for $DOMAIN to $AI_MODE"
#                             PRCM Volt DPLLM DPLLN CORE ARMDIV SLEEP MPUPWRST DSPPWRST GFXPWRST COREPWRST MDMPWRST DSP/CORE/WKUPDPLL/MDM IDLE
    dpm-create-opt $AUTOIDLE-  -1    -1  -1    -1   -1   -1     -1     -1       -1      -1       -1        -1       $IDLE_PARAMS

    echo "Creating DPM class $AUTOIDLE-CL"
    echo create_class $AUTOIDLE-CL $AUTOIDLE-OPT > $CMD

    echo "Creating policy $AUTOIDLE-POL"	
    echo create_policy $AUTOIDLE-POL \
	$AUTOIDLE-CL $AUTOIDLE-CL $AUTOIDLE-CL $AUTOIDLE-CL \
	$AUTOIDLE-CL $AUTOIDLE-CL $AUTOIDLE-CL $AUTOIDLE-CL \
	$AUTOIDLE-CL \
	$AUTOIDLE-CL $AUTOIDLE-CL $AUTOIDLE-CL $AUTOIDLE-CL > $CMD
    
    echo "Setting policy $AUTOIDLE-POL"
    echo set_policy $AUTOIDLE-POL > $CMD

    echo " "
    echo "$DOMAIN AutoIdle Mode change complete."
}

function dpm-autoidle-test
{
     echo " "
     echo "AutoIdle test for Domain $AI_DOMAIN, Mode $AI_MODE"
     M="$AI_MODE"
     case "$AI_DOMAIN" in
#                                             DSP  CORE WKUP DPLL MDM
         0   ) dpm-domain-autoidle-test DPLL "-1   -1   -1   $M   -1 " ;;
         1   ) dpm-domain-autoidle-test DSP  "$M   -1   -1   -1   -1 " ;;
         2   ) dpm-domain-autoidle-test CORE "-1   $M   -1   -1   -1 " ;;
         3   ) dpm-domain-autoidle-test WKUP "-1   -1   $M   -1   -1 " ;;
         4   ) dpm-domain-autoidle-test MDM  "-1   -1   -1   -1   $M " ;;
         *   ) echo "'$AI_DOMAIN' is invalid";;
     esac
}

function dpm-test-fast
{
    echo "$OMAP Fast"
    echo "--------------------------"
    echo "Operating Point: $DefaultFastOp"
    eval dpm-do-all aaa-$RANDOM \$$DefaultFastOp
}

function dpm-test-slow
{
    echo "$OMAP Slow"
    echo "--------------------------"
    echo "Operating Point: $DefaultSlowOp"
    eval dpm-do-all aaa-$RANDOM \$$DefaultSlowOp
}

function dpm-test-production-points
{
    echo "$OMAP Production Points"
    echo "--------------------------"

    for op in $AllOps
    do
       echo
       echo "Operating Point: $op"
       eval dpm-do-all aaa-$RANDOM \$$op
    done
}

# this function is used from the dpm-menu function to change the value
# defined at the top of this file
function dpm-get-count
{
    echo "Current count is $DPM_COUNT"
    echo -n "Enter new count value: "
    read newval
    if [ $newval != 0 ] ; then
	export DPM_COUNT=$newval
    fi
    echo "New count is '$DPM_COUNT'"
}

# this function is used from the dpm-menu function to change the value
# defined at the top of this file
function dpm-get-delay
{
    echo "Current delay value is $DPM_DELAY"
    echo -n "Enter new delay value: "
    read newval
    if [ $newval != 0 ] ; then
	export DPM_DELAY=$newval
    fi
    echo "New delay value is $DPM_DELAY"
}

# this function is used from the dpm-menu function to change the value
# defined at the top of this file
function dpm-get-sleep
{
    echo "Current sleep mode value is $S_MODE"
    echo "1 = Device On, 2 = Standby, 3 = Sleep, 4 = Retention"
    echo "  NOTE: Dormant mode (5) and Off mode (7) not currently supported"
    echo -n "Enter new sleep value: "
    read newval
    if [ $newval != 0 ] ; then
	export S_MODE=$newval
    fi
    echo "New sleep mode value is $S_MODE"
}

function dpm-set-pwrst-mode
{
    echo "Current power state mode value is $PWRST_MODE"
    echo "0 = Off, 1 = ON, 2 = RETENTION, 3 = SLEEP, 4 = STANDBY, 5=DORMANT"
    echo -n "Enter new power state mode value: "
    read newval
    if [ $newval != "" ] ; then
	export PWRST_MODE=$newval
    fi
    echo "New power state mode value is $PWRST_MODE"
}

function dpm-set-pwrst-domain
{
    echo "Current Power Domain value is $PWRST_DOMAIN"
    echo "0 = MPU, 1 = DSP, 2 = GFX, 3 = CORE, 4 = MDM"
    echo -n "Enter new Power Domain value: "
    read newval
    if [ $newval != "" ] ; then
	export PWRST_DOMAIN=$newval
    fi
    echo "New Power Domain value is $PWRST_DOMAIN"
}

function dpm-set-autoidle-mode
{
    echo "Current AutoIdle Mode value is $AI_MODE"
    echo "DPLL: 0 = Disabled, 1 = Low Power Bypass, 2 = Fast Re-Lock Bypass, 3 = Enabled"
    echo "Other: 0 = Disabled, 1 = Enabled"
    echo -n "Enter new AutoIdle Mode value: "
    read newval
    if [ $newval != "" ] ; then
	export AI_MODE=$newval
    fi
    echo "New AutoIdle Mode value is $AI_MODE"
}

function dpm-set-autoidle-domain
{
    echo "Current AutoIdle Domain value is $AI_DOMAIN"
    echo "0 = DPLL, 1 = DSP, 2 = CORE, 3 = WKUP, 4 = MDM"
    echo -n "Enter new AutoIdle Domain value: "
    read newval
    if [ $newval != "" ] ; then
	export AI_DOMAIN=$newval
    fi
    echo "New AutoIdle Domain value is $AI_DOMAIN"
}

# this function is used from the dpm-menu function to change the value
# of force (to override constraint conflicts)
function dpm-toggle-force
{
    DPM_FORCE=`cat $DPM_FORCE_FILE`
    echo "Force was : $DPM_FORCE"
    if [ "$DPM_FORCE" = "0" ] ; then
	DPM_FORCE=1
    else
	DPM_FORCE=0
    fi
	
    echo $DPM_FORCE > $DPM_FORCE_FILE
    echo -n "Force is now: "
    cat $DPM_FORCE_FILE
}

function dpm-stress-test
{
    export DPM_HI=aaa-$RANDOM
    export DPM_LO=aaa-$RANDOM

    echo "This is a stress test to jump between Fast and Slow frequencies"
    echo "within the same PRCM set rapidly many times. Delaying between"
    echo "changes is optional."
    echo "Repeat count is $DPM_COUNT"

    eval dpm-do-all \$DPM_HI \$$DefaultFastOp
    eval dpm-do-all \$DPM_LO \$$DefaultSlowOp

    i="1"
    while [ $i -le $DPM_COUNT ]
      do
      dpm-set-policy $DPM_HI
      dpm-check-speeds
      sleep $DPM_DELAY
      dpm-set-policy $DPM_LO
      dpm-check-speeds
      sleep $DPM_DELAY
      echo "================ ITERATION #$i COMPLETE"
      i=`expr $i + 1`
    done
}

function dpm-repeat-production
{
    echo "This is a stress test to repeat the entire series of Production"
    echo "Operating pints. Delaying between series is optional"
    echo "Repeat count is $DPM_COUNT"

    i="1"
    while [ $i -le $DPM_COUNT ]
      do	  
      dpm-test-production-points
      echo "================ ITERATION # $i COMPLETE"
      i=`expr $i + 1`
      sleep $DPM_DELAY
    done
}

function sleep-repeat
{
    # Make sure that force mode is on
    echo "Turning on force mode"
    echo 1 > $DPM_FORCE_FILE
    echo -n "Force mode is now: "
    cat $DPM_FORCE_FILE

    echo " "
    echo -n "Time before auto-wakeup is: "
    cat /proc/sys/pm/sleeptime

    echo "Beginning sleep test in three seconds"
    sleep 3

    i="1"
    while [ $i -le $DPM_COUNT ]
      do	  
      echo "================ GOING TO SLEEP # $i"
      dpm-sleep-test
      echo "================ WAKEUP # $i"
      i=`expr $i + 1`
      sleep $DPM_DELAY
    done
}

function configure-ops
{
    OMAP=$(grep ^Processor /proc/cpuinfo | awk '{ print $5 }')

    if [ "$OMAP" = "OMAP2420" ]
    then
#                     PRCM Volt DPLLM DPLLN CORE ARMDIV SLEEP
	OpPRCMIIFast="   0  -1    -1    -1   -1    -1    -1  "
	OpPRCMIISlow="   1  -1    -1    -1   -1    -1    -1  "
	OpPRCMIIIFast="  2  -1    -1    -1   -1    -1    -1  "
	OpPRCMIIISlow="  3  -1    -1    -1   -1    -1    -1  "
	DefaultOp=OpPRCMIIFast
	DefaultFastOp=OpPRCMIIIFast
	DefaultSlowOp=OpPRCMIIISlow
	AllOps="OpPRCMIIFast OpPRCMIISlow OpPRCMIIIFast OpPRCMIIISlow"

    elif [ "$OMAP" = "OMAP2430" ]
    then
#                    PRCM Volt DPLLM DPLLN CORE ARMDIV SLEEP
	OpPRCM2Fast="    0  -1    -1    -1   -1    -1    -1  "
	OpPRCM2Slow="    1  -1    -1    -1   -1    -1    -1  "
	OpPRCM5aFast="   2  -1    -1    -1   -1    -1    -1  "
	OpPRCM5aSlow="   3  -1    -1    -1   -1    -1    -1  "
	OpPRCM5bFast="   4  -1    -1    -1   -1    -1    -1  "
	OpPRCM5bSlow="   5  -1    -1    -1   -1    -1    -1  "
	# At a given time we can either work on
	# 1. PRCM 5a/5b
	# OR
	# 2. PRCM 2
	# check my frequency now..
	# Dirty hack for the time being... 
	# Read BogoMips and find the valid operating point set
	BMips=` cat /proc/cpuinfo |grep Bogo|cut -d ':' -f 2|cut -d '.' -f1`
	if [ $BMips -eq 109 -o $BMips -eq 219 ]; then
		# Set this for PRCM 2.. cant switch to other PRCM values..
		DefaultOp=OpPRCM2Fast
		DefaultFastOp=OpPRCM2Fast
		DefaultSlowOp=OpPRCM2Slow
		AllOps="OpPRCM2Fast OpPRCM2Slow"
	else
		DefaultOp=OpPRCM5aFast
		DefaultFastOp=OpPRCM5aFast
		DefaultSlowOp=OpPRCM5aSlow
		AllOps="OpPRCM2Fast OpPRCM2Slow OpPRCM5aFast OpPRCM5aSlow OpPRCM5bFast"
	fi
   else
      echo "Unrecognized processor $OMAP"
      exit 1
   fi
}

function dpm-menu
{
    done=0
    
    echo " "
    echo "OMAP24XX DPM Tests"
    echo "------------------"
    echo "2 - Run $OMAP Production Points"
    echo "4 - Sleep Test: put target to sleep"
    echo "5 - Toggle DPM force"
    echo "6 - Stress test: jump between fast and slow frequencies repeatedly"
    echo "7 - Stress test: repeat Production Operating Points"
    echo "8 - Change count for stress tests (Currently $DPM_COUNT)"
    echo "9 - Change delay for stress tests (Currently $DPM_DELAY)"
    echo "a - Change Sleep Mode for Sleep Test (Currently $S_MODE)"
    echo "t - Domain Power State Change Test (using domain $PWRST_DOMAIN and mode $PWRST_MODE)"
    echo "u - Change Power State Mode (Currently $PWRST_MODE)"
    echo "v - Change Power State Domain (Currently $PWRST_DOMAIN)"
    echo "w - AutoIdle Test (using domain $AI_DOMAIN and mode $AI_MODE)"
    echo "x - Change AutoIdle Mode (Currently $AI_MODE)"
    echo "y - Change AutoIdle Domain (Currently $AI_DOMAIN)"
    echo "s - Change to Slow Operating point"
    echo "f - Change Fast Operating point"
    echo " "
    echo "0 - quit"
    echo " "

    echo -n "Pick one: "
    read x
    
    case "$x" in 
	0   ) echo "To run this menu again, run 'dpm-menu'"
              done=1;;
        2   ) dpm-test-production-points;;
	4   ) dpm-sleep-test;;
	5   ) dpm-toggle-force;;
	6   ) dpm-stress-test;;
	7   ) dpm-repeat-production;;
	8   ) dpm-get-count;;
	9   ) dpm-get-delay;;
	a   ) dpm-get-sleep;;
	t   ) dpm-domain-pwrst-test;;
	u   ) dpm-set-pwrst-mode;;
	v   ) dpm-set-pwrst-domain;;
	w   ) dpm-autoidle-test;;
	x   ) dpm-set-autoidle-mode;;
	y   ) dpm-set-autoidle-domain;;
        s   ) dpm-test-slow;;
        f   ) dpm-test-fast;;

	*   ) echo "'$x' is invalid, try again";;
	esac

	# call it again (unless done)
	if [ $done = 0 ] ; then
	   dpm-menu
	fi
}

# Start the menu when script is run
configure-ops
dpm-menu
