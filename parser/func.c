#include "func.h"
#include "type.h"

int func_def_read_arg(struct token *fn, struct file *f)
{
    str *name = str_new();
    enum GM_TYPE type = unit_and_type_read(name, f);
    if (type == -1) {
        printf("gmc: func_def_read_arg: In function define: %s\n"
               "| %s\n"
               "| ^ : Missing function argument type separator ':'",
               fn->name, &f->src[f->pos]);
        return 1;
    }
    token_args_append(fn, type);

    return 0;
}

int func_def_read_args(struct token *fn, struct file *f)
{
    while (f->src[f->pos] != ')') {
        file_try_skip_space(f);
        func_def_read_arg(fn, f);
        file_try_skip_space(f);
    }

    return 0;
}

int parse_func_call(struct token *fn, struct file *f)
{
    for (int i = 0; i < fn->argc; i++) {
    }

    return 0;
}

int parse_func_def(struct file *f)
{
    struct str *name = str_new();
    struct token result;
    enum GM_TYPE type;
    file_try_skip_space(f);
    type = unit_and_type_read(name, f);
    if (type == -1) {
        printf("gmc: parse_func_def: In function define: %s\n"
               "| %s\n"
               "| ^ : Missing function return value type separator ':'",
               result.name, &f->src[f->pos]);
        return 1;
    }
    str_append(name, 1, "\0");
    result.name = name->s;
    result.result_type = type;

    while (f->src[f->pos] != ';') {
        file_try_skip_space(f);

        if (f->src[f->pos] == '(') {
            f->pos += 1;
            func_def_read_args(&result, f);
            file_try_skip_space(f);
        }

        f->pos += 1;
    }

    token_register(&result, 1);
    return 0;
}
