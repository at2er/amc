/* This file is part of amc.
   SPDX-License-Identifier: GPL-3.0-or-later
*/
#include "literal.h"
#include "../die.h"
#include "../expr.h"
#include "../lexer.h"
#include "../panic.h"
#include "../parser.h"
#include "../type.h"
#include <assert.h>
#include <sclexer.h>
#include <stdbool.h>
#include <stdlib.h>

static bool check_expr_end(struct sclexer_tok *tok);
static struct yz_expr *merge_expr(struct yz_expr *parent,
		struct yz_expr *current);
static struct yz_expr *merge_parent_to_lhs(struct yz_expr *parent,
		struct yz_expr *current);
static struct yz_expr *merge_to_parent_rhs(struct yz_expr *parent,
		struct yz_expr *current);
static int parse_binary_expr(struct parser *parser,
		struct yz_expr **parent,
		uint8_t first);

enum EXPR_BINDING_POWER_LEVEL {
	TERM_LITERAL = UCHAR_MAX,
	BINARY_DIV_AND_MUL = 3,
	BINARY_ADD_AND_SUB = 2,
	BINARY_ASSIGN = 1
};

static const uint8_t expr_binding_power[YZ_EXPR_TYPE_COUNT] = {
	[YZ_EXPR_TERM_LITERAL] = UCHAR_MAX,

	[YZ_EXPR_BINARY_MUL] = BINARY_DIV_AND_MUL,
	[YZ_EXPR_BINARY_DIV] = BINARY_DIV_AND_MUL,

	[YZ_EXPR_BINARY_ADD] = BINARY_ADD_AND_SUB,
	[YZ_EXPR_BINARY_SUB] = BINARY_ADD_AND_SUB,

	[YZ_EXPR_BINARY_ASSIGN] = BINARY_ASSIGN,
	[YZ_EXPR_BINARY_ADD_ASSIGN] = BINARY_ASSIGN,
	[YZ_EXPR_BINARY_DIV_ASSIGN] = BINARY_ASSIGN,
	[YZ_EXPR_BINARY_MUL_ASSIGN] = BINARY_ASSIGN,
	[YZ_EXPR_BINARY_SUB_ASSIGN] = BINARY_ASSIGN,
};

bool check_expr_end(struct sclexer_tok *tok)
{
	if (tok->kind != SCLEXER_SYMBOL)
		return true;
	if (tok->data.symbol < LEXER_SYM_INFIX_ASSIGN)
		return true;
	if (tok->data.symbol > LEXER_SYM_INFIX_SUB_ASSIGN)
		return true;
	return false;
}

struct yz_expr *merge_expr(struct yz_expr *parent,
		struct yz_expr *current)
{
	uint8_t current_power, parent_power;
	assert(parent && current);
	current_power = expr_binding_power[current->type];
	parent_power  = expr_binding_power[parent->type];
	if (parent_power > current_power)
		return merge_to_parent_rhs(parent, current);
	return merge_parent_to_lhs(parent, current);
}

struct yz_expr *merge_parent_to_lhs(struct yz_expr *parent,
		struct yz_expr *current)
{
	current->data.binary.lhs =
		calloc(1, sizeof(*current->data.binary.rhs));
	current->data.binary.lhs->data.expr = parent;
	current->data.binary.lhs->type.type = YZ_EXPR;
	current->data.binary.lhs->type.data.self = parent->sum_type;
	return current;
}

struct yz_expr *merge_to_parent_rhs(struct yz_expr *parent,
		struct yz_expr *current)
{
	current->data.binary.lhs = parent->data.binary.rhs;
	parent->data.binary.rhs =
		calloc(1, sizeof(*parent->data.binary.rhs));
	parent->data.binary.rhs->data.expr = current;
	parent->data.binary.rhs->type.type = YZ_EXPR;
	parent->data.binary.rhs->type.data.self = current->sum_type;
	return parent;
}

int parse_binary_expr(struct parser *parser,
		struct yz_expr **parent,
		uint8_t first)
{
	struct yz_expr *expr = *parent;
	if (!first)
		expr = calloc(1, sizeof(*expr));
	expr->data.binary.lhs = expr->data.term;
	expr->type = map_tok_type2expr_type(CUR_TOK(parser).data.symbol);
	if (expr->type == -1)
		goto panic_map_tok;
	eat_tok(parser);
	expr->data.binary.rhs = parse_literal(parser);

	if (first) {
		expr->sum_type = (struct yz_type*)
			convert_type_implicity(
				&expr->data.binary.lhs->type,
				&expr->data.binary.rhs->type);
		if (!expr->sum_type)
			goto err_implicit_convertion_type;
		return 0;
	}

	expr->sum_type = &expr->data.binary.rhs->type;
	if ((*parent = merge_expr(*parent, expr)) == NULL)
		die(PANIC_FMT"failed to merge expr\n", PANIC_FMT_ARG);

	(*parent)->sum_type = (struct yz_type*)
		convert_type_implicity(
			&(*parent)->data.binary.lhs->type,
			&(*parent)->data.binary.rhs->type);
	if (*parent == NULL)
		goto err_implicit_convertion_type;

	return 0;
panic_map_tok:
	die(PANIC_FMT"failed to map token type to expr type\n",
			PANIC_FMT_ARG);
	return 1;
err_implicit_convertion_type:
	printf(ERR_FMT"unsupport implicit convertions type '%s' and '%s'\n",
			ERR_FMT_ARG(parser),
			type_get_str(expr->data.binary.lhs->type.type),
			type_get_str(expr->data.binary.rhs->type.type));
	return 1;
}

struct yz_expr *parse_expr(struct parser *parser)
{
	uint8_t is_first = 1;
	struct yz_expr *result = calloc(1, sizeof(*result));
	result->data.term = parse_literal(parser);
	if (!result->data.term)
		goto err_free_result;
	result->type = YZ_EXPR_TERM_LITERAL;
	result->sum_type = &result->data.term->type;
	while (!check_expr_end(&CUR_TOK(parser))) {
		if (parse_binary_expr(parser, &result, is_first))
			goto err_free_result;
		is_first = 0;
	}
	return result;
err_free_result:
	free_yz_expr(result);
	return NULL;
}
