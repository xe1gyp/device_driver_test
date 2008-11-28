#!/bin/bash

#set -x
# dpm-test-omap24xx-mvl41.bash
# 
# Test functions for DPM and Frequency and Voltage changes for the
# TI OMAP 2420/2430 on Mobilinux 4.x.
#
# Updated as of 2430 ES1.x silicon, needs PRCM #2 updates for 2.x.
#
# Based on the CEE 3.1 version.
#
# How to use:
# - Load this file on the target via bash:
#   . ./dpm-test-omap24xx-mvl41.bash
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

echo "Loading DPM Testing Functions..."

echo "Initializing DPM..."
echo init      > /sys/dpm/control

function dpm-create-opt
{
    if [ "${1}" = "" ] ; then
	echo "Usage: dpm-create-opt <rootname> [<prcm> <volt> <dpllmult> <dplldiv> <coremult> <armdiv> <sleepmode> ]"
    else
	echo -e "Creating op ${1}OPT \c"

	echo create ${1}OPT > /sys/dpm/op/control

	if [ "${2}" != "" ] ; then 
	    echo -e "prcm=${2} \c"
	    echo -n "${2}" > /sys/dpm/op/${1}OPT/prcm
        fi

	if [ "${3}" != "" ] ; then 
	    echo -e "v=${3} \c"
	    echo -n "${3}" > /sys/dpm/op/${1}OPT/v
        fi

	if [ "${4}" != "" ] ; then 
	    echo -e "dpll-mult=${4} \c"
	    echo -n "${4}" > /sys/dpm/op/${1}OPT/dpll-mult
        fi

	if [ "${5}" != "" ] ; then 
	    echo -e "dpll-div=${5} \c"
	    echo -n "${5}" > /sys/dpm/op/${1}OPT/dpll-div
        fi

	if [ "${6}" != "" ] ; then 
	    echo -e "core-mult=${6} \c"
	    echo -n "${6}" > /sys/dpm/op/${1}OPT/core-mult
        fi

	if [ "${7}" != "" ] ; then 
	    echo -e "arm-div=${7} \c"
	    echo -n "${7}" > /sys/dpm/op/${1}OPT/arm-div
        fi

	if [ "${8}" != "" ] ; then 
	    echo -e "sleep=${8} \c"
	    echo -n "${8}" > /sys/dpm/op/${1}OPT/sleep
        fi

	if [ "${9}" != "" ] ; then 
	    echo -e "mpu-pwrst=${9} \c"
	    echo -n "${9}" > /sys/dpm/op/${1}OPT/mpu-pwrst
        fi

	if [ "${10}" != "" ] ; then 
	    echo -e "dsp-pwrst=${10} \c"
	    echo -n "${10}" > /sys/dpm/op/${1}OPT/dsp-pwrst
        fi

	if [ "${11}" != "" ] ; then 
	    echo -e "gfx-pwrst=${11} \c"
	    echo -n "${11}" > /sys/dpm/op/${1}OPT/gfx-pwrst
        fi

	if [ "${12}" != "" ] ; then 
	    echo -e "core-pwrst=${12} \c"
	    echo -n "${12}" > /sys/dpm/op/${1}OPT/core-pwrst
        fi

	if [ "${13}" != "" ] ; then 
	    echo -e "mdm-pwrst=${13} \c"
	    echo -n "${13}" > /sys/dpm/op/${1}OPT/mdm-pwrst
        fi

	if [ "${14}" != "" ] ; then 
	    echo -e "dsp-idle=${14} \c"
	    echo -n "${14}" > /sys/dpm/op/${1}OPT/dsp-idle
        fi

	if [ "${15}" != "" ] ; then 
	    echo -e "core-idle=${15} \c"
	    echo -n "${15}" > /sys/dpm/op/${1}OPT/core-idle
        fi

	if [ "${16}" != "" ] ; then 
	    echo -e "wkup-idle=${16} \c"
	    echo -n "${16}" > /sys/dpm/op/${1}OPT/wkup-idle
        fi

	if [ "${17}" != "" ] ; then 
	    echo -e "dpll-idle=${17} \c"
	    echo -n "${17}" > /sys/dpm/op/${1}OPT/dpll-idle
        fi

	if [ "${18}" != "" ] ; then 
	    echo -e "mdm-idle=${18} \c"
	    echo -n "${18}" > /sys/dpm/op/${1}OPT/mdm-idle
        fi

        echo
    fi
}

function dpm-create-policy
{
    if [ "${1}" = "" ] ; then
	echo "Usage: dpm-create-policy <rootname>"
    else
	echo "Creating policy ${1}POL"
	echo create ${1}POL > /sys/dpm/policy/control
	echo -n ${1}OPT > /sys/dpm/policy/${1}POL/idle
	echo -n ${1}OPT > /sys/dpm/policy/${1}POL/idle-task
	echo -n ${1}OPT > /sys/dpm/policy/${1}POL/sleep
	echo -n ${1}OPT > /sys/dpm/policy/${1}POL/task-4
	echo -n ${1}OPT > /sys/dpm/policy/${1}POL/task-3
	echo -n ${1}OPT > /sys/dpm/policy/${1}POL/task-2
	echo -n ${1}OPT > /sys/dpm/policy/${1}POL/task-1
	echo -n ${1}OPT > /sys/dpm/policy/${1}POL/task
	echo -n ${1}OPT > /sys/dpm/policy/${1}POL/task+1
	echo -n ${1}OPT > /sys/dpm/policy/${1}POL/task+2
	echo -n ${1}OPT > /sys/dpm/policy/${1}POL/task+3
	echo -n ${1}OPT > /sys/dpm/policy/${1}POL/task+4
    fi
}
function dpm-set-policy
{
    if [ "${1}" = "" ] ; then
	echo "Usage: dpm-set-policy <rootname>"
    else
	echo "Activating policy ${1}POL"
	echo -n ${1}POL > /sys/dpm/policy/active
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
grep Bogo /proc/cpuinfo
}

