
CC = gcc
CFLAGS = -Wall -std=c11 -D_DEFAULT_SOURCE 

SRCS = $(wildcard src/*.c)
HDRS = $(wildcard src/*.h)
BDIR = build/swanset
TDIR = test/*.c
TBDIR = test/test
.PHONY: all build clean test-argman

all: build

build: $(SRCS) $(HDRS)
	@mkdir -p build
	$(CC) $(SRCS) $(CFLAGS) -o $(BDIR)

test: $(TDIR)
	$(CC) $(TDIR) $(CFLAGS) -o $(TBDIR)
	$(TBDIR)

test-argman:
	@mkdir -p build
	$(CC) -Isrc tests/argman_test.c src/argman.c $(CFLAGS) -o build/test-argman
	./build/test-argman

clean:
	rm -f $(BDIR)
	rm -f build/test-argman
	rm -f ./test/
