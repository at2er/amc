/* This file is part of amc.
   SPDX-License-Identifier: GPL-3.0-or-later
*/
#include "expr.h"
#include "let.h"
#include "operand.h"
#include "panic.h"
#include <mcb/mcb.h>

void compile_let(struct mcb_context *mcb,
		struct yz_ident *ident,
		const struct yz_expr *expr)
{
	struct mcb_operand result;
	enum MCB_SIZE size = get_size(expr->sum_type);
	if (MCB_CALL(mcb, alloc_var)(mcb, &result, NULL, size))
		PANIC_MCB_CALL;
	compile_expr(mcb, &result, expr);
}
