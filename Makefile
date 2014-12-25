CC=gcc
BIN_DIR=out/bin/
OBJ_DIR=out/obj/
LFLAGS=-Wall -Iinc -std=c99
RFLAGS=-ledit -lm
LNFLAGS=-Wall -Iinc -W -Os -g

all: out.dir linenoise.o mpc.o builtins.o func.o lenv.o lval.o zlango.o zlango

out.dir: 
	mkdir -p $(BIN_DIR) $(OBJ_DIR)

linenoise.o: lib/linenoise/linenoise.c
	$(CC) -c lib/linenoise/linenoise.c -o $(OBJ_DIR)linenoise.o $(LNFLAGS)

mpc.o: lib/mpc/mpc.c
	$(CC) -c lib/mpc/mpc.c -o $(OBJ_DIR)mpc.o $(LFLAGS)

builtins.o: src/builtins.c
	$(CC) -c src/builtins.c -o $(OBJ_DIR)builtins.o $(LFLAGS)

func.o: src/func.c
	$(CC) -c src/func.c -o $(OBJ_DIR)func.o $(LFLAGS)

lenv.o: src/lenv.c
	$(CC) -c src/lenv.c -o $(OBJ_DIR)lenv.o $(LFLAGS)

lval.o: src/lval.c
	$(CC) -c src/lval.c -o $(OBJ_DIR)lval.o $(LFLAGS)

zlango.o: zlango.c
	$(CC) -c zlango.c -o $(OBJ_DIR)zlango.o $(LFLAGS)

zlango: out/obj/zlango.o out/obj/lval.o out/obj/func.o out/obj/lenv.o out/obj/builtins.o out/obj/mpc.o out/obj/linenoise.o
	$(CC) out/obj/zlango.o out/obj/lval.o out/obj/func.o out/obj/lenv.o out/obj/builtins.o out/obj/mpc.o out/obj/linenoise.o -o $(BIN_DIR)zlango $(RFLAGS) $(LFLAGS) 

.PHONY: clean

clean:
	rm -rf out/