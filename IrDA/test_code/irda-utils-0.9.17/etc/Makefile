CC = gcc $(RPM_OPT_FLAGS)
LD = ld
RM = rm -f

RM_CMD = $(RM) *.BAK *.bak *.o ,* *~ *.a

all:

clean:
	$(RM_CMD)

distclean:
	$(RM_CMD)

install:
	@./install-etc $(ROOT)
