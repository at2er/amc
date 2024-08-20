#include "str.h"

int str_append(str *src, int len, const char *s)
{
    if (src == NULL)
        return 1;
    str_expand(src, len);
    memcpy(&src->s[src->len - 1], s, len);
    return 0;
}

int str_expand(str *src, int len)
{
    if (src == NULL)
        return 1;
    src->s = realloc(src->s, src->len + len);
    src->len += len;
    return 0;
}

void str_free(str *src)
{
    free(src->s);
    src->s = NULL;
    src->len = 0;
}

str *str_new()
{
    str *s = malloc(sizeof(*s));
    s->s = NULL;
    s->len = 0;
    return s;
}
