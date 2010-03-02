#!/bin/sh

#generic settings
export DEBUGFS_DIRECTORY=/debug
export DEBUGFS_PMCOUNT=$DEBUGFS_DIRECTORY/pm_debug/count
LOCAL_POWER_DOMAIN=core_pwrdm
LOCAL_HIT=RET
# Mount the Debugfs
mkdir -p $DEBUGFS_DIRECTORY
mount -t debugfs debugfs $DEBUGFS_DIRECTORY

#Enable Sleep while Idle
echo 1 > /debug/pm_debug/sleep_while_idle

# Changing the governor to userspace 
echo "userspace" > /sys/devices/system/cpu/cpu0/cpufreq/scaling_governor

# AVS
echo 1 > /debug/pm_debug/sr1_autocomp
echo 1 > /debug/pm_debug/sr2_autocomp

#make display blank
echo "1" > /sys/class/graphics/fb0/blank

# Checking if scaling_setspeed has been created
if [ ! -e /sys/devices/system/cpu/cpu0/cpufreq/scaling_setspeed ]
then
	echo "Fatal: scaling_setspeed is not available"
	exit 1
fi

# Changing through all the frequencies available
available_frequencies=`cat /sys/devices/system/cpu/cpu0/cpufreq/scaling_available_frequencies`

while true 
do
	for i in $available_frequencies
	do
		echo "Setting ARM Frequency to" $i
		echo $i > /sys/devices/system/cpu/cpu0/cpufreq/scaling_setspeed
		cur_frequency=`cat /sys/devices/system/cpu/cpu0/cpufreq/scaling_cur_freq`
		if [ "$i" != "$cur_frequency" ]
		then
			echo "Fatal: Current frequency is different from the set one"
			exit 1
		fi
		cur_frequency=`cat /debug/clock/virt_26m_ck/osc_sys_ck/sys_ck/dpll2_ck/dpll2_m2_ck/iva2_ck/rate`
		echo "ARM Frequency ==>" $i
		echo "DSP/IVA2 Frequency ==> " $cur_frequency
		cur_frequency=`cat /debug/clock/virt_26m_ck/osc_sys_ck/sys_ck/dpll3_ck/dpll3_m2_ck/core_ck/l3_ick/rate`
		echo "L3 Clock ==> " $cur_frequency  
	done
done

umount $DEBUGFS_DIRECTORY
