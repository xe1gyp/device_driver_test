# !/bin/sh

TV_STATUS=$1
OUTPUT=$2
SETIMG_PARAMETERS=$3
STREAMING_PARAMETERS=$4
SETWIN_PARAMETERS=$5

PREVIOUS_GFX=`cat /sys/class/display_control/omap_disp_control/graphics`
PREVIOUS_VD1=`cat /sys/class/display_control/omap_disp_control/video1`
PREVIOUS_VD2=`cat /sys/class/display_control/omap_disp_control/video2`
RESULT=0

if [ "$OUTPUT" = "LCD" ];then
	echo lcd > /sys/class/display_control/omap_disp_control/video2
	echo lcd > /sys/class/display_control/omap_disp_control/video1
	echo SENT GFX, VIDEO1 AND VIDEO 2 TO LCD
	sleep 3
fi

if [ "$OUTPUT" = "TV" ];then
	echo tv > /sys/class/display_control/omap_disp_control/video2
	echo tv > /sys/class/display_control/omap_disp_control/video1
	echo SENT GFX, VIDEO1 AND VIDEO 2 TO TV
	sleep 3
fi

# Usage: setimg <vid> <fmt> <width> <height>
$TESTBIN/setimg 1 $SETIMG_PARAMETERS
RESULT=`command_tracking.sh $RESULT $?`

$TESTBIN/setimg 2 $SETIMG_PARAMETERS
RESULT=`command_tracking.sh $RESULT $?`

if [ "$TV_STATUS" = "CONNECTED" ];then
	echo CONNECT THE TV TO S-VIDEO OUTPUT OF THE BOARD
	echo PRESS ANY KEY WHEN DONE
	if [ -z "$STRESS" ]; then
		$WAIT_KEY
	else
		sleep 2
	fi
	$TESTBIN/tv_detect_app &
	sleep 5
	# Usage: streaming <vid> <inputfile> [<n>]
	$TESTBIN/streaming 1 $STREAMING_PARAMETERS&
	sleep 10
	echo DISCONNECT THE TV FROM S-VIDEO OUTPUT OF THE BOARD
	sleep 15
fi

if [ "$TV_STATUS" = "DISCONNECTED" ];then
	echo DISCONNECT THE TV TO S-VIDEO OUTPUT OF THE BOARD
	echo PRESS ANY KEY WHEN DONE
        if [ -z "$STRESS" ]; then
                $WAIT_KEY
        else
		sleep 2
	fi
        $TESTBIN/tv_detect_app &
	sleep 3
        echo CONNECT THE TV TO S-VIDEO OUTPUT OF THE BOARD
	echo PRESS ANY KEY WHEN DONE
        if [ -z "$STRESS" ]; then
                $WAIT_KEY
	else
		sleep 2
        fi
        # Usage: streaming <vid> <inputfile> [<n>]
        $TESTBIN/streaming 1 $STREAMING_PARAMETERS&
	sleep 15
        echo DISCONNECT THE TV FROM S-VIDEO OUTPUT OF THE BOARD
	sleep 10
fi

echo "$PREVIOUS_GFX" > /sys/class/display_control/omap_disp_control/graphics
echo "$PREVIOUS_VD2" > /sys/class/display_control/omap_disp_control/video2
echo "$PREVIOUS_VD1" > /sys/class/display_control/omap_disp_control/video1
sleep 3

if [ -z "$STRESS" ]; then
	stress_message.sh
fi

exit $RESULT
