all: irpsion5

clean:
	rm -f *.o *~ core

distclean:
	rm -f irpsion5 *.o *~ core

install:
	install irpsion5 $(ROOT)/usr/bin

CFLAGS += -g -I../include -Wall -Wstrict-prototypes $(RPM_OPT_FLAGS)

irpsion5: irpsion5.c
