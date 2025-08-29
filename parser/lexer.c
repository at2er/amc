/* This file is part of amc.
   SPDX-License-Identifier: GPL-3.0-or-later
*/
#include "include/lexer.h"
#include <ctype.h>
#include <sclexer.h>
#include <stdlib.h>
#include <string.h>

static int lexer_get_op_with_eq(struct lexer_tok *result,
		struct sclexer *lexer,
		enum TOK_TYPE base_op);
static int lexer_handle_equal_sign(struct lexer_tok *result,
		struct sclexer *lexer);
static int lexer_handle_type_str(struct lexer_tok *result,
		struct sclexer *lexer);
static int lexer_handle_type_sym(struct lexer_tok *result,
		struct sclexer *lexer);
static int lexer_use_sclexer_type(struct lexer_tok *result);

int lexer_get_op_with_eq(struct lexer_tok *result, struct sclexer *lexer,
		enum TOK_TYPE base_op)
{
	result->type = base_op;
	if (lexer->cur[0] == '=')
		result->type++;
	return sclexer_next_column(lexer);
}

int lexer_handle_equal_sign(struct lexer_tok *result, struct sclexer *lexer)
{
	switch (lexer->cur[0]) {
	case '=': result->type = TOK_TYPE_OP_EQUAL;    break;
	case '>': result->type = TOK_TYPE_BLOCK_START; break;
	default:
		result->type = TOK_TYPE_OP_ASSIGN;
		return 0;
		break;
	}
	sclexer_next_column(lexer);
	return 0;
}

int lexer_handle_type_str(struct lexer_tok *result, struct sclexer *lexer)
{
	result->type = TOK_TYPE_STR;
	return lexer_str_slice2str(&result->data.s,
			&result->self.type_data.str);
}

int lexer_handle_type_sym(struct lexer_tok *result, struct sclexer *lexer)
{
#define CASE_TYPE_ONLY(SIGN, TYPE) \
	case SIGN: result->type = TYPE; return 0; break;
#define CASE_OP_WITH_EQ(SIGN, BASE_OP) \
	case SIGN: return lexer_get_op_with_eq(result, lexer, BASE_OP); break
	switch (result->self.type_data.c) {
	case '=': return lexer_handle_equal_sign(result, lexer);
	case '!':
		result->type = TOK_TYPE_OP_NOT;
		if (lexer->cur[0] == '=')
			result->type = TOK_TYPE_OP_NOT_EQUAL;
		return 0;

	CASE_TYPE_ONLY(';', TOK_TYPE_COMMENT);
	CASE_TYPE_ONLY('@', TOK_TYPE_DECORATOR);
	CASE_TYPE_ONLY('&', TOK_TYPE_OP_GET_ADDR);

	CASE_TYPE_ONLY(':', TOK_TYPE_COLON);
	CASE_TYPE_ONLY(',', TOK_TYPE_COMMA);
	CASE_TYPE_ONLY('.', TOK_TYPE_DOT);
	CASE_TYPE_ONLY('"', TOK_TYPE_DOUBLE_QUOTATION);
	CASE_TYPE_ONLY('?', TOK_TYPE_QUESTION);
	CASE_TYPE_ONLY('\'', TOK_TYPE_QUOTATION);
	CASE_TYPE_ONLY('|', TOK_TYPE_PIPE_LINE);

	CASE_TYPE_ONLY('{', TOK_TYPE_BRACE_L);
	CASE_TYPE_ONLY('}', TOK_TYPE_BRACE_R);
	CASE_TYPE_ONLY('[', TOK_TYPE_BRACKET_L);
	CASE_TYPE_ONLY(']', TOK_TYPE_BRACKET_R);
	CASE_TYPE_ONLY('(', TOK_TYPE_PAREN_L);
	CASE_TYPE_ONLY(')', TOK_TYPE_PAREN_R);

	CASE_OP_WITH_EQ('>', TOK_TYPE_OP_GREATER);
	CASE_OP_WITH_EQ('<', TOK_TYPE_OP_LESS);
	CASE_OP_WITH_EQ('+', TOK_TYPE_OP_ADD);
	CASE_OP_WITH_EQ('-', TOK_TYPE_OP_SUB);
	CASE_OP_WITH_EQ('*', TOK_TYPE_OP_MUL);
	CASE_OP_WITH_EQ('/', TOK_TYPE_OP_DIV);
	}
#undef CASE_TYPE_ONLY
#undef CASE_OP
	return lexer_use_sclexer_type(result);
}

int lexer_use_sclexer_type(struct lexer_tok *result)
{
	result->type = TOK_TYPE_SCLEXER;
	return 0;
}

int lexer_read_symbol_name(str *dest, struct sclexer *lexer)
{
	struct sclexer_tok tok;
	if (!dest || !lexer)
		return 1;
	if (sclexer_read_tok(&tok, lexer))
		return 1;
	if (tok.type != SCLEXER_TOK_TYPE_STR)
		goto err_token_not_str;
	return lexer_str_slice_dup2str(dest, &tok.type_data.str);
err_token_not_str:
	printf(LEXER_ERR_FMT"Token not str\n", LEXER_ERR_FMT_ARG_REF(lexer));
	return 1;
}

int lexer_read_tok(struct lexer_tok *result, struct sclexer *lexer)
{
	if (!result || !lexer)
		return 1;
	if (sclexer_read_tok(&result->self, lexer))
		return 1;
	switch (result->self.type) {
	case SCLEXER_TOK_TYPE_SPACE:
		if (result->self.type_data.c == '\n') {
			result->type = TOK_TYPE_NEXT_LINE;
			return 0;
		}
		return lexer_read_tok(result, lexer);
	case SCLEXER_TOK_TYPE_STR: return lexer_handle_type_str(result, lexer);
	case SCLEXER_TOK_TYPE_SYM: return lexer_handle_type_sym(result, lexer);
	default: break;
	}
	return lexer_use_sclexer_type(result);
}

int lexer_str_slice2str(str *dest, struct sclexer_str_slice *slice)
{
	if (!dest || !slice)
		return 1;
	dest->s = slice->s;
	dest->len = slice->len;
	return 0;
}

int lexer_str_slice_dup2str(str *dest, struct sclexer_str_slice *slice)
{
	if (!dest || !slice)
		return 1;
	dest->s = malloc(slice->len + 1);
	dest->len = slice->len;
	strncpy(dest->s, slice->s, slice->len);
	dest->s[dest->len] = '\0';
	return 0;
}

enum TRY_RESULT lexer_try_next_line(struct sclexer *lexer)
{
	struct lexer_tok tok;
	if (lexer_read_tok(&tok, lexer))
		return TRY_RESULT_FAULT;
	if (tok.type != TOK_TYPE_COMMENT && tok.type != TOK_TYPE_NEXT_LINE)
		return TRY_RESULT_NOT_HANDLED;
	if (sclexer_get_line(lexer))
		return TRY_RESULT_FAULT;
	return TRY_RESULT_HANDLED;
}

enum TRY_RESULT lexer_try_read_str(struct sclexer *lexer,
		const char *str, int slen)
{
	if (!isspace(lexer->cur[slen])
			&& !(lexer->stop_chrs_in_str
				&& strchr(lexer->stop_chrs_in_str,
					lexer->cur[slen])))
		return TRY_RESULT_NOT_HANDLED;
	if (strncmp(lexer->cur, str, slen) != 0)
		return TRY_RESULT_NOT_HANDLED;
	lexer->cur = &lexer->cur[slen];
	lexer->column += slen;
	return TRY_RESULT_HANDLED;
}
