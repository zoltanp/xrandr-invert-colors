SHELL=/bin/bash

CFLAGS= -Wall -Wextra
CFLAGS+= -O0 -ggdb
CFLAGS+= -std=c99

LDFLAGS= -lxcb-randr -lxcb

PREFIX=/usr/local
BINDIR=$(PREFIX)/bin

TARGET=xrandr-invert-colors.bin

all: $(TARGET)

$(TARGET): gamma_randr.h gamma_randr.c xrandr-invert-colors.c
	$(CC) $(CFLAGS) -c gamma_randr.c
	$(CC) $(CFLAGS) -c xrandr-invert-colors.c
	$(CC) $(CFLAGS) -o $(TARGET) xrandr-invert-colors.o gamma_randr.o $(LDFLAGS)

clean:
	rm -f $(TARGET) xrandr-invert-colors.o gamma_randr.o

install:
	install -v -s -p -m 755 -o root -g root $(TARGET) $(BINDIR)/$(shell basename $(TARGET) .bin)

deps-apt:
	apt-get install libxcb-randr0-dev

