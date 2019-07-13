CC	= gcc
ASM64	= yasm -f elf64 -DYASM -D__x86_64__ -DPIC

CFLAGS	= -g -Wall -fno-stack-protector

PROGS	= libmini.so

all: $(PROGS)

libmini.so: libmini64.asm libmini.c
	$(ASM64) $< -o libmini64.o
	$(CC) -c $(CFLAGS) -fPIC -nostdlib libmini.c
	ld -shared libmini64.o libmini.o -o $@

.PHONY: test clean

test: start.asm test.c
	$(ASM64) $< -o start.o
	$(CC) -c $(CFLAGS) -nostdlib -I. -I.. -DUSEMINI test.c
	ld -m elf_x86_64 --dynamic-linker /lib64/ld-linux-x86-64.so.2 -o test test.o start.o -L. -L.. -lmini
	LD_LIBRARY_PATH=. ./test

clean:
	rm -f a.out test *.o $(PROGS) peda-*

