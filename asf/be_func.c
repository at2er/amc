#include "../include/be_func.h"
#include "../include/backend.h"
#include "../parser/parser.h"
#include "../utils/str/str.h"
#include "object.h"
#include "register.h"
#include <string.h>

extern struct object_array asm_objects;

static int ret_mainfn(struct section_node *node, union type_container *v);
static struct reg *ret_reg_get(enum ATOM_TYPE type);

int ret_mainfn(struct section_node *node, union type_container *v)
{
    const char *template = "movq $60, %%rax\n"
                           "movq $%s, %%rdi\n"
                           "syscall\n";
    int len = strlen(template) - 2;
    str_expand(node->s, len);
    snprintf(node->s->s, len, template, v->s);

    return 0;
}

struct reg *ret_reg_get(enum ATOM_TYPE type)
{
    switch (atom_type_table[type -= AM_TYPE_OFFSET].len) {
    case 4:
        return &regs[AM_BE_REG_32_OFFSET];
        break;
    case 8:
        return &regs[AM_BE_REG_64_OFFSET];
        break;
    default:
        printf("amc: ret_reg_get: unsupport type");
        backend_stop(BE_STOP_SIGNAL_ERR);
        break;
    }

    return NULL;
}

int backend_func_def(const char *name, int len, enum ATOM_TYPE type)
{
    const char *template = ".globl %s\n"
                           "%s:\n";
    int template_len = strlen(template) - 4 + len * 2;
    struct section_node *node = malloc(sizeof(*node));
    object_section_append(&asm_objects.o[asm_objects.cur]->texts, node);
    node->s = str_new();
    str_expand(node->s, template_len);
    snprintf(&node->s->s[0], template_len, template, name, name);
    return 0;
}

int backend_return(struct file *f, enum ATOM_TYPE type, union type_container *v)
{
    struct section_node *node = malloc(sizeof(*node));
    object_section_append(&asm_objects.o[asm_objects.cur]->texts, node);
    node->s = str_new();
    if (parser_global_conf.in_main)
        return ret_mainfn(node, v);

    char mov_len = '\0';
    const char *ret = "ret\n";
    const char *ret_v = "mov%c %s, %%%s\n%s";
    int ret_l = strlen(ret);
    int ret_vl = strlen(ret_v) - 7;
    int len = ret_l;
    struct reg *ax = NULL;

    if (type == ATOM_VOID) {
        str_expand(node->s, len);
        snprintf(node->s->s, len, "%s", ret);
        return 0;
    }

    ax = ret_reg_get(type);
    mov_len = mov_len_get(ax);
    if (ax == NULL)
        return 1;
    if (mov_len == '\0')
        return 1;

    len += strlen(ax->name) + ret_vl + 2;
    str_expand(node->s, len);
    snprintf(node->s->s, len, ret_v, mov_len, v->s, ax->name, ret);

    return 0;
}
