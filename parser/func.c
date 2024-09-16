#include "func.h"
#include "../include/backend.h"
#include "../include/be_func.h"
#include "../utils/die.h"
#include "block.h"
#include "file.h"
#include "parser.h"
#include "type.h"

static int func_call_main(struct file *f, struct token *t, struct token *fn);
static int func_call_read_arg(struct token *fn, struct file *f);
static int func_call_read_args(struct token *fn, struct file *f);
static int func_def_check_main(str *name);
static int func_def_main(struct file *f, struct token *fn);
static int func_def_read_arg(struct token *fn, struct file *f);
static int func_def_read_args(struct token *fn, struct file *f);
static int func_def_read_block(struct file *f, struct token *fn);
static int func_def_read_name_type(struct file *f, str *name, enum ATOM_TYPE *type);

int func_call_main(struct file *f, struct token *t, struct token *fn)
{
    printf("amc: You cannot call the main function!");
    backend_stop(BE_STOP_SIGNAL_ERR);
    return 0;
}

int func_call_read_arg(struct token *fn, struct file *f)
{
    str *arg = str_new();
    while (f->src[f->pos] != ',' || f->src[f->pos] != ' ') {
        if (f->src[f->pos] == '\0')
            die("amc: end of file.");
        str_append(arg, 1, &f->src[f->pos]);
    }

    file_pos_next(f);
    file_try_skip_space(f);
    str_free(arg);
    return 0;
}

int func_call_read_args(struct token *fn, struct file *f)
{
    int i = 0;
    while (f->src[f->pos] != ')') {
        switch (f->src[f->pos]) {
        case '\0':
            die("amc: end of file.");
            break;
        case '\n':
            if (i == fn->argc)
                return 0;
            printf("amc: ");
            backend_stop(BE_STOP_SIGNAL_ERR);
            break;
        }

        func_call_read_arg(fn, f);
    }

    if (i == fn->argc)
        return 0;

    return 1;
}

int func_def_check_main(str *name)
{
    const char *template = "main";
    const int template_len = 5;

    if (name->len != template_len)
        return 0;
    if (strncmp(name->s, template, name->len) == 0)
        return 1;

    return 0;
}

int func_def_main(struct file *f, struct token *fn)
{
    while (f->src[f->pos] != '{') {
        if (f->src[f->pos] == '\0')
            die("amc: parse_func_def: end of file.");
        f->pos += 1;
    }

    parser_global_conf.in_main = 1;
    backend_func_def("_start", 7, ATOM_VOID);
    func_def_read_block(f, fn);
    parser_global_conf.in_main = 0;

    return 0;
}

int func_def_read_arg(struct token *fn, struct file *f)
{
    str *name = str_new();
    enum ATOM_TYPE type = unit_and_type_read(name, f);
    if (type == -1) {
        printf("amc: func_def_read_arg: In function define: %s\n"
               "| %s\n"
               "| ^ : Missing function argument type separator ':'",
               fn->name, &f->src[f->pos]);
        backend_stop(BE_STOP_SIGNAL_ERR);
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
    file_pos_next(f);

    return 0;
}

int func_def_read_block(struct file *f, struct token *fn)
{
    file_try_skip_space(f);
    if (f->src[f->pos] == '{') {
        f->pos += 1;
        file_line_next(f);
        parse_block(f, fn);
    }

    return 0;
}

// TODO: read arguments and call function.
int parse_func_call(struct file *f, struct token *t, struct token *fn)
{
    func_call_read_args(t, f);
    return 0;
}

int parse_func_def(struct file *f, struct token *t, struct token *fn)
{
    struct str *name = str_new();
    struct token result;
    enum ATOM_TYPE type;
    file_try_skip_space(f);
    func_def_read_name_type(f, name, &type);
    str_append(name, 1, "\0");
    result.name = name->s;
    result.name_len = name->len;
    result.result_type = type;

    if (func_def_check_main(name)) {
        result.parse_function = func_call_main;
        func_def_main(f, &result);
        token_register(&result, 1);
        return 0;
    }

    result.parse_function = parse_func_call;
    file_try_skip_space(f);
    if (f->src[f->pos] == '(') {
        f->pos += 1;
        func_def_read_args(&result, f);
    }
    backend_func_def(result.name, result.name_len, result.result_type);
    func_def_read_block(f, &result);

    token_register(&result, 1);
    return 0;
}

int func_def_read_name_type(struct file *f, str *name, enum ATOM_TYPE *type)
{
    *type = unit_and_type_read(name, f);
    if (type == AMC_ERR_TYPE) {
        str_append(name, 1, "\0");
        printf("amc: parse_func_def: In function define: %s\n"
               "| %c\n"
               "| ^ : Missing function return value type separator ':'",
               name->s, f->src[f->pos]);
        backend_stop(BE_STOP_SIGNAL_ERR);
        return 1;
    }

    return 0;
}

int parse_func_ret(struct file *f, struct token *t, struct token *fn)
{
    file_try_skip_space(f);
    str *s = str_new();
    unit_read_until("\n", s, f);
    str_append(s, 1, "\0");

    union type_container v = {.s = s->s};
    backend_return(f, fn->result_type, &v);

    str_free(s);
    return 0;
}
