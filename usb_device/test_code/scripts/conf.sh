#!/bin/sh

# TestSuite General Variables
export USBDEVICE_POSTFIX=`date "+%Y%m%d-%H%M%S"`
export USBDEVICE_ROOT=`pwd`

export USBDEVICE_DIR_HANDLER=${USBDEVICE_ROOT}/handler
export USBDEVICE_DIR_HELPER=${USBDEVICE_ROOT}/helper
export USBDEVICE_DIR_TMP=${USBDEVICE_ROOT}/tmp
export USBDEVICE_DIR_TEST=${USBDEVICE_ROOT}/test
export USBDEVICE_DIR_SCENARIOS="${USBDEVICE_ROOT}/scenarios"
export USBDEVICE_DIR_BINARIES=${USBDEVICE_ROOT}/../bin
export USBDEVICE_DIR_LOG=${USBDEVICE_ROOT}/log

export USBDEVICE_FILE_OUTPUT=${USBDEVICE_ROOT}/output.$USBDEVICE_POSTFIX
export USBDEVICE_FILE_LOG=${USBDEVICE_ROOT}/log.$USBDEVICE_POSTFIX
export USBDEVICE_FILE_TMP=${USBDEVICE_DIR_TMP}/tmp.$USBDEVICE_POSTFIX
export USBDEVICE_FILE_CMD=cmd.$USBDEVICE_POSTFIX


export USBDEVICE_DURATION=""
export USBDEVICE_PRETTY_PRT=""
export USBDEVICE_VERBOSE=""
export USBDEVICE_SCENARIO_NAMES=""
export USBDEVICE_STRESS=""
export PATH="${USBDEVICE_ROOT}:${USBDEVICE_DIR_HELPER}:${USBDEVICE_DIR_BINARIES}:${PATH}"


# Utils General Variables
export UTILS_DIR=${USBDEVICE_ROOT}/../../utils
. ${USBDEVICE_ROOT}/../../utils/configuration/general.configuration
export UTILS_DIR_BIN=${UTILS_DIR}/bin
export UTILS_DIR_HANDLERS=${USBDEVICE_ROOT}/../../utils/handlers

export PATH="$PATH:$UTILS_DIR_BIN:$UTILS_DIR_HANDLERS:$UTILS_DIR_SCRIPTS"

# USB Device Generic
export DD_IF=/dev/urandom
export count=1
export processor=1
export LOCAL_EXECUTION_TIMES=1
export LOCAL_TIME_TO_WAIT=10
export LOCAL_IRQ_NUMBER=92
export HOSTIPADDR=$TESTER_ADDRESS # Need to use Tester_address for getting IP

export USBDEVICE_LSUSB_BEFORE=$USBDEVICE_DIR_TMP/lsusb-before.txt
export USBDEVICE_LSUSB_AFTER=$USBDEVICE_DIR_TMP/lsusb-after.txt
export USBDEVICE_LSSD_BEFORE=$USBDEVICE_DIR_TMP/lssd-before.txt
export USBDEVICE_LSSD_AFTER=$USBDEVICE_DIR_TMP/lssd-after.txt

# USB Specific directories
export RESULTS_STORAGE=${TMPBASE}
export USB_DEVFS_ENTRY=/dev/sdb
export USB_DEVFS_PARTITION=/dev/sdb1
export USB_MOUNTPOINT_PATH=/mnt/usb

#PATHS
export DPM_SCRIPT=${USBDEVICE_DIR_HELPER}/dpm_test_8x.bash
export MISC_PATH=${USBDEVICE_ROOT}/misc
export SSH_PATH=${MISC_PATH}/id_rsa
export KEY_PATH=${UTILBIN}

#USB
#Common
export DELAY0=1
export DELAY1=3
export DELAY2=5
export DELAY3=7
export DELAY4=10
export DELAY5=30
export DELAYEXT=50
export DELAYMAX=30
export ENUM_COMM=/proc/bus/usb/devices
export PROC_INT=/proc/driver/musb_hdrc
export NET_INTERFACE=/proc/net/dev
export SLEEP_STATE=/sys/power/state

# handler.ram.drive..sh
export RAMDRIVE_PATH=/mnt/ramdrive
export RAMDRIVE_SIZE=200M

# Dummy File
export DUMMY_FILE_NAME=temp.file

# USB Host Specific Variables
export USBDEVICE_DIR_MODULES=${USBDEVICE_ROOT}/../modules

##Device
export REMOTE_PATH=/data/utilities/usb
export REMOTE_IP=10.87.230.75
###File storage gadget
export GADGET_ST_FILE=OMAPonLinuxPC
export GADGET_ST_NODE=/dev/sdb
export GADGET_PROC=/proc/scsi/scsi
### Ethernet gadget
export GADGET_IP=128.247.77.13
export HOST_IP=128.247.76.46
export NETMASK_IP=255.255.254.0

# End of file
