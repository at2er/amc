include config.mk
include asf/config.mk
include parser/config.mk

STRDIR = utils/str
STRLIB = $(STRDIR)/libstr.a
PARSERDIR = parser
BACKEND = asf
PARSER_TARGET = $(addprefix $(PARSERDIR)/, $(PARSER_OBJ))
PARSER_DEBUG_TARGET = $(addprefix $(PARSERDIR)/, $(PARSER_DEBUG_OBJ))
BACKEND_TARGET = $(addprefix $(BACKEND)/, $(BACKEND_OBJ))
BACKEND_DEBUG_TARGET = $(addprefix $(BACKEND)/, $(BACKEND_DEBUG_OBJ))

SRC = main.c
OBJ = $(SRC:.c=.o)
DEBUG_OBJ = $(SRC:.c=.debug.o)
DEBUG_TARGET = $(TARGET).debug
TARGET = gmc

CLIBS = -L$(STRDIR) -lstr

.PHONY: all clean
all: $(TARGET)
debug: $(DEBUG_TARGET)

main.o: main.c
	$(CC) $(CFLAGS) -c $< -o $@

$(PARSER_TARGET):
	@$(MAKE) -C $(PARSERDIR)

$(BACKEND_TARGET):
	@$(MAKE) -C $(BACKEND)

$(PARSER_DEBUG_TARGET):
	@$(MAKE) -C $(PARSERDIR) debug

$(BACKEND_DEBUG_TARGET):
	@$(MAKE) -C $(BACKEND) debug

$(STRLIB):
	@$(MAKE) -C $(STRDIR)

$(TARGET): $(OBJ) $(PARSER_TARGET) $(BACKEND_TARGET) $(STRLIB)
	$(CC) $(CFLAGS) -o $@ $(OBJ) $(PARSER_TARGET) $(BACKEND_TARGET) $(CLIBS)

$(DEBUG_TARGET): $(DEBUG_OBJ) $(PARSER_DEBUG_TARGET) $(BACKEND_DEBUG_TARGET) $(STRLIB)
	$(CC) $(CFLAGS) $(CDEBUG) -o $@ $(OBJ) $(PARSER_DEBUG_TARGET) $(BACKEND_DEBUG_TARGET) $(CLIBS)

clean:
	rm -f $(TARGET) $(OBJ) $(DEBUG_TARGET) $(DEBUG_OBJ)
	@$(MAKE) -C $(STRDIR) clean
	@$(MAKE) -C $(PARSERDIR) clean
	@$(MAKE) -C $(BACKEND) clean
