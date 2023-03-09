CC=gcc
CFLAGS=-Wall -Wextra -Werror -pedantic -std=c99 -g
FRAMEWORKS=-framework ApplicationServices -framework Carbon

PROGRAM=macos-keystrokes-fun

SOURCES=main.c

.PHONY: all clean

all: $(PROGRAM)

$(PROGRAM): $(SOURCES)
	$(CC) $(CFLAGS) -o $(PROGRAM) $(SOURCES) $(FRAMEWORKS)

clean:
	rm -f $(PROGRAM)
