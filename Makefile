CC=cc
FLAGS=-std=c11 -Wall -pedantic
CFLAGS=$(FLAGS) -g
LFLAGS=-lm

BINARY = zlango
BINDIR = out/bin
OBJDIR = out/obj

all: mkdir linenoise.o mpc.o builtins.o dict.o eval.o main.o parser.o print.o repl.o types.o util.o zlango.o bin

mkdir:
	mkdir -p $(BINDIR) $(OBJDIR)

linenoise.o: lib/linenoise/linenoise.c
	$(CC) $(CFLAGS) -c lib/linenoise/linenoise.c -o $(OBJDIR)/linenoise.o

mpc.o: lib/mpc/mpc.c
	$(CC) $(CFLAGS) -c lib/mpc/mpc.c -o $(OBJDIR)/mpc.o 

builtins.o: src/builtins.c
	$(CC) $(CFLAGS) -c src/builtins.c -o $(OBJDIR)/builtins.o 

dict.o: src/dict.c
	$(CC) $(CFLAGS) -c src/dict.c -o $(OBJDIR)/dict.o 

eval.o: src/eval.c
	$(CC) $(CFLAGS) -c src/eval.c -o $(OBJDIR)/eval.o 

main.o: src/main.c
	$(CC) $(CFLAGS) -c src/main.c -o $(OBJDIR)/main.o 

parser.o: src/parser.c
	$(CC) $(CFLAGS) -c src/parser.c -o $(OBJDIR)/parser.o 

print.o: src/print.c
	$(CC) $(CFLAGS) -c src/print.c -o $(OBJDIR)/print.o 

repl.o: src/repl.c
	$(CC) $(CFLAGS) -c src/repl.c -o $(OBJDIR)/repl.o 

types.o: src/types.c
	$(CC) $(CFLAGS) -c src/types.c -o $(OBJDIR)/types.o 

util.o: src/util.c
	$(CC) $(CFLAGS) -c src/util.c -o $(OBJDIR)/util.o 

zlango.o: src/zlango.c
	$(CC) $(CFLAGS) -c src/zlango.c -o $(OBJDIR)/zlango.o 

bin: 
	$(CC) $(OBJDIR)/*.o $(CFLAGS) $(LFLAGS) -o $(BINDIR)/$(BINARY)

clean:
	rm -rf out/
