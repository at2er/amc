#include "keywords.h"
//#include "expr.h"

static int if_block_parse(struct file *f, struct token *fn);
static int if_condition_parse(struct file *f);

int if_block_parse(struct file *f, struct token *fn)
{
    return 0;
}

int if_condition_parse(struct file *f)
{
    //parse_expr(f);
    return 0;
}

int parse_if(struct file *f, struct token *t, struct token *fn)
{
    while (f->src[f->pos] != '{') {
        if_condition_parse(f);
    }

    return 0;
}

int parse_elif(struct file *f, struct token *t, struct token *fn)
{
    return 0;
}

int parse_else(struct file *f, struct token *t, struct token *fn)
{
    return 0;
}
