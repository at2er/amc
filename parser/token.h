#ifndef AMC_TOKEN_H
#define AMC_TOKEN_H
#include "../utils/cint.h"
#include "../utils/str/str.h"
#include "file.h"
#include "type.h"

struct token_flag {
    unsigned int toplevel:1, in_block:1;
};

struct token {
    const char *name;
    u32 name_len;
    int (*parse_function)(struct file *f, struct token *t, struct token *fn);
    struct token_flag flags;

    u8 argc;
    enum ATOM_TYPE result_type;
    enum ATOM_TYPE *args;
};

struct token_group {
    const char* name;
    u8 size;
    struct token* tokens;
};

int token_args_append(struct token *self, enum ATOM_TYPE type);

/**
 * Find token in all group.
 * @return:
 *   1: if is not found token.
 *   0: if found token.
 */
int token_find(str *unit, struct token **result);

/**
 * Basically the same as 'token_find'.
 * But it is find token in group.
 * @return:
 *   1: if is not found token.
 *   0: if found token.
 */
int token_find_in_group(str *unit, int group, struct token **result);
int token_group_find(const char *name);
int token_group_register(struct token_group *group);
int token_groups_init();
int token_register(struct token *token, int group);

#endif
