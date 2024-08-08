include config.mk
include backend.mk

STRDIR = utils/str
STRLIB = $(STRDIR)/libstr.a
PARSERDIR = parser
PARSERLIB = $(PARSERDIR)/libparser.a

SRC = main.c
OBJ = $(SRC:.c=.o)\
	$(BACKEND)/$(BACKEND_OBJ)
TARGET = gmc

DEBUG_TARGET = $(TARGET).debug
DEBUG_OBJ = $(SRC:.c=.debug.o) $(BACKEND_OBJ:.o=.debug.o)

CLIBS = -L$(STRDIR) -L$(PARSERDIR)

.PHONY: all clean
all: $(TARGET)

main.o: main.c
	$(CC) $(CFLAGS) -c $< -o $@

$(STRLIB):
	$(MAKE) -C $(STRDIR)

$(PARSERLIB):
	$(MAKE) -C $(PARSERDIR)

$(BACKENDLIB):
	$(MAKE) -C $(BACKEND)

$(TARGET): $(OBJ) $(STRLIB) $(PARSERLIB)
	$(CC) $(CFLAGS) $(CLIBS) -o $@ $(OBJ)

$(DEBUG_TARGET): $(DEBUG_OBJ) $(STRLIB) $(PARSERLIB)
	$(CC) $(CFLAGS) $(CDEBUG) $(CLIBS) -o $@ $(OBJ)

clean:
	rm -f $(TARGET) $(OBJ) $(DEBUG_TARGET) $(DEBUG_OBJ)
	@$(MAKE) -C $(STRDIR) clean
	@$(MAKE) -C $(PARSERDIR) clean
	@$(MAKE) -C $(BACKEND) clean
