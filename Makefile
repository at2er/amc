include config.mk

include backend/config.mk
include checker/config.mk
include comptime/config.mk
include core/config.mk
include parser/config.mk
include utils/config.mk

# libs
CLIBS  = -L$(STRDIR) -lstr \
	-L$(SCTRIE_DIR) -I$(SCTRIE_DIR) -lsctrie \
	-L$(GETARG_DIR) -I$(GETARG_DIR) -lgetarg

GETARG_DIR = lib/libgetarg
GETARG_LIB = $(GETARG_DIR)/libgetarg.a

SCTRIE_DIR = lib/libsctrie
SCTRIE_LIB = $(SCTRIE_DIR)/libsctrie.a

STRDIR = $(UTILS)/str
STRLIB = $(STRDIR)/libstr.a

OBJ = $(SRC:.c=.o)
PREFIX = /usr/local
SRC = main.c
TARGET = amc

BUILD       = build
UTILS       = utils
CHECKER     = checker
CORE        = core
COMPTIME    = comptime
PARSER      = parser
BACKEND     = backend
MODULES     = $(UTILS) $(CHECKER) $(CORE) $(COMPTIME) $(PARSER) $(BACKEND)
MODULES_OBJ = $(UTILS_OBJ)\
              $(CHECKER_OBJ)\
              $(CORE_OBJ)\
              $(COMPTIME_OBJ)\
              $(PARSER_OBJ)\
              $(BACKEND_OBJ)
OBJ += $(MODULES_OBJ:../%=%)

.PHONY: all clean install
.PHONY: $(GETARG_LIB) $(SCTRIE_LIB) $(STRLIB) $(MODULES)
all: $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD):
	mkdir -p $(BUILD)

$(GETARG_LIB):
	@$(MAKE) -C $(GETARG_DIR)
$(SCTRIE_LIB):
	@$(MAKE) -C $(SCTRIE_DIR)
$(STRLIB):
	@$(MAKE) -C $(STRDIR)

$(MODULES): $(BUILD)
	@$(MAKE) -C $@
$(MODULES_OBJ): $(MODULES)

$(TARGET): $(MODULES_OBJ) $(OBJ) $(GETARG_LIB) $(SCTRIE_LIB) $(STRLIB)
	$(CC) $(CFLAGS) -o $@ $(OBJ) $(CLIBS)

clean:
	rm -f $(TARGET) $(OBJ)
	@$(MAKE) -C $(CHECKER) clean
	@$(MAKE) -C $(CORE) clean
	@$(MAKE) -C $(COMPTIME) clean
	@$(MAKE) -C $(STRDIR) clean
	@$(MAKE) -C $(PARSER) clean
	@$(MAKE) -C $(BACKEND) clean
	@$(MAKE) -C $(UTILS) clean
	@$(MAKE) -C $(SCTRIE_DIR) clean
	@$(MAKE) -C $(GETARG_DIR) clean

install: all
	mkdir -p $(PREFIX)/bin
	cp -f $(TARGET) $(PREFIX)/bin/$(TARGET)