function dpm-do-all
{
    if [ "${1}" = "" ] ; then
	echo "Usage: dpm-do-all <rootname> [ <prcm> <volt> <dpllmult> <dplldiv> <coremult> <armdiv> <sleepmode> ]"
	echo "Create operating point from arguments; create a class and a policy"
 	echo "with new operating point configured for all operating states."
	echo "NOTE: you may *not* re-use names"
    else
	dpm-create-opt ${1} ${2} ${3} ${4} ${5} ${6} ${7} ${8}
	dpm-create-and-set-policy ${1}
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
    SLEEPSTATE="sleep"
    
    eval dpm-create-opt \$AWAKE- \$$DefaultFastOp
#                             PRCM Volt DPLLM DPLLN CORE ARMDIV SLEEP
    dpm-create-opt $ASLEEP-     ""  ""  ""    ""    ""   ""     $S_MODE

    echo "Creating policy $POLICY"
    echo create $POLICY > /sys/dpm/policy/control
    echo -n ${AWAKE}-OPT > /sys/dpm/policy/${POLICY}/idle
    echo -n ${AWAKE}-OPT > /sys/dpm/policy/${POLICY}/idle-task
    echo -n ${AWAKE}-OPT > /sys/dpm/policy/${POLICY}/task
    echo -n ${ASLEEP}-OPT > /sys/dpm/policy/${POLICY}/${SLEEPSTATE}
    
    echo "Activating policy $POLICY"
    echo -n $POLICY > /sys/dpm/policy/active

    echo "Setting state to '${SLEEPSTATE}' (mode $S_MODE)"
    echo -n ${SLEEPSTATE} > /sys/dpm/state/active
    echo "The system is awake again."
}

function dpm-pwrst-test-domain
{
    DOMAIN="$1"
    PARAM="$2"
    RAND=$RANDOM
    PWRST=pwrst-$RAND

    echo
    echo "Old values:"
    grep "^$DOMAIN" /proc/pwromap24xx

    echo
    echo "Changing $DOMAIN Power State Mode to $PWRST_MODE"
    dpm-create-opt $PWRST-
    echo "${PARAM}=${PWRST_MODE}"
    echo -n "${PWRST_MODE}" > /sys/dpm/op/${PWRST}-OPT/${PARAM}

    dpm-create-and-set-policy $PWRST-

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
     case "$PWRST_DOMAIN" in
         0   ) dpm-pwrst-test-domain MPU mpu-pwrst ;;
         1   ) dpm-pwrst-test-domain DSP dsp-pwrst ;;
         2   ) dpm-pwrst-test-domain GFX gfx-pwrst ;;
         3   ) dpm-pwrst-test-domain COR core-pwrst ;;
         4   ) dpm-pwrst-test-domain MDM mdm-pwrst;;
         *   ) echo "'$PWRST_DOMAIN' is invalid";;
     esac
}

function dpm-domain-autoidle-test
{
    DOMAIN="$1"
    PARAM="$2"
    RAND=$RANDOM
    AUTOIDLE=autoidle-$RAND

    echo " "
    echo "Changing AutoIdle for $DOMAIN to $AI_MODE"
    dpm-create-opt $AUTOIDLE-
    echo "${PARAM}=${AI_MODE}"
    echo -n "${AI_MODE}" > /sys/dpm/op/${AUTOIDLE}-OPT/${PARAM}
    dpm-create-and-set-policy $AUTOIDLE-
    echo " "
    echo "$DOMAIN AutoIdle Mode change complete."
}

function dpm-autoidle-test
{
     echo " "
     echo "AutoIdle test for Domain $AI_DOMAIN, Mode $AI_MODE"
     case "$AI_DOMAIN" in
         0   ) dpm-domain-autoidle-test DPLL dpll-idle ;;
         1   ) dpm-domain-autoidle-test DSP  dsp-idle ;;
         2   ) dpm-domain-autoidle-test CORE core-idle ;;
         3   ) dpm-domain-autoidle-test WKUP wkup-idle ;;
         4   ) dpm-domain-autoidle-test MDM  mdm-idle ;;
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
    OMAP=$(grep ^Processor /proc/cpuinfo | awk '{ print $4 }')

    if [ "$OMAP" = "OMAP2420" ]
    then
#                     PRCM Volt DPLLM DPLLN CORE ARMDIV SLEEP
	OpPRCMIIFast="0"
	OpPRCMIISlow="1"
	OpPRCMIIIFast="2"
	OpPRCMIIISlow="3"
	DefaultFastOp=OpPRCMIIFast
	DefaultSlowOp=OpPRCMIISLow
	AllOps="OpPRCMIIFast OpPRCMIISlow OpPRCMIIIFast OpPRCMIIISlow "

    elif [ "$OMAP" = "OMAP2430" ]
    then
#                    PRCM Volt DPLLM DPLLN CORE ARMDIV SLEEP
       OpPRCM2Fast="0"
       OpPRCM2Slow="1"
       OpPRCM5aFast="2"
       OpPRCM5aSlow="3"
       OpPRCM5bFast="4"
       OpPRCM5bSlow="5"
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
		DefaultFastOp=OpPRCM2Fast
		DefaultSlowOp=OpPRCM2Slow
		AllOps="OpPRCM2Fast OpPRCM2Slow"
	else
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
