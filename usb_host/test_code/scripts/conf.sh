#!/bin/sh
################################################################################
# Configuration script
# Author  : Axel Castaneda Gonzalez
# Date    : November 27, 2006
# Description: Configuration script with user specified value for some
#	environment variables.
# Change Log:
#	9 Jan 2009 - Diego Zavala Trujillo - Updating environment variables.
################################################################################


#These variables dont' need to be modified
export POSTFIX=`date "+%Y%m%d-%H%M%S"`
export TESTROOT=${PWD}
export TESTBIN=${PWD}/../bin
export UTILBIN=${PWD}/../../utils/bin
export TESTMODS=${PWD}/../mods
export TESTSCRIPT=${PWD}/helper
export TMPBASE=${TESTROOT}/tmp
export TMPFILE=${TMPBASE}/tmp.$POSTFIX
export CMDFILE=cmd.$POSTFIX
export TESTDIR=${TESTROOT}/test
export PRETTY_PRT=""
export VERBOSE=""
export OUTPUTFILE=${TESTROOT}/output.$POSTFIX
export LOGBASE=${TESTROOT}/log
export LOGFILE=${LOGBASE}/log.$POSTFIX
export LOGERROR=${LOGBASE}/log-error.$POSTFIX
export DURATION="1h"
export PATH="${PATH}:${TESTROOT}:${TESTBIN}:${TESTSCRIPT}"
export TC_SCENARIO="${TESTROOT}/scenarios"
export SCENARIO_NAMES=""
##########################################

#USB
#Common
export DELAY0=1
export DELAY1=3
export DELAY2=5
export DELAY3=7
export DELAY4=10
export DELAY5=20
export DELAYEXT=20
export ENUM_COMM=/proc/bus/usb/devices
export PROC_INT=/proc/driver/musb_hdrc
export SLEEP_STATE=/sys/power/state
#export SYSFS_MENTORNODE=/sys/devices/platform/musb_hdrc.0/power/state
#export SYSFS_OHCINODE=/sys/devices/platform/ohci/power/state
#export SUSPEND="3"
#export RESUME="0"
export USB_DRIVER="MENTOR"
#export SLOW_FREQ='s\n0\n'
#export FAST_FREQ='f\n0\n'
#export SWITCH_FREQ='6\n0\n'
#export SWITCH_FREQS='9\n1\n6\n0\n'
#export SYSTEM_SUSPEND='4\n0\n'

#PATHS
export SCRIPT_PATH=${pwd}
export MODULE_PATH=${TESTMODS}
export DPM_SCRIPT=${TESTSCRIPT}/dpm_test_8x.bash
export MISC_PATH=${TESTROOT}/misc
export SSH_PATH=${MISC_PATH}/id_rsa
export KEY_PATH=${UTILBIN}
##########################################

##Host

## Enumeration
export BUS="01"
## Mass storage
export MASS_DIRECTORY=${MISC_PATH}/mass_storage
export MASS_FILE=6m #uImage18.5_HSHOST #6m
## Serial adapter
export SERIAL_FILE_HOST=${MISC_PATH}/file.txt
export SERIAL_FILE_CLIENT=$REMOTE_PATH/ls.txt
## Ethernet adapter
export LOCAL_IP=10.87.230.232
export ETH1_IP=10.87.230.244
## HID
export HID_NODE=/dev/event1
##########################################

##Device
export REMOTE_PATH=/data/utilities/usb
export REMOTE_IP=10.87.230.75
###File storage gadget
#export GADGET_ST_FILE=tempfile
#export GADGET_ST_NODE=/dev/sdc1
export GADGET_ST_FILE=OMAPonLinuxPC
export GADGET_ST_NODE=/dev/sdc
export GADGET_PROC=/proc/scsi/scsi
export SCSI_STORAGE=/proc/scsi/usb-storage
### Ethernet gadget
export GADGET_IP=192.168.0.7
export HOST_IP=192.168.0.77
export NETMASK_IP=255.255.255.0

##########################################

# 23.x
##Device
###File storage gadget
export VFAT_FILE=${MISC_PATH}/vfat-4M
export VEXT_FILE=${MISC_PATH}/file_ext2_4Mb
##OTG
export HOST_MODE="Host"
export PERIPHERAL_MODE="Peripheral"
export QUEUE_EMPTY="ep1"
