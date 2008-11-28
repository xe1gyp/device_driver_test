# MAKE FILE FOR AUDIO TESTING
# Copyright (C) 2004-2006 Texas Instruments, Inc. 
# - Gsample Rev 1.5 Shift of data:

CC := $(CROSS_COMPILE)gcc

CFLAGS=-Wall -O2 -lm $(CEXTRAFLAGS)
BINARIES= timemeasure.o dspget player volumePlay recorder mixer mixerinfo fulldup sineGen t2_src_changer sineGen_i386 graph_helper evtest
TESTSUITE=scripts/
BIN=$(TESTSUITE)/bin
RAW=$(TESTSUITE)/raw_sin
SAMPLE_RATES="48000 44100 32000 24000 22050 16000 12000 11025 8000"

all: $(BINARIES)

install:
	make -C lib install
	cp -rf timemeasure.o dspget dspopen dspset player volumePlay recorder mixer mixerinfo fulldup sineGen t2_src_changer sineGen_i386 graph_helper evtest ../bin
	./gen_SinFFiles.sh

test_m: $(TESTSUITE)
	make -C $(TESTSUITE) CC=$(CC) CFLAGS="$(CFLAGS)" SAMPLERATES="$(SAMPLERATES)" BIN="../$(BIN)" RAW="../$(RAW)"

clean:
	rm -f $(TESTSUITE)/*dat* $(TESTSUITE)/test.raw 
	rm -f *.o *~ $(BINARIES) $(BIN)/* ;
	make clean -C lib; 
	#make clean -C $(TESTSUITE) RAW="../$(RAW)"

timemeasure.o: timemeasure.c
	$(CC) $(BASECFLAGS) -c timemeasure.c

fulldup: timemeasure.o fullDupTest.c
	${CC} $(CFLAGS) fullDupTest.c timemeasure.o -o fulldup

recorder: recorder.c timemeasure.o
	${CC} $(CFLAGS) recorder.c timemeasure.o -o recorder

evtest: evtest.c
	${CC} $(CFLAGS) evtest.c -o evtest

player: timemeasure.o player.c
	${CC} $(CFLAGS) player.c timemeasure.o -o player

t2_src_changer: t2_src_changer.c
	${CC} $(CFLAGS) t2_src_changer.c -o t2_src_changer

sineGen_i386: sineGen.c
	gcc $(CFLAGS) sineGen.c -o sineGen_i386

graph_helper:graph_helper.c
	gcc $(CFLAGS) graph_helper.c -o graph_helper
sineGen: sineGen.c
	${CC} $(CFLAGS) sineGen.c -o sineGen

volumePlay: volumePlay.c
	${CC} $(CFLAGS) volumePlay.c -o volumePlay

dspget: 
	make -C lib CC=$(CC) CFLAGS="$(CFLAGS)" BIN=$(BIN)

mixer: mixer.c
	$(CC) -o mixer mixer.c

mixerinfo: mixerinfo.c
	$(CC) -o mixerinfo mixerinfo.c
