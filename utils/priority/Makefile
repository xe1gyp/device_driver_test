CC := $(CROSS_COMPILE)gcc

TARGETS=scheduler_priority_get scheduler_priority_set

all: $(TARGETS)

scheduler_priority_get:
	$(CC) $(CFLAGS) scheduler_priority_get.c -o scheduler_priority_get

scheduler_priority_set:
	$(CC) $(CFLAGS) scheduler_priority_set.c -o scheduler_priority_set

install:
	cp $(TARGETS) $(UTILS_BINARIES)

clean:
	rm -f *.o $(TARGETS)
