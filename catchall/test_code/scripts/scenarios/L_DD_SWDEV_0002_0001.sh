#!/bin/sh
################################################################################
# Scenario:    L_DD_SWDEV_0002_0001
# Author  :    Ricardo Perez Olivares
# Date    :    November 19, 2010
# Description: Checking for Debug Support
################################################################################

#Automatic check for exported variables: ARCH and CROSS_COMPILE

echo -e "Checking for exported variable CROSS_COMPILE\n"
env | grep CROSS_COMPILE

if [ $? -eq 0 ]
	then
		echo -e "FOUND: CROSS_COMPILE exported"
		echo -e "+++++++++++++++++++++++++++++\n"
	else
		echo -e "NOT FOUND : CROSS_COMPILE"
		echo -e "*************************"
		echo -e "Exporting variable CROSS_COMPILE . . . \n"
                export CROSS_COMPILE=arm-none-linux-gnueabi-
fi 

echo -e "Checking for exported variable ARCH\n"
env | grep ARCH

if [ $? -eq 0 ]
        then
                echo -e "FOUND: ARCH exported"
		echo -e "++++++++++++++++++++\n"
        else
		echo -e "NOT FOUND: ARCH"
		echo -e "***************"
                echo -e "Exporting variable ARCH . . . \n"
                export ARCH=arm
fi

# Begin L_DD_SWDEV_0002_0001

echo "Checking if Debug Support enabled on: omap_4430sdp_defconfig"
echo "Analisys starting....."
echo -e "\n"
sleep 1

#Erasing previus LOG files
rm *.log

#Configuring Kernel
make mrproper
make omap_4430sdp_defconfig

#Array containing the elements to look for:
DEBUG=(CONFIG_SYSFS=y DEBUG_INFO=y DEBUG_KERNEL=y DEBUG_SYSFS=y)

for i in ${DEBUG[*]}
do
	echo -e "Looking for required enabled macros in .conf file: "
	sleep 1
	printf "      %s\n" $i
	printf "      %s\n" $i >> debug.log
done

echo -e "==========================="
echo -e "= Macros found in .config:= "
echo -e "===========================\n"

lines=( $(< "debug.log" ) )
printf "%s\n" "${lines[@]}" | nl

if [ ${#lines[@]} -eq ${#DEBUG[*]} ]
	then
#		echo -e "\n"
		echo -e "--------------------------------------------"
		echo -e "PASS: All required enabled macros were found"
		echo -e "--------------------------------------------\n"
	else
#		echo -e "\n"
		echo -e "++++++++++++++++++++++++++++++++++++"
		echo -e "FAIL: At least one item were missing"
		echo -e "++++++++++++++++++++++++++++++++++++\n"
fi

# End L_DD_SWDEV_0002_0001
