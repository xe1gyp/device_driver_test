# (C) Texas Instruments 2009
# Initial Version 21.Oct.2009
#

if [ "$TEST_PLATFORM" = "OMAP" ]
then
	export OMAP_BATT_SYSFS=/sys/class/power_supply #Common path
	export OMAP_BATT_PRESENT="${OMAP_BATT_SYSFS}/twl4030_bci_battery/present"
	export OMAP_BATT_MAIN_VOLTAGE="${OMAP_BATT_SYSFS}/twl4030_bci_battery/voltage_now"
	export OMAP_BATT_BACK_VOLTAGE="${OMAP_BATT_SYSFS}/twl4030_bci_bk_batt/voltage_now"
	export OMAP_BATT_CHARGE_SRC="${OMAP_BATT_SYSFS}/twl4030_bci_battery/online"
	export OMAP_BATT_TEMP="${OMAP_BATT_SYSFS}/twl4030_bci_battery/uevent"
fi

if [ "$TEST_PLATFORM" = "OMAP4" ]
then
	export OMAP_BATT_SYSFS=/sys/class/power_supply #Common path
	export OMAP_BATT_PRESENT="${OMAP_BATT_SYSFS}/twl6030_bci_battery/present"
	export OMAP_BATT_MAIN_VOLTAGE="${OMAP_BATT_SYSFS}/twl6030_bci_battery/voltage_now"
	export OMAP_BATT_BACK_VOLTAGE="${OMAP_BATT_SYSFS}/twl6030_bci_bk_batt/voltage_now"
	export OMAP_BATT_CHARGE_SRC="${OMAP_BATT_SYSFS}/twl6030_bci_battery/online"
	export OMAP_BATT_TEMP="${OMAP_BATT_SYSFS}/twl6030_bci_battery/uevent"
fi

if [ "$TEST_PLATFORM" = "DUMMY" ]
then
	export OMAP_BATT_SYSFS=/sys/class/net #Common path
	export OMAP_BATT_PRESENT="${OMAP_BATT_SYSFS}/eth0/tx_queue_len"
	export OMAP_BATT_MAIN_VOLTAGE="${OMAP_BATT_SYSFS}/eth0/address"
	export OMAP_BATT_BACK_VOLTAGE="${OMAP_BATT_SYSFS}/eth0/addr_len"
	export OMAP_BATT_CHARGE_SRC="${OMAP_BATT_SYSFS}/eth0/ifindex"
	export OMAP_BATT_TEMP="${OMAP_BATT_SYSFS}/eth0/mtu"
fi

# The below functions return the battery state values from various sysfs entries
# The values are exported into the named variable passed as the last parameter
# to the function
# note:\$$# = Dereference the last parameter passed to the function

# Example usage
# source battery_util.sh
# Declare a dummy variable to store the battery temperature
# Battery_temp_val=
# get_batt_temperature "Battery_temp_val"
# echo $Battery_temp_val  #This will print the value as stored by the function

get_battery_presence()
{
	eval "export \$$#=`cat $OMAP_BATT_PRESENT`"
}

get_main_batt_voltage()
{
	eval "export \$$#=`cat $OMAP_BATT_MAIN_VOLTAGE`"
}

get_back_batt_voltage()
{
	eval "export \$$#=`cat $OMAP_BATT_BACK_VOLTAGE`"
}

get_batt_charging_source()
{
	eval "export \$$#=`cat $OMAP_BATT_CHARGE_SRC`"
}

get_batt_temperature()
{
	eval "export \$$#=`cat $OMAP_BATT_TEMP`"
}


battery_test_get_battery_state()
{
	get_battery_presence "$1"
	if [ $? != 0 ]
	then
		echo "Error"
		Global_ERR=-1
	fi

	get_main_batt_voltage "$2"
	if [ $? != 0 ]
	then
		echo "Error"
		Global_ERR=-1
	fi

	get_back_batt_voltage "$3"
	if [ $? != 0 ]
	then
		echo "Error"
		Global_ERR=-1
	fi

	get_batt_charging_source "$4"
	if [ $? != 0 ]
	then
		echo "Error"
		Global_ERR=-1
	fi

	get_batt_temperature "$5"
	if [ $? != 0 ]
	then
		echo "Error"
		Global_ERR=-1
	fi
}
