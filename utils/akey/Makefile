# Monterrey LBP Team - Test Team
# Misael Lopez Cruz
# Compile akey utility
CC := $(CROSS_COMPILE)gcc

TARGETS=akey waitkey choice

all: $(TARGETS)

akey:
	$(CC) $(CFLAGS) akey.c -o akey

waitkey:
	$(CC) $(CFLAGS) waitkey.c -o waitkey

choice:
	$(CC) $(CFLAGS) choice.c -o choice

clean:
	rm -f $(BIN_DIR)/$(TARGETS)
	rm -f *.o $(TARGETS)
