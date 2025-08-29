PARSER_SRC =\
	array.c\
	block.c\
	cache.c\
	comment.c\
	constructor.c\
	decorator.c\
	enum.c\
	expr.c\
	func.c\
	identifier.c\
	if.c\
	indent.c\
	keywords.c\
	let.c\
	lexer.c\
	loop.c\
	match.c\
	module.c\
	op.c\
	parser.c\
	ptr.c\
	struct.c\
	type.c\
	utils.c\
	val.c

PARSER_BUILD = ../build/parser
PARSER_OBJ = $(addprefix $(PARSER_BUILD)/, $(PARSER_SRC:.c=.o))
