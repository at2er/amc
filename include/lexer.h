/* This file is part of amc.
   SPDX-License-Identifier: GPL-3.0-or-later
*/
#ifndef AMC_LEXER_H
#define AMC_LEXER_H
#include "keyword.h"
#include "token.h"
#include "../utils/str/str.h"
#include <sclexer.h>

#define lexer_tok_for_each(CUR, BEGIN) \
	for (struct lexer_tok *CUR = BEGIN, \
			*__lexer_tok_for_each__next; \
			__lexer_tok_for_each__next = CUR ? CUR->next : NULL, \
			CUR != NULL; \
			CUR = __lexer_tok_for_each__next)

struct lexer_block {
	struct lexer_tok *begin, *end;
	uint16_t indent;
};

union lexer_tok_data {
	struct lexer_block block;
	enum KEYWORDS keyword;
	str s;
	int64_t sint;
	uint64_t uint;
};

struct lexer_tok {
	struct lexer_tok *next, *prev;
	union lexer_tok_data data;
	enum TOK_TYPE type;
};

struct lexer {
	struct sclexer self;
	struct lexer_block *cur_block;
	struct lexer_block root;
};

void lexer_eat_tok(struct lexer_block *blk);
int lexer_eat_tok_with(enum TOK_TYPE expect, struct lexer_block *blk);
void lexer_free_tok(struct lexer_tok *self);
const char *lexer_get_tok_str(enum TOK_TYPE type);
void lexer_init(struct lexer *lexer);
int lexer_parse_file(struct lexer *lexer, const char *fpath);
void lexer_print_block(struct lexer_block *blk);

#endif
