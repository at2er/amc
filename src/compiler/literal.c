/* This file is part of amc.
   SPDX-License-Identifier: GPL-3.0-or-later
*/
#include "expr.h"
#include "fn.h"
#include "ident.h"
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

void compile_literal(struct mcb_context *mcb,
		struct mcb_operand *self,
		const struct yz_literal *src)
{
	struct mcb_operand imm;
	assert(self && src);
	if (yz_type_is_integer(src->type.type)) {
		get_imm_from_literal(&imm, src);
		if (MCB_CALL(mcb, gen_mov)(mcb, self, &imm))
			PANIC_MCB_CALL;
		return;
	} else if (src->type.type == YZ_EXPR) {
		compile_expr(mcb, self, src->data.expr);
		return;
	} else if (src->type.type == YZ_FUNC_CALL) {
		compile_func_call(mcb, self, src->data.func_call);
		return;
	} else if (src->type.type == YZ_IDENT_LITERAL) {
		compile_ident_literal(mcb, self, src->data.ident);
		return;
	}
	die(PANIC_FMT"failed to get operand data\n", PANIC_FMT_ARG);
}
