export OMAP_BATT_SYSFS=/sys/class/power_supply #Common path
export OMAP_BATT_MAIN_VOLTAGE="${OMAP_BATT_SYSFS}/twl6030_battery/voltage_now"
export OMAP_BATT_BACK_VOLTAGE="${OMAP_BATT_SYSFS}/twl6030_bk_battery/\
voltage_now"
export OMAP_BATT_CHARGER_ONLINE="${OMAP_BATT_SYSFS}/twl6030_battery/online"
export OMAP_BATT_AC_CHARGER_ONLINE="${OMAP_BATT_SYSFS}/twl6030_ac/online"
export OMAP_BATT_USB_CHARGER_ONLINE="${OMAP_BATT_SYSFS}/twl6030_usb/online"
export OMAP_BATT_TEMP="${OMAP_BATT_SYSFS}/twl6030_battery/uevent"
export OMAP_BATT_CHARGING_STATUS="${OMAP_BATT_SYSFS}/twl6030_battery/status"
export OMAP_BATT_CHARGING="${OMAP_BATT_SYSFS}/twl6030_battery/device/charging"
export OMAP_BATT_CHARGER_CAPACITY="${OMAP_BATT_SYSFS}/twl6030_battery/capacity"
export OMAP_FG_ACCUMULATOR="${OMAP_BATT_SYSFS}/twl6030_battery/device/\
fg_accumulator"
export OMAP_FG_COUNTER="${OMAP_BATT_SYSFS}/twl6030_battery/device/fg_counter"
export OMAP_FG_MODE="${OMAP_BATT_SYSFS}/twl6030_battery/device/fg_mode"
export OMAP_FG_OFFSET="${OMAP_BATT_SYSFS}/twl6030_battery/device/fg_offset"
export OMAP_VBUS_VOLTAGE="${OMAP_BATT_SYSFS}/twl6030_battery/device/\
vbus_voltage"
export OMAP_CHARGE_CURRENT="${OMAP_BATT_SYSFS}/twl6030_battery/device/\
charge_current"
export OMAP_CHARGE_SRC="${OMAP_BATT_SYSFS}/twl6030_battery/device/charge_src"
export OMAP_CIN_LIMIT="${OMAP_BATT_SYSFS}/twl6030_battery/device/cin_limit"
export OMAP_CURRENT_AVG_INTERVAL="${OMAP_BATT_SYSFS}/twl6030_battery/\
device/current_avg_interval"
ssh_connect()
{
	echo "\nSSH Connection in progress\n"
	ssh testing_host phtest "$1" "$2"
	if [ $? -ne 0 ] ; then
		echo "\n!!!!!!!!!!!!!!SSH not configured!!!!!!!!!!!!!!!!!!!!\n"
		exit 1
	fi
}


TESTCASE_ID=$1

case "$TESTCASE_ID" in

#Indicate USB or AC power supply connection
	211)
		ssh_connect $ac_charger_port 0
		charger_source=`cat $OMAP_BATT_AC_CHARGER_ONLINE`
		if [ $charger_source -eq 2 ]; then
	        	echo "\n\nAC Charger source not cut!!!\n\n"
			exit 1
		else
			echo "\n AC Charger not present\n"
		fi
	;;


	212)
		ssh_connect $ac_charger_port 1
		charger_source=`cat $OMAP_BATT_AC_CHARGER_ONLINE`
		if [ $charger_source -eq 2 ]; then
			echo "\n\nAC Charger source present\n\n"
		else
			echo "\n AC Charger not present\n\n"
			exit 1
		fi
	;;


	213)
		ssh_connect $usb_charger_port 0
		charger_source=`cat $OMAP_BATT_USB_CHARGER_ONLINE`
		if [ $charger_source -eq 3 ]; then
			echo "\n\nUSB Charging source not cut!!!\n\n"
			exit 1
		else
			echo "\n AC Charger not present\n\n"
		fi
	;;


	214)
		ssh_connect $usb_charger_port 1
		charger_source=`cat $OMAP_BATT_USB_CHARGER_ONLINE`
		if [ $charger_source -eq 3 ]; then
		        echo "\n\nUSB charger source present\n\n"
		else
		        echo "\n\nUSB charging source not found\n\n "
			exit 1
		fi
	;;


