#ifndef AMC_KEYWORDS_H
#define AMC_KEYWORDS_H
#include "file.h"
#include "token.h"

int parse_struct(struct file *f, struct token *t, struct token *fn);
int parse_const(struct file *f, struct token *t, struct token *fn);
int parse_var(struct file *f, struct token *t, struct token *fn);
int parse_let(struct file *f, struct token *t, struct token *fn);
int parse_if(struct file *f, struct token *t, struct token *fn);
int parse_elif(struct file *f, struct token *t, struct token *fn);
int parse_else(struct file *f, struct token *t, struct token *fn);
int parse_match(struct file *f, struct token *t, struct token *fn);

int keyword_init();

#endif
