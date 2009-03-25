#!/bin/sh

OPERATION=$1
CACHE_SIZE=$2
RECORD_SIZE=$3
FILE_SIZE=$4

initialSetup () {

	test -d $DEFAULT_MOUNT_POINT || mkdir $DEFAULT_MOUNT_POINT
	echo && $TESTBIN/flash_eraseall $JFFS_OPTIONS $MTD_CHAR_DEV1 > /dev/null 2>&1
	mount -t jffs2 $MTD_BLK_DEV1 $DEFAULT_MOUNT_POINT
	mount | grep "$DEFAULT_MOUNT_POINT" && echo
	cd $DEFAULT_MOUNT_POINT

}

cleanUp () {

	cd /
	umount $DEFAULT_MOUNT_POINT && sync

}

getAverageThroughput () {
  TValue=`cat "${TMPBASE}/temp.${COUNTER}.results" | bc`
	if [ "$COUNTER" -eq "1" ] ; then
		TMinimun=$TValue
	fi
	isTrue=`echo "${TValue} < ${TMinimun}" | bc`
	if [ "$isTrue" -eq "1" ] ; then
		TMinimun=$TValue
	fi
	TTempAverage=`echo "${TValue}" | bc`
	TAverage=`echo "${TAverage}+${TTempAverage}" | bc`
}

getAverageCPUUtilization () {
	local FILE=$1
	local Space=$2
	local TMaximun=0
	n=0

  while read line
	do
		CTempAverage=`echo $line | bc`

		isTrue=`echo "$CTempAverage > $TMaximun" | bc`
		if [ "$isTrue" -eq 1 ] ; then
			TMaximun=$CTempAverage
		fi

		CAverage=`echo "scale = 2; ${CAverage}+${CTempAverage}"|bc`
		n=`echo "$n+1"|bc`
	done < $FILE

	echo "scale = 2;$CAverage/$n" | bc > ${TMPBASE}/cpu.result
	CPU_UTILIZATION=`cat ${TMPBASE}/cpu.result`
	if [ "$Space" == "User" ]; then
		CPUMaximunUser=$TMaximun
	else
		CPUMaximunSystem=$TMaximun
	fi

}

convertToMBPS () {
	local Value=$1
	MBits=`echo "scale = ${SCALE}; (${Value}/1000)*8" | bc`
}

# ==================
# Main
# ==================

export TValue=0
export TAverage=0
export TMinimun=0
export TTempAverage=0
export CValue=0
export CAverage=0
export CTempAverage=0
export MBits=0
export SCALE=5
export CPUUtilizationUser=0
export CPUUtilizationSystem=0
export CPUMaximunUser=0
export CPUMaximunSystem=0


