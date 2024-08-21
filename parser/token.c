#include "token.h"
#include <stdlib.h>
#include <string.h>

static struct {
    int size;
    struct token_group *groups;
} *token_groups = NULL;

int token_args_append(struct token *self, enum ATOM_TYPE type)
{
    self->argc += 1;
    self->args = realloc(self->args, sizeof(enum ATOM_TYPE) * self->argc);
    self->args[self->argc - 1] = type;

    return 0;
}

int token_find(str *unit, struct token **result)
{
    for (int i = 0; i < token_groups->size; i++) {
        if (!token_find_in_group(unit, i, result))
            return 0;
    }

    return 1;
}

int token_find_in_group(str *unit, int group, struct token **result)
{
    for (int i = 0; i < token_groups->groups[group].size; i++) {
        if (strncmp(unit->s, token_groups->groups[group].tokens[i].name,
                    unit->len)
            == 0) {
            *result = &token_groups->groups[group].tokens[i];
            return 0;
        }
    }

    return 1;
}

int token_group_find(const char *name)
{
    for (int i = 0; i < token_groups->size; i++) {
        if (strcmp(name, token_groups->groups[i].name) == 0) {
            return i;
        }
    }

    return -1;
}

int token_group_register(struct token_group *group)
{
    token_groups->size += 1;
    token_groups->groups = realloc(
        token_groups->groups, sizeof(struct token_group) * token_groups->size);
    if (token_groups == NULL)
        return 1;

    struct token_group *last = &token_groups->groups[token_groups->size - 1];
    last->name = group->name;
    last->tokens = group->tokens;

    return 0;
}

int token_groups_init()
{
    token_groups = malloc(sizeof(*token_groups));
    if (token_groups == NULL)
        return 1;
    token_groups->groups = NULL;
    token_groups->size = 0;

    struct token_group default_group = {"default", 0, NULL};
    struct token_group func_group = {"func", 0, NULL};
    token_group_register(&default_group);
    token_group_register(&func_group);
    return 0;
}

int token_register(struct token *token, int group)
{
    token_groups->groups[group].size += 1;
    token_groups->groups[group].tokens =
        realloc(token_groups->groups[group].tokens,
                sizeof(struct token) * token_groups->groups[group].size);

    struct token *last = &token_groups->groups[group]
                              .tokens[token_groups->groups[group].size - 1];
    last->name = token->name;
    last->parse_function = token->parse_function;

    return 0;
}
