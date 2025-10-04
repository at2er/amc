/* This file is part of amc.
   SPDX-License-Identifier: GPL-3.0-or-later
*/
#include "compiler/fn.h"
#include "die.h"
#include "fn.h"
#include "lexer.h"
#include "object.h"
#include "panic.h"
#include "parser.h"
#include "parser/block.h"
#include "parser/fn.h"
#include "parser/type.h"
#include "parser/utils.h"
#include "module.h"
#include "symbol.h"
#include <assert.h>
#include <sctrie.h>

static enum LIST_PARSER_RESULT get_func_arg(struct parser *parser,
		struct yz_func *self);
static int get_func_args(struct parser *parser, struct yz_func *self);
static struct yz_symbol **get_func_dest_symbol_tree(struct parser *parser,
		enum YZ_SCOPE_TYPE scope_of);
static int get_func_name(struct parser *parser, str *result);
static int get_func_result_type(struct parser *parser, struct yz_func *self);
static int get_func_sign(struct parser *parser, struct yz_func *self);
static int parse_func_body(struct parser *parser, const struct yz_func *self);

enum LIST_PARSER_RESULT get_func_arg(struct parser *parser,
		struct yz_func *self)
{
	struct yz_ident *ident = calloc(1, sizeof(*ident));
	struct yz_symbol *wrapper;
	if (parser_get_ident_with_type(parser, &ident->name, &ident->type))
		return LIST_ERR;
	if (ident->type.type == YZ_VOID)
		goto err_void;
	ident->object = create_yz_object();
	wrapper = sctrie_append_elem(parser->symbols, sizeof(*wrapper),
			STR_UNWRAP(ident->name));
	wrapper->type = YZ_FUNC_ARG;
	wrapper->data.yz_func_arg = ident;
	self->argc++;
	self->args = realloc(self->args, self->argc * sizeof(*self->args));
	self->args[self->argc - 1] = ident;
#define CASE(TYPE, RESULT) \
	case TYPE: lexer_eat_tok(parser->lexer.cur_block); return RESULT
	switch (CUR_TOK(parser)->type) {
	CASE(TOK_COMMA,   LIST_CONTINUE);
	CASE(TOK_PAREN_R, LIST_END);
	default: break;
	}
#undef CASE
	return LIST_ERR;
err_void:
	printf(ERR_FMT"function argument type cannot be 'void'\n",
			ERR_FMT_ARG(parser));
	return LIST_ERR;
}

int get_func_args(struct parser *parser, struct yz_func *self)
{
	enum LIST_PARSER_RESULT ret;
	assert(CUR_TOK(parser)->type == TOK_PAREN_L);
	lexer_eat_tok(parser->lexer.cur_block);
	while ((ret = get_func_arg(parser, self)) != LIST_END) {
		if (ret == LIST_ERR)
			return 1;
	}
	return 0;
}

int get_func_name(struct parser *parser, str *result)
{
	assert(result);
	if (CUR_TOK(parser)->type != TOK_IDENT)
		goto err_miss_func_name;
	*result = CUR_TOK(parser)->data.s;
	CUR_TOK(parser)->data.s.s = NULL;
	lexer_eat_tok(parser->lexer.cur_block);
	return 0;
err_miss_func_name:
	printf(ERR_FMT"miss function name\n", ERR_FMT_ARG(parser));
	return 1;
}

struct yz_symbol **get_func_dest_symbol_tree(struct parser *parser,
		enum YZ_SCOPE_TYPE scope_of)
{
	switch (scope_of) {
	case YZ_SCOPE_PRIVATE: return parser->symbols;
	case YZ_SCOPE_PUBLIC:  return parser->cur_mod->symbols;
	default: break;
	}
	die(PANIC_FMT"failed to get scope '%d'\n", PANIC_FMT_ARG, scope_of);
	return NULL;
}

int get_func_result_type(struct parser *parser, struct yz_func *self)
{
	if (lexer_eat_tok_with(TOK_COLON, parser->lexer.cur_block))
		goto err_unexpected_tok;
	if (CUR_TOK(parser)->type != TOK_IDENT)
		return 1;
	self->type.type = type_get(STR_UNWRAP(CUR_TOK(parser)->data.s));
	if (self->type.type == -1)
		return 1;
	lexer_eat_tok(parser->lexer.cur_block);
	return 0;
err_unexpected_tok:
	printf(ERR_FMT"miss function result type, unexpected token: '%s'\n",
			ERR_FMT_ARG(parser),
			lexer_get_tok_str(CUR_TOK(parser)->type));
	return 1;
}

int get_func_sign(struct parser *parser, struct yz_func *self)
{
	if (CUR_TOK(parser)->type == TOK_PAREN_L)
		if (get_func_args(parser, self))
			return 1;
	return get_func_result_type(parser, self);
}

int parse_func_body(struct parser *parser, const struct yz_func *self)
{
	if (CUR_TOK(parser)->type == TOK_EOL)
		return 0;
	if (CUR_TOK(parser)->type != TOK_BLOCK_START)
		goto err_miss;
	if (parse_block(parser))
		return 1;
	lexer_eat_tok(parser->lexer.cur_block);
	return 0;
err_miss:
	printf(ERR_FMT"miss function body, unexpected token: '%s'\n",
			ERR_FMT_ARG(parser),
			lexer_get_tok_str(CUR_TOK(parser)->type));
	return 0;
}

int parse_func_def(struct parser *parser, enum YZ_SCOPE_TYPE scope_of)
{
	struct yz_func *self = calloc(1, sizeof(*self));
	struct yz_symbol *wrapper, **dst;
	if (get_func_name(parser, &self->name))
		goto err_free_self;
	self->path = self->name;
	if (get_func_sign(parser, self))
		goto err_free_self;
	if ((dst = get_func_dest_symbol_tree(parser, scope_of)) == NULL)
		goto err_free_self;
	wrapper = sctrie_append_elem(dst, sizeof(*wrapper),
			STR_UNWRAP(self->name));
	wrapper->type = YZ_FUNC;
	wrapper->data.yz_func = self;
	if (CUR_TOK(parser)->type == TOK_EOL)
		return 0;
	if (CUR_TOK(parser)->type != TOK_BLOCK_START)
		goto err_unexpected_tok;
	compile_fn_def(parser->mcb, self);
	if (parse_func_body(parser, self))
		goto err_free_self;
	compile_fn_def_end(parser->mcb);
	return 0;
err_free_self:
	free_yz_func(self);
	return 1;
err_unexpected_tok:
	printf(ERR_FMT"unexpected token: '%s' on function declaration\n",
			ERR_FMT_ARG(parser),
			lexer_get_tok_str(CUR_TOK(parser)->type));
	return 1;
}
