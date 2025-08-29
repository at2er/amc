/* This file is part of amc.
   SPDX-License-Identifier: GPL-3.0-or-later
*/
#ifndef AMC_PARSER_LEXER_H
#define AMC_PARSER_LEXER_H
#include "../../utils/str/str.h"
#include "../../utils/utils.h"
#include <sclexer.h>

#define LEXER_ERR_FMT "%s: %s: %lu,%lu: "
#define LEXER_ERR_FMT_ARG(LEXER) \
	__func__, (LEXER).fpath, \
	(LEXER).line, (LEXER).column

#define LEXER_ERR_FMT_ARG_REF(LEXER) \
	__func__, (LEXER)->fpath, \
	(LEXER)->line, (LEXER)->column

enum LEXER_RESULT {
	LEXER_RESULT_CONTINUE,
	LEXER_RESULT_END,
	LEXER_RESULT_FAULT
};

union lexer_tok_data {
	str s;
	void *v;
};

enum TOK_TYPE {
	TOK_TYPE_SCLEXER, // use sclexer's kind

	TOK_TYPE_STR,

	TOK_TYPE_COMMENT,
	TOK_TYPE_DECORATOR,

	TOK_TYPE_BLOCK_START,
	TOK_TYPE_BRACE_L,
	TOK_TYPE_BRACE_R,
	TOK_TYPE_BRACKET_L,
	TOK_TYPE_BRACKET_R,
	TOK_TYPE_COLON,
	TOK_TYPE_COMMA,
	TOK_TYPE_DOT,
	TOK_TYPE_DOUBLE_QUOTATION,
	TOK_TYPE_NEXT_LINE,
	TOK_TYPE_PAREN_L,
	TOK_TYPE_PAREN_R,
	TOK_TYPE_PIPE_LINE,
	TOK_TYPE_QUESTION,
	TOK_TYPE_QUOTATION,

	TOK_TYPE_OP_GET_ADDR,
	TOK_TYPE_OP_NOT,

	TOK_TYPE_OP_ASSIGN,
	TOK_TYPE_OP_EQUAL,
	TOK_TYPE_OP_NOT_EQUAL,
	TOK_TYPE_OP_LESS,
	TOK_TYPE_OP_LESS_EQUAL,
	TOK_TYPE_OP_GREATER,
	TOK_TYPE_OP_GREATER_EQUAL,

	TOK_TYPE_OP_ADD,
	TOK_TYPE_OP_ADD_ASSIGN,
	TOK_TYPE_OP_DIV,
	TOK_TYPE_OP_DIV_ASSIGN,
	TOK_TYPE_OP_MUL, // also for * (star)
	TOK_TYPE_OP_MUL_ASSIGN,
	TOK_TYPE_OP_SUB,
	TOK_TYPE_OP_SUB_ASSIGN,

	TOK_TYPE_COUNT
};

struct lexer_tok {
	union lexer_tok_data data;
	enum TOK_TYPE type;
	struct sclexer_tok self;
};

/**
 * Clone a string to `dest` from current file position.
 * @param dest: Empty str type.
 *   .s = NULL
 *   .len = 0
 */
int lexer_read_symbol_name(str *dest, struct sclexer *lexer);
int lexer_read_tok(struct lexer_tok *result, struct sclexer *lexer);
int lexer_str_slice2str(str *dest, struct sclexer_str_slice *slice);
int lexer_str_slice_dup2str(str *dest, struct sclexer_str_slice *slice);
enum TRY_RESULT lexer_try_next_line(struct sclexer *lexer);
enum TRY_RESULT lexer_try_read_str(struct sclexer *lexer,
		const char *str, int slen);

#endif
