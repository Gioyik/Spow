CC=gcc
BIN_DIR=out/bin/
LFLAGS=-std=c99 -Wall
RFLAGS=-ledit -lm
OBJ_DIR=out/obj/

all: out.dir argtable.o mpc.o lval.o zlango.o zlango

out.dir: 
	mkdir -p $(BIN_DIR) $(OBJ_DIR)

argtable.o: lib/argtable/argtable3.c
	$(CC) -c lib/argtable/argtable3.c -o $(OBJ_DIR)argtable3.o

mpc.o: lib/mpc/mpc.c
	$(CC) -c lib/mpc/mpc.c -o $(OBJ_DIR)mpc.o $(LFLAGS)

lval.o: src/lval.c
	$(CC) -c src/lval.c -o $(OBJ_DIR)lval.o $(LFLAGS)

zlango.o: zlango.c
	$(CC) -c zlango.c -o $(OBJ_DIR)zlango.o $(LFLAGS)

zlango: out/obj/zlango.o out/obj/lval.o out/obj/mpc.o out/obj/argtable3.o
	$(CC) out/obj/zlango.o out/obj/lval.o out/obj/mpc.o out/obj/argtable3.o -o $(BIN_DIR)zlango $(RFLAGS) $(LFLAGS) 

clean:
	rm -rf out/