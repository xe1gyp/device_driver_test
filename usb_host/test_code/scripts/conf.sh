#!/bin/sh

# TestSuite General Variables
export USBHOST_POSTFIX=`date "+%Y%m%d-%H%M%S"`
export USBHOST_ROOT=`pwd`

export USBHOST_DIR_HELPER=${USBHOST_ROOT}/helper
export USBHOST_DIR_TMP=${USBHOST_ROOT}/tmp
export USBHOST_DIR_TEST=${USBHOST_ROOT}/test
export USBHOST_DIR_SCENARIOS="${USBHOST_ROOT}/scenarios"
export USBHOST_BIN=${USBHOST_ROOT}/../bin
export USBHOST_DIR_LOG=${USBDEVICE_ROOT}/log

export USBHOST_FILE_OUTPUT=${USBHOST_ROOT}/output.$USBHOST_POSTFIX
export USBHOST_FILE_LOG=${USBHOST_ROOT}/log.$USBHOST_POSTFIX
export USBHOST_FILE_TMP=${USBHOST_DIR_TMP}/tmp.$USBHOST_POSTFIX
export USBHOST_FILE_CMD=cmd.$USBHOST_POSTFIX

export USBHOST_DURATION=""
export USBHOST_PRETTY_PRT=""
export USBHOST_VERBOSE=""
export USBHOST_SCENARIO_NAMES=""
export USBHOST_STRESS=""

export PATH="${USBHOST_ROOT}:${USBHOST_DIR_HELPER}:${PATH}"

# Utils General Variables
export UTILS_DIR=$USBHOST_ROOT/../../utils/
export UTILS_DIR_BIN=${USBHOST_ROOT}/../../utils/bin
export UTILS_DIR_HANDLERS=${USBHOST_ROOT}/../../utils/handlers
export UTILSMODULES=${USBHOST_ROOT}/../modules
export UTILS=${USBHOST_ROOT}/../utils
export SYSFS_EHCI_OMAP=/sys/devices/platform/ehci-omap.0/
export SYSFS_OHCI_OMAP=/sys/devices/platform/ohci-omap.0/
export USBHOST_ENUMERATION=/sys/devices/platform/musb_hdrc/mode

. $UTILS_DIR/configuration/general.configuration

export PATH="$PATH:$UTILS_DIR_BIN:$UTILS_DIR_HANDLERS:$UTILS_DIR_SCRIPTS"

# USB Host Specific Variables
export USBHOST_TEMP_VARIABLE=1
export USBHOST_MODULES_STORAGE=${TESTMODULES}
export USBHOST_RESULTS_STORAGE=${TMPBASE}
export USBHOST_DEVFS_ENTRY=/dev/sda
export USBHOST_DEVFS_PARTITION=/dev/sda1
export USBHOST_MOUNTPOINT_PATH=/mnt/mass_storage
export USBHOST_HID_NODE=/dev/event2

# Remove any error file
handlerError.sh "clean"

# End of file
