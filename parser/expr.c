#include "expr.h"
#include "block.h"
#include "unit.h"

static int expr_operator_find(str *unit);

int expr_operator_find(str *unit)
{
    return 0;
}

int parse_expr(struct file *f, struct token *fn)
{
    str *value_l = str_new();
    str *value_r = str_new();
    str *operator= str_new();

    unit_read(value_l, f);
    file_try_skip_space(f);
    unit_read(operator, f);
    file_try_skip_space(f);
    unit_read(value_r, f);
    file_try_skip_space(f);

    if (f->src[f->pos] == '{') {
        parse_block(f, fn);
    }

    return 0;
}
