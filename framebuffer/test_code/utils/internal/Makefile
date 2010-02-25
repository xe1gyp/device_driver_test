export CC:=$(CROSS_COMPILE)gcc

objects = fbcmap fbblank fbmode fbrotate \
fbmmap fbsize fbshift fbmirror fbout fbflipbut fbtest wait_for_vsync \
fb_mirror

all: fb_user_test_o liblog fb_user_tests $(objects)

install:
	cp $(objects) ../../bin
	cp fb_user_tests ../../bin
	cp fb_blank_stress.sh ../../bin
	cp fb_user_tests.o ../../bin
	cp fb_write_stress.sh ../../bin


fb_user_test_o: fb_user_tests.c
	$(CC) $(CFLAGS) -c fb_user_tests.c -o fb_user_tests.o

liblog:
	make -C ./libs/log

fb_user_tests: fb_user_tests.o libs/log/libtestlog.a
	$(CC) $(CFLAGS) fb_user_tests.o libs/log/libtestlog.a -o fb_user_tests

fbcmap: fb_general.c
	echo "#define FUNCTION_CMAP" > fb_general_config.h
	$(CC) $(CFLAGS) fb_general.c -o fbcmap

fbblank: fb_general.c
	echo "#define FUNCTION_BLANK" > fb_general_config.h
	$(CC) $(CFLAGS) fb_general.c -o fbblank

fbmode: fb_general.c
	echo "#define FUNCTION_BPP" > fb_general_config.h
	$(CC) $(CFLAGS) fb_general.c -o fbmode

fbrotate: fb_general.c
	echo "#define FUNCTION_ROTATE" > fb_general_config.h
	$(CC) $(CFLAGS) fb_general.c -o fbrotate

fbmmap: fb_general.c
	echo "#define FUNCTION_MMAP" > fb_general_config.h
	$(CC) $(CFLAGS) fb_general.c -o fbmmap

fbsize: fb_general.c
	echo "#define FUNCTION_SIZE" > fb_general_config.h
	$(CC) $(CFLAGS) fb_general.c -o fbsize

fbshift: fb_general.c
	echo "#define FUNCTION_SHIFT" > fb_general_config.h
	$(CC) $(CFLAGS) fb_general.c -o fbshift

fbmirror: fb_general.c
	echo "#define FUNCTION_MIRROR" > fb_general_config.h
	$(CC) $(CFLAGS) fb_general.c -o fbmirror

fbout: fb_general.c
	echo "#define FUNCTION_OUTPUTDEV" > fb_general_config.h
	$(CC) $(CFLAGS) fb_general.c -o fbout

fbflipbut: fb_general.c
	echo "#define FUNCTION_FLIPBUF" > fb_general_config.h
	$(CC) $(CFLAGS) fb_general.c -o fbflipbut

fbtest: fbtest.c
	$(CC) $(CFLAGS) fbtest.c -o fbtest

wait_for_vsync: wait_for_vsync.c
	$(CC) $(CFLAGS) wait_for_vsync.c -o wait_for_vsync

fb_mirror: fb_mirror.c
	$(CC) $(CFLAGS) fb_mirror.c -o fb_mirror

clean:
	rm -fr $(objects)
	make -C libs/log clean
	rm -fr fb_user_tests.o
	rm -fr fb_user_tests
	rm -fr fb_general_config.h
