#!/bin/sh

ALPHA_MODE=$1
OUTPUT=$2
GLOBAL_ALPHA_GFX=$3
GLOBAL_ALPHA_VID2=$4
ALPHA_STATUS=$5
NUM_BUFF=6
LCD=/sys/devices/platform/omapdss/display0
HDMI=/sys/devices/platform/omapdss/display2
ALPHA_ENABLED=/sys/devices/platform/omapdss/manager0/alpha_blending_enabled
GFX_ALPHA_ENTRY=/sys/devices/platform/omapdss/overlay0/global_alpha
VID2_ALPHA_ENTRY=/sys/devices/platform/omapdss/overlay2/global_alpha
RESULT=0

#Testing Global Alpha Blending
if [ "$ALPHA_MODE" = "GLOBAL" ];then

	# Configuring the test scenario for LCD
	if [ "$OUTPUT" = "LCD" ]; then
		echo 0 > $HDMI/enabled
		echo 1 > $LCD/enabled
		echo SENT GFX, VIDEO1 AND VIDEO 2 TO LCD
		sleep 3
		# Changing Framebuffer configuration
		$TESTSCRIPT/fbmode 32; sleep 2

	elif [ "$OUTPUT" = "HDMI" ];then
		echo 0 > $LCD/enabled
		echo 1 > $HDMI/enabled
		echo SENT GFX, VIDEO1 AND VIDEO 2 TO HDTV
		sleep 3
		# Changing Framebuffer configuration
                $TESTSCRIPT/fbmode 32; sleep 2
	fi

	# Changing the framebuffer color
	cat $VIDEOFILES/argb32.out > /dev/fb0; sleep 2

	if [ "$ALPHA_STATUS" = "ON" ]; then
		# Eneabling alphablending for TV device.
		echo 1 > $ALPHA_ENABLED
		echo ALPHA BLENDING ENABLED

	elif [ "$ALPHA_STATUS" = "OFF" ]; then
		# Disabling alphablending for LCD output device.
                echo 0 > $ALPHA_ENABLED
		echo ALPHA BLENDING DISABLED
	fi

	# Changing the framebuffer transparency
	echo $GLOBAL_ALPHA_GFX > $GFX_ALPHA_ENTRY
	# Changing the video transperency
	echo $GLOBAL_ALPHA_VID2 > $VID2_ALPHA_ENTRY

	$TESTBIN/setimg 1 YUYV 176 144
	RESULT=`command_tracking.sh $RESULT $?`

	$TESTBIN/setcrop 1 0 0 176 144
	RESULT=`command_tracking.sh $RESULT $?`

	$TESTBIN/setrotation 1 90
	RESULT=`command_tracking.sh $RESULT $?`
	$TESTBIN/setwin 1 0 0 176 144
	RESULT=`command_tracking.sh $RESULT $?`

	$TESTBIN/setimg 2 YUYV 176 144
	RESULT=`command_tracking.sh $RESULT $?`

	$TESTBIN/setcrop 2 0 0 176 144
	RESULT=`command_tracking.sh $RESULT $?`

	$TESTBIN/setrotation 2 0
	RESULT=`command_tracking.sh $RESULT $?`

	$TESTBIN/setwin 2 0 50 176 144
	RESULT=`command_tracking.sh $RESULT $?`

	$TESTBIN/streaming 1 $NUM_BUFF $VIDEOFILES/video_qcif_yuv_75 &
	$TESTBIN/streaming 2 $NUM_BUFF $VIDEOFILES/video_qcif_yuv_75
	RESULT=`command_tracking.sh $RESULT $?`
	
	if [ -z "$STRESS" ]; then
		stress_message.sh
	fi
fi
exit $RESULT
