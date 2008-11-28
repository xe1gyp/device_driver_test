#!/bin/bash

if [ -z "$1" ] ; then
	LOCATION=/VID
else
	LOCATION="$1"
fi

STREAM_LOC=$LOCATION/streams

MOUNT=/bin/mount
UMOUNT=/bin/umount
MKDIR=/bin/mkdir
GREP=/bin/mkdir
DF=/bin/df
LS=ls

#video test bins
BRIGHTNESS=$LOCATION/brightness
FRAMERATE=$LOCATION/framerate
OVERLAY=$LOCATION/overlay
STREAMING=$LOCATION/streaming
YDISY=$LOCATION/ydis
CONTRAST=$LOCATION/contrast    
IOCTL=$LOCATION/ioctl
READ=$LOCATION/read
STREAMING_USER=$LOCATION/streaming_user

#TI created bins
LOG=$LOCATION/log

#Log files
RESULT_FILE=$LOCATION/cam-result
RESULT_ORG=$LOCATION/cam-result.$$
CAMDBG_FILE=$LOCATION/cam_dbgfile
CAMDBG_ORG=$LOCATION/cam_dbgfile.$$
CAMTMP_FILE=/tmp/tmpcam

TestResult() 
{
	if [ $1 = 0 ] ; then
		$LOG $RESULT_FILE PASS $2
	else
		$LOG $RESULT_FILE FAIL $2
	fi
}

TestBandC()
{

	if [ $1 = "B" ] ; then
		$OVERLAY >>$CAMDBG_FILE &
		sleep 1
		$BRIGHTNESS -3 >>$CAMDBG_FILE
		sleep 3
		$BRIGHTNESS -2 >>$CAMDBG_FILE
		sleep 3
		$BRIGHTNESS -1 >>$CAMDBG_FILE
		sleep 3
		$BRIGHTNESS 0 >>$CAMDBG_FILE
		sleep 3
		$BRIGHTNESS 1 >>$CAMDBG_FILE
		sleep 3
		$BRIGHTNESS 2 >>$CAMDBG_FILE
		sleep 3
		$BRIGHTNESS 3 >>$CAMDBG_FILE
		sleep 3
	elif [ $1 = "C" ] ; then
		$OVERLAY >> $CAMDBG_FILE &
		sleep 1
		$CONTRAST -2 >>$CAMDBG_FILE
		sleep 3
		$CONTRAST -1 >>$CAMDBG_FILE>>$CAMDBG_FILE
		sleep 3
		$CONTRAST 0 >>$CAMDBG_FILE
		sleep 3
		$CONTRAST 1 >>$CAMDBG_FILE
		sleep 3
		$CONTRAST 2 >>$CAMDBG_FILE
		sleep 3
	fi

}

#  RunCAMTest : size format read overlay streaming bright contrast
RunCAMTest()
{
	xcord=0
	ycord=0
	proc=
	while [ $# -ne 0 ] ; do
                case $# in
                 9)
                        ScenID=$1
                        shift
                        ;;
                 8)
                        TestID=$1
                        shift
                        ;;
                 7)
                        size=$1
                        shift
                        ;;
                 6)
                        format=$1
                        shift
                        ;;
                 5)
                        cread=$1
                        shift
                        ;;
                 4)
                        coverlay=$1
                        shift
                        ;;
                 3)
                        cstreaming=$1
                        shift
                        ;;
                 2)
                        cbrightness=$1
                        shift
                        ;;
                 1)
                        ccontrast=$1
                        shift
                        ;;
                esac
	done

	$LOG $RESULT_FILE SCEN $ScenID
        $LOG $RESULT_FILE START $TestID
	echo "---- $TestID -----" >>$CAMDBG_FILE
	
	$IOCTL $size $format $xcord $ycord >>$CAMDBG_FILE

	if [ $cread -eq 1 ] ; then
                $READ>>$CAMDBG_FILE
        fi
	if [ $coverlay -eq 1 ] ; then
		proc=$OVERLAY
                $OVERLAY >>$CAMDBG_FILE &
        fi
	if [ $cstreaming -eq 1 ] ; then
		proc=$STREAMING
                $STREAMING >>$CAMDBG_FILE &
        fi
	if [ $cbrightness -eq 1 ] ; then
		proc=$OVERLAY
		TestBandC B
	fi
	if [ $ccontrast -eq 1 ] ; then
		proc=$OVERLAY
		TestBandC C
	fi
	if [ $watch = "y" ] ; then
		sleep 5
	else
		sleep 1
	fi

	if [ $proc ] ; then
		PID=`ps -ef |grep $proc|head -1|sed -e "s/   */ /g"|cut -d' ' -f2`
		if [ $PID ] ; then
			kill -9 $PID
		fi
	fi

	$LOG $RESULT_FILE PASS $TestID
}