#Power level of the main battery
	31)
		main_voltage=`cat $OMAP_BATT_MAIN_VOLTAGE`
		echo "\nMain battery voltage is $main_voltage microvolts\n"
		if [ $main_voltage -lt 4218000 || $main_voltage -lt 2500000] ;\
 then
			echo "\n Invalid voltage\n "
			exit 1
		fi
	;;


#Power level of the back up battery
	32)
		bkup_voltage=`cat $OMAP_BATT_BACK_VOLTAGE`
		echo "\nBack up battery voltage is $bkup_voltage microvolts\n"
		if [ $bkup_voltage -gt 2500000] ; then
			echo "\n Invalid voltage\n "
			exit 1
		fi

	;;


#Retrieval of current  and other battery values
	41)

		echo "\n:::::::Main battery values $main_battery_values::::::\n"
		cat $OMAP_BATT_TEMP
		if [ $? -ne 0 ] ; then
			echo "\nError!!\n"
			exit 1
		fi

	;;


#Monitoring battery charge state
	51)

		echo "\n::::Main Battery charging status::::\n"
		cat $OMAP_BATT_CHARGING_STATUS
		if [ $? -ne 0 ] ; then
			echo "\nError checking the status\n"
			exit 1
		fi


	;;


#Notifying battery charging has started
	521)
		ssh_connect $ac_charger_port 0
		ssh_connect $usb_charger_port 1

		echo "\nBattery charging status\n"
		cat $OMAP_BATT_CHARGING_STATUS | grep "Charging"
		if [ $? -ne 0 ] ; then
			echo "\nCharging has not started!!\n"
			exit 1
		else
			echo "\nUSB Charging has started!!\n"
        	fi


	;;


	522)
		ssh_connect $usb_charger_port 0
		ssh_connect $ac_charger_port 1

		echo "\nBattery charging status\n"
		cat $OMAP_BATT_CHARGING_STATUS | grep "Charging"
		if [ $? -ne 0 ] ; then
			echo "\nCharging has not started!!\n"
			exit 1
		else
			echo "\n\n AC Charging started\n"
		fi


        ;;


#Notifying when battery charging has stopped
	531)
		ssh_connect $ac_charger_port 0
		ssh_connect $usb_charger_port 0
		ssh_connect $ac_charger_port 1

		cat $OMAP_BATT_CHARGING_STATUS | grep "Charging"
		if [ $? -ne 0 ] ; then
			echo "\nCharging has not started!!\n"
			exit 1
		else
			echo "\n\n AC Charging started\n"
		fi

		ssh_connect $ac_charger_port 0
		echo "\nBattery charging status\n"
		cat $OMAP_BATT_CHARGING_STATUS
		cat $OMAP_BATT_CHARGING_STATUS | grep "Discharging"
		if [ $? -ne 0 ] ; then
			echo "\nDisCharging has not started!!\n"
			exit 1
		else
			echo "\n discharging\n"
		fi

	;;

	532)
		ssh_connect $ac_charger_port 0
		ssh_connect $usb_charger_port 0
		ssh_connect $usb_charger_port 1

		cat $OMAP_BATT_CHARGING_STATUS | grep "Charging"
		if [ $? -ne 0 ] ; then
			echo "\nCharging has not started!!\n"
			exit 1
		else
			echo "\n\n USB Charging started\n"
		fi
		ssh_connect $usb_charger_port 0
		echo "\nBattery charging status\n"
		cat $OMAP_BATT_CHARGING_STATUS
		cat $OMAP_BATT_CHARGING_STATUS | grep "Discharging"
		if [ $? -ne 0 ] ; then
			echo "\nDisCharging has not started!!\n"
			exit 1
		else
			echo "\n discharging\n"
		fi

	;;


#AC Charger support
	61)
		ssh_connect $ac_charger_port 1
		ssh_connect $usb_charger_port 0

		ac_charger=`cat $OMAP_BATT_AC_CHARGER_ONLINE`

		if [ $ac_charger -eq 2 ]; then
		        echo "\n\nCharging via AC source\n\n"
		else
        		echo "\n\n No AC Charging\n\n"
			exit 1
		fi
	;;


#USB Charger support
	62)
		ssh_connect $ac_charger_port 0
		ssh_connect $usb_charger_port 1

		usb_charger=`cat $OMAP_BATT_USB_CHARGER_ONLINE`

		if [ $usb_charger -eq 3 ]; then
		        echo "\n\nCharging via USB\n\n"
		else
		        echo "\n\nNo USB Charging\n\n"
			exit 1
		fi

	;;


