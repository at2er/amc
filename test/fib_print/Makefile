AR = ar
CC = gcc
YZC = ../../amc

.PHONY: all clean

all: main

p.o: p.c
	$(CC) -c -o $@ $^

main: main.yz p.o
	$(YZC) -o main main.yz \
		--as "yasm" \
		--as-flags "-f elf64 -p gas" \
		--ld "musl-gcc" \
		--ld-flags "-static p.o"

clean:
	rm -rf .cache p.o main
