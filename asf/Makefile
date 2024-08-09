include ../config.mk
include config.mk

.PHONY: all clean debug
all: $(BACKEND_OBJ)
debug: $(BACKEND_DEBUG_OBJ)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.debug.o: %.c
	$(CC) $(CFLAGS) $(CDEBUG) -c $< -o $@

clean:
	rm -f $(BACKEND_OBJ) $(BACKEND_DEBUG_OBJ)
