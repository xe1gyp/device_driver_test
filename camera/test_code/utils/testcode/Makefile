#/* ============================================================================
#             Texas Instruments OMAP(TM) Platform Software
#  (c) Copyright Texas Instruments, Incorporated.  All Rights Reserved.
#
#  Use of this software is controlled by the terms and conditions found 
#  in the license agreement under which this software has been supplied.
# ==============================================================================*/

CC := $(CROSS_COMPILE)gcc
CFLAGS += -I$(KDIR)/include -I$(KDIR)/arch/arm/plat-omap/include -I$(KDIR)/arch/arm/include

objects := af_stream af_stream_rel burst_mode burst_mode_120 \
	   burst_zoom capture flash_driver reszhq virtsenhq p2rhq prvhq\
	   h3a_stream hist_test multi_open prev_wrap process_cam rsz_vbuff \
	   streaming_abs streaming_frame i421_errata_af_stats\
	   streaming_to_video_file streaming_zoom fps snapshot sensor_modes

all: $(objects)

install:
	cp $(objects) ../../bin

af_stream: af_stream.c lib.c ioctl.c framerateLib.c
	@echo "  CC  af_stream"
	@$(CC) $(CFLAGS) -g af_stream.c lib.c ioctl.c framerateLib.c -o af_stream

af_stream_rel: af_stream_rel.c lib.c ioctl.c framerateLib.c
	@echo "  CC  af_stream_rel"
	@$(CC) $(CFLAGS) -g af_stream_rel.c lib.c ioctl.c framerateLib.c -o af_stream_rel

burst_mode: burst_mode.c lib.c ioctl.c framerateLib.c
	@echo "  CC  burst_mode"
	@$(CC) $(CFLAGS) -g burst_mode.c lib.c ioctl.c framerateLib.c -o burst_mode

burst_mode_120: burst_mode_120.c lib.c ioctl.c framerateLib.c
	@echo "  CC  burst_mode_120"
	@$(CC) $(CFLAGS) -g burst_mode_120.c lib.c ioctl.c framerateLib.c -o burst_mode_120

burst_zoom: burst_zoom.c lib.c ioctl.c
	@echo "  CC  burst_zoom"
	@$(CC) $(CFLAGS) -g burst_zoom.c lib.c ioctl.c -o burst_zoom

fps: fps.c lib.c ioctl.c framerateLib.c
	@echo "  CC  fps"
	@$(CC) $(CFLAGS) -g fps.c lib.c ioctl.c framerateLib.c -o fps

# this program is from V4L2 site with a bug fix for user ptr mode.
capture: capture.c
	@echo "  CC  capture"
	@$(CC) $(CFLAGS) -g capture.c -o capture

flash_driver: flash_driver.c lib.c ioctl.c
	@echo "  CC  flash_driver"
	@$(CC) $(CFLAGS) -g flash_driver.c lib.c ioctl.c -o flash_driver


i421_errata_af_stats: i421_errata_af_stats.c
	@echo "  CC  i421_errata_af_stats"
	@$(CC) $(CFLAGS) -g i421_errata_af_stats.c lib.c framerateLib.c -o i421_errata_af_stats

h3a_stream: h3a_stream.c lib.c ioctl.c framerateLib.c
	@echo "  CC  h3a_stream"
	@$(CC) $(CFLAGS) -g h3a_stream.c lib.c ioctl.c framerateLib.c -o h3a_stream
	
hist_test: hist_test.c lib.c ioctl.c framerateLib.c
	@echo "  CC  hist_test"
	@$(CC) $(CFLAGS) -g hist_test.c lib.c ioctl.c framerateLib.c -o hist_test

multi_open: multi_open.c lib.c framerateLib.c ioctl.c
	@echo "  CC  multi_open"
	@$(CC) $(CFLAGS) -g multi_open.c lib.c framerateLib.c ioctl.c -o multi_open

prev_wrap: prev_wrap.c
	@echo "  CC  prev_wrap"
	@$(CC) $(CFLAGS) -g prev_wrap.c -o prev_wrap

process_cam: process_cam.c framerateLib.c lib.c ioctl.c
	@echo "  CC  process_cam"
	@$(CC) $(CFLAGS) -g process_cam.c framerateLib.c lib.c ioctl.c -o process_cam

reszhq: reszhq.c cmd_options.c file_operations.c
	@echo "  CC  reszhq"
	@$(CC) $(CFLAGS) -g reszhq.c cmd_options.c file_operations.c -o reszhq

virtsenhq: virtsenhq.c cmd_options.c file_operations.c
	@echo "  CC  virtsenhq"
	@$(CC) $(CFLAGS) -g virtsenhq.c cmd_options.c file_operations.c -o virtsenhq

p2rhq: p2rhq.c cmd_options.c file_operations.c
	@echo "  CC  p2rhq"
	@$(CC) $(CFLAGS) -g p2rhq.c cmd_options.c file_operations.c -o p2rhq

prvhq: p2rhq.c cmd_options.c file_operations.c
	@echo "  CC  prvhq"
	@$(CC) $(CFLAGS) -g prvhq.c cmd_options.c file_operations.c -o prvhq

rsz_vbuff: rsz_vbuff.c
	@echo "  CC  rsz_vbuff"
	@$(CC) $(CFLAGS) -g rsz_vbuff.c -o rsz_vbuff

sensor_modes: sensor_modes.c
	@echo "  CC  sensor_modes"
	@$(CC) $(CFLAGS) -g sensor_modes.c -o sensor_modes

streaming_abs: streaming_abs.c framerateLib.c lib.c ioctl.c
	@echo "  CC  streaming_abs"
	@$(CC) $(CFLAGS) -g streaming_abs.c framerateLib.c lib.c ioctl.c -o streaming_abs

streaming_frame: streaming_frame.c framerateLib.c lib.c ioctl.c dss_utils.c
	@echo "  CC  streaming_frame"
	@$(CC) $(CFLAGS) -g streaming_frame.c framerateLib.c lib.c ioctl.c dss_utils.c -o streaming_frame

streaming_to_video_file: streaming_to_video_file.c lib.c ioctl.c framerateLib.c
	@echo "  CC  streaming_to_video_file"
	@$(CC) $(CFLAGS) -g streaming_to_video_file.c lib.c ioctl.c framerateLib.c -o streaming_to_video_file

streaming_zoom: streaming_zoom.c lib.c ioctl.c framerateLib.c
	@echo "  CC  streaming_zoom"
	@$(CC) $(CFLAGS) -g streaming_zoom.c lib.c ioctl.c framerateLib.c -o streaming_zoom

snapshot: snapshot.c snapshot_lsc.c framerateLib.c lib.c ioctl.c
	@echo "  CC  snapshot"
	@$(CC) $(CFLAGS) -g snapshot.c snapshot_lsc.c framerateLib.c lib.c ioctl.c -o snapshot

clean:
	@echo "Cleaning up objects..."
	@rm -f $(objects) *~ *.o *.bak
	@echo "Done!"
