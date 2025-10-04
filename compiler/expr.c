/* This file is part of amc.
   SPDX-License-Identifier: GPL-3.0-or-later
*/
#include "compiler/expr.h"
#include "compiler/oprand.h"
#include "compiler/panic.h"
#include "literal.h"
#include "panic.h"
#include "type.h"
#include "../utils/die.h"
#include <assert.h>
#include <mcb/mcb.h>
#include <mcb/oprand.h>

static void compile_binary(struct mcb_context *mcb,
		struct mcb_oprand *container,
		const struct yz_expr *self,
		mcb_gen_calc_f func);
static void get_literal_oprand(struct mcb_context *mcb,
		struct mcb_oprand *self,
		const struct yz_literal *src);

void compile_binary(struct mcb_context *mcb,
		struct mcb_oprand *container,
		const struct yz_expr *self,
		mcb_gen_calc_f func)
{
	struct mcb_oprand lhs, rhs, tmp;
	int stored = 0;
	get_literal_oprand(mcb, &lhs, self->data.binary.lhs);
	if (self->data.binary.rhs->type.type == YZ_EXPR) {
		stored = 1;
		if (MCB_CALL(push)(mcb, &tmp, &lhs))
			PANIC_MCB_CALL;
	}
	get_literal_oprand(mcb, &rhs, self->data.binary.rhs);
	if (MCB_CALL(alloc_reg)(mcb, container, NULL))
		PANIC_MCB_CALL;
	if (stored) {
		// `container` in here is previous expression's result.
		if (MCB_CALL(alloc_reg)(mcb, &rhs, container))
			PANIC_MCB_CALL;
		if (MCB_CALL(pop)(mcb, container, &tmp))
			PANIC_MCB_CALL;
	}
	if (func(mcb, container, &lhs, &rhs))
		PANIC_MCB_CALL;
	if (MCB_CALL(drop_reg)(mcb, container))
		PANIC_MCB_CALL;
	if (stored && MCB_CALL(drop_reg)(mcb, &rhs))
		PANIC_MCB_CALL;
}

void get_literal_oprand(struct mcb_context *mcb,
		struct mcb_oprand *self,
		const struct yz_literal *src)
{
	assert(self && src);
	if (YZ_TYPE_IS_DIGIT(src->type.type)) {
		self->size = get_size(&src->type);
		self->data.sint = src->data.sint;
		self->type = MCB_IMM;
		return;
	} else if (src->type.type == YZ_EXPR) {
		compile_expr(mcb, self, src->data.expr);
		return;
	}
	die(PANIC_FMT"failed to get oprand data\n", PANIC_FMT_ARG);
}

void compile_expr(struct mcb_context *mcb,
		struct mcb_oprand *container,
		const struct yz_expr *self)
{
	assert(container && self);
	assert(self->sum_type);
	container->size = get_size(self->sum_type);
#define CASE_BINARY(TYPE, INST) \
	case TYPE: compile_binary(mcb, container, self, MCB_CALL(INST)); \
		   return
	switch (self->type) {
	case YZ_EXPR_TERM_LITERAL:
		get_literal_oprand(mcb, container, self->data.term);
		return;
	CASE_BINARY(YZ_EXPR_BINARY_ADD, gen_add);
	CASE_BINARY(YZ_EXPR_BINARY_SUB, gen_sub);
	default: break;
	}
#undef CASE
	die(PANIC_FMT"failed to compile expr '%d'\n",
			PANIC_FMT_ARG, self->type);
}