for COUNTER in $(seq 1 $REPETITIONS);
do

	initialSetup

	if [ "$COUNTER" -eq "1" ]; then
		echo > $TMPBASE/temp.0.results
		echo " [ OPERATION=$OPERATION  |  CACHE_SIZE=256k  |  RECORD_SIZE=${RECORD_SIZE}  |  FILE_SIZE=${FILE_SIZE} ] " >> $TMPBASE/temp.0.results
		echo >> $TMPBASE/temp.0.results
		echo " -> List of Throughput " >> $TMPBASE/temp.0.results
		echo >> $TMPBASE/temp.0.results
	fi

	#echo " -> Sleep time : ${SLEEPTIME} second(s) " && sleep $SLEEPTIME
	top2 -d 1 -b | grep Cpu > ${TMPBASE}/cpu.usage &

	echo "-> PPC Tag <-"

	if [ "$FILE_SIZE" == "" ] ; then

		if [ "$OPERATION" == "reader" ] ; then
			echo " -> Executing : iozone -i1 -i0 -S $CACHE_SIZE -+u -t -r $RECORD_SIZE"
			$IOZONE -i1 -i0 -S $CACHE_SIZE -+u -t -r $RECORD_SIZE | grep Parent | grep "8 readers" | cut -d= -f2 | cut -dK -f1 >> $TMPBASE/temp.${COUNTER}.results
		else
			echo " -> Executing : iozone -i1 -i0 -S $CACHE_SIZE -+u -t -r $RECORD_SIZE"
			$IOZONE -i1 -i0 -S $CACHE_SIZE -+u -t -r $RECORD_SIZE | grep Parent | grep initial | cut -d= -f2 | cut -dK -f1 >> $TMPBASE/temp.${COUNTER}.results
		fi

	else

		if [ "$OPERATION" == "reader" ] ; then
			echo " -> Executing : iozone -i1 -i0 -S $CACHE_SIZE -+u -s $FILE_SIZE -t -r $RECORD_SIZE"
			$IOZONE -i1 -i0 -S $CACHE_SIZE -+u -s $FILE_SIZE -t -r $RECORD_SIZE | grep Parent | grep "8 readers" | cut -d= -f2 | cut -dK -f1 >> $TMPBASE/temp.${COUNTER}.results
		else
			echo " -> Executing : iozone -i1 -i0 -S $CACHE_SIZE -+u -s $FILE_SIZE -t -r $RECORD_SIZE"
			$IOZONE -i1 -i0 -S $CACHE_SIZE -+u -s $FILE_SIZE -t -r $RECORD_SIZE | grep Parent | grep initial | cut -d= -f2 | cut -dK -f1 >> $TMPBASE/temp.${COUNTER}.results
		fi

	fi

	echo "-> PPC Tag <-"

	killall top2
	#echo " -> Sleep time : ${SLEEPTIME} second(s) " && sleep $SLEEPTIME
	getAverageThroughput
	cat ${TMPBASE}/cpu.usage | sed -e 's/Cpu(s)://g' -e 's/[ ,a-z]//g' | cut -d% -f1 >> $TMPBASE/user.utilization
	cat ${TMPBASE}/cpu.usage | sed -e 's/Cpu(s)://g' -e 's/[ ,a-z]//g' | cut -d% -f2 >> $TMPBASE/system.utilization
	cleanUp

done

#echo >> $TMPBASE/temp.${COUNTER}.results
#echo " -> List of User CPU Utilization " >> $TMPBASE/temp.${COUNTER}.results
#echo >> $TMPBASE/temp.${COUNTER}.results
#cat $TMPBASE/user.utilization >> $TMPBASE/temp.${COUNTER}.results

#echo >> $TMPBASE/temp.${COUNTER}.results
#echo " -> List of System CPU Utilization " >> $TMPBASE/temp.${COUNTER}.results
#echo >> $TMPBASE/temp.${COUNTER}.results
#cat $TMPBASE/system.utilization >> $TMPBASE/temp.${COUNTER}.results

echo >> ${TMPBASE}/temp.${COUNTER}.results

# Throughput
TAverage=`echo "scale = ${SCALE}; ${TAverage}/${COUNTER}" | bc`
convertToMBPS $TAverage
echo " -> Avg. Throughput (Mbps)          : ${MBits}" >> $TMPBASE/temp.${COUNTER}.results
convertToMBPS $TMinimun
echo " -> Min. Throughput (Mbps)          : ${MBits}" >> $TMPBASE/temp.${COUNTER}.results

# User CPU Utilization
getAverageCPUUtilization $TMPBASE/user.utilization User
CPUUtilizationUser=$CPU_UTILIZATION
echo " -> Avg. CPU Utilization (%) User   : $CPUUtilizationUser" >> $TMPBASE/temp.${COUNTER}.results
echo " -> Max. CPU Utilization (%) User   : $CPUMaximunUser" >> $TMPBASE/temp.${COUNTER}.results
rm $TMPBASE/user.utilization

# System CPU Utilization
getAverageCPUUtilization $TMPBASE/system.utilization System
CPUUtilizationSystem=$CPU_UTILIZATION
echo " -> Avg. CPU Utilization (%) System : $CPUUtilizationSystem" >> $TMPBASE/temp.${COUNTER}.results
echo " -> Max. CPU Utilization (%) System : $CPUMaximunSystem" >> $TMPBASE/temp.${COUNTER}.results
echo >> $TMPBASE/temp.${COUNTER}.results
rm $TMPBASE/system.utilization

# End of file