#Selection of battery charging source
	711)
		ssh_connect $ac_charger_port 1

		echo "startac" > $OMAP_BATT_CHARGING

		ac_charger=`cat $OMAP_BATT_CHARGER_ONLINE`

		if [ $ac_charger -eq 2 ]; then
		        echo "\n\nAC Charger is the Charging source\n\n"
		else
		        echo "\n\n Charging not happening via AC!!!\n\n"
			exit 1
		fi
	;;


	712)
		ssh_connect $usb_charger_port 1

		echo "startusb" > $OMAP_BATT_CHARGING

		usb_charger=`cat $OMAP_BATT_CHARGER_ONLINE`

		if [ $usb_charger -eq 3 ]; then
		        echo "\n\nUSB is the charging source\n\n"
		else
		        echo "\n\nCharging not happening via USB!!!\n\n"
			exit 1
		fi

	;;


#Battery capacity
	119)
		echo "Battery capacity"
		cat $OMAP_BATT_CHARGER_CAPACITY
	;;


#Robustness test plugging in AC and USB chargers and plugging out n times
	1711)
		LOOP_COUNT=2
		export i=0;
		while [ "$i" -lt "$LOOP_COUNT" ]
		do
			ssh_connect $ac_charger_port 0
			ssh_connect $usb_charger_port 0
			ac_charger=`cat $OMAP_BATT_AC_CHARGER_ONLINE`
			usb_charger=`cat $OMAP_BATT_USB_CHARGER_ONLINE`
			if [ $ac_charger -ne 2 -a $usb_charger -ne 3 ]; then
				i=`expr $i + 1`
			else
				echo "\n error!\n"
				exit 1;
			fi
			ssh_connect $ac_charger_port 1
			ssh_connect $usb_charger_port 1
			ac_charger=`cat $OMAP_BATT_AC_CHARGER_ONLINE`
			usb_charger=`cat $OMAP_BATT_USB_CHARGER_ONLINE`
			if [ $ac_charger -eq 2 -a $usb_charger -eq 3 ]; then
                                i=`expr $i + 1`
			else
				echo "\n error!\n"
				exit 1;
			fi
			done
	;;

	1712)
		LOOP_COUNT=1
		export i=0;
		while [ "$i" != "$LOOP_COUNT" ]
		do
			ssh_connect $usb_charger_port 0
			ssh_connect $ac_charger_port 1
			echo "\n\n$i:: The voltage with AC on and USB cut:: "
			voltage1=`cat $OMAP_BATT_MAIN_VOLTAGE`
			ac_charger=`cat $OMAP_BATT_AC_CHARGER_ONLINE`
			online_src=`cat $OMAP_BATT_CHARGER_ONLINE`
			if [ $ac_charger -eq 2 -a $online_src -eq 2 ]; then
				echo "\n\nAC Charger is the Charging source\n\n"
				i=`expr $i + 1`
			else
				echo "\n\n Charging not happening via AC!!!\n\n"
				exit 1
			fi
		done
		;;


#FG Parameters
	181)
		echo "\n::::::::::::::::::::FG parameters::::::::::::::::::\n"
		echo "FG Accumulator value::"
		cat $OMAP_FG_ACCUMULATOR
		echo "FG Counter value::"
		cat $OMAP_FG_COUNTER
		echo "FG mode value::"
		cat $OMAP_FG_MODE
		echo "FG offset value::"
		cat $OMAP_FG_OFFSET
	;;


#VBus voltage
	182)
		echo "\nVbus Voltage\n"
		cat $OMAP_VBUS_VOLTAGE

	;;


#Current parameteres
	183)
		echo "\n::::::::::::::::Current parameters:::::::::::::::::\n"
		echo "Charge current value::"
		cat $OMAP_CHARGE_CURRENT
		echo "Charge source::"
		cat $OMAP_CHARGE_SRC
		echo "Input current limit::"
		cat $OMAP_CIN_LIMIT
		echo "Current avg interval::"
		cat $OMAP_CURRENT_AVG_INTERVAL
	;;


	*)
		echo "\n wrong id\n"
		exit 1
	;;
esac
