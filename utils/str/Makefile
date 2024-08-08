include ../../config.mk

SRC = str.c
OBJ = $(SRC:.c=.o)
TARGET = libstr.a

.PHONY: all clean
all: $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJ)
	$(AR) -rcs $@ $(OBJ)

clean:
	rm -f $(TARGET) $(OBJ)
