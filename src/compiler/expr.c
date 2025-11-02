/* This file is part of amc.
   SPDX-License-Identifier: GPL-3.0-or-later
*/
#include "expr.h"
#include "fn.h"
#include "literal.h"
#include "operand.h"
#include "panic.h"
#include "../die.h"
#include "../literal.h"
#include "../type.h"
#include <assert.h>
#include <mcb/expr.h>
#include <mcb/mcb.h>
#include <mcb/operand.h>
#include <stdlib.h>

static struct mcb_expr *build_mcb_expr(const struct yz_expr *self);
static void compile_binary(struct mcb_context *mcb,
		struct mcb_operand *self,
		const struct yz_expr *src);
static enum MCB_EXPR_OPERATOR map_to_mcb_expr_op(enum YZ_EXPR_TYPE type);

struct mcb_expr *build_mcb_expr(const struct yz_expr *self)
{
	struct mcb_expr *result = calloc(1, sizeof(*result));
	build_mcb_expr_operand(&result->lhs, self->data.binary.lhs);
	build_mcb_expr_operand(&result->rhs, self->data.binary.rhs);
	result->op = map_to_mcb_expr_op(self->type);
	return result;
}

void compile_binary(struct mcb_context *mcb,
		struct mcb_operand *self,
		const struct yz_expr *src)
{
	struct mcb_expr *expr = build_mcb_expr(src);
	if (MCB_CALL(mcb, eval_expr)(mcb, self, expr))
		PANIC_MCB_CALL;
	mcb_free_expr(expr);
}

enum MCB_EXPR_OPERATOR map_to_mcb_expr_op(enum YZ_EXPR_TYPE type)
{
	switch (type) {
	case YZ_EXPR_BINARY_ADD:        return MCB_EXPR_OPERATOR_IS_ADD;
	case YZ_EXPR_BINARY_ADD_ASSIGN: return MCB_EXPR_OPERATOR_IS_ADD;
	case YZ_EXPR_BINARY_DIV:        return MCB_EXPR_OPERATOR_IS_DIV;
	case YZ_EXPR_BINARY_DIV_ASSIGN: return MCB_EXPR_OPERATOR_IS_DIV;
	case YZ_EXPR_BINARY_MUL:        return MCB_EXPR_OPERATOR_IS_MUL;
	case YZ_EXPR_BINARY_MUL_ASSIGN: return MCB_EXPR_OPERATOR_IS_MUL;
	case YZ_EXPR_BINARY_SUB:        return MCB_EXPR_OPERATOR_IS_SUB;
	case YZ_EXPR_BINARY_SUB_ASSIGN: return MCB_EXPR_OPERATOR_IS_SUB;
		break;
	default: break;
	}
	return -1;
}

void build_mcb_expr_operand(struct mcb_expr_operand *result,
		const struct yz_literal *literal)
{
	if (literal->type.type == YZ_EXPR) {
		result->type = MCB_EXPR_OPERAND_IS_EXPR;
		result->inner.expr = build_mcb_expr(literal->data.expr);
		return;
	} else if (literal->type.type == YZ_FUNC_CALL) {
		build_mcb_expr_func_call_operand(result, literal->data.func_call);
		return;
	}
	result->type = MCB_EXPR_OPERAND_IS_OPERAND;
	get_imm_from_literal(&result->inner.operand, literal);
}

void compile_expr(struct mcb_context *mcb,
		struct mcb_operand *result,
		const struct yz_expr *self)
{
	assert(result && self);
	if (self->type >= YZ_EXPR_BINARY_ADD
			&& self->type <= YZ_EXPR_BINARY_SUB_ASSIGN) {
		compile_binary(mcb, result, self);
		return;
	}
	switch (self->type) {
	case YZ_EXPR_TERM_LITERAL:
		compile_literal(mcb, result, self->data.term);
		return;
	default: break;
	}
	die(PANIC_FMT"failed to compile expr '%s'\n",
			PANIC_FMT_ARG,
			get_yz_expr_type_str(self->type));
}
