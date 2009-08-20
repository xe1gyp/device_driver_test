CC := $(CROSS_COMPILE)gcc

TARGETS=pan bump scanner
all:$(TARGETS)

pan:
	$(CC) $(CFLAGS) pan.c zoolib.c splitstr.c -o pan

bump:
	$(CC) $(CFLAGS) bump.c zoolib.c -o bump

scanner:
	$(CC) $(CFLAGS) scan.c scanner.c reporter.c tag_report.c symbol.c splitstr.c debug.c -o scanner

clean:
	rm -f *.o $(TARGETS)

install:
	cp $(TARGETS) ../bin/
