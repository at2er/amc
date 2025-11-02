/* This file is part of amc.
   SPDX-License-Identifier: GPL-3.0-or-later
*/
#include "block.h"
#include "fn.h"
#include "literal.h"
#include "utils.h"
#include "../compiler/fn.h"
#include "../die.h"
#include "../fn.h"
#include "../lexer.h"
#include "../object.h"
#include "../panic.h"
#include "../parser.h"
#include "../module.h"
#include "../symbol.h"
#include "../type.h"
#include <assert.h>
#include <sclexer.h>
#include <sctrie.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

static enum PARSER_LIST_RESULT check_func_arg_end(struct parser *parser);
static int get_func_arg(struct parser *parser, struct yz_func *self);
static int get_func_args(struct parser *parser, struct yz_func *self);
static int get_func_call_arg(struct parser *parser,
		struct yz_func_call *self,
		uint16_t index);
static struct yz_symbol *get_func_dest_symbol_tree(struct parser *parser,
		enum YZ_SCOPE_TYPE scope_of);
static int get_func_name(struct parser *parser, str *result);
static int get_func_result_type(struct parser *parser, struct yz_func *self);
static int get_func_sign(struct parser *parser, struct yz_func *self);

enum PARSER_LIST_RESULT check_func_arg_end(struct parser *parser)
{
	if (CUR_TOK(parser).kind != SCLEXER_SYMBOL)
		return LIST_CONTINUE;
	switch (CUR_TOK(parser).data.symbol) {
	case LEXER_SYM_COMMA:   eat_tok(parser); return LIST_CONTINUE;
	case LEXER_SYM_PAREN_R: eat_tok(parser); return LIST_END;
	default: break;
	}
	return LIST_ERR;
}

int get_func_arg(struct parser *parser, struct yz_func *self)
{
	struct yz_ident *ident = calloc(1, sizeof(*ident));
	struct yz_symbol *wrapper;
	if (get_ident_with_type(parser, &ident->name, &ident->type))
		return LIST_ERR;
	if (ident->type.type == YZ_VOID)
		goto err_void;

	ident->object = create_yz_object();

	wrapper = append_symbol(&parser->symbols, &ident->name);
	wrapper->type = YZ_FUNC_ARG;
	wrapper->data.yz_func_arg = ident;
	append_symbol_to_scope(wrapper, parser->cur_scope);

	self->argc++;
	self->args = realloc(self->args, self->argc * sizeof(*self->args));
	self->args[self->argc - 1] = ident;

	return 0;
err_void:
	printf(ERR_FMT"function argument type cannot be 'void'\n",
			ERR_FMT_ARG(parser));
	return 1;
}

int get_func_args(struct parser *parser, struct yz_func *self)
{
	enum PARSER_LIST_RESULT ret;
	eat_tok_with_sym(LEXER_SYM_PAREN_L, parser);
	while ((ret = check_func_arg_end(parser)) != LIST_END) {
		if (ret == LIST_ERR)
			return 1;
		if (get_func_arg(parser, self))
			return 1;
	}
	return 0;
}

int get_func_call_arg(struct parser *parser,
		struct yz_func_call *self,
		uint16_t index)
{
	const struct yz_ident *cur = self->callee->args[index];
	struct yz_literal *val;

	if (CUR_TOK(parser).kind == SCLEXER_EOL)
		goto err_too_few_arg;

	if (!(val = parse_literal(parser)))
		goto err_cannot_parse_func_arg;

	if (!convert_type_implicity(&cur->type, &val->type))
		goto err_implicit_convertion_type;

	self->args[index] = val;
	return 0;
err_too_few_arg:
	printf(ERR_FMT"too few arguments for function '%s'\n",
			ERR_FMT_ARG(parser),
			self->callee->name.s);
	return 1;
err_cannot_parse_func_arg:
	printf(ERR_FMT"failed to parse function argument\n",
			ERR_FMT_ARG(parser));
	return 1;
err_implicit_convertion_type:
	printf(ERR_FMT"unsupport implicit convertions type '%s' and '%s'\n",
			ERR_FMT_ARG(parser),
			type_get_str(cur->type.type),
			type_get_str(val->type.type));
	return 1;
}

