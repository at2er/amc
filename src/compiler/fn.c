/* This file is part of amc.
   SPDX-License-Identifier: GPL-3.0-or-later
*/
#include "fn.h"
#include "expr.h"
#include "literal.h"
#include "operand.h"
#include "panic.h"
#include <mcb/expr.h>
#include <mcb/mcb.h>
#include <mcb/operand.h>
#include <mcb/size.h>
#include <stdint.h>
#include <stdlib.h>

static struct mcb_expr_operand *build_func_args(
		const struct yz_func_call *self);
static void get_arg_operands(const struct yz_func *self,
		struct mcb_operand **result);

struct mcb_expr_operand *build_func_args(const struct yz_func_call *self)
{
	struct mcb_expr_operand *result = calloc(self->argc, sizeof(*result));
	for (uint16_t i = 0; i < self->argc; i++)
		build_mcb_expr_operand(&result[i], self->args[i]);
	return result;
}

void get_arg_operands(const struct yz_func *self, struct mcb_operand **result)
{
	for (int i = 0; i < self->argc; i++) {
		result[i] = &self->args[i]->operand;
		result[i]->size = get_size(&self->args[i]->type);
	}
}

void build_mcb_expr_func_call_operand(struct mcb_expr_operand *result,
		const struct yz_func_call *self)
{
	result->inner.func_call = calloc(1, sizeof(*result->inner.func_call));
	result->inner.func_call->argc = self->argc;
	result->inner.func_call->args = build_func_args(self);
	result->inner.func_call->name = self->callee->path.s;
	result->type = MCB_EXPR_OPERAND_IS_FUNC_CALL;
	result->size = get_size(&self->callee->type);
}

void compile_func_call(struct mcb_context *mcb,
		struct mcb_operand *result,
		const struct yz_func_call *self)
{
	struct mcb_operand args[self->argc];
	for (uint16_t i = 0; i < self->argc; i++) {
		if (MCB_CALL(mcb, func_call_arg_prepare)(mcb, i, &args[i],
					get_size(&self->callee->args[i]->type)))
			PANIC_MCB_CALL;
		compile_literal(mcb, &args[i], self->args[i]);
	}
	if (MCB_CALL(mcb, func_call)(mcb, self->argc, args,
				self->callee->path.s))
		PANIC_MCB_CALL;
	for (uint16_t i = 0; i < self->argc; i++) {
		if (MCB_CALL(mcb, drop_reg)(mcb, &args[i]))
			PANIC_MCB_CALL;
	}
}

void compile_func_def(struct mcb_context *mcb, const struct yz_func *self)
{
	struct mcb_operand *args[self->argc + 1];
	args[self->argc] = NULL;
	get_arg_operands(self, args);
	if (MCB_CALL(mcb, enter_func)(mcb, 0, args, MCB_I64, self->path.s))
		PANIC_MCB_CALL;
}

void compile_func_def_end(struct mcb_context *mcb)
{
	if (MCB_CALL(mcb, exit_func)(mcb))
		PANIC_MCB_CALL;
}
