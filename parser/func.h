#ifndef AMC_FUNC_H
#define AMC_FUNC_H
#include "file.h"
#include "token.h"
#include "unit.h"

int parse_func_call(struct file *f, struct token *t, struct token *fn);
int parse_func_def(struct file *f, struct token *t, struct token *fn);
int parse_func_ret(struct file *f, struct token *t, struct token *fn);

#endif
