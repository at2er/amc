/* This file is part of amc.
   SPDX-License-Identifier: GPL-3.0-or-later
*/
#include "include/expr.h"
#include "include/lexer.h"
#include "include/op.h"
#include "include/val.h"
#include "../include/backend.h"
#include "../include/parser.h"
#include <sclexer.h>
#include <stdlib.h>
#include <string.h>

struct expr_context {
	struct parser *parser;

	unsigned int first:1, top:1;
};

struct expr_operator {
	int power;
	enum OP_ID id;
};

static const struct expr_operator operators[TOK_TYPE_COUNT] = {
	[TOK_TYPE_OP_MUL] = {5, OP_MUL},
	[TOK_TYPE_OP_DIV] = {5, OP_DIV},
	[TOK_TYPE_OP_ADD] = {4, OP_ADD},
	[TOK_TYPE_OP_SUB] = {4, OP_SUB},

	[TOK_TYPE_OP_EQUAL]         = {3, OP_EQ},
	[TOK_TYPE_OP_NOT_EQUAL]     = {3, OP_NE},
	[TOK_TYPE_OP_LESS]          = {3, OP_LT},
	[TOK_TYPE_OP_LESS_EQUAL]    = {3, OP_LE},
	[TOK_TYPE_OP_GREATER]       = {3, OP_GT},
	[TOK_TYPE_OP_GREATER_EQUAL] = {3, OP_GE},

	[TOK_TYPE_OP_ASSIGN]     = {1, OP_ASSIGN    },
	[TOK_TYPE_OP_ADD_ASSIGN] = {1, OP_ASSIGN_ADD},
	[TOK_TYPE_OP_DIV_ASSIGN] = {1, OP_ASSIGN_DIV},
	[TOK_TYPE_OP_MUL_ASSIGN] = {1, OP_ASSIGN_MUL},
	[TOK_TYPE_OP_SUB_ASSIGN] = {1, OP_ASSIGN_SUB}
};

static const enum OP_ID unary_ops[UCHAR_MAX] = {
	['*'] = OP_EXTRACT_VAL,
	['&'] = OP_GET_ADDR
};

static int expr_append(struct expr **parent, struct expr *expr);
static int expr_check_end(struct expr_context *self, struct lexer_tok *tok);
static int expr_merge_prev_rval(struct expr *parent, struct expr *expr);
static int expr_op(struct expr_context *self, struct expr *expr,
		struct lexer_tok *tok);
static int expr_op_term(struct expr_context *self, struct expr **parent);

int expr_append(struct expr **parent, struct expr *expr)
{
	expr->vall->data.expr = *parent;
	expr->vall->type.v = expr->vall->data.expr;
	expr->vall->type.type = AMC_EXPR;
	expr->sum_type = yz_type_max(&expr->vall->type, &expr->valr->type);
	if (expr->sum_type == NULL)
		return 1;
	*parent = expr;
	return 0;
}

int expr_check_end(struct expr_context *self, struct lexer_tok *tok)
{
	if (self->top) {
		if (tok->type == TOK_TYPE_NEXT_LINE)
			return 1;
		return 0;
	}
	if (tok->type == TOK_TYPE_PAREN_R)
		return 1;
	return 0;
}

int expr_merge_prev_rval(struct expr *parent, struct expr *expr)
{
	expr->vall->data = parent->valr->data;
	expr->vall->type = parent->valr->type;
	expr->sum_type = yz_type_max(&expr->vall->type, &expr->valr->type);
	if (expr->sum_type == NULL)
		return 1;
	parent->valr->data.expr = expr;
	parent->valr->type.type = AMC_EXPR;
	parent->valr->type.v = parent->valr->data.expr;
	parent->sum_type = yz_type_max(&parent->vall->type,
			&parent->valr->type);
	if (parent->sum_type == NULL)
		return 1;
	return 0;
}

