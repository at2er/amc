PARSER_SRC =\
	token.c\
	keywords.c\
	unit.c\
	parser.c\
	file.c
PARSER_OBJ = $(PARSER_SRC:.c=.o)
PARSER_DEBUG_OBJ = $(PARSER_SRC:.c=.debug.o)
