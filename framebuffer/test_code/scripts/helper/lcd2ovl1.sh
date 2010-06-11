echo "1" > /sys/devices/platform/omapdss/display1/enabled
sleep 1
echo "0" > /sys/devices/platform/omapdss/overlay1/enabled
echo "2lcd" > /sys/devices/platform/omapdss/overlay1/manager
echo "1" > /sys/devices/platform/omapdss/overlay1/enabled
sleep 1
echo "0" > /sys/devices/platform/omapdss/overlay1/enabled
echo "2lcd" > /sys/devices/platform/omapdss/overlay1/manager
echo "1" > /sys/devices/platform/omapdss/overlay1/enabled


sleep 2

