#!/bin/bash
# ARGUMENTS--->
# device $1 (E.g./dev/mtd/3)
#SEC_START=\$TEST_$2
#SEC_OFF=\$$3
#START=\$TEST_$4
#OFFSET=\$$5
#LOCK=\$$6
#LOCKSEC=\$$7
if [ $# -lt 5 ]; then
	echo "Not enuf args"
	exit 1
fi
hexConvert()
{
	if [ -z "$1" ]; then
		echo "INVALID VALUE"
		exit 1
	fi
	VALUE=$(( $1 ))
	printf "0x%x" $VALUE
}
if [ "$DEFAULT_DEVICE" = "STRATA" ]; then
	#sector 0x40000
	SECTORSIZE_ONE=$SECTORSIZE_ONE_STRATA
	# Max NOR size
	MAX_SIZE=$MAX_SIZE_STRATA
	JFFS_FILE=$BASE/$STRATA_JFFS_FILE
	JFFS_OPTION="-j"
fi

if [ "$DEFAULT_DEVICE" = "SIBLEY" ]; then
	#sector 0x40000
	SECTORSIZE_ONE=$SECTORSIZE_ONE_SIB
	# Max NOR size
	MAX_SIZE=$MAX_SIZE_SIB
	JFFS_FILE=$BASE/$SIBLEY_JFFS_FILE
	JFFS_OPTION=""
fi
SECTORSIZE_TWO=$(( $SECTORSIZE_ONE * 2 ))
SECTORSIZE_THREE=$(( $SECTORSIZE_ONE * 3 ))
SECTORSIZE_FOUR=$(( $SECTORSIZE_ONE * 4 ))
#Sector Size
SECTORSIZE=`hexConvert "$SECTORSIZE_ONE"`
SECTORSIZE2=`hexConvert "$SECTORSIZE_TWO"`
#Sector Size -1
SECTORSIZE_1=`hexConvert "$SECTORSIZE_ONE - 1"`
SECTORSIZE_2=`hexConvert "$SECTORSIZE_ONE - 2"`
SECTORSIZE2_1=`hexConvert "$SECTORSIZE_TWO - 1"`
SECTORSIZE2_2=`hexConvert "$SECTORSIZE_TWO - 2"`
#Sector Size +1
SECTORSIZE_P=`hexConvert "$SECTORSIZE_ONE + 1"`
SECTORSIZE_P2=`hexConvert "$SECTORSIZE_ONE + 2"`
SECTORSIZE2_P=`hexConvert "$SECTORSIZE_TWO + 1"`
SECTORSIZE2_P2=`hexConvert "$SECTORSIZE_TWO + 2"`

# Test Sectors
TEST_X=`hexConvert "$SECTORSIZE_ONE "`
TEST_X_1=`hexConvert "$SECTORSIZE_ONE - 1"`
TEST_X_P=`hexConvert "$SECTORSIZE_ONE + 1"`
TEST_Y=`hexConvert "$SECTORSIZE_TWO"`
TEST_Y_1=`hexConvert "$SECTORSIZE_TWO - 1"`
TEST_Y_P=`hexConvert "$SECTORSIZE_TWO + 1"`
TEST_YD=`hexConvert "$SECTORSIZE_THREE"`
TEST_YD_1=`hexConvert "$SECTORSIZE_THREE - 1"`
TEST_YD_P=`hexConvert "$SECTORSIZE_THREE + 1"`
TEST_YP=`hexConvert "$SECTORSIZE_FOUR"`
TEST_YP_1=`hexConvert "$SECTORSIZE_FOUR - 1"`
TEST_YP_P=`hexConvert "$SECTORSIZE_FOUR + 1"`

DEBUG=$TESTBIN/mtd_debug
GENE=$TESTBIN/genfile
DIFF=$TESTBIN/differ

Erase(){
	$DEBUG erase $DEV $*
}

Write(){
	$DEBUG write $DEV $*
}

Read(){
	$DEBUG read $DEV $*
}

EraseAll(){
	$DEBUG eraseall $DEV $*
}

DEV=$1
eval SEC_START=\$TEST_$2
eval SEC_OFF=\$$3
eval START=\$TEST_$4
eval OFFSET=\$$5
eval LOCKME=$6
eval LOCKSEC=$7
set -x
BIN_UNLOCK=$TESTBIN/flash_unlock
BIN_LOCK=$TESTBIN/flash_lock
if [ $LOCKME = "lock" ]; then
	$BIN_LOCK $DEV $SEC_START $LOCKSEC
	RES=$?
	if [ $RES -ne 0 ]; then
		echo "Failed in Flash_lock $SEC_START $SEC_OFF"
		exit 0
	fi
else
	$BIN_UNLOCK $DEV $SEC_START $LOCKSEC
	RES=$?
	if [ $RES -ne 0 ]; then
		echo "Failed in flash_unlock $SEC_START $SEC_OFF"
		exit $RES
	fi
fi
# Do the test Run
Erase $SEC_START $SEC_OFF
RES=$?
if [ $RES -ne 0 ]; then
	echo "Failed in Erase $SEC_START $SEC_OFF"
	exit $RES
fi
$GENE $OFFSET 120 >$TMPFILE

Write $START $OFFSET $TMPFILE
RES=$?
if [ $RES -ne 0 ]; then
	echo "Failed in Write $START $OFFSET $TMPFILE"
	exit $RES
fi

rm -f  $TMPFILE.read.$$

Read $START $OFFSET $TMPFILE.read.$$
RES=$?
if [ $RES -ne 0 ]; then
	echo "FAILED IN Read $START $OFFSET $TMPFILE.read.$$"
	exit $RES
fi

#Check Diff
$DIFF $OFFSET $TMPFILE $TMPFILE.read.$$ >>$TMPFILE
RES=$?
rm -f $TMPFILE.* $TMPFILE
exit $RES
