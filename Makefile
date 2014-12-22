CC=gcc
DEP_ARGTABLE=lib/argtable/argtable3.c
BIN_DIR=out/bin/
CFLAGS=-ledit

default: bin

all: bin

bin: zlango.c
	mkdir -p $(BIN_DIR)
	$(CC) zlango.c $(DEP_ARGTABLE) $(CFLAGS) -o $(BIN_DIR)zlango

clean:
	rm -rf out/

