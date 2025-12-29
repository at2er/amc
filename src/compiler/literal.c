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
#include <mcb/operand.h>

void compile_literal(struct mcb_context *mcb,
		struct mcb_operand *self,
		const struct yz_literal *src)
{
	struct mcb_operand imm;
	assert(self && src);
	if (yz_type_is_integer(src->type.type)) {
		build_mcb_imm(&imm, src);
		if (MCB_CALL(mcb, mov, self, &imm))
			PANIC_MCB_CALL;
		return;
	}
	switch (src->type.type) {
	case YZ_EXPR:
		compile_expr(mcb, self, src->data.expr);
		return;
	case YZ_FUNC_CALL:
		compile_func_call(mcb, self, src->data.func_call);
		return;
	case YZ_IDENT_LITERAL:
		compile_ident_literal(mcb, self, src->data.ident);
		return;
	default: break;
	}
	panicf("failed to get operand data with type '%s'",
			type_get_str(src->type.type));
}
