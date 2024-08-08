#ifndef STR_H
#define STR_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct str {
    int len;
    char *s;
} str;

int str_append(str *src, int len, const char *s);
int str_expand(str *src, int len);
void str_free(str *src);
str* str_new();

#endif
