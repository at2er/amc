CC = gcc
CFLAGS = -Wall -std=c99
AR = ar
PREFIX = /usr/local

HEADER_DIR = $(PREFIX)/include
TARGET_DIR = $(PREFIX)/lib

HEADER = sctrie.h
TARGET = libsctrie.a

SRC = sctrie.c
OBJ = $(SRC:.c=.o)

.PHONY: all clean install uninstall
all: $(TARGET) $(HEADER)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJ)
	$(AR) -rcs $@ $(OBJ)

clean:
	rm -f $(OBJ) $(TARGET)

install:
	mkdir -p $(HEADER_DIR) $(TARGET_DIR)
	cp -f $(HEADER) $(HEADER_DIR)/$(HEADER)
	cp -f $(TARGET) $(TARGET_DIR)/$(TARGET)

uninstall:
	rm -f $(HEADER_DIR)/$(HEADER) $(TARGET_DIR)/$(TARGET)
