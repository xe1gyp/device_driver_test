#!/bin/sh

# TestSuite General Variables
export USBHOST_POSTFIX=`date "+%Y%m%d-%H%M%S"`
export USBHOST_ROOT=`pwd`

export USBHOST_DIR_HELPER=${USBHOST_ROOT}/helper
export USBHOST_DIR_TMP=${USBHOST_ROOT}/tmp
export USBHOST_DIR_TEST=${USBHOST_ROOT}/test
export USBHOST_DIR_SCENARIOS="${USBHOST_ROOT}/scenarios"
export USBHOST_DIR_BINARIES=${USBHOST_ROOT}/../binaries

export USBHOST_FILE_OUTPUT=${USBHOST_ROOT}/output.$USBHOST_POSTFIX
export USBHOST_FILE_LOG=${USBHOST_ROOT}/log.$USBHOST_POSTFIX
export USBHOST_FILE_TMP=${USBHOST_DIR_TMP}/tmp.$USBHOST_POSTFIX
export USBHOST_FILE_CMD=cmd.$USBHOST_POSTFIX

export USBHOST_DURATION=""
export USBHOST_PRETTY_PRT=""
export USBHOST_VERBOSE=""
export USBHOST_SCENARIO_NAMES=""
export USBHOST_STRESS=""

export PATH="${USBHOST_ROOT}:${USBHOST_DIR_HELPER}:${USBHOST_DIR_BINARIES}:${PATH}"

# Utils General Variables
. ${USBHOST_ROOT}/../../utils/configuration/general.configuration
export UTILS_DIR_BIN=${USBHOST_ROOT}/../../utils/bin
export UTILS_DIR_HANDLERS=${USBHOST_ROOT}/../../utils/handlers

# USB Host Specific Variables
export USBHOST_DIR_MODULES=${USBHOST_ROOT}/../modules
export USBHOST_HID_NODE=/dev/event2
export USBHOST_SYSFS_EHCI_OMAP=/sys/devices/platform/ehci-omap.0/
export USBHOST_SYSFS_OHCI_OMAP=/sys/devices/platform/ohci-omap.0/
export USBHOST_SYSFS_MUSB_ENUMERATION=/sys/devices/platform/musb_hdrc/mode

# End of file
