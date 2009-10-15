#!/bin/sh

TESTCASEID=$1

case "$TESTCASEID" in
	# Transfer to Existing Device
	1)
		verifyspeed.sh && for i in $I2C_ADDRESSES; do i2cdump -y $I2C_ADAPTER_1 $i b || exit 1; done
		verifyspeed.sh && testrw.sh $I2C_ADAPTER_1 $I2C_REG_1 $I2C_REG_1_VALUE_INITIAL || exit 1
		verifyspeed.sh && for i in $I2C_ADDRESSES; do i2cdump -y $I2C_ADAPTER_1 $i b || exit 1; done
	;;
	# Transfer to Multiple Devices
	2)
		verifyspeed.sh && for i in $I2C_ADDRESSES; do i2cdump -y $I2C_ADAPTER_1 $i b || exit 1; done
		verifyspeed.sh && (testrw.sh $I2C_ADAPTER_1 $I2C_REG_1 $I2C_REG_1_VALUE_INITIAL &) && testrw.sh $I2C_ADAPTER_1 $I2C_REG_2 $I2C_REG_2_VALUE_INITIAL && wait.sh testrw.sh 1 || exit 1
		verifyspeed.sh && for i in $I2C_ADDRESSES; do i2cdump -y $I2C_ADAPTER_1 $i b || exit 1; done
	;;
	# Transfer to Non Existing Device
	3)
		verifyspeed.sh && for i in $I2C_ADDRESSES; do i2cdump -y $I2C_ADAPTER_1 $i b || exit 1; done
		verifyspeed.sh && (testrw.sh $I2C_ADAPTER_1 $I2C_REG_INVALID $I2C_REG_1_VALUE_INITIAL &) && testrw.sh $I2C_ADAPTER_1 $I2C_REG_INVALID $I2C_REG_2_VALUE_INITIAL && wait.sh testrw.sh 1 && exit 1
		verifyspeed.sh && for i in $I2C_ADDRESSES; do i2cdump -y $I2C_ADAPTER_1 $i b || exit 1; done
	;;
	# Transfer Cancellation to Existing Device
	4)
		verifyspeed.sh && for i in $I2C_ADDRESSES; do i2cdump -y $I2C_ADAPTER_1 $i b || exit 1; done
		verifyspeed.sh && ((testrw.sh $I2C_ADAPTER_1 $I2C_REG_1 $I2C_REG_1_VALUE_INITIAL &) && testrw.sh $I2C_ADAPTER_1 $I2C_REG_2 $I2C_REG_2_VALUE_INITIAL &) && sleep 1 && killall testrw.sh || exit 1
		verifyspeed.sh && for i in $I2C_ADDRESSES; do (i2cdump -y $I2C_ADAPTER_1 $i b &) && killall i2cdump || exit 1; done
		verifyspeed.sh && for i in $I2C_ADDRESSES; do i2cdump -y $I2C_ADAPTER_1 $i b || exit 1; done
	;;
	# Transfer using Interrupt Mode, the interrupts should be different due to i2c transfers
	5)
		verifyspeed.sh && for i in $I2C_ADDRESSES; do i2cdump -y $I2C_ADAPTER_1 $i b || exit 1; done
		verifyspeed.sh && echo `cat $PROCFS_INTERRUPTS | grep $INTERRUPT_PREFIX | grep $INT_24XX_I2C1_IRQ: | sed -e "s/ */ /g" | cut -d ' ' -f3` > $TEMP_FILE_1 || exit 1
		verifyspeed.sh && for i in $I2C_ADDRESSES; do i2cdump -y $I2C_ADAPTER_1 $i b || exit 1; done
		verifyspeed.sh && echo `cat $PROCFS_INTERRUPTS | grep $INTERRUPT_PREFIX | grep $INT_24XX_I2C1_IRQ: | sed -e "s/ */ /g" | cut -d ' ' -f3` > $TEMP_FILE_2 || exit 1
		verifyspeed.sh && cat $TEMP_FILE_1 && cat $TEMP_FILE_2; cmp $TEMP_FILE_1 $TEMP_FILE_2 && exit 1
		verifyspeed.sh && for i in $I2C_ADDRESSES; do i2cdump -y $I2C_ADAPTER_1 $i b || exit 1; done
	;;
	# Run a read and keep switching from one process to another
	6)
		verifyspeed.sh &&  for i in $I2C_ADDRESSES;do  $UTILS_DIR_HANDLERS/handlerCpuAffinity.sh "switch" "smp_i2cdump  -y -f  1 $i 5 b " "5" "15" || exit 1; done
	;;
	# Run two reads in parallel
	7)
		verifyspeed.sh && (($UTILS_DIR_HANDLERS/handlerCpuAffinity.sh "switch" "i2cdump  -y -f  1 0x48  b  0 0x4E 25" "5" "15"  &)   && $UTILS_DIR_HANDLERS/handlerCpuAffinity.sh "switch" "i2cdump  -y -f  1 0x48  b  0 0x4E 25" "5" "15"  &) && wait.sh handlerCpuAffinity.sh 5  || exit 1
	;;
	#Run the read and write in parallel
	8)
		verifyspeed.sh && (($UTILS_DIR_HANDLERS/handlerCpuAffinity.sh "switch" "i2cdump  -y -f  1 0x48  b  0 0x4E 25" "5" "15"  &) && $UTILS_DIR_HANDLERS/handlerCpuAffinity.sh "switch" "i2cset  -r  -y -f  1 0x48  0x2  0x3 b 0 50" "50" "15"   & ) && wait.sh handlerCpuAffinity.sh 5  || exit 1
	;;
	#Two writes in parallel
	9)
		verifyspeed.sh && (($UTILS_DIR_HANDLERS/handlerCpuAffinity.sh "switch" "i2cset  -r  -y -f  1 0x48  0x2  0x3 b 0 50" "5" "15" &) && $UTILS_DIR_HANDLERS/handlerCpuAffinity.sh "switch" "i2cset  -r  -y -f  1 0x48  0x2  0x3 b 0 50" "5" "15" &) && wait.sh handlerCpuAffinity.sh 5  || exit 1
	;;
	# Write while the CPU affinity is switched
	10)
                verifyspeed.sh &&  $UTILS_DIR_HANDLERS/handlerCpuAffinity.sh "switch" "i2cset  -r  -y -f  1 0x48  0x2  0x3 b 0 50" "5" "15" || exit 1
	;;
	11)
                verifyspeed.sh &&  $UTILS_DIR_HANDLERS/handlerIrqAffinity.sh  "switch" "i2cset  -r  -y -f  1 0x48  0x2  0x3 b 0 50" "5" "15" "88" || exit 1
	;;
	12)
                verifyspeed.sh &&  $UTILS_DIR_HANDLERS/handlerIrqAffinity.sh "switch" "i2cdump  -y -f  1 0x48  b  0 0x4E 25" "5" "15" "88" || exit 1
	;;
	*)
		echo "No Test Case ID Found!";
		return 1
	;;
esac

# End of file
