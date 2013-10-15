
CFLAGS= -Wall -Wextra
CFLAGS+= -O0 -ggdb
CFLAGS+= -std=c99

LDFLAGS= -lxcb-randr

all: xrandr-invert-colors.bin

xrandr-invert-colors.bin: gamma_randr.h gamma_randr.c xrandr-invert-colors.c
	$(CC) $(CFLAGS) -c gamma_randr.c
	$(CC) $(CFLAGS) -c xrandr-invert-colors.c
	$(CC) $(CFLAGS) -o xrandr-invert-colors.bin xrandr-invert-colors.o gamma_randr.o $(LDFLAGS)

clean:
	rm -f xrandr-invert-colors.bin xrandr-invert-colors.o gamma_randr.o


