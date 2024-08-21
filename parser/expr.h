#ifndef AMC_EXPR_H
#define AMC_EXPR_H
#include "backend.h"
#include "file.h"
#include "type.h"

struct expr_operator {
    const char *sym;
    const int priority;
    int (*parse_func)(struct file *f);
};

int expr_op_and(struct file *f);
int expr_op_not(struct file *f);
int expr_op_eq(struct file *f);
int expr_op_ne(struct file *f);
int expr_op_lt(struct file *f);
int expr_op_le(struct file *f);
int expr_op_gt(struct file *f);
int expr_op_ge(struct file *f);

static const struct expr_operator operators[] = {
    {"*",   1, comp_cmd_mul},
    {"/",   1, comp_cmd_div},
    {"+",   2, comp_cmd_add},
    {"-",   2, comp_cmd_sub},
    {"not", 3, expr_op_not },
    {"!",   3, expr_op_not },
    {"==",  3, expr_op_eq  },
    {"!=",  3, expr_op_ne  },
    {"<",   3, expr_op_lt  },
    {"<=",  3, expr_op_le  },
    {">",   3, expr_op_gt  },
    {">=",  3, expr_op_ge  },
    {"and", 4, expr_op_and },
};

int expr_evaluation(struct file *f);
int expr_if(struct file *f);
int expr_match(struct file *f);
int parse_eval_expr(struct file *f);

#endif
