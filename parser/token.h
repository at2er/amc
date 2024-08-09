#ifndef GMC_TOKEN_H
#define GMC_TOKEN_H
#include "../utils/str/str.h"
#include "type.h"
#include "file.h"

struct token {
    const char *name;
    int (*parse_function)(struct file *f);
    int argc;
    struct argument {
        enum GM_TYPE type;
        void *v;
    } *args;
};

struct token_group {
    const char* name;
    int size;
    struct token* tokens;
};

int token_find(str *unit, struct token *result);
int token_group_find(const char *name);
int token_group_register(struct token_group *group);
int token_groups_init();
int token_register(struct token *token, int group);

#endif
