PARSER_SRC =\
	block.c\
	comment.c\
	file.c\
	func.c\
	if.c\
	keywords.c\
	match.c\
	parser.c\
	token.c\
	type.c\
	unit.c
PARSER_OBJ = $(PARSER_SRC:.c=.o)
PARSER_DEBUG_OBJ = $(PARSER_SRC:.c=.debug.o)
