#!/bin/sh

# TestSuite General Variables
export ETHERNET_POSTFIX=`date "+%Y%m%d-%H%M%S"`
export ETHERNET_ROOT=`pwd`

export ETHERNET_DIR_BINARIES=${ETHERNET_ROOT}/../binaries
export ETHERNET_DIR_HELPER=${ETHERNET_ROOT}/helper
export ETHERNET_DIR_TMP=${ETHERNET_ROOT}/tmp
export ETHERNET_DIR_TEST=${ETHERNET_ROOT}/test
export ETHERNET_DIR_SCENARIOS="${ETHERNET_ROOT}/scenarios"

export ETHERNET_FILE_OUTPUT=${ETHERNET_ROOT}/output.$ETHERNET_POSTFIX
export ETHERNET_FILE_LOG=${ETHERNET_ROOT}/log.$ETHERNET_POSTFIX
export ETHERNET_FILE_TMP=${ETHERNET_DIR_TMP}/tmp.$ETHERNET_POSTFIX
export ETHERNET_FILE_CMD=cmd.$ETHERNET_POSTFIX

export ETHERNET_DURATION=""
export ETHERNET_PRETTY_PRT=""
export ETHERNET_VERBOSE=""
export ETHERNET_SCENARIO_NAMES=""
export ETHERNET_STRESS=""
export ETHERNET_PANIC=$ETHERNET_DIR_TMP/ethernet.panic

export PATH="${ETHERNET_ROOT}:${ETHERNET_DIR_HELPER}:$ETHERNET_DIR_BINARIES:${PATH}"

# Utils General Variables
. ${ETHERNET_ROOT}/../../utils/configuration/general.configuration
export UTILS_DIR_BIN=${ETHERNET_ROOT}/../../utils/bin
export UTILS_DIR_HANDLERS=${ETHERNET_ROOT}/../../utils/handlers
export UTILSCRIPTS=${ETHERNET_ROOT}/../../utils/scripts

# Specific Ethernet Variables
export ETHERNET_PACKET_SIZE=32768
export ETHERNET_INTERFACE=eth0
export ETHERNET_IFCONFIG_IPADDR=$ETHERNET_DIR_TMP/ifconfig.ipaddr
export ETHERNET_IFCONFIG_HWADDR=$ETHERNET_DIR_TMP/ifconfig.hwaddr

export ETHERNET_EXTERNAL_HOST_IPADDR=$ETHERNET_DIR_TMP/external.ipaddr
export ETHERNET_EXTERNAL_HOST_HWADDR=
export ETHERNET_NUTTCP_SERVER_IPADDR=$ETHERNET_DIR_TMP/nuttcp.server.ipaddr
export ETHERNET_KERNEL_MESSAGES=$ETHERNET_DIR_TMP/dmesg

if [ ! `echo 1+1 | bc` ]; then
	echo "FATAL: bc application unavailable, cannot continue"
	exit 1
fi

# End of file
