#ifndef GMC_FUNC_H
#define GMC_FUNC_H
#include "file.h"
#include "token.h"
#include "unit.h"

int parse_func_call(struct token *fn, struct file *f);
int parse_func_def(struct file *f);

#endif
