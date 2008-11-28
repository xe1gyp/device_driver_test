
LOADLIBES += -lm
LFLAGS += -l
CFLAGS += -w 

TARGETS=pan bump scanner
all:$(TARGETS)

pan: pan.o zoolib.o splitstr.o


bump: bump.o zoolib.o


scanner: scan.o scanner.o reporter.o tag_report.o symbol.o splitstr.o debug.o


clean:
	rm -f *.o $(TARGETS)

install:
	cp $(TARGETS) ../bin/
