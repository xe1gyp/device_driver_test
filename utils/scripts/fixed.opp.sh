#!/bin/sh
FOLDER_ENTRIES=/sys/devices/system/cpu/cpu0/cpufreq/
SETSPEED_ENTRY=$FOLDER_ENTRIES/scaling_setspeed
GOVERNOR_ENTRY=$FOLDER_ENTRIES/scaling_governor
FREQUENCIES_ENTRY=$FOLDER_ENTRIES/scaling_available_frequencies
SYS_CK_FOLDER=/debug/clock/virt_26m_ck/osc_sys_ck/sys_ck/
CUR_ARM_FREQUENCY_ENTRY=$FOLDER_ENTRIES/scaling_cur_freq
CUR_DSP_FREQUENCY_ENTRY=$SYS_CK_FOLDER/dpll2_ck/dpll2_m2_ck/iva2_ck/rate
WARNING_MESSAGE="Current frequency is different from the set one"
ITERATIONS=$1
COMMAND=$2
STATUS=0

function set_frequencies
{
        for i in $available_frequencies
        do
                echo $i > $SETSPEED_ENTRY
                cur_arm_frequency=`cat $CUR_ARM_FREQUENCY_ENTRY`
                cur_dsp_frequency=`cat $CUR_DSP_FREQUENCY_ENTRY`
                if [ "$i" != "$cur_arm_frequency" ]
                then
                        echo "$WARNING_MESSAGE ($i)"
                fi
                echo "ARM frequency: $cur_arm_frequency"
                echo "DSP frequency: $cur_dsp_frequency"

                if [ "$COMMAND" != "" ]
                then
                        $COMMAND
                        let STATUS=$STATUS+`echo $?`
                fi
                sleep 1
        done
}

# Changing the governor to userspace
echo "userspace" > $GOVERNOR_ENTRY

# Checking if scaling_setspeed has been created
if [ ! -e $SETSPEED_ENTRY ]
then
        echo "Fatal: scaling_setspeed is not available"
        exit 1
fi

# Changing through all the frequencies available
available_frequencies=`cat $FREQUENCIES_ENTRY`

if [ "$ITERATIONS" == "" ]
then
        while [ true ]
        do
                set_frequencies
        done
else
        COUNTER=0

        while [ $COUNTER -lt $ITERATIONS ]
        do
                set_frequencies
                let COUNTER=$COUNTER+1
        done

        exit $STATUS
fi
