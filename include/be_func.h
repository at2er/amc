/**
 * For "parser/func".
 */
#include "../parser/type.h"

int backend_func_def(const char *name, int len, enum ATOM_TYPE type);
int backend_func_call(const char *name);
int backend_return(struct file *f, enum ATOM_TYPE type, union type_container *v);
