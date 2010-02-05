TARGETS:=checkfs makefiles

all: $(TARGETS)

checkfs: checkfs.c Makefile common.h comm.o
	$(CC) -g -Wall checkfs.c comm.o -o checkfs

comm.o: comm.c Makefile
	$(CC) -g -Wall -c comm.c -o comm.o

makefiles: makefiles.c Makefile common.h
	$(CC) -g -Wall makefiles.c -o makefiles

clean:
	rm -f makefiles checkfs *~ *.o

binaries: $(TARGETS)
	cp $(TARGETS) $(BIN_DIR)