int expr_op(struct expr_context *self, struct expr *expr, struct lexer_tok *tok)
{
	if (!REGION_INT(tok->type, TOK_TYPE_OP_ASSIGN, TOK_TYPE_OP_SUB_ASSIGN))
		return LEXER_RESULT_FAULT;
	expr->op = operators[tok->type].id;
	expr->op_power = operators[tok->type].power;
	if (REGION_INT(expr->op, OP_ASSIGN, OP_ASSIGN_SUB)) {
		if (op_assign(self->parser, expr))
			return LEXER_RESULT_FAULT;
		return LEXER_RESULT_END;
	}
	return LEXER_RESULT_CONTINUE;
}

int expr_op_term(struct expr_context *self, struct expr **parent)
{
	struct expr *expr = *parent;
	int ret = 0;
	struct lexer_tok tok;
	if (lexer_read_tok(&tok, &self->parser->lexer))
		return LEXER_RESULT_FAULT;
	if (expr_check_end(self, &tok))
		return LEXER_RESULT_END;
	if (expr->valr->type.type != AMC_ERR_TYPE) {
		expr = calloc(1, sizeof(*expr));
		expr->vall = calloc(1, sizeof(*expr->vall));
		expr->valr = calloc(1, sizeof(*expr->valr));
	}
	if ((ret = expr_op(self, expr, &tok)) == LEXER_RESULT_FAULT)
		return LEXER_RESULT_FAULT;
	if (parse_val(self->parser, expr->valr))
		return LEXER_RESULT_FAULT;
	if (expr == *parent)
		return LEXER_RESULT_CONTINUE;
	if ((*parent)->op_power < expr->op_power) {
		// 1 + 2 * 3
		if (expr_merge_prev_rval(*parent, expr))
			return LEXER_RESULT_FAULT;
	} else {
		// 1 * 2 + 1
		if (expr_append(parent, expr))
			return LEXER_RESULT_FAULT;
	}
	return LEXER_RESULT_CONTINUE;
}

int expr_apply(struct parser *parser, struct expr *e)
{
	if (EXPR_IS_SINGLE_TERM(e)) {
		if (e->vall->type.type == AMC_EXPR)
			return expr_apply(parser, e->vall->data.expr);
		return 0;
	}
	if (EXPR_IS_UNARY(e))
		return op_apply_special(parser, e);
	if (e->vall->type.type == AMC_EXPR) {
		if (expr_apply(parser, e->vall->data.expr))
			return 1;
	}
	if (e->valr->type.type == AMC_EXPR) {
		if (expr_apply(parser, e->valr->data.expr))
			return 1;
	}
	if (e->op >= OP_SPECIAL_START)
		return op_apply_special(parser, e);
	if (backend_call(ops[e->op])(e))
		goto err_backend_failed;
	return 0;
err_backend_failed:
	printf(LEXER_ERR_FMT"Backend call failed\n",
			LEXER_ERR_FMT_ARG(parser->lexer));
	backend_stop(BE_STOP_SIGNAL_ERR);
	return 1;
}

struct expr *parse_expr(struct parser *parser, int top)
{
	struct expr *expr = calloc(1, sizeof(*expr));
	int ret = 0;
	struct expr_context self = {
		.parser = parser,
		.first = 1, .top = top
	};
	expr->vall = calloc(1, sizeof(*expr->vall));
	expr->valr = calloc(1, sizeof(*expr->valr));
	if (parse_val(parser, expr->vall))
		goto err_free_expr;
	self.first = 0;
	while ((ret = expr_op_term(&self, &expr)) != LEXER_RESULT_END) {
		if (ret == LEXER_RESULT_FAULT)
			goto err_free_expr;
	}
	return expr;
err_free_expr:
	free_expr(expr);
	return NULL;
}

struct expr *parse_expr_unary(struct parser *parser, char index)
{
	struct expr *res = calloc(1, sizeof(*res));
	res->op = unary_ops[(uint8_t)index];
	res->op_power = 10;
	res->valr = calloc(1, sizeof(*res->valr));
	return 0;
}
