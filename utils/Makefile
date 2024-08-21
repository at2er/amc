include ../config.mk
include config.mk

.PHONY: all clean debug
all: $(UTILS_OBJ)
debug: $(UTILS_DEBUG_OBJ)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.debug.o: %.c
	$(CC) $(CFLAGS) $(CDEBUG) -c $< -o $@

clean:
	rm -f $(UTILS_OBJ) $(UTILS_DEBUG_OBJ)
