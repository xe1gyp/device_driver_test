#!/bin/sh

# TestSuite General Variables
export ETHERNET_POSTFIX=`date "+%Y%m%d-%H%M%S"`
export ETHERNET_ROOT=`pwd`

export ETHERNET_DIR_BINARIES=${ETHERNET_ROOT}/../bin
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
export UTILS_DIR=$ETHERNET_ROOT/../../utils/
export UTILS_DIR_BIN=$UTILS_DIR/bin
export UTILS_DIR_HANDLERS=$UTILS_DIR/handlers
export UTILS_DIR_SCRIPTS=$UTILS_DIR/scripts

. $UTILS_DIR/configuration/general.configuration

export PATH="$PATH:$UTILS_DIR_BIN:$UTILS_DIR_HANDLERS:$UTILS_DIR_SCRIPTS"

# Specific Ethernet Variables
export ETHERNET_DEV_URANDOM=/dev/urandom
export ETHERNET_PACKET_SIZE=32768
export ETHERNET_INTERFACE=eth0
export ETHERNET_IFCONFIG_IPADDR=$ETHERNET_DIR_TMP/ethernet.ifconfig.ipaddr
export ETHERNET_IFCONFIG_HWADDR=$ETHERNET_DIR_TMP/ethernet.ifconfig.hwaddr

export ETHERNET_EXTERNAL_HOST_IPADDR=$ETHERNET_DIR_TMP/ethernet.host.ipaddr
export ETHERNET_EXTERNAL_HOST_HWADDR=$ETHERNET_DIR_TMP/ethernet.host.hwaddr
export ETHERNET_NUTTCP_SERVER_IPADDR=$ETHERNET_DIR_TMP/ethernet.nuttcp.server.ipaddr
export ETHERNET_KERNEL_MESSAGES=$ETHERNET_DIR_TMP/ethernet.kernel.messages

export ETHERNET_NFS_DIR=$ETHERNET_DIR_TMP/ethernet.nfs.directory.path

export ETHERNET_IRQ=$ETHERNET_DIR_TMP/ethernet.irq
handlerIrq.sh "get" "irq" "eth0" "$ETHERNET_IRQ"

# NFS
export ETHERNET_NFS_LOCAL=$ETHERNET_DIR_TMP/nfs

if [ ! `echo 1+1 | bc` ]; then
	echo "FATAL: bc application unavailable, cannot continue"
	exit 1
fi

# Remove any error file
handlerError.sh "clean"

# End of file
