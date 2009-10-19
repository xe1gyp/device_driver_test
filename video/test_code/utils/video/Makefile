CC := $(CROSS_COMPILE)gcc
OBJS := showinfo setimg setwin setcrop streaming setlink setrotation setmirror setbgcolor \
	settransck enablecolkey setcolorconv setdefcolcon streaming_process tv_detect_app

all: $(OBJS)

showinfo: showinfo.c lib.c
	$(CC) -g -static showinfo.c lib.c -o showinfo

setimg: setimg.c
	$(CC) $(CFLAGS) -g setimg.c lib.c -o setimg

setwin: setwin.c
	$(CC) $(CFLAGS) -g setwin.c lib.c -o setwin

setcrop: setcrop.c
	$(CC) $(CFLAGS) -g setcrop.c lib.c -o setcrop

streaming: streaming.c
	$(CC) $(CFLAGS) -g streaming.c lib.c -o streaming

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

tv_detect_app: tv_detect_app.c lib.h
	$(CC) -g $< -o $@

install:
	
	cp $(OBJS) ../../bin/

clean:
	rm -f $(OBJS) *~ *.o
