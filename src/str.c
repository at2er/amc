/* SPDX-License-Identifier: MIT */
#include "str.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

char *str2chr(const char *s, int len)
{
	char *result = NULL;
	if (s[len - 1] != '\0') {
		result = malloc(len + 1);
		result[len] = '\0';
	} else {
		result = malloc(len);
	}
	memcpy(result, s, len);
	return result;
}

int str_append(str *dest, int len, const char *s)
{
	int last, src_has_zero = 0;
	if (dest == NULL)
		return 1;
	last = dest->len;
	if (dest->len > 0 && dest->s[last - 1] == '\0')
		last -= 1;
	if (len > 1 && s[len - 1] == '\0') {
		len -= 1;
		src_has_zero = 1;
	}
	str_expand(dest, len);
	memcpy(&dest->s[last], s, len + src_has_zero);
	return 0;
}

int str_copy(str *src, str *dest)
{
	if (src == NULL || dest == NULL)
		return 1;
	if ((dest->s = str2chr(src->s, src->len)) == NULL)
		return 1;
	dest->len = src->len;
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
	if (src == NULL)
		return;
	str_free_noself(src);
	free(src);
}

void str_free_noself(str *src)
{
	if (src == NULL)
		return;
	if (src->s != NULL)
		free(src->s);
	src->s = NULL;
	src->len = 0;
}

str *str_new(void)
{
	str *s = malloc(sizeof(*s));
	s->s = NULL;
	s->len = 0;
	return s;
}
