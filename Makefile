#!/usr/bin/make -f

CC      ?= gcc
INCLUDE ?= -Iinclude
CFLAGS  ?= -Wstrict-prototypes -Wall -O2 $(INCLUDE)
DEPS    := sock_srv.h

OBJFILES = sock_srv.o
EXMPL_BINFILES = examples/echo_srv

all: build

build: $(OBJFILES)

examples: $(EXMPL_BINFILES)

examples/echo_srv: build
	$(CC) $(CFLAGS) -o $@ $(OBJFILES) $@.c

clean:
	-rm -f $(OBJFILES)
	-rm -f $(EXMPL_BINFILES)

.PHONY: all build examples clean
