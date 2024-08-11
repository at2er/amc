#include "keywords.h"
#include "../utils/utils.h"
#include "../utils/str/str.h"
#include "file.h"
#include "type.h"
#include "token.h"
#include "unit.h"
#include "func.h"
#include "backend.h"

static int parse_struct(struct file *f);
static int parse_const(struct file *f);
static int parse_var(struct file *f);
static int parse_let(struct file *f);
static int parse_match(struct file *f);
static int parse_if(struct file *f);
static int parse_elif(struct file *f);
static int parse_else(struct file *f);
static int parse_return(struct file *f);

static struct token keywords[] = {
    {"parse_func",   parse_func_def  },
    {"parse_struct", parse_struct},
    {"parse_const",  parse_const },
    {"parse_var",    parse_var   },
    {"parse_let",    parse_let   },
    {"parse_match",  parse_match },
    {"parse_if",     parse_if    },
    {"parse_elif",   parse_elif  },
    {"parse_else",   parse_else  },
    {"parse_return", parse_return},
};

int parse_struct(struct file *f)
{
    return 0;
}

int parse_const(struct file *f)
{
    return 0;
}

int parse_var(struct file *f)
{
    return 0;
}

int parse_let(struct file *f)
{
    return 0;
}

int parse_match(struct file *f)
{
    return 0;
}

int parse_if(struct file *f)
{
    return 0;
}

int parse_elif(struct file *f)
{
    return 0;
}

int parse_else(struct file *f)
{
    return 0;
}

int parse_return(struct file *f)
{
    return 0;
}

int keyword_init()
{
    struct token_group kws = {"keywords", LENGTH(keywords), keywords};
    token_group_register(&kws);
    return 0;
}
