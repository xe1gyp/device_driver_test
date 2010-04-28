#!/bin/sh

ENABLE=$1

SLEEP_IDLE=`echo /debug/pm_debug/sleep_while_idle`
OFF_MODE=`echo /debug/pm_debug/enable_off_mode`
VOLT_OFF_IDLE=`echo /debug/pm_debug/voltage_off_while_idle`

mkdir -p /debug
mount -t debugfs debugfs /debug

if [ $ENABLE -eq 1 ] ;then

    echo 1 > $SLEEP_IDLE
    echo 1 > $OFF_MODE
    echo 1 > $VOLT_OFF_IDLE

elif [ $ENABLE -eq 0 ] ;then

    echo 0 > $SLEEP_IDLE
    echo 0 > $OFF_MODE
    echo 0 > $VOLT_OFF_IDLE

fi
