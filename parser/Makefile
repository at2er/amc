include ../config.mk
include config.mk

.PHONY: all clean debug
all: $(PARSER_OBJ)
debug: $(PARSER_DEBUG_OBJ)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.debug.o: %.c
	$(CC) $(CFLAGS) $(CDEBUG) -c $< -o $@

clean:
	rm -f $(PARSER_OBJ) $(PARSER_DEBUG_OBJ)
