ifeq ($(origin CC), default)
CC = gcc
endif
ifeq ($(origin LD), default)
LD = gcc
endif

CFLAGS ?= -c -std=c90 -Wpedantic -ffunction-sections -fdata-sections -s -O3 -flto
LFLAGS ?= -s -ffunction-sections -fdata-sections -O3 -flto
OFLAG  ?= -o

all: build/bfc

build/bfc: build/main.o build/lex.o build/parse.o build/compile.o
	$(LD) $(LFLAGS) $(OFLAG) $@ $^

build/main.o: src/main.c
	$(CC) $(CFLAGS) $(OFLAG) $@ $<

build/lex.o: src/lex.c
	$(CC) $(CFLAGS) $(OFLAG) $@ $<

build/parse.o: src/parse.c
	$(CC) $(CFLAGS) $(OFLAG) $@ $<

build/compile.o: src/compile.c
	$(CC) $(CFLAGS) $(OFLAG) $@ $<

test: all FORCE
	build/bfc test/cat.bf -o test/cat.c --debug --target C
	build/bfc test/cat.bf -o test/cat.asm --debug --target X64_LINUX

FORCE:
