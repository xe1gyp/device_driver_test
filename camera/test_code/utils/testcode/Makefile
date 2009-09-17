#/* ============================================================================
#             Texas Instruments OMAP(TM) Platform Software
#  (c) Copyright Texas Instruments, Incorporated.  All Rights Reserved.
#
#  Use of this software is controlled by the terms and conditions found 
#  in the license agreement under which this software has been supplied.
# ==============================================================================*/

CC := $(CROSS_COMPILE)gcc
INC := $(KDIR)/include

objects := af_stream burst_mode burst_mode_120 burst_zoom capture flash_driver \
	   h3a_stream hist_test multi_open prev_wrap process_cam rsz_vbuff \
	   streaming_abs streaming_frame \
	   streaming_to_video_file streaming_zoom

all: $(objects)

install:
	@echo "Installing test applications in bin directory..."
	@if test -d bin; then rm -rf bin/; fi
	@mkdir -p bin
	@cp -f $(objects) bin/
	@cp bin/* ../../bin/
	@echo "Done!"

af_stream: af_stream.c lib.c ioctl.c framerateLib.c
	@echo "  CC  af_stream"
	@$(CC) -g af_stream.c lib.c ioctl.c framerateLib.c -o af_stream

burst_mode: burst_mode.c lib.c ioctl.c framerateLib.c
	@echo "  CC  burst_mode"
	@$(CC) -I$(INC) -g burst_mode.c lib.c ioctl.c framerateLib.c -o burst_mode

burst_mode_120: burst_mode_120.c lib.c ioctl.c framerateLib.c
	@echo "  CC  burst_mode_120"
	@$(CC) -g burst_mode_120.c lib.c ioctl.c framerateLib.c -o burst_mode_120

burst_zoom: burst_zoom.c lib.c ioctl.c
	@echo "  CC  burst_zoom"
	@$(CC) -g burst_zoom.c lib.c ioctl.c -o burst_zoom

# this program is from V4L2 site with a bug fix for user ptr mode.
capture: capture.c
	@echo "  CC  capture"
	@$(CC) -g capture.c -o capture

flash_driver: flash_driver.c lib.c ioctl.c
	@echo "  CC  flash_driver"
	@$(CC) -g flash_driver.c lib.c ioctl.c -o flash_driver

h3a_stream: h3a_stream.c lib.c ioctl.c framerateLib.c
	@echo "  CC  h3a_stream"
	@$(CC) -g h3a_stream.c lib.c ioctl.c framerateLib.c -o h3a_stream
	
hist_test: hist_test.c lib.c ioctl.c framerateLib.c
	@echo "  CC  hist_test"
	@$(CC) -g hist_test.c lib.c ioctl.c framerateLib.c -o hist_test

multi_open: multi_open.c lib.c framerateLib.c ioctl.c
	@echo "  CC  multi_open"
	@$(CC) -g multi_open.c lib.c framerateLib.c ioctl.c -o multi_open

prev_wrap: prev_wrap.c
	@echo "  CC  prev_wrap"
	@$(CC) -g prev_wrap.c -o prev_wrap

process_cam: process_cam.c framerateLib.c lib.c ioctl.c
	@echo "  CC  process_cam"
	@$(CC) -g process_cam.c framerateLib.c lib.c ioctl.c -o process_cam

rsz_vbuff: rsz_vbuff.c
	@echo "  CC  rsz_vbuff"
	@$(CC) -g rsz_vbuff.c -o rsz_vbuff

streaming_abs: streaming_abs.c framerateLib.c lib.c ioctl.c
	@echo "  CC  streaming_abs"
	@$(CC) -g streaming_abs.c framerateLib.c lib.c ioctl.c -o streaming_abs

streaming_frame: streaming_frame.c framerateLib.c lib.c ioctl.c
	@echo "  CC  streaming_frame"
	@$(CC) -g streaming_frame.c framerateLib.c lib.c ioctl.c -o streaming_frame

streaming_to_video_file: streaming_to_video_file.c lib.c ioctl.c framerateLib.c
	@echo "  CC  streaming_to_video_file"
	@$(CC) -I$(INC) -g streaming_to_video_file.c lib.c ioctl.c framerateLib.c -o streaming_to_video_file

streaming_zoom: streaming_zoom.c lib.c ioctl.c framerateLib.c
	@echo "  CC  streaming_zoom"
	@$(CC) -g streaming_zoom.c lib.c ioctl.c framerateLib.c -o streaming_zoom

clean:
	@echo "Cleaning up objects..."
	@rm -f $(objects) *~ *.o *.bak
	@rm -rf bin/
	@echo "Done!"
