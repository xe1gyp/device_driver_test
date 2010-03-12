TARGETS= backbeat maimparts partbeat

all: $(TARGETS)

install:
	@set -e; for i in $(TARGETS) ; do ln -f $$i $(SUITE_DIR)/bin/$$i ; done

clean:
