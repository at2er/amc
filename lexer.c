/* This file is part of amc.
   SPDX-License-Identifier: GPL-3.0-or-later

   Use the f**king macro magic to make codes jet into your ass.
   If no macro, I can't think what my codes will do.
   Like stir my hands, or f**king my eyes and /dev/barin?
   Use `cat /dev/brain/power` to see my barin plz.
*/
#include "lexer.h"
#include <assert.h>
#include <limits.h>
#include <sclexer.h>
#include <stdlib.h>

/* Very convenient, isn't it? */
#define ERR_FMT "\x1b[31mlexer error\x1b[0m: \x1b[1m%s\x1b[0m: %lu,%lu: "
#define ERR_FMT_ARG(LEXER) \
	(LEXER).fpath, \
	(LEXER).line,  \
	(LEXER).column
#define ERR_FMT_ARG_REF(LEXER) \
	(LEXER)->fpath, \
	(LEXER)->line,  \
	(LEXER)->column
#define LAST_TOK(LEXER) (LEXER)->cur_block->end

enum LEXER_RESULT {
	LEXER_CONTINUE,
	LEXER_EOB,
	LEXER_EOF,
	LEXER_EOL,
	LEXER_ERR
};

static void handle_ident(struct lexer *lexer, struct sclexer_tok *tok);
static enum LEXER_RESULT handle_block(struct lexer *lexer);
static enum LEXER_RESULT handle_pair(struct lexer *lexer, enum TOK_TYPE endof);
static enum LEXER_RESULT handle_string(struct lexer *lexer);
static enum LEXER_RESULT handle_sym(struct lexer *lexer, char c);
static enum LEXER_RESULT handle_sym_equal(struct lexer *lexer);
static enum LEXER_RESULT handle_sym_equal_suffix(struct lexer *lexer);
static enum LEXER_RESULT handle_sym_not(struct lexer *lexer);
static void init_block(struct lexer_block *block);
static void join_eol(struct lexer_block *block);
static void join_tok(struct lexer_block *block, struct lexer_tok *tok);
static enum LEXER_RESULT parse_block_line(struct lexer *lexer);
static uint16_t read_indent(char *line);
static enum LEXER_RESULT read_tok(struct lexer *lexer);
static enum LEXER_RESULT read_tok_failed(struct sclexer *sclexer);
static enum LEXER_RESULT read_tok_in_line(struct lexer *lexer);
static int try_handle_keyword(struct lexer *lexer, struct sclexer_tok *tok);

/**
 * Use your vim's `:s`, and these code will jet into your ass.
 * Yes, I can use macro to implement this.
 * If you want to see `#include "tok_str.def"` and two X macro jet into
 * your /dev/eyes and /dev/barin.
 */
static const char *tok_str[TOK_COUNT] = {
	[TOK_EOB]                 = "TOK_EOB",
	[TOK_EOF]                 = "TOK_EOF",
	[TOK_EOL]                 = "TOK_EOL",
	[TOK_DECORATOR]           = "TOK_DECORATOR",
	[TOK_IDENT]               = "TOK_IDENT",
	[TOK_INT]                 = "TOK_INT",
	[TOK_INT_NEG]             = "TOK_INT_NEG",
	[TOK_KEYWORD]             = "TOK_KEYWORD",
	[TOK_STRING]              = "TOK_STRING",
	[TOK_BRACE_L]             = "TOK_BRACE_L",
	[TOK_BRACE_R]             = "TOK_BRACE_R",
	[TOK_BRACKET_L]           = "TOK_BRACKET_L",
	[TOK_BRACKET_R]           = "TOK_BRACKET_R",
	[TOK_PAREN_L]             = "TOK_PAREN_L",
	[TOK_PAREN_R]             = "TOK_PAREN_R",
	[TOK_BLOCK_START]         = "TOK_BLOCK_START",
	[TOK_COLON]               = "TOK_COLON",
	[TOK_COMMA]               = "TOK_COMMA",
	[TOK_DOT]                 = "TOK_DOT",
	[TOK_VLINE]               = "TOK_VLINE",
	[TOK_INFIX_ASSIGN]        = "TOK_INFIX_ASSIGN",
	[TOK_INFIX_EQUAL]         = "TOK_INFIX_EQUAL",
	[TOK_INFIX_NOT_EQUAL]     = "TOK_INFIX_NOT_EQUAL",
	[TOK_INFIX_LESS]          = "TOK_INFIX_LESS",
	[TOK_INFIX_LESS_EQUAL]    = "TOK_INFIX_LESS_EQUAL",
	[TOK_INFIX_GREATER]       = "TOK_INFIX_GREATER",
	[TOK_INFIX_GREATER_EQUAL] = "TOK_INFIX_GREATER_EQUAL",
	[TOK_INFIX_ADD]           = "TOK_INFIX_ADD",
	[TOK_INFIX_ADD_ASSIGN]    = "TOK_INFIX_ADD_ASSIGN",
	[TOK_INFIX_DIV]           = "TOK_INFIX_DIV",
	[TOK_INFIX_DIV_ASSIGN]    = "TOK_INFIX_DIV_ASSIGN",
	[TOK_INFIX_MUL]           = "TOK_INFIX_MUL",
	[TOK_INFIX_MUL_ASSIGN]    = "TOK_INFIX_MUL_ASSIGN",
	[TOK_INFIX_SUB]           = "TOK_INFIX_SUB",
	[TOK_INFIX_SUB_ASSIGN]    = "TOK_INFIX_SUB_ASSIGN",
	[TOK_UNARY_EXTRACT_PTR]   = "TOK_UNARY_EXTRACT_PTR",
	[TOK_UNARY_GET_ADDR]      = "TOK_UNARY_GET_ADDR",
	[TOK_UNARY_NOT]           = "TOK_UNARY_NOT",
};

void handle_ident(struct lexer *lexer, struct sclexer_tok *tok)
{
	struct lexer_tok *res;
	if (try_handle_keyword(lexer, tok))
		return;
	if (LAST_TOK(lexer) && LAST_TOK(lexer)->type == TOK_INFIX_MUL)
		LAST_TOK(lexer)->type = TOK_UNARY_EXTRACT_PTR;
	res = calloc(1, sizeof(*res));
	res->type = TOK_IDENT;
	res->data.s.len = tok->type_data.str.len;
	res->data.s.s = str2chr(
			tok->type_data.str.s,
			tok->type_data.str.len);
	join_tok(lexer->cur_block, res);
}

enum LEXER_RESULT handle_block(struct lexer *lexer)
{
	struct lexer_tok *eob;
	struct lexer_block *orig_blk = lexer->cur_block;
	enum LEXER_RESULT ret;
	assert(LAST_TOK(lexer)->type == TOK_BLOCK_START);
	LAST_TOK(lexer)->data.block.indent = lexer->cur_block->indent + 1;
	lexer->cur_block = &LAST_TOK(lexer)->data.block;
	if (lexer->self.cur[0] != '\n') {
		ret = read_tok_in_line(lexer);
		goto end;
	}
	if (sclexer_get_line(&lexer->self) == EOF) {
		ret = LEXER_EOF;
		goto end;
	}
	while ((ret = parse_block_line(lexer)) != LEXER_EOB) {
		if (ret != LEXER_CONTINUE)
			goto end;
	}
	ret = LEXER_EOL;
end:
	eob = calloc(1, sizeof(*eob));
	eob->type = TOK_EOB;
	join_tok(lexer->cur_block, eob);
	lexer->cur_block = orig_blk;
	return ret;
}

enum LEXER_RESULT handle_pair(struct lexer *lexer, enum TOK_TYPE endof)
{
	enum LEXER_RESULT ret;
	while ((ret = read_tok(lexer)) != LEXER_EOF) {
		if (ret == LEXER_EOL && sclexer_get_line(&lexer->self) == EOF)
			goto err_eof;
		if (ret != LEXER_CONTINUE)
			return ret;
		if (LAST_TOK(lexer)->type == endof)
			return LEXER_CONTINUE;
	}
err_eof:
	printf(ERR_FMT"pair '%s' not end\n",
			ERR_FMT_ARG(lexer->self),
			tok_str[endof]);
	return LEXER_ERR;
}

enum LEXER_RESULT handle_string(struct lexer *lexer)
{
	struct sclexer_str_slice slice;
	if (sclexer_read_to(&slice, &lexer->self, "\""))
		return read_tok_failed(&lexer->self);
	LAST_TOK(lexer)->data.s.s = str2chr(slice.s, slice.len - 1);
	return LEXER_CONTINUE;
}

enum LEXER_RESULT handle_sym(struct lexer *lexer, char c)
{
	struct lexer_tok *tok;
#define JOIN_NEW_TOK(TYPE) \
	tok = calloc(1, sizeof(*tok)); \
	tok->type = TYPE; \
	join_tok(lexer->cur_block, tok)
#define CASE(C, TYPE) \
	case C: JOIN_NEW_TOK(TYPE); return LEXER_CONTINUE
#define CASE_H(C, TYPE, HANDLER) \
	case C: \
		JOIN_NEW_TOK(TYPE); \
		return HANDLER(lexer)
#define CASE_PAIR(C_OPEN, C_CLOSE, OPEN, CLOSE) \
	case C_OPEN:  JOIN_NEW_TOK(OPEN); return handle_pair(lexer, CLOSE); \
	CASE(C_CLOSE, CLOSE)
	switch (c) {
	case ';': return LEXER_EOL;
	CASE('@', TOK_DECORATOR);
	CASE(':', TOK_COLON);
	CASE(',', TOK_COMMA);
	CASE('.', TOK_DOT);
	CASE('|', TOK_VLINE);
	CASE('&', TOK_UNARY_GET_ADDR);
	CASE_PAIR('{', '}', TOK_BRACE_L,   TOK_BRACE_R);
	CASE_PAIR('[', ']', TOK_BRACKET_L, TOK_BRACKET_R);
	CASE_PAIR('(', ')', TOK_PAREN_L,   TOK_PAREN_R);
	CASE_H('"', TOK_STRING,        handle_string);
	CASE_H('=', TOK_INFIX_ASSIGN,  handle_sym_equal);
	CASE_H('+', TOK_INFIX_ADD,     handle_sym_equal_suffix);
	CASE_H('/', TOK_INFIX_DIV,     handle_sym_equal_suffix);
	CASE_H('*', TOK_INFIX_MUL,     handle_sym_equal_suffix);
	CASE_H('-', TOK_INFIX_SUB,     handle_sym_equal_suffix);
	CASE_H('<', TOK_INFIX_LESS,    handle_sym_equal_suffix);
	CASE_H('>', TOK_INFIX_GREATER, handle_sym_equal_suffix);
	CASE_H('!', TOK_UNARY_NOT,     handle_sym_not);
	}
#undef CASE
#undef CASE_H
#undef CASE_PAIR
	printf(ERR_FMT"symbol '%c' not handled\n",
			ERR_FMT_ARG(lexer->self), c);
	return LEXER_ERR;
}

enum LEXER_RESULT handle_sym_equal(struct lexer *lexer)
{
	struct sclexer_tok tok;
	switch (lexer->self.cur[0]) {
	case '=':
		LAST_TOK(lexer)->type = TOK_INFIX_EQUAL;
		if (sclexer_read_tok(&tok, &lexer->self))
			return read_tok_failed(&lexer->self);
		return LEXER_CONTINUE;
	case '>':
		LAST_TOK(lexer)->type = TOK_BLOCK_START;
		if (sclexer_read_tok(&tok, &lexer->self))
			return read_tok_failed(&lexer->self);
		return handle_block(lexer);
	}
	return LEXER_CONTINUE;
}

