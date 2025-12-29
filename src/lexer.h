/* This file is part of amc.
   SPDX-License-Identifier: GPL-3.0-or-later
*/
#ifndef AMC_LEXER_H
#define AMC_LEXER_H
#include "str.h"
#include <sclexer.h>

#define TOK_IDENT_UNWRAP(TOK) (TOK).data.str.begin, (TOK).data.str.len
#define TOK_IDENT_UNWRAP_REF(TOK) (TOK)->data.str.begin, (TOK)->data.str.len
#define TOK_IS_SYM(TOK, SYM) \
	((TOK)->kind == SCLEXER_SYMBOL \
	 && (TOK)->data.symbol == SYM)

enum LEXER_SYMBOLS {
	LEXER_SYM_BRACE_L,   // {
	LEXER_SYM_BRACE_R,   // }
	LEXER_SYM_BRACKET_L, // [
	LEXER_SYM_BRACKET_R, // ]
	LEXER_SYM_PAREN_L,   // (
	LEXER_SYM_PAREN_R,   // )

	LEXER_SYM_BLOCK_ENTER,  // =>
	LEXER_SYM_COLON,
	LEXER_SYM_COMMA,
	LEXER_SYM_DOT,
	LEXER_SYM_VLINE,

	LEXER_SYM_INFIX_ASSIGN, // =
	LEXER_SYM_INFIX_EQUAL,  // ==
	LEXER_SYM_INFIX_NOT_EQUAL,
	LEXER_SYM_INFIX_LESS,
	LEXER_SYM_INFIX_LESS_EQUAL,
	LEXER_SYM_INFIX_GREATER,
	LEXER_SYM_INFIX_GREATER_EQUAL,
	LEXER_SYM_INFIX_ADD,
	LEXER_SYM_INFIX_ADD_ASSIGN,
	LEXER_SYM_INFIX_DIV,
	LEXER_SYM_INFIX_DIV_ASSIGN,
	LEXER_SYM_INFIX_MUL,
	LEXER_SYM_INFIX_MUL_ASSIGN,
	LEXER_SYM_INFIX_SUB,
	LEXER_SYM_INFIX_SUB_ASSIGN,

	LEXER_SYM_UNARY_GET_ADDR,
	LEXER_SYM_UNARY_NOT,

	LEXER_SYMBOLS_COUNT
};

void dup_tok_str2str(str *dst, const struct sclexer_tok *tok);
const char *get_token_str(const struct sclexer_tok *tok);
void init_lexer(struct sclexer *self, const char *fpath);
void print_tokens(struct sclexer *self,
		struct sclexer_tok *tokens,
		size_t count);

#endif
