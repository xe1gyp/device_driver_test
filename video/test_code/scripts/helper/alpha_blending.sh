#!/bin/sh

ALPHA_MODE=$1
OUTPUT=$2
GLOBAL_ALPHA_GFX=$3
GLOBAL_ALPHA_VID2=$4
ALPHA_STATUS=$5
GRAPHICS_ENTRY=/sys/class/display_control/omap_disp_control/graphics
VIDEO1_ENTRY=/sys/class/display_control/omap_disp_control/video1
VIDEO2_ENTRY=/sys/class/display_control/omap_disp_control/video2
TV_ALPHA_ENTRY=/sys/class/display_control/omap_disp_control/tv_alphablend
LCD_ALPHA_ENTRY=/sys/class/display_control/omap_disp_control/lcd_alphablend
GFX_ALPHA_ENTRY=/sys/class/display_control/omap_disp_control/gfx_global_alpha
VID2_ALPHA_ENTRY=/sys/class/display_control/omap_disp_control/vid2_global_alpha

#Testing Global Alpha Blending
if [ "$ALPHA_MODE" = "GLOBAL" ];then

	#Saving the previous status for graphics, video1 and video2 sys entries
	PREVIOUS_GFX=`cat $GRAPHICS_ENTRY`
	PREVIOUS_VIDEO1=`cat $VIDEO1_ENTRY`
	PREVIOUS_VIDEO2=`cat $VIDEO2_ENTRY`

	# Configuring the test scenario for LCD
	if [ "$OUTPUT" = "LCD" ];then
		echo lcd > $GRAPHICS_ENTRY
		echo lcd > $VIDEO1_ENTRY
		echo lcd > $VIDEO2_ENTRY
		echo SENT GFX, VIDEO1 AND VIDEO 2 TO LCD
		sleep 3
		# Changing Framebuffer configuration
		$TESTSCRIPT/fbmode 32; sleep 2
		$TESTSCRIPT/fbout 0; sleep 1
	fi
	
	# Configuring the test scenario for TV
	if [ "$OUTPUT" = "TV" ];then
	        echo tv > $GRAPHICS_ENTRY
	        echo tv > $VIDEO1_ENTRY
	        echo tv > $VIDEO2_ENTRY
		echo SENT GFX, VIDEO1 AND VIDEO 2 TO TV
		sleep 3
		# Changing Framebuffer configuration
		$TESTSCRIPT/fbmode 32; sleep 2
		$TESTSCRIPT/fbout 1; sleep 1
	fi

	# Changing the framebuffer color
        cat $VIDEOFILES/argb32.out > /dev/fb0; sleep 2

	if [ "$ALPHA_STATUS" = "ON" ];then
		if [ "$OUTPUT" = "TV" ];then
			# Eneabling alphablending for TV output device.
			echo on > $TV_ALPHA_ENTRY
		fi
		if [ "$OUTPUT" = "LCD" ];then
			# Eneabling alphablending for LCD output device.
			echo on > $LCD_ALPHA_ENTRY
		fi
		echo ALPHA BLENDING ENABLED
	elif [ "$ALPHA_STATUS" = "OFF" ];then
		if [ "$OUTPUT" = "TV" ];then
			# Disabling alphablending for TV output device.
                        echo off > $TV_ALPHA_ENTRY
                fi
                if [ "$OUTPUT" = "LCD" ];then
			# Disabling alphablending for LCD output device.
                        echo off > $LCD_ALPHA_ENTRY
                fi

		echo ALPHA BLENDING DISABLED
	fi

	# Changing the framebuffer transparency
	echo $GLOBAL_ALPHA_GFX > $GFX_ALPHA_ENTRY
	# Changing the video transperency
	echo $GLOBAL_ALPHA_VID2 > $VID2_ALPHA_ENTRY

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
	
	# Reseting to the previous values.
	if [ "$OUTPUT" = "TV" ];then
		 echo off > $TV_ALPHA_ENTRY
		 $TESTSCRIPT/fbout 0
	else
		echo off > $LCD_ALPHA_ENTRY
	fi

	echo "$PREVIOUS_GFX" > $GRAPHICS_ENTRY
	echo "$PREVIOUS_VIDEO2" > $VIDEO2_ENTRY
	echo "$PREVIOUS_VIDEO1" > $VIDEO1_ENTRY

	sleep 3

fi

if [ -z "$STRESS" ]; then
	strees_messages.sh
fi

