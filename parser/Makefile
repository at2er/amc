include ../config.mk

SRC =\
	token.c\
	keywords.c\
	unit.c\
	parser.c\

OBJ = $(SRC:.c=.o)
TARGET = libparser.a

DEBUG_OBJ = $(SRC:.c=.debug.o)
DEBUG_TARGET = $(TARGET).debug.a

.PHONY: all clean debug
all: $(TARGET)
debug: $(DEBUG_TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.debug.o: %.c
	$(CC) $(CFLAGS) $(CDEBUG) -c $< -o $@

$(TARGET): $(OBJ)
	$(AR) -rcs $@ $^

$(DEBUG_TARGET): $(DEBUG_OBJ)
	$(AR) -rcs $@ $^

clean:
	rm -f $(TARGET) $(OBJ) $(DEBUG_TARGET) $(DEBUG_OBJ)
