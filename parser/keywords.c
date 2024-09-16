#include "keywords.h"
#include "../utils/utils.h"
#include "func.h"
#include "token.h"

//#include "if.c"
//#include "match.c"

static struct token keywords[] = {
    {"func",   4, parse_func_def, {1, 0}},
    //{"struct", 6, parse_struct,   2},
    //{"const",  5, parse_const,    2},
    //{"var",    3, parse_var,      2},
    //{"let",    3, parse_let,      2},
    //{"match",  5, parse_match,    0},
    {"if",     2, parse_if,       {0, 1}},
    {"elif",   2, parse_elif,     {0, 1}},
    {"else",   2, parse_else,     {0, 1}},
    {"return", 6, parse_func_ret, {0, 1}},
};

int keyword_init()
{
    struct token_group kws = {"keywords", LENGTH(keywords), keywords};
    token_group_register(&kws);
    return 0;
}
