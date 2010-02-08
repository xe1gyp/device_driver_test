CC := arm-none-linux-gnueabi-gcc
OBJS := showinfo setimg setwin setcrop streaming streaming_tiler setlink setrotation setmirror \ 	setbgcolor settransck enablecolkey setcolorconv setdefcolcon streaming_process \ 	tv_detect_app

all: $(OBJS)

showinfo: showinfo.c lib.c
	$(CC) -g -static showinfo.c lib.c -o showinfo

setimg: setimg.c
	$(CC) -g setimg.c lib.c -o setimg

setwin: setwin.c
	$(CC) -g setwin.c lib.c -o setwin

setcrop: setcrop.c
	$(CC) -g setcrop.c lib.c -o setcrop

streaming: streaming.c
	$(CC) -g streaming.c lib.c -o streaming

streaming_tiler: streaming_tiler.c
	$(CC) -g streaming_tiler.c lib.c -o streaming_tiler

setlink: setlink.c
	$(CC) -g setlink.c lib.c -o setlink

setrotation: setrotation.c
	$(CC) -g setrotation.c lib.c -o setrotation

setmirror: setmirror.c
	$(CC) -g setmirror.c lib.c -o setmirror

setbgcolor: setbgcolor.c
	$(CC) -g setbgcolor.c lib.c -o setbgcolor

enablecolkey: enablecolkey.c
	$(CC) -g enablecolkey.c lib.c -o enablecolkey

settransck: settransck.c
	$(CC) -g settransck.c lib.c -o settransck

setcolorconv: setcolorconv.c
	$(CC) -g setcolorconv.c lib.c -o setcolorconv

setdefcolcon: setdefcolcon.c
	$(CC) -g setdefcolcon.c lib.c -o setdefcolcon

streaming_process: streaming_process.c
	$(CC) -g streaming_process.c -o streaming_process

tv_detect_app: tv_detect_app.c lib.h
	$(CC) -g $< -o $@

install:
	
	cp $(OBJS) ../../bin/

clean:
	rm -f $(OBJS) *~ *.o
