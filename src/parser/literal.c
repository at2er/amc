/* This file is part of amc.
   SPDX-License-Identifier: GPL-3.0-or-later
*/
#include "expr.h"
#include "fn.h"
#include "literal.h"
#include "../die.h"
#include "../fn.h"
#include "../lexer.h"
#include "../literal.h"
#include "../panic.h"
#include "../parser.h"
#include <sclexer.h>
#include <sctrie.h>
#include <stdio.h>
#include <stdlib.h>

static struct yz_literal *parse_expr_literal(struct parser *parser);
static struct yz_literal *parse_func_arg_literal(struct parser *parser,
		struct yz_ident *arg);
static struct yz_literal *parse_func_call_literal(struct parser *parser,
		struct yz_func *callee);
static struct yz_literal *parse_ident_literal(struct parser *parser,
		struct yz_symbol *sym);
static struct yz_literal *parse_number_literal(struct parser *parser);
static struct yz_literal *parse_symbol_literal(struct parser *parser);

struct yz_literal *parse_expr_literal(struct parser *parser)
{
	struct yz_literal *self = calloc(1, sizeof(*self));
	eat_tok(parser);
	self->data.expr = parse_expr(parser);
	self->type.data.self = self->data.expr->sum_type;
	self->type.type = YZ_EXPR;
	if (!eat_tok_with_sym(LEXER_SYM_PAREN_R, parser))
		goto err_expr_not_end;
	return self;
err_expr_not_end:
	free_yz_literal(self);
	printf(ERR_FMT"expression not end\n", ERR_FMT_ARG(parser));
	return NULL;
}

struct yz_literal *parse_func_arg_literal(struct parser *parser,
		struct yz_ident *arg)
{
	struct yz_literal *self = calloc(1, sizeof(*self));
	self->data.ident = arg;
	self->type.type = YZ_IDENT_LITERAL;
	self->type.data.self = &self->data.ident->type;
	return self;
}

struct yz_literal *parse_func_call_literal(struct parser *parser,
		struct yz_func *callee)
{
	struct yz_literal *self = calloc(1, sizeof(*self));
	self->data.func_call = parse_func_call(parser, callee);
	self->type.type = YZ_FUNC_CALL;
	self->type.data.self = &self->data.func_call->callee->type;
	if (!self->data.func_call)
		goto err_free_self;
	return self;
err_free_self:
	free_yz_literal(self);
	return NULL;
}

struct yz_literal *parse_ident_literal(struct parser *parser,
		struct yz_symbol *sym)
{
	struct yz_literal *self = calloc(1, sizeof(*self));
	self->data.ident = sym->data.yz_ident;
	self->type.type = YZ_IDENT_LITERAL;
	self->type.data.self = &self->data.ident->type;
	return self;
}

struct yz_literal *parse_number_literal(struct parser *parser)
{
	struct yz_literal *self = calloc(1, sizeof(*self));
	self->data.sint = CUR_TOK(parser).data.sint;
	if (CUR_TOK(parser).kind == SCLEXER_INT) {
		self->type.type = get_integer_literal_type(false,
				self->data.uint);
	} else if (CUR_TOK(parser).kind == SCLEXER_INT_NEG) {
		self->type.type = get_integer_literal_type(true,
				self->data.sint);
	} else {
		goto panic_unsupport_tok;
	}
	eat_tok(parser);
	return self;
panic_unsupport_tok:
	die(LOC_FMT PANIC_FMT"failed to parse token '%s'\n",
			LOC_FMT_ARG(parser), PANIC_FMT_ARG,
			get_token_str(&CUR_TOK(parser)));
	free_yz_literal(self);
	return NULL;
}

struct yz_literal *parse_symbol_literal(struct parser *parser)
{
	struct yz_symbol *sym = find_symbol_in_parser(parser,
			TOK_IDENT_UNWRAP(CUR_TOK(parser)));
	if (!sym)
		goto err_sym_not_found;
	switch (sym->type) {
	case YZ_FUNC:
		return parse_func_call_literal(parser, sym->data.yz_func);
	case YZ_FUNC_ARG:
		return parse_func_arg_literal(parser, sym->data.yz_func_arg);
	case YZ_IDENT:
		return parse_ident_literal(parser, sym);
	default: break;
	}
	
	die(PANIC_FMT"unprocessed symbol '%.*s'\n", PANIC_FMT_ARG,
			(int)CUR_TOK(parser).data.str.len,
			CUR_TOK(parser).data.str.begin);
	return NULL;
err_sym_not_found:
	printf(ERR_FMT"symbol '%.*s' not found\n", ERR_FMT_ARG(parser),
			(int)CUR_TOK(parser).data.str.len,
			CUR_TOK(parser).data.str.begin);
	return NULL;
}

struct yz_literal *parse_literal(struct parser *parser)
{
	switch (CUR_TOK(parser).kind) {
	case SCLEXER_IDENT:
		return parse_symbol_literal(parser);
	case SCLEXER_INT:
	case SCLEXER_INT_NEG:
		return parse_number_literal(parser);
	case SCLEXER_SYMBOL:
		if (!CUR_TOK_IS_SYM(parser, LEXER_SYM_PAREN_L))
			break;
		return parse_expr_literal(parser);
	default: break;
	}
	return NULL;
}
