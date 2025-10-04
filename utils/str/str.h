#ifndef STR_H
#define STR_H

#define STR_EMPTY {.len = 0, .s = NULL}
#define STR_UNWRAP(STR) (STR).s, (STR).len
#define STR_UNWRAP_REF(STR) (STR)->s, (STR)->len

typedef struct str {
	int len;
	char *s;
} str;

char *str2chr(const char *s, int len);
int str_append(str *dest, int len, const char *s);
int str_copy(str *src, str *dest);
int str_expand(str *src, int len);
void str_free(str *src);
void str_free_noself(str *src);
str* str_new(void);

#endif
