CC = gcc
CPPFLAGS = -Iinclude
CFLAGS = -std=c17 -Wall -Wextra -O2
LDLIBS = -lmicrohttpd -lsqlite3 -lcrypto

TARGET = morselab.exe
SOURCES = $(wildcard src/*.c)
HEADERS = $(wildcard include/*.h)

.PHONY: all

all: $(TARGET)

$(TARGET): $(SOURCES) $(HEADERS)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(SOURCES) -o $@ $(LDFLAGS) $(LDLIBS)
