TARGETS= backbeat maimparts partbeat

all: $(TARGETS)

install:
	@set -e; for i in $(TARGETS) ; do ln -f $$i ../../../bin/$$i ; done

clean:
