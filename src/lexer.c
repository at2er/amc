/* This file is part of amc.
   SPDX-License-Identifier: GPL-3.0-or-later
*/
#include "keyword.h"
#include "lexer.h"
#include "utils.h"
#include <assert.h>
#include <limits.h>
#include <sclexer.h>
#include <stdint.h>

static const char *comments[1] = {";"};
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
static const char *symbols[LEXER_SYMBOLS_COUNT] = {
	[LEXER_SYM_BRACE_L]             = "{",
	[LEXER_SYM_BRACE_R]             = "}",
	[LEXER_SYM_BRACKET_L]           = "[",
	[LEXER_SYM_BRACKET_R]           = "]",
	[LEXER_SYM_PAREN_L]             = "(",
	[LEXER_SYM_PAREN_R]             = ")",
	[LEXER_SYM_BLOCK_ENTER]         = "=>",
	[LEXER_SYM_COLON]               = ":",
	[LEXER_SYM_COMMA]               = ",",
	[LEXER_SYM_DOT]                 = ".",
	[LEXER_SYM_VLINE]               = "|",
	[LEXER_SYM_INFIX_ASSIGN]        = "=",
	[LEXER_SYM_INFIX_EQUAL]         = "==",
	[LEXER_SYM_INFIX_NOT_EQUAL]     = "!=",
	[LEXER_SYM_INFIX_LESS]          = "<",
	[LEXER_SYM_INFIX_LESS_EQUAL]    = "<=",
	[LEXER_SYM_INFIX_GREATER]       = ">",
	[LEXER_SYM_INFIX_GREATER_EQUAL] = ">=",
	[LEXER_SYM_INFIX_ADD]           = "+",
	[LEXER_SYM_INFIX_ADD_ASSIGN]    = "+=",
	[LEXER_SYM_INFIX_DIV]           = "/",
	[LEXER_SYM_INFIX_DIV_ASSIGN]    = "/=",
	[LEXER_SYM_INFIX_MUL]           = "*",
	[LEXER_SYM_INFIX_MUL_ASSIGN]    = "*=",
	[LEXER_SYM_INFIX_SUB]           = "-",
	[LEXER_SYM_INFIX_SUB_ASSIGN]    = "-=",
	[LEXER_SYM_UNARY_GET_ADDR]      = "&",
	[LEXER_SYM_UNARY_NOT]           = "!"
};

void dup_tok_str2str(str *dst, const struct sclexer_tok *tok)
{
	assert(dst && tok);
	dst->len = tok->data.str.len;
	dst->s = str2chr(tok->data.str.begin, dst->len);
}

const char *get_token_str(const struct sclexer_tok *tok)
{
	switch (tok->kind) {
	case SCLEXER_KEYWORD:
		return keywords[tok->data.keyword];
	case SCLEXER_SYMBOL:
		return symbols[tok->data.symbol];
	default: break;
	}
	return sclexer_kind_names(tok->kind);
}

void lexer_init(struct sclexer *self, const char *fpath)
{
	self->enable_indent = true;
	self->comments = comments;
	self->comments_count = 1;
	self->keywords = keywords;
	self->keywords_count = KEYWORDS_COUNT;
	self->symbols = symbols;
	self->symbols_count = LEXER_SYMBOLS_COUNT;
	sclexer_init(self, fpath);
}

void print_tokens(struct sclexer *self,
		struct sclexer_tok *tokens,
		size_t count)
{
	uint16_t shift = 0;
	for (size_t i = 0; i < count; i++) {
		if (tokens[i].kind == SCLEXER_INDENT_BLOCK_END)
			shift -= PRINT_SHIFT;
		printf("%*s", shift, "");
		sclexer_print_tok(self, &tokens[i]);
		if (tokens[i].kind == SCLEXER_INDENT_BLOCK_BEGIN)
			shift += PRINT_SHIFT;
	}
}
