include ../config.mk
include ../backend.mk

DEBUG_OBJ = $(BACKEND_SRC:.c=.debug.o)

.PHONY: all clean debug
all: $(BACKEND_OBJ)
debug: $(DEBUG_OBJ)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.debug.o: %.c
	$(CC) $(CFLAGS) $(CDEBUG) -c $< -o $@

clean:
	rm -f $(BACKEND_OBJ) $(DEBUG_OBJ)
