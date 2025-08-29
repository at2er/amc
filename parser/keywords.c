/* This file is part of amc.
   SPDX-License-Identifier: GPL-3.0-or-later
*/
#include "include/keywords.h"
#include "../include/symbol.h"
#include "../utils/utils.h"
#include <string.h>

#define KW_DEF(NAME, NAME_LEN, PARSE_FUNC,            \
		REC, TOPLEVEL, IN_BLOCK)              \
	{                                             \
		.name           = {.len = NAME_LEN, .s =NAME}, \
		.parse_function = PARSE_FUNC,         \
		.flags = {                            \
			.only_declaration = 0,        \
			.rec              = REC,      \
			.toplevel         = TOPLEVEL, \
			.in_block         = IN_BLOCK, \
			.is_init          = 0,        \
			.mut              = 0         \
		}                                     \
	}

#define KW_REC 1
#define KW_TOPLEVEL 1
#define KW_IN_BLOCK 1

static struct symbol keywords[] = {
	KW_DEF("enum",   4, parse_enum,     !KW_REC,  KW_TOPLEVEL,  KW_IN_BLOCK),
	KW_DEF("fn",     2, parse_func_def, !KW_REC,  KW_TOPLEVEL, !KW_IN_BLOCK),
	KW_DEF("if",     2, parse_if,        KW_REC, !KW_TOPLEVEL,  KW_IN_BLOCK),
	KW_DEF("let",    3, parse_let,      !KW_REC,  KW_TOPLEVEL,  KW_IN_BLOCK),
	KW_DEF("match",  5, parse_match,     KW_REC, !KW_TOPLEVEL,  KW_IN_BLOCK),
	KW_DEF("mod",    3, parse_mod,      !KW_REC,  KW_TOPLEVEL, !KW_IN_BLOCK),
	KW_DEF("pub",    3, parse_pub,      !KW_REC,  KW_TOPLEVEL, !KW_IN_BLOCK),
	KW_DEF("ret",    3, parse_func_ret, !KW_REC, !KW_TOPLEVEL,  KW_IN_BLOCK),
	KW_DEF("struct", 6, parse_struct,   !KW_REC,  KW_TOPLEVEL,  KW_IN_BLOCK),
	KW_DEF("while",  5, parse_while,     KW_REC, !KW_TOPLEVEL,  KW_IN_BLOCK),
};

int keyword_find(str *token, struct symbol **result)
{
	for (int i = 0; i < LENGTH(keywords); i++) {
		if (token->len != keywords[i].name.len)
			continue;
		if (strncmp(token->s, keywords[i].name.s, token->len) == 0) {
			*result = &keywords[i];
			return 1;
		}
	}
	return 0;
}
