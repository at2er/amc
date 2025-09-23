/* This file is part of amc.
   SPDX-License-Identifier: GPL-3.0-or-later
*/
#include "keyword.h"
#include <string.h>

static const char *keywords[KEYWORDS_COUNT] = {
	[KEYWORD_ELSE]   = "else",
	[KEYWORD_ENUM]   = "enum",
	[KEYWORD_FN]     = "fn",
	[KEYWORD_IF]     = "if",
	[KEYWORD_LET]    = "let",
	[KEYWORD_MATCH]  = "match",
	[KEYWORD_MOD]    = "mod",
	[KEYWORD_MUT]    = "mut",
	[KEYWORD_PUB]    = "pub",
	[KEYWORD_RETURN] = "return",
	[KEYWORD_STRUCT] = "struct",
	[KEYWORD_WHILE]  = "while",
};

enum KEYWORDS keyword_get(const char *str, int len)
{
	for (enum KEYWORDS i = 0; i < KEYWORDS_COUNT; i++) {
		if (strlen(keywords[i]) != len)
			continue;
		if (strncmp(str, keywords[i], len) == 0)
			return i;
	}
	return -1;
}

const char *keyword_get_str(enum KEYWORDS kw)
{
	if (kw >= KEYWORDS_COUNT)
		return NULL;
	return keywords[kw];
}
