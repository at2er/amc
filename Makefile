include config.mk

# libs
#LIB_DEPS =
include lib/libgetarg.mk
include lib/libmcb.mk
include lib/libsctrie.mk
include lib/libsclexer.mk
CLIBS = $(LIBGETARG) $(LIBMCB) $(LIBSCTRIE) $(LIBSCLEXER)

TARGET_DIR = $(PREFIX)/bin

BUILD_DIR = build

TARGET = amc

SUB_DIRS = src src/compiler src/parser
SRC = $(wildcard *.c $(addsuffix /*.c,$(SUB_DIRS)))
OBJ = $(addprefix $(BUILD_DIR)/,$(SRC:.c=.o))
OBJ_DIRS = $(BUILD_DIR) $(addprefix $(BUILD_DIR)/,$(SUB_DIRS))
OBJ_DEPS = $(addprefix $(BUILD_DIR)/,$(SRC:.c=.d))

CC_CMD = $(CC) $(CFLAGS) -g3 -o $@

all: $(TARGET) $(HEADER)

$(OBJ_DIRS):
	mkdir -p $@

$(BUILD_DIR)/%.o: %.c | $(OBJ_DIRS)
	@echo "  CC    " $@
	@$(CC_CMD) -c -MMD $<

$(TARGET): $(OBJ)
	@echo "  LINK  " $@
	@$(CC_CMD) $(OBJ) $(CLIBS)

clean:
	@echo "  CLEAN"
	@rm -f $(OBJ) $(TARGET)

install:
	mkdir -p $(TARGET_DIR)
	cp -f $(TARGET) $(TARGET_DIR)/$(TARGET)

uninstall:
	rm -f $(TARGET_DIR)/$(TARGET)

%.h:
	@:
ifeq (,$(filter clean,$(MAKECMDGOALS)))
-include $(OBJ_DEPS)
endif

.PHONY: all clean install uninstall
