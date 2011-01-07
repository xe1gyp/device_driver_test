CC := $(CROSS_COMPILE)gcc
CFLAGS+= -I$(KDIR)/include -I$(KDIR)/arch/arm/include/
OBJS := showinfo setimg setwin setcrop streaming streaming_tiler setlink setrotation setmirror \
setbgcolor settransck enablecolkey setcolorconv setdefcolcon streaming_process \
tv_detect_app createRGB streaming_usrptr v4l2_rolling_bars streaming_tear v4l2_ramdomcolorblocks \
streaming_repeat \

ifeq ($(TARGET_PLATFORM), OMAP_4430)
	OBJS += streaming_wb
endif


all: $(OBJS)

showinfo: showinfo.c lib.c
	$(CC) $(CFLAGS) -g -static showinfo.c lib.c -o showinfo

setimg: setimg.c
	$(CC) $(CFLAGS) -g setimg.c lib.c -o setimg

setwin: setwin.c
	$(CC) $(CFLAGS) -g setwin.c lib.c -o setwin

setcrop: setcrop.c
	$(CC) $(CFLAGS) -g setcrop.c lib.c -o setcrop

streaming: streaming.c
	$(CC) $(CFLAGS) -g streaming.c lib.c -o streaming

streaming_tiler: streaming_tiler.c
	$(CC) $(CFLAGS) -g streaming_tiler.c lib.c -o streaming_tiler

setlink: setlink.c
	$(CC) $(CFLAGS) -g setlink.c lib.c -o setlink

setrotation: setrotation.c
	$(CC) $(CFLAGS) -g setrotation.c lib.c -o setrotation

setmirror: setmirror.c
	$(CC) $(CFLAGS) -g setmirror.c lib.c -o setmirror

setbgcolor: setbgcolor.c
	$(CC) $(CFLAGS) -g setbgcolor.c lib.c -o setbgcolor

enablecolkey: enablecolkey.c
	$(CC) $(CFLAGS) -g enablecolkey.c lib.c -o enablecolkey

settransck: settransck.c
	$(CC) $(CFLAGS) -g settransck.c lib.c -o settransck

setcolorconv: setcolorconv.c
	$(CC) $(CFLAGS) -g setcolorconv.c lib.c -o setcolorconv

setdefcolcon: setdefcolcon.c
	$(CC) $(CFLAGS) -g setdefcolcon.c lib.c -o setdefcolcon

streaming_process: streaming_process.c
	$(CC) $(CFLAGS) -g streaming_process.c -o streaming_process

streaming_wb: streaming_wb.c
	$(CC) $(CFLAGS) -g streaming_wb.c lib.c -o streaming_wb

v4l2_rolling_bars: v4l2_rolling_bars.c
	$(CC) $(CFLAGS) -g v4l2_rolling_bars.c lib.c -o v4l2_rolling_bars

streaming_tear: streaming_tear.c
	$(CC) $(CFLAGS) -g streaming_tear.c lib.c -o streaming_tear

streaming_repeat: streaming_repeat.c
	$(CC) $(CFLAGS) -g streaming_repeat.c lib.c -o streaming_repeat

v4l2_ramdomcolorblocks: v4l2_ramdomcolorblocks.c
	$(CC) $(CFLAGS) -g v4l2_ramdomcolorblocks.c lib.c -o v4l2_ramdomcolorblocks

tv_detect_app: tv_detect_app.c lib.h
	$(CC) $(CFLAGS) -g $< -o $@

createRGB: createRGB.c
	$(CC) $(CFLAGS) -g createRGB.c	-o createRGB

streaming_usrptr: streaming_usrptr.c
	$(CC) $(CFLAGS) -g streaming_usrptr.c -o streaming_usrptr

install:
	
	cp -vf $(OBJS) $(SUITE_DIR)/bin/

clean:
	rm -f $(OBJS) *~ *.o
