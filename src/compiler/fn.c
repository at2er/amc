/* This file is part of amc.
   SPDX-License-Identifier: GPL-3.0-or-later
*/
#include "fn.h"
#include "literal.h"
#include "operand.h"
#include "panic.h"
#include <mcb/expr.h>
#include <mcb/inst/func.h>
#include <mcb/mcb.h>
#include <mcb/operand.h>
#include <mcb/size.h>
#include <stdint.h>
#include <stdlib.h>

static void get_arg_operands(const struct yz_func *self,
		struct mcb_operand **result);
static enum MCB_ENTER_FUNC_FLAGS get_flags(const struct yz_func *self);

void get_arg_operands(const struct yz_func *self, struct mcb_operand **result)
{
	for (int i = 0; i < self->argc; i++) {
		result[i] = &self->args[i]->operand;
		result[i]->size = get_size(&self->args[i]->type);
	}
}

enum MCB_ENTER_FUNC_FLAGS get_flags(const struct yz_func *self)
{
	enum MCB_ENTER_FUNC_FLAGS result = 0;
	switch (self->scope_of) {
	case YZ_SCOPE_PUBLIC:
		result |= MCB_ENTER_FUNC_FLAG_EXPORT;
		break;
	case YZ_SCOPE_PRIVATE:
		break;
	default:
		panicf("unsupport scope of '%d'", self->scope_of);
	}
	return result;
}

void compile_func_call(struct mcb_context *mcb,
		struct mcb_operand *result,
		const struct yz_func_call *self)
{
	struct mcb_operand args[self->argc];
	for (uint16_t i = 0; i < self->argc; i++) {
		if (MCB_CALL(mcb, prepare_func_call_arg, i, &args[i],
					get_size(&self->callee->args[i]->type)))
			PANIC_MCB_CALL;
		compile_literal(mcb, &args[i], self->args[i]);
	}
	if (MCB_CALL(mcb, call_func, self->argc, args,
				self->callee->path.s))
		PANIC_MCB_CALL;
	for (uint16_t i = 0; i < self->argc; i++) {
		if (MCB_CALL(mcb, drop_reg, &args[i]))
			PANIC_MCB_CALL;
	}
}

void compile_func_def(struct mcb_context *mcb, const struct yz_func *self)
{
	struct mcb_operand *args[self->argc + 1];
	enum MCB_ENTER_FUNC_FLAGS flags;
	args[self->argc] = NULL;
	get_arg_operands(self, args);
	flags = get_flags(self);
	if (MCB_CALL(mcb, enter_func, flags, args, MCB_I64, self->path.s))
		PANIC_MCB_CALL;
}

void compile_func_def_end(struct mcb_context *mcb)
{
	if (MCB_CALL_0(mcb, exit_func))
		PANIC_MCB_CALL;
}