FrameRtTest()
{
	while [ $# -ne 0 ] ; do
                case $# in
                 5)
                        ScenID=$1
                        shift
                        ;;
                 4)
                        TestID=$1
                        shift
                        ;;
                 3)
                        size=$1
                        shift
                        ;;
                 2)
                        format=$1
                        shift
                        ;;
                 1)
                        rate=$1
                        shift
                        ;;
                esac
	done

	$LOG $RESULT_FILE SCEN $ScenID
        $LOG $RESULT_FILE START $TestID
	echo "---- $TestID -----" >>$CAMDBG_FILE
	
	$IOCTL $size $format $xcord $ycord >>$CAMDBG_FILE
	$FRAMERATE $rate
	time $STREAMING > $CAMTMP_FILE

	$LOG $RESULT_FILE PASS $TestID
}

#------------ Test start
set +x

if [ ! -f $BRIGHTNESS  -o ! -f $FRAMERATE  -o ! -f $OVERLAY  -o ! -f $STREAMING  -o ! -f $YDIS  -o ! -f $CONTRAST  -o ! -f $IOCTL  -o ! -f $READ  -o ! -f $STREAMING_USER ] ; then
	echo "Few of the binaries not present. Test case execs not possible"
	exit -1
else
	echo "Starting Camera tests"
fi

if [  -f $RESULT_FILE ] ; then
	mv $RESULT_FILE $RESULT_ORG
fi

if [  -f $CAMDBG_FILE ] ; then
	mv $CAMDBG_FILE $CAMDBG_ORG
fi

if [ $1 = "-i" ] ; then
	watch=y
else
	echo -n "Someone watching camera [y/n] : "
	read watch
fi

#-----------Prepare the logfile with info

$LOG $RESULT_FILE ADDFILE
$LOG $RESULT_FILE INIT CameraTests

#------------ RUN TESTS
#  RunCAMTest : scenID  TestcaseID  size format : read overlay streaming bright contrast
RunCAMTest L_DD_CAMERA_0002 L_DD_CAMERA_0002_0001 QQCIF RGB565 1 0 0 0 0
RunCAMTest L_DD_CAMERA_0002 L_DD_CAMERA_0002_0002 QQVGA RGB565 1 0 0 0 0
RunCAMTest L_DD_CAMERA_0002 L_DD_CAMERA_0002_0003 QCIF 	RGB565 1 0 0 0 0
RunCAMTest L_DD_CAMERA_0002 L_DD_CAMERA_0002_0004 QVGA 	RGB565 1 0 0 0 0
RunCAMTest L_DD_CAMERA_0002 L_DD_CAMERA_0002_0005 CIF 	RGB565 1 0 0 0 0
RunCAMTest L_DD_CAMERA_0002 L_DD_CAMERA_0002_0006 VGA 	RGB565 1 0 0 0 0


RunCAMTest L_DD_CAMERA_0003 L_DD_CAMERA_0003_0001 QQCIF YUYV 0 1 0 0 0
RunCAMTest L_DD_CAMERA_0003 L_DD_CAMERA_0003_0002 QQVGA YUYV 0 1 0 0 0
RunCAMTest L_DD_CAMERA_0003 L_DD_CAMERA_0003_0003 QCIF 	YUYV 0 1 0 0 0
RunCAMTest L_DD_CAMERA_0003 L_DD_CAMERA_0003_0004 QVGA 	YUYV 0 1 0 0 0
RunCAMTest L_DD_CAMERA_0003 L_DD_CAMERA_0003_0005 CIF 	YUYV 0 1 0 0 0
RunCAMTest L_DD_CAMERA_0003 L_DD_CAMERA_0003_0006 VGA 	YUYV 0 1 0 0 0


