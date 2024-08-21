include config.mk
include asf/config.mk
include parser/config.mk
include utils/config.mk

UTILSDIR = utils
STRDIR = $(UTILSDIR)/str
STRLIB = $(STRDIR)/libstr.a
PARSERDIR = parser
BACKEND = asf
PARSER_TARGET = $(addprefix $(PARSERDIR)/, $(PARSER_OBJ))
PARSER_DEBUG_TARGET = $(addprefix $(PARSERDIR)/, $(PARSER_DEBUG_OBJ))
BACKEND_TARGET = $(addprefix $(BACKEND)/, $(BACKEND_OBJ))
BACKEND_DEBUG_TARGET = $(addprefix $(BACKEND)/, $(BACKEND_DEBUG_OBJ))
UTILS_TARGET = $(addprefix $(UTILSDIR)/, $(UTILS_OBJ))
UTILS_DEBUG_TARGET = $(addprefix $(UTILSDIR)/, $(UTILS_DEBUG_OBJ))

SRC = main.c
OBJ = $(SRC:.c=.o)
TARGET = amc
DEBUG_OBJ = $(SRC:.c=.debug.o)
DEBUG_TARGET = $(TARGET).debug

CLIBS = -L$(STRDIR) -lstr

.PHONY: all clean debug debug_target
.PHONY: $(PARSERDIR) $(BACKEND) $(UTILSDIR)
all: $(TARGET)
debug: $(DEBUG_TARGET)
debug_target:
	@$(MAKE) -C $(PARSERDIR) debug
	@$(MAKE) -C $(BACKEND) debug
	@$(MAKE) -C $(UTILSDIR) debug

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.debug.o: %.c
	$(CC) $(CFLAGS) $(CDEBUG) -c $< -o $@

$(STRLIB):
	@$(MAKE) -C $(STRDIR)

$(PARSERDIR) $(BACKEND) $(UTILSDIR):
	@$(MAKE) -C $@

$(TARGET): $(OBJ) $(PARSERDIR) $(BACKEND) $(UTILSDIR) $(STRLIB)
	$(CC) $(CFLAGS) -o $@ $(OBJ) $(PARSER_TARGET) $(UTILS_TARGET) $(BACKEND_TARGET) $(CLIBS)

$(DEBUG_TARGET): $(DEBUG_OBJ) debug_target $(STRLIB)
	$(CC) $(CFLAGS) $(CDEBUG) -o $@ $(DEBUG_OBJ) $(PARSER_DEBUG_TARGET) $(UTILS_DEBUG_TARGET) $(BACKEND_DEBUG_TARGET) $(CLIBS)

clean:
	rm -f $(TARGET) $(OBJ) $(DEBUG_TARGET) $(DEBUG_OBJ)
	@$(MAKE) -C $(STRDIR) clean
	@$(MAKE) -C $(PARSERDIR) clean
	@$(MAKE) -C $(BACKEND) clean
	@$(MAKE) -C $(UTILSDIR) clean
