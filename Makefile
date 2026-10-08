
CC = gcc
CFLAGS = -Wall -std=c11 -D_DEFAULT_SOURCE 

SRCS = $(wildcard src/*.c)
HDRS = $(wildcard src/*.h)
BDIR = build/swanset
TDIR = test/*.c
TBDIR = test/test
.PHONY: all build clean

all: build

build: $(SRCS) $(HDRS)
	@mkdir -p build
	$(CC) $(SRCS) $(CFLAGS) -o $(BDIR)

test: $(TDIR)
	$(CC) $(TDIR) $(CFLAGS) -o $(TBDIR)
	$(TBDIR)
	

clean:
	rm -f $(BDIR)
	rm -f ./test/
