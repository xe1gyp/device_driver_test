echo 1 > /sys/devices/platform/omapdss/manager0/alpha_blending_enabled

# Usage: setimg <vid> <fmt> <width> <height>
$TESTBIN/setimg 2 YUYV 176 144

# Usage: streaming <vid> <inputfile> [<n>]
$TESTBIN/streaming 2 $VIDEOFILES/video_qcif_yuv_75
RESULT=`command_tracking.sh $RESULT $?`

#Starting the pre-multiplied GFX data
$TESTBIN/pre_multialpha &

i=0

while [ $i -lt 40 ]
do
$TESTBIN/streaming 2 $VIDEOFILES/video_qcif_yuv_75
let i=i+1
done

echo 0 > /sys/devices/platform/omapdss/manager0/alpha_blending_enabled
