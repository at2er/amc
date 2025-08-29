CORE_SRC =\
	array.c\
	const.c\
	enum.c\
	expr.c\
	module.c\
	ptr.c\
	scope.c\
	struct.c\
	symbol.c\
	type.c\
	val.c

CORE_BUILD = ../build/core
CORE_OBJ = $(addprefix $(CORE_BUILD)/, $(CORE_SRC:.c=.o))
