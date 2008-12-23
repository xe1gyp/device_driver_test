#!/bin/sh

VIDEO_PIPELINE=$1
SETIMG_PARAMETERS=$2

# Usage: setimg <vid> <fmt> <width> <height>
$TESTBIN/setimg $VIDEO_PIPELINE $SETIMG_PARAMETERS
