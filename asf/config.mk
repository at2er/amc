BACKEND_SRC =\
	backend.c\
	be_func.c\
	be_if.c\
	be_match.c\
	be_operator.c\
	object.c\
	register.c\
	target.c
BACKEND_OBJ = $(BACKEND_SRC:.c=.o)
BACKEND_DEBUG_OBJ = $(BACKEND_SRC:.c=.debug.o)