enum LEXER_RESULT handle_sym_equal_suffix(struct lexer *lexer)
{
	struct sclexer_tok tok;
	if (lexer->self.cur[0] != '=')
		return LEXER_CONTINUE;
	if (sclexer_read_tok(&tok, &lexer->self))
		return read_tok_failed(&lexer->self);
	LAST_TOK(lexer)->type++;
	return LEXER_CONTINUE;
}

enum LEXER_RESULT handle_sym_not(struct lexer *lexer)
{
	struct sclexer_tok tok;
	if (lexer->self.cur[0] != '=')
		return LEXER_CONTINUE;
	if (sclexer_read_tok(&tok, &lexer->self))
		return read_tok_failed(&lexer->self);
	LAST_TOK(lexer)->type = TOK_INFIX_NOT_EQUAL;
	return LEXER_CONTINUE;
}

void init_block(struct lexer_block *block)
{
	assert(block);
	block->begin = NULL;
	block->end = NULL;
	block->indent = 0;
}

void join_eol(struct lexer_block *block)
{
	struct lexer_tok *eol;
	assert(block);
	if (block->end == NULL || block->end->type == TOK_EOL)
		return;
	eol = calloc(1, sizeof(*eol));
	eol->type = TOK_EOL;
	join_tok(block, eol);
}

void join_tok(struct lexer_block *block, struct lexer_tok *tok)
{
	assert(block && tok);
	tok->next = NULL;
	tok->prev = block->end;
	if (tok->prev) {
		tok->prev->next = tok;
	} else {
		block->begin = tok;
	}
	block->end = tok;
}

enum LEXER_RESULT parse_block_line(struct lexer *lexer)
{
	if (read_indent(lexer->self.buf) != lexer->cur_block->indent)
		return LEXER_EOB;
	return read_tok_in_line(lexer);
}

uint16_t read_indent(char *line)
{
	uint16_t res = 0;
	while (line[res] == '\t')
		res++;
	return res;
}

enum LEXER_RESULT read_tok(struct lexer *lexer)
{
	struct lexer_tok *res;
	struct sclexer_tok tok;
	if (sclexer_read_tok(&tok, &lexer->self))
		return read_tok_failed(&lexer->self);
	switch (tok.type) {
	case SCLEXER_TOK_TYPE_EMPTY: return LEXER_EOF;
	case SCLEXER_TOK_TYPE_INT:
		res = calloc(1, sizeof(*res));
		res->type = TOK_INT;
		res->data.uint = tok.type_data.uint;
		join_tok(lexer->cur_block, res);
		break;
	case SCLEXER_TOK_TYPE_INT_NEG:
		res = calloc(1, sizeof(*res));
		res->type = TOK_INT_NEG;
		res->data.sint = tok.type_data.sint;
		join_tok(lexer->cur_block, res);
		break;
	case SCLEXER_TOK_TYPE_SPACE:
		if (tok.type_data.c == '\n')
			return LEXER_EOL;
		break;
	case SCLEXER_TOK_TYPE_STR:
		handle_ident(lexer, &tok);
		break;
	case SCLEXER_TOK_TYPE_SYM: return handle_sym(lexer, tok.type_data.c);
	}
	return LEXER_CONTINUE;
}

enum LEXER_RESULT read_tok_failed(struct sclexer *sclexer)
{
	printf(ERR_FMT"failed to read tok with 'sclexer_read_tok'\n",
			ERR_FMT_ARG_REF(sclexer));
	return LEXER_ERR;
}

enum LEXER_RESULT read_tok_in_line(struct lexer *lexer)
{
	enum LEXER_RESULT ret;
	while ((ret = read_tok(lexer)) != LEXER_EOL) {
		if (ret != LEXER_CONTINUE)
			return ret;
	}
	join_eol(lexer->cur_block);
	if (sclexer_get_line(&lexer->self) == EOF)
		return LEXER_EOF;
	return LEXER_CONTINUE;
}

