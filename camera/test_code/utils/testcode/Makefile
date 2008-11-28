#/* ============================================================================
#             Texas Instruments OMAP(TM) Platform Software
#  (c) Copyright Texas Instruments, Incorporated.  All Rights Reserved.
#
#  Use of this software is controlled by the terms and conditions found 
#  in the license agreement under which this software has been supplied.
# ==============================================================================*/

CC := $(CROSS_COMPILE)gcc

objects := showinfo streaming burst_mode hist_test\
streaming_to_video_file streaming_zoom burst_zoom capture\
h3a_stream rsz_vbuff prev_wrap streaming_frame Test_Case_ccdc5\
Test_Case_preview5 Test_Case_tables5 streaming_to_video \
multi_open af_stream streaming_zoom_factor streaming_abs \
burst_mode_120 process_cam

#Excluded (No longer supported 04-21-2008) read read_burst overlay read_raw\
 overlayZoom overlay_h3a contrast brigthness  crop crop2 capture overlay_h3a.c\
 read_raw2.c streaming_read framerate 

all: $(objects)

install:
	cp -fv $(objects) ../../bin/

showinfo: showinfo.c lib.c
	$(CC) -g showinfo.c lib.c -o showinfo

read: read.c lib.c
	$(CC) -g read.c lib.c -o read

ioctl2: ioctl2.c lib.c
	$(CC) -g ioctl2.c lib.c -o ioctl2

ioctl: ioctl.c lib.c
	$(CC) -g ioctl.c lib.c -o ioctl

overlay: overlay.c lib.c
	$(CC) -g overlay.c lib.c -o overlay

streaming: streaming.c lib.c ioctl.c
	$(CC) -g streaming.c lib.c ioctl.c -o streaming

framerate: framerate.c lib.c
	$(CC) -g framerate.c lib.c -o framerate

streaming_to_video_file: streaming_to_video_file.c lib.c ioctl.c framerateLib.c
	$(CC) -g streaming_to_video_file.c lib.c ioctl.c framerateLib.c -o streaming_to_video_file

read_raw: read_raw.c lib.c
	$(CC) -g read_raw.c lib.c -o read_raw

streaming_read: streaming_read.c lib.c ioctl.c
	$(CC) -g streaming_read.c lib.c ioctl.c -o streaming_read

overlayZoom: overlayZoom.c lib.c
	$(CC) overlayZoom.c lib.c -o overlayZoom

burst_mode: burst_mode.c lib.c ioctl.c framerateLib.c
	$(CC) -g burst_mode.c lib.c ioctl.c framerateLib.c -o burst_mode

overlay_h3a: overlay_h3a.c lib.c
	$(CC) -g overlay_h3a.c lib.c -o overlay_h3a

hist_stream: hist_stream.c lib.c
	$(CC) -g hist_stream.c lib.c -o hist_stream

hist_test: hist_test.c lib.c ioctl.c framerateLib.c
	$(CC) -g hist_test.c lib.c ioctl.c framerateLib.c -o hist_test

rsz_vbuff: rsz_vbuff.c rsz_vbuff.h
	$(CC) -g rsz_vbuff.c rsz_vbuff.h -o rsz_vbuff

prev_wrap: prev_wrap.c prev_wrap.h
	$(CC) -g prev_wrap.c prev_wrap.h -o prev_wrap

# this program is from V4L2 site with a bug fix for user ptr mode. 
capture: capture.c
	$(CC) -g capture.c -o capture

h3a_stream: h3a_stream.c lib.c ioctl.c framerateLib.c
	$(CC) -g h3a_stream.c lib.c ioctl.c framerateLib.c -o h3a_stream
	
streaming_zoom: streaming_zoom.c lib.c ioctl.c framerateLib.c
	$(CC) -g streaming_zoom.c lib.c ioctl.c framerateLib.c -o streaming_zoom
	
af_stream: af_stream.c lib.c ioctl.c framerateLib.c
	$(CC) -g af_stream.c lib.c ioctl.c framerateLib.c -o af_stream

burst_zoom: burst_zoom.c lib.c ioctl.c
	$(CC) -g burst_zoom.c lib.c ioctl.c -o burst_zoom

streaming_frame: streaming_frame.c framerateLib.c lib.c ioctl.c
	$(CC) -g streaming_frame.c framerateLib.c lib.c ioctl.c -o streaming_frame

Test_Case_ccdc5: Test_Case_ccdc5.c lib.c ioctl.c
	$(CC) -g Test_Case_ccdc5.c lib.c ioctl.c -o Test_Case_ccdc5

Test_Case_preview5: Test_Case_preview5.c lib.c ioctl.c
	$(CC) -g Test_Case_preview5.c lib.c ioctl.c -o Test_Case_preview5

Test_Case_tables5: Test_Case_tables5.c lib.c ioctl.c
	$(CC) -g Test_Case_tables5.c lib.c ioctl.c -o Test_Case_tables5

Test_Case_ccdc: Test_Case_ccdc.c lib.c ioctl.c
	$(CC) -g Test_Case_ccdc.c lib.c ioctl.c -o Test_Case_ccdc5

Test_Case_preview: Test_Case_preview.c lib.c ioctl.c
	$(CC) -g Test_Case_preview.c lib.c ioctl.c -o Test_Case_preview5

Test_Case_tables: Test_Case_tables.c lib.c ioctl.c
	$(CC) -g Test_Case_tables.c lib.c ioctl.c -o Test_Case_tables5

streaming_to_video: streaming_to_video.c lib.c
	$(CC) -g streaming_to_video.c lib.c -o streaming_to_video

streaming_zoom_frame: streaming_zoom_frame.c lib.c framerateLib.c ioctl.c
	$(CC) -g streaming_zoom_frame.c lib.c framerateLib.c ioctl.c -o streaming_zoom_frame

multi_open: multi_open.c ioctl.c lib.c framerateLib.c
	$(CC) -g multi_open.c ioctl.c lib.c framerateLib.c -o multi_open

streaming_zoom_factor: streaming_zoom_factor.c ioctl.c lib.c framerateLib.c
	$(CC) -g streaming_zoom_factor.c ioctl.c lib.c framerateLib.c -o streaming_zoom_factor

h3a_stream_debug: h3a_stream_debug.c lib.c ioctl.c framerateLib.c
	$(CC) -g h3a_stream_debug.c lib.c ioctl.c framerateLib.c -o h3a_stream_debug

streaming_abs: streaming_abs.c ioctl.c framerateLib.c absLayer.h lib.c
	$(CC) -g streaming_abs.c ioctl.c framerateLib.c absLayer.h lib.c -o streaming_abs

burst_mode_120: burst_mode_120.c ioctl.c framerateLib.c absLayer.h lib.c
	$(CC) -g burst_mode_120.c ioctl.c framerateLib.c absLayer.h lib.c -o burst_mode_120

process_cam: process_cam.c lib.c ioctl.c framerateLib.c
	$(CC) -g process_cam.c lib.c ioctl.c framerateLib.c -o process_cam

clean:
	rm -f $(objects) *~ *.o *.bak
