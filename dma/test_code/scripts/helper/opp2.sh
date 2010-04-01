#!/bin/sh
cd /sys/devices/system/cpu/cpu0/cpufreq/
echo userspace > scaling_governor
gove=`cat /sys/devices/system/cpu/cpu0/cpufreq/scaling_governor`
echo The governer is $gove
orig_freq=`cat /sys/devices/system/cpu/cpu0/cpufreq/scaling_cur_freq`
echo The original OPP is $orig_freq
echo 250000  > scaling_setspeed
curr_freq=`cat /sys/devices/system/cpu/cpu0/cpufreq/scaling_cur_freq`
echo The current OPP is $curr_freq











