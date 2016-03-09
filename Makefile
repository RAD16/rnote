# Musl-libc Makefile

CC=musl-gcc
CFLAGS=-Wall -g -static -Os

all: rnote

install:
	cp rnote /usr/local/bin/note

clean: 
	rm -f rnote

uninstall:
	rm -f /usr/local/bin/note