int try_handle_keyword(struct lexer *lexer, struct sclexer_tok *tok)
{
	struct lexer_tok *res = calloc(1, sizeof(*res));
	res->data.keyword = keyword_get(
			tok->type_data.str.s,
			tok->type_data.str.len);
	if (res->data.keyword == -1)
		return 0;
	res->type = TOK_KEYWORD;
	join_tok(lexer->cur_block, res);
	return 1;
}

void lexer_eat_tok(struct lexer_block *blk)
{
	struct lexer_tok *tok;
	assert(blk);
	tok = blk->begin;
	blk->begin = blk->begin->next;
	lexer_free_tok(tok);
}

int lexer_eat_tok_with(enum TOK_TYPE expect, struct lexer_block *blk)
{
	assert(blk);
	if (blk->begin->type != expect)
		return 1;
	lexer_eat_tok(blk);
	return 0;
}

void lexer_free_tok(struct lexer_tok *self)
{
	if (self->prev)
		self->prev->next = self->next;
	if (self->next)
		self->next->prev = self->prev;
	switch (self->type) {
	case TOK_STRING:
	case TOK_IDENT:
		str_free_noself(&self->data.s);
		break;
	default: break;
	}
	free(self);
}

const char *lexer_get_tok_str(enum TOK_TYPE type)
{
	if (type >= TOK_COUNT)
		return NULL;
	return tok_str[type];
}

void lexer_init(struct lexer *lexer)
{
	assert(lexer);
	lexer->cur_block = &lexer->root;
	lexer->self.stop_chrs_in_str = "!@#$%^&*[]()+=;:'\",<.>/?";
	init_block(lexer->cur_block);
}

int lexer_parse_file(struct lexer *lexer, const char *fpath)
{
	struct lexer_tok *eof;
	enum LEXER_RESULT ret;
	assert(lexer && fpath);
	sclexer_init(fpath, &lexer->self);
	if (sclexer_get_line(&lexer->self) == EOF)
		goto end;
	while ((ret = read_tok_in_line(lexer)) != LEXER_EOF) {
		if (ret != LEXER_CONTINUE)
			goto err_end;
	}
end:
	eof = calloc(1, sizeof(*eof));
	eof->type = TOK_EOF;
	join_tok(lexer->cur_block, eof);
	sclexer_end(&lexer->self);
	return 0;
err_end:
	sclexer_end(&lexer->self);
	return 1;
}

/**
 * Just for debug, so don't care these shits.
 * Otherwise, You will feel a f**king sheep f**king your ass. <3
 * Indent will touch your eyes like this sheep.
 * Maybe we should stir up these thits?  :/
 * Rewrite amc made me crazy, so let's write the fucking shits and throw them
 * to internet.
 * (What am I f**king say? I use 5 minutes, just write these comment. :>)
 */
void lexer_print_block(struct lexer_block *blk)
{
	lexer_tok_for_each(cur, blk->begin) {
		switch (cur->type) {
		case TOK_IDENT:
			printf("<%s: '%s'>\n",
					tok_str[cur->type],
					cur->data.s.s);
			break;
		case TOK_KEYWORD:
			printf("<%s: '%s'>\n",
					tok_str[cur->type],
					keyword_get_str(cur->data.keyword));
			break;
		case TOK_STRING:
			printf("<%s: '%s'>\n",
					tok_str[cur->type],
					cur->data.s.s);
			break;
		case TOK_BRACE_L:
		case TOK_BRACE_R:
		case TOK_BRACKET_L:
		case TOK_BRACKET_R:
		case TOK_PAREN_L:
		case TOK_PAREN_R:
			printf("<\x1b[32m%s\x1b[0m>\n", tok_str[cur->type]);
			break;
		case TOK_BLOCK_START:
			printf("<\x1b[33mBLOCK_START\x1b[0m>\n");
			if (cur->type == TOK_BLOCK_START)
				lexer_print_block(&cur->data.block);
			printf("<\x1b[33mBLOCK_END\x1b[0m>\n");
			break;
		default:
			printf("<%s>\n", tok_str[cur->type]);
			break;
		}
	}
}
