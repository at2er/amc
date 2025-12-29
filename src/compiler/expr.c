/* This file is part of amc.
   SPDX-License-Identifier: GPL-3.0-or-later
*/
#include "expr.h"
#include "fn.h"
#include "ident.h"
#include "literal.h"
#include "operand.h"
#include "panic.h"
#include "../literal.h"
#include "../type.h"
#include <assert.h>
#include <mcb/expr.h>
#include <mcb/mcb.h>
#include <mcb/inst/calc.h>
#include <mcb/operand.h>
#include <mcb/size.h>

static void alloc_reg_if_need(struct mcb_context *mcb,
		struct mcb_operand *container,
		const struct yz_expr *expr);
static void compile_binary(struct mcb_context *mcb,
		struct mcb_operand *result,
		const struct yz_expr *src,
		mcb_calc_f func);
static void compile_binary_operand(struct mcb_context *mcb,
		struct mcb_operand *result,
		const struct yz_literal *src);

void alloc_reg_if_need(struct mcb_context *mcb,
		struct mcb_operand *container,
		const struct yz_expr *expr)
{
	if (yz_type_is_integer(expr->data.binary.rhs->type.type))
		return;
	if (MCB_CALL(mcb, alloc_reg, container, NULL,
				get_size(expr->sum_type)))
		PANIC_MCB_CALL;
}

void compile_binary(struct mcb_context *mcb,
		struct mcb_operand *result,
		const struct yz_expr *src,
		mcb_calc_f func)
{
	struct mcb_operand lhs = {0}, rhs = {0};
	lhs = *result;
	compile_binary_operand(mcb, &lhs, src->data.binary.lhs);
	alloc_reg_if_need(mcb, &rhs, src);
	compile_binary_operand(mcb, &rhs, src->data.binary.rhs);
	if (func(mcb, result, &lhs, &rhs))
		PANIC_MCB_CALL;
}

void compile_binary_operand(struct mcb_context *mcb,
		struct mcb_operand *result,
		const struct yz_literal *src)
{
	if (yz_type_is_integer(src->type.type)) {
		build_mcb_imm(result, src);
		return;
	}
	switch (src->type.type) {
	case YZ_EXPR:
		compile_expr(mcb, result, src->data.expr);
		return;
	case YZ_FUNC_CALL:
		compile_func_call(mcb, result, src->data.func_call);
		return;
	case YZ_IDENT_LITERAL:
		compile_ident_literal(mcb, result, src->data.ident);
		return;
	default: break;
	}
	panicf("failed to get operand data with type '%s'",
			type_get_str(src->type.type));
}

void compile_expr(struct mcb_context *mcb,
		struct mcb_operand *result,
		const struct yz_expr *self)
{
	assert(result && self);

#define CASE_BINARY(TYPE, GENERATOR) \
	case TYPE: \
		compile_binary(mcb, result, self, MCB_FUNC(mcb, GENERATOR)); \
		return

	switch (self->type) {
	CASE_BINARY(YZ_EXPR_BINARY_ADD, add);
	CASE_BINARY(YZ_EXPR_BINARY_DIV, div);
	CASE_BINARY(YZ_EXPR_BINARY_MUL, mul);
	CASE_BINARY(YZ_EXPR_BINARY_SUB, sub);
	case YZ_EXPR_TERM_LITERAL:
		compile_literal(mcb, result, self->data.term);
		return;
	default: break;
	}
	panicf("failed to compile expr '%s'",
			get_yz_expr_type_str(self->type));
#undef CASE_BINARY
}
