#!/bin/sh

# =============================================================================
# Variables
# =============================================================================

LOCAL_COMMAND=$1
LOCAL_FIELD=$2
LOCAL_DATA=$3

# =============================================================================
# Functions
# =============================================================================

# None

# =============================================================================
# Main
# =============================================================================

handlerError.sh "test"
if [ $? -eq 1 ]; then
	exit 1
fi

if [ "$LOCAL_COMMAND" = "get" ]; then

	if [ "$LOCAL_FIELD" = "ipaddr" ]; then

		ifconfig $ETHERNET_INTERFACE | grep addr: | sed -e 's@inet addr:@@' | sed q | awk '{print $1}' > $ETHERNET_IFCONFIG_IPADDR
		if [ -z $TESTER_ADDRESS ]; then
			cat $ETHERNET_IFCONFIG_IPADDR > $ETHERNET_EXTERNAL_HOST_IPADDR
		else
			echo $TESTER_ADDRESS > $ETHERNET_EXTERNAL_HOST_IPADDR
		fi

	elif [ "$LOCAL_FIELD" = "hwaddr" ]; then
		ifconfig | grep HWaddr | awk '{print $5}' > $ETHERNET_IFCONFIG_HWADDR
	fi

elif [ "$LOCAL_COMMAND" = "set" ]; then

	if [ "$LOCAL_FIELD" = "ipaddr" ]; then
		echo "to be implemented"
	elif [ "$LOCAL_FIELD" = "hwaddr" ]; then
		echo "to be implemented"
	elif [ "$LOCAL_FIELD" = "mtu" ]; then
		ifconfig $ETHERNET_INTERFACE mtu $LOCAL_DATA
		echo "Checking if mtu is set to $LOCAL_DATA"
		ifconfig $ETHERNET_INTERFACE | grep -i MTU | grep -i $LOCAL_DATA && exit 0 || exit 1;
	elif [ "$LOCAL_FIELD" = "mode" ]; then
		ifconfig $ETHERNET_INTERFACE $LOCAL_DATA
		echo "Checking if $LOCAL_DATA was enabled"
		ifconfig $ETHERNET_INTERFACE | grep -i $LOCAL_DATA && exit 0 || exit 1;
	fi

elif [ "$LOCAL_COMMAND" = "unset" ]; then

	if [ "$LOCAL_FIELD" = "mode" ]; then
		ifconfig $ETHERNET_INTERFACE "-$LOCAL_DATA"
		echo "Checking if $LOCAL_DATA was disabled"
		ifconfig $ETHERNET_INTERFACE | grep -i $LOCAL_DATA && exit 1 || exit 0
	fi

elif [ "$LOCAL_COMMAND" = "interface" ]; then

	if [ "$LOCAL_FIELD" = "up" ]; then

		ifconfig $ETHERNET_INTERFACE up
		ifconfig | grep $ETHERNET_INTERFACE
		if [ $? -eq 1 ]; then
			handlerError.sh "log" "1" "halt" "handlerIfconfig.sh"
		fi

	elif [ "$LOCAL_FIELD" = "down" ]; then

		ifconfig $ETHERNET_INTERFACE down
		ifconfig | grep $ETHERNET_INTERFACE
		if [ $? -eq 0 ]; then
			handlerError.sh "log" "1" "halt" "handlerIfconfig.sh"
		fi
	fi

fi

# End of file
