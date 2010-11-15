CC := $(CROSS_COMPILE)gcc
CFLAGS+=-g -O -Wall -I$(KDIR)

TARGETS = evtest

all: $(TARGETS)

install:
	cp -vf $(TARGETS) ../../bin

$(TARGETS):
	$(CC) $(CFLAGS) $@.c -o $@

clean:
	rm -vf $(TARGETS)
