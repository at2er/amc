/* This file is part of amc.
   SPDX-License-Identifier: GPL-3.0-or-later
*/
#ifndef AMC_LEXER_H
#define AMC_LEXER_H
#include "keyword.h"
#include "../utils/str/str.h"
#include <sclexer.h>

#define lexer_tok_for_each(CUR, BLOCK) \
	for (struct lexer_tok *CUR = BLOCK->begin, \
			*__lexer_tok_for_each__next; \
			__lexer_tok_for_each__next = CUR ? CUR->next : NULL, \
			CUR != NULL; \
			CUR = __lexer_tok_for_each__next)


enum TOK_TYPE {
	TOK_EOB, // end of block
	TOK_EOF,
	TOK_EOL,

	TOK_DECORATOR,
	TOK_IDENT,
	TOK_INT,
	TOK_INT_NEG,
	TOK_KEYWORD,
	TOK_STRING,

	TOK_BRACE_L,   // {
	TOK_BRACE_R,   // }
	TOK_BRACKET_L, // [
	TOK_BRACKET_R, // ]
	TOK_PAREN_L,   // (
	TOK_PAREN_R,   // )

	TOK_BLOCK_START,
	TOK_COLON,
	TOK_COMMA,
	TOK_DOT,
	TOK_VLINE,

	TOK_INFIX_ASSIGN,
	TOK_INFIX_EQUAL,
	TOK_INFIX_NOT_EQUAL,
	TOK_INFIX_LESS,
	TOK_INFIX_LESS_EQUAL,
	TOK_INFIX_GREATER,
	TOK_INFIX_GREATER_EQUAL,
	TOK_INFIX_ADD,
	TOK_INFIX_ADD_ASSIGN,
	TOK_INFIX_DIV,
	TOK_INFIX_DIV_ASSIGN,
	TOK_INFIX_MUL,
	TOK_INFIX_MUL_ASSIGN,
	TOK_INFIX_SUB,
	TOK_INFIX_SUB_ASSIGN,

	TOK_UNARY_EXTRACT_PTR,
	TOK_UNARY_GET_ADDR,
	TOK_UNARY_NOT,

	TOK_COUNT
};

struct lexer_block {
	struct lexer_tok *begin, *end;
	uint16_t indent;
};

union lexer_tok_data {
	struct lexer_block block;
	enum KEYWORDS keyword;
	str s;
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

void lexer_init(struct lexer *lexer);
int lexer_parse_file(struct lexer *lexer, const char *fpath);
void lexer_print_block(struct lexer_block *blk);
void lexer_eat_tok(struct lexer *lexer);

#endif