RunCAMTest L_DD_CAMERA_0004 L_DD_CAMERA_0004_0001 QQCIF	RGB565 0 0 1 0 0
RunCAMTest L_DD_CAMERA_0004 L_DD_CAMERA_0004_0002 QQVGA	RGB565 0 0 1 0 0
RunCAMTest L_DD_CAMERA_0004 L_DD_CAMERA_0004_0003 QCIF	RGB565 0 0 1 0 0
RunCAMTest L_DD_CAMERA_0004 L_DD_CAMERA_0004_0004 QVGA	RGB565 0 0 1 0 0
RunCAMTest L_DD_CAMERA_0004 L_DD_CAMERA_0004_0005 CIF	RGB565 0 0 1 0 0
RunCAMTest L_DD_CAMERA_0004 L_DD_CAMERA_0004_0006 VGA	RGB565 0 0 1 0 0

RunCAMTest L_DD_CAMERA_0005 L_DD_CAMERA_0005_0001 QCIF	YUYV 0 0 0 0 1
RunCAMTest L_DD_CAMERA_0005 L_DD_CAMERA_0005_0002 QCIF	YUYV 0 0 0 1 0

FrameRtTest L_DD_CAMERA_0007 L_DD_CAMERA_0007_0001 QCIF RGB565 10
FrameRtTest L_DD_CAMERA_0007 L_DD_CAMERA_0007_0002 QCIF RGB565 20
FrameRtTest L_DD_CAMERA_0007 L_DD_CAMERA_0007_0003 QCIF RGB565 25
FrameRtTest L_DD_CAMERA_0007 L_DD_CAMERA_0007_0004 QCIF RGB565 30


################## TV #############
$LOG $RESULT_FILE SCEN L_DD_CAMERA_0008
$LOG $RESULT_FILE START L_DD_CAMERA_0008_0001
"echo "tv" > /proc/drivers/display/video1"
"./ioctl QCIF YUYV 0 0"
"./overlay"

$LOG $RESULT_FILE SCEN L_DD_CAMERA_0008
$LOG $RESULT_FILE START L_DD_CAMERA_0008_0001
"echo "tv" > /proc/drivers/display/video1"
"./ioctl VGA YUYV 0 0"
"./overlay"

######## Power management tests ##############
L_DD_CAMERA_2001_0001
mount -t sysfs none /sys

$LOG $RESULT_FILE SCEN L_DD_CAMERA_2001
$LOG $RESULT_FILE START L_DD_CAMERA_2001_0001
"./ioctl QCIF RGB565 0 0"
"./streaming&"
"echo "suspend powerdown 0">/sys/devices/L4/omap24xxcam0/power"
"echo "resume poweron">/sys/devices/L4/omap24xxcam0/power"

$LOG $RESULT_FILE SCEN L_DD_CAMERA_2001
$LOG $RESULT_FILE START L_DD_CAMERA_2001_0002
"./ioctl QCIF YUYV 0 0"
"./overlay&"
"echo "suspend powerdown 0">/sys/devices/L4/omap24xxcam0/power"
"echo "resume poweron">/sys/devices/L4/omap24xxcam0/power"

$LOG $RESULT_FILE SCEN L_DD_CAMERA_2001
$LOG $RESULT_FILE START L_DD_CAMERA_2001_0004
"./ioctl QCIF YUYV 0 0"
"./overlay&"
"echo "suspend powerdown 0">/sys/devices/L4/omap24xxcam0/power"
"echo "suspend powerdown 0">/sys/devices/L4/omap24xxcam0/power"
"echo "resume poweron">/sys/devices/L4/omap24xxcam0/power"




