#!/bin/sh

################################################################################
# Scenario:    L_DD_SWDEV_0001_0001
# Author  :    Ricardo Perez Olivares
# Date    :    November 18, 2010
# Description: Compile Time Warnings
################################################################################

# Begin L_DD_SWDEV_0001_0001

rm *.log

if [ $? -eq 0 ]

        then
                echo -e "OK: log files removed successfully!!\n"        
        else
                echo -e "*.log files doesn't exist or can't be removed, please check...\n"
fi

echo "------------------------------------------"
echo "Configuration used: omap_4430sdp_defconfig"
echo "Analisys of warnings start....."
echo "------------------------------------------"
echo -e "\n"
sleep 2

#Configuring Kernel
make mrproper

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

make omap_4430sdp_defconfig  2>&1 | tee defconfig_warnings.log
make -j4 uImage 2>&1 | tee uimage_warnings.log

echo "###################################################"
echo "#  ANALYSING WARNINGS ON: omap_4430sdp_defconfig  #"
echo "###################################################"
echo -e "\n"
sleep 2

#Checking for warnings / errors in defconfig

if [ $? -eq 0 ]
		then

			echo "=================================================="
			echo "Looking for warnings / errors in log file ..."
			echo "=================================================="
			echo -e "\n"
			sleep 1

			cat defconfig_warnings.log | grep -o 'warning' | wc -l > defconfig_warnings_number.log
			echo "NUMBER OF WARNINGS:"
			cat defconfig_warnings_number.log
			echo -e "\n"

			cat defconfig_warnings.log | grep -o 'error' | wc -l > defconfig_error_number.log
			echo "NUMBER OF ERRORS:"
			cat defconfig_error_number.log
			echo -e "\n"

		else
	               	echo "+++++++++++++++++++++++++++++++++++++++++"
	                echo "+    NO warnings / errors were found    +"
        	        echo "+++++++++++++++++++++++++++++++++++++++++"
			echo -e "\n"
			sleep 1
fi

echo "###################################################"
echo "#  ANALYSING WARNINGS ON: uImage compilation      #"
echo "###################################################"
echo -e "\n"
sleep 2

if [ $? -eq 0 ]
                then

                        echo "=================================================="
                        echo "     Looking for warnings in log file ...         "
                        echo "=================================================="
                        echo -e "\n"
                        sleep 1

			cat uimage_warnings.log | grep -o 'warning' | wc -l > uimage_warnings_number.log
			echo "NUMBER OF WARNINGS:"
			cat uimage_warnings_number.log
                        echo -e "\n"

                else
                        echo "+++++++++++++++++++++++++++++++++++++++++"
                        echo "+                 PASS 		      +"
			echo "+ 	NO warnings  were found       +"
                        echo "+++++++++++++++++++++++++++++++++++++++++"
                        echo -e "\n"
                        sleep 1

fi

# End L_DD_SWDEV_0001_0001