int get_func_name(struct parser *parser, str *result)
{
	assert(result);
	if (CUR_TOK(parser).kind != SCLEXER_IDENT)
		goto err_miss_func_name;
	dup_tok_str2str(result, &CUR_TOK(parser));
	eat_tok(parser);
	return 0;
err_miss_func_name:
	printf(ERR_FMT"miss function name\n", ERR_FMT_ARG(parser));
	return 1;
}

struct yz_symbol *get_func_dest_symbol_tree(struct parser *parser,
		enum YZ_SCOPE_TYPE scope_of)
{
	switch (scope_of) {
	case YZ_SCOPE_PRIVATE: return &parser->symbols;
	case YZ_SCOPE_PUBLIC:  return &parser->cur_mod->symbols;
	default: break;
	}
	die(PANIC_FMT"failed to get scope '%d'\n", PANIC_FMT_ARG, scope_of);
	return NULL;
}

int get_func_result_type(struct parser *parser, struct yz_func *self)
{
	if (!eat_tok_with_sym(LEXER_SYM_COLON, parser))
		goto err_unexpected_tok;
	if (CUR_TOK(parser).kind != SCLEXER_IDENT)
		return 1;
	self->type.type = type_get(TOK_IDENT_UNWRAP(CUR_TOK(parser)));
	if (self->type.type == -1)
		return 1;
	eat_tok(parser);
	return 0;
err_unexpected_tok:
	printf(ERR_FMT"miss function result type, unexpected token: '%s'\n",
			ERR_FMT_ARG(parser),
			get_token_str(&CUR_TOK(parser)));
	return 1;
}

int get_func_sign(struct parser *parser, struct yz_func *self)
{
	if (get_func_args(parser, self))
		return 1;
	return get_func_result_type(parser, self);
}

struct yz_func_call *parse_func_call(struct parser *parser,
		struct yz_func *callee)
{
	struct yz_func_call *result;
	if (!eat_tok_with_kind(SCLEXER_IDENT, parser))
		return NULL;
	result = calloc(1, sizeof(*result));
	result->callee = callee;
	result->argc = callee->argc;
	result->args = calloc(result->argc, sizeof(*result->args));
	for (uint16_t i = 0; i < result->argc; i++) {
		if (get_func_call_arg(parser, result, i))
			goto err_free_result;
	}
	return result;
err_free_result:
	free_yz_func_call(result);
	return NULL;
}

int parse_func_def(struct parser *parser, enum YZ_SCOPE_TYPE scope_of)
{
	struct yz_func *self = calloc(1, sizeof(*self));
	struct yz_symbol *wrapper, *dst_sym_tree;

	parser->cur_scope = create_yz_scope(parser->cur_scope);

	if (get_func_name(parser, &self->name))
		goto err_free_self;
	self->path = self->name;
	if (get_func_sign(parser, self))
		goto err_free_self;

	dst_sym_tree = get_func_dest_symbol_tree(parser, scope_of);
	if (dst_sym_tree == NULL)
		goto err_free_self;

	wrapper = append_symbol(dst_sym_tree, &self->name);
	wrapper->type = YZ_FUNC;
	wrapper->data.yz_func = self;

	if (CUR_TOK(parser).kind == SCLEXER_EOL)
		return 0;
	if (!CUR_TOK_IS_SYM(parser, LEXER_SYM_BLOCK_ENTER))
		goto err_miss_func_body;

	compile_func_def(&parser->mcb, self);
	parser->cur_func = self;
	if (parse_block(parser))
		goto err_free_self;

	compile_func_def_end(&parser->mcb);
	parser->cur_func = NULL;
	parser->cur_scope = drop_scope(parser->cur_scope);
	return 0;
err_free_self:
	free_yz_func(self);
	return 1;
err_miss_func_body:
	printf(ERR_FMT"function body not found in definition\n",
			ERR_FMT_ARG(parser));
	return 1;
}
