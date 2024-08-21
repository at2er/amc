#include "keywords.h"
#include "../utils/utils.h"
#include "file.h"
#include "token.h"
#include "func.h"
#include "match.h"

static int parse_struct(struct file *f);
static int parse_const(struct file *f);
static int parse_var(struct file *f);
static int parse_let(struct file *f);

static struct token keywords[] = {
    {"func",   parse_func_def, 1},
    //{"struct", parse_struct  , 2},
    //{"const",  parse_const   , 2},
    //{"var",    parse_var     , 2},
    //{"let",    parse_let     , 2},
    {"match",  parse_match   , 0},
    {"return", parse_func_ret, 0},
};

int keyword_init()
{
    struct token_group kws = {"keywords", LENGTH(keywords), keywords};
    token_group_register(&kws);
    return 0;
}
