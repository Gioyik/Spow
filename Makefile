CC=gcc
DEPS_LIB=lib/mpc/mpc.c
BIN_DIR=out/bin/
CFLAGS=-ledit -lm -std=c99 -Wall
OBJ_DIR=out/obj/

all: compile-obj compile-bin

compile-obj: lib/argtable/argtable3.c
	mkdir -p $(OBJ_DIR)
	$(CC) -c lib/argtable/argtable3.c -o $(OBJ_DIR)argtable3.o

compile-bin: zlango.c lib/mpc/mpc.c out/obj/argtable3.o
	mkdir -p $(BIN_DIR)
	$(CC) zlango.c $(DEPS_LIB) $(OBJ_DIR)argtable3.o $(CFLAGS) -o $(BIN_DIR)zlango

clean:
	rm -rf out/

