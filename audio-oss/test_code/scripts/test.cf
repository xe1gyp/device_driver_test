###############
# Sample Configuration Script
# Rename as test.cf to use
###############
# if we have a working rtc, this is good.. else use next line
export POSTFIX=`date "+%Y%m%d-%H%M%S"`
#export POSTFIX=$$
# Load up the defaults
export HOMEDIR=${PWD}
export TESTROOT=${PWD}
export TESTBIN=${PWD}/../bin
export TESTSCRIPT=${PWD}/scripts
export TMPBASE=${TESTROOT}/tmp
export TMPFILE=${TMPBASE}/tmp.$POSTFIX
export CMDFILE=cmd.$POSTFIX
export TESTDIR=${TESTROOT}/test
export PRETTY_PRT=""
export VERBOSE=""
export OUTPUTFILE=${TESTROOT}/output.$POSTFIX
export LOGFILE=${TESTROOT}/log.$POSTFIX
export DURATION="1"
export PATH="${PATH}:${TESTROOT}:${TESTBIN}:${TESTSCRIPT}"
export TC_SCENARIO="${TESTROOT}/scenarios"
export SCENARIO_NAMES=""
export DSP_DEVICE="/dev/sound/dsp"
export MIXER_DEVICE="/dev/sound/mixer"
export PLAY_VOLUME="1"
export RECORD_VOLUME="25700"
export CHANNELS="1 2"
export FORMATS="S16_LE S32_LE"
# bit size 4096 is actually 32 bit..
export BIT_SIZES="16 4096"
#export SAMPLE_RATES="48000 44100 32000 24000 22050 16000 12000 11025 8000"
export REDUCED_RATES="48000 44100 16000 8000"
export BUFFER_SIZES="4 1024 2047 4096 8000 65536 65540"
export REDUCED_SIZES="1024 8000"
#Mixer devices
#export MIXER_DEVICES_L="vol line rec line1"
export MIXER_DEVICES_L="vol rec"
#export MIXER_DEVICES_R="vol rec line1"
export MIXER_DEVICES_R="vol rec"
export MIXER_LESSBADDEVICES="speaker mic phout"
export MIXER_BADDEVICES="bass line2"
export VOL_CHECK="0 1 11 24 55 67 72 100"
export VOL_UNCHECK="-1 101 256 65532"
# Update this with proper values
export TESTMP3=${TESTROOT}/test.mp3
export INPUT_EVENT=/dev/input/event0
export UTILBIN=$HOMEDIR/../../utils/bin

if [ `uname -r` = "2.4.20_mvlcee31-omap2420_gsm_gprs" ]; then
	export MMC_DEVICE=/dev/mmc/part1
else
	# 2.6 kernel stuff
	if [ -d /dev/mmc/blk0 ]; then
		export MMC_DEVICE=/dev/mmc/blk0/part1
	else
		export MMC_DEVICE=/dev/mmcblk0p1
	fi
fi
