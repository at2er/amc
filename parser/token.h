#ifndef AMC_TOKEN_H
#define AMC_TOKEN_H
#include "../utils/str/str.h"
#include "type.h"
#include "file.h"

struct token {
    const char *name;
    int (*parse_function)(struct file *f);
    int toplevel;

    int argc;
    enum ATOM_TYPE result_type;
    enum ATOM_TYPE *args;
};

struct token_group {
    const char* name;
    int size;
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
