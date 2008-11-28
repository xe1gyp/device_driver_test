#!/bin/sh

ALPHA_MODE=$1
OUTPUT=$2
G_ALPHA_GFX=$3
G_ALPHA_VID2=$4
ALPHA_STATUS=$5

#$TESTSCRIPT/fbmode 32

if [ "$ALPHA_MODE" = "GLOBAL" ];then

	PREV_GFX=`cat /sys/class/display_control/omap_disp_control/graphics`
	PREV_V1=`cat /sys/class/display_control/omap_disp_control/video1`
	PREV_V2=`cat /sys/class/display_control/omap_disp_control/video2`

	if [ "$OUTPUT" = "LCD" ];then
		echo lcd > /sys/class/display_control/omap_disp_control/graphics
		echo lcd > /sys/class/display_control/omap_disp_control/video2
		echo lcd > /sys/class/display_control/omap_disp_control/video1
		echo SENT GFX, VIDEO1 AND VIDEO 2 TO LCD
		sleep 3
		$TESTSCRIPT/fbmode 32; sleep 2
		$TESTSCRIPT/fbout 0; sleep 1
	fi
	
	if [ "$OUTPUT" = "TV" ];then
	        echo tv > /sys/class/display_control/omap_disp_control/graphics
	        echo tv > /sys/class/display_control/omap_disp_control/video2
	        echo tv > /sys/class/display_control/omap_disp_control/video1
		echo SENT GFX, VIDEO1 AND VIDEO 2 TO TV
		sleep 3
		$TESTSCRIPT/fbmode 32; sleep 2
		$TESTSCRIPT/fbout 1; sleep 1
	fi

	cat $VIDEOFILES/argb32.out > /dev/fb0; sleep 2

	if [ "$ALPHA_STATUS" = "ON" ];then
		if [ "$OUTPUT" = "TV" ];then
			echo on > /sys/class/display_control/omap_disp_control/tv_alphablend
		fi
		if [ "$OUTPUT" = "LCD" ];then
			echo on > /sys/class/display_control/omap_disp_control/lcd_alphablend
		fi
		echo ALPHA BLENDING ENABLED
	elif [ "$ALPHA_STATUS" = "OFF" ];then
		if [ "$OUTPUT" = "TV" ];then
                        echo off > /sys/class/display_control/omap_disp_control/tv_alphablend
                fi
                if [ "$OUTPUT" = "LCD" ];then
                        echo off > /sys/class/display_control/omap_disp_control/lcd_alphablend
                fi

		echo ALPHA BLENDING DISABLED
	fi

	echo $G_ALPHA_GFX >  /sys/class/display_control/omap_disp_control/gfx_global_alpha
	echo $G_ALPHA_VID2 >  /sys/class/display_control/omap_disp_control/vid2_global_alpha

	#cat $VIDEOFILES/argb32.out > /dev/fb0; sleep 2

	$TESTBIN/setimg 1 YUYV 176 144
	$TESTBIN/setcrop 1 0 0 176 144
	$TESTBIN/setrotation 1 90
	$TESTBIN/setwin 1 0 0 176 144
	$TESTBIN/setimg 2 YUYV 176 144
	$TESTBIN/setcrop 2 0 0 176 144
	$TESTBIN/setrotation 2 0
	$TESTBIN/setwin 2 0 50 176 144
	$TESTBIN/streaming 1 $VIDEOFILES/video_qcif_yuv_75 &
	$TESTBIN/streaming 2 $VIDEOFILES/video_qcif_yuv_75
	
	if [ "$OUTPUT" = "TV" ];then
		$TESTSCRIPT/fbout 0
	fi
	echo off > /sys/class/display_control/omap_disp_control/lcd_alphablend
	echo "$PREV_GFX" > /sys/class/display_control/omap_disp_control/graphics
	echo "$PREV_V2" > /sys/class/display_control/omap_disp_control/video2
	echo "$PREV_V1" > /sys/class/display_control/omap_disp_control/video1

	sleep 3

fi

if [ -z "$STRESS" ]; then
	  echo "Was the video streamed correctly with the desired alpha options?"
	$WAIT_ANSWER
	ERR=$?
	if [ $ERR -eq 1 ]; then
		echo "FAIL"
		exit 1
	else
		echo "PASS"
	exit 0
	fi
fi

