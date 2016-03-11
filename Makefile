# Musl-libc Makefile

CFLAGS=-Wall -static -g -Os -FORTIFY_SOURCE

all: rnote

install:
	cp rnote /usr/local/bin/note

clean: 
	rm -f rnote

uninstall:
	rm -f /usr/local/bin/note

CC= musl-gcc
