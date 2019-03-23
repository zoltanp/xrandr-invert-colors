.POSIX:

CFLAGS = -Wall -Wextra -O0 -ggdb -std=c99
LIBS   = -lxcb-randr -lxcb

PREFIX = /usr/local
BINDIR = $(PREFIX)/bin
TARGET = xrandr-invert-colors

SRC = gamma_randr.c xrandr-invert-colors.c
OBJ = $(SRC:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(LDFLAGS) -o $@ $(OBJ) $(LIBS)

$(OBJ): gamma_randr.h

clean:
	rm -f $(TARGET) $(OBJ)

install: $(TARGET)
	mkdir -p $(DESTDIR)$(BINDIR)
	cp $(TARGET) $(DESTDIR)$(BINDIR)
	chmod 755 $(DESTDIR)$(BINDIR)/$(TARGET)

deps-apt:
	apt-get install libxcb-randr0-dev

