#!/bin/sh
if [ $# -ne 1 ]; then
	echo "require at least insert/remove as argument oh and I depend a lot on exported variables :("
	exit 2
fi
if [ "$1" = "insert" ]; then
	#Check module!!
	if [ ! -f "$MODDIR/$MODULE_NAME.$MODULE_EXT" ]; then
		echo "Module file: $MODDIR/$MODULE_NAME.$MODULE_EXT not found- check exported vars"
		exit 1;
	fi

	if [ -z "$I2C_SPEED0" ]; then
		I2C_SPEED0=$I2C_DEF_SPEED0
	fi

	if [ -z "$I2C_SPEED1" ]; then
		I2C_SPEED1=$I2C_DEF_SPEED1
	fi
	#insert
	#CMD="insmod $MODDIR/$MODULE_NAME.$MODULE_EXT $I2C_VAR_SPEED0=$I2C_SPEED0 $I2C_VAR_SPEED1=$I2C_SPEED1"
	#echo "$MODDIR/$MODULE_NAME.$MODULE_EXT $I2C_VAR_SPEED0=$I2C_SPEED0 $I2C_VAR_SPEED1=$I2C_SPEED1"
	CMD="insmod $MODDIR/$MODULE_NAME.$MODULE_EXT"
	echo "$MODDIR/$MODULE_NAME.$MODULE_EXT"
else
	#remove
	CMD="rmmod $MODULE_NAME"
	echo "rmmod $MODULE_NAME"
fi
#execute me..
$CMD
RET=$?
if [ $RET -eq 0 ]; then
	echo "Success"
else
	echo "Failed!!!"
fi
exit $RET
