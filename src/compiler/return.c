/* This file is part of amc.
   SPDX-License-Identifier: GPL-3.0-or-later
*/
#include "expr.h"
#include "operand.h"
#include "panic.h"
#include "return.h"
#include "../expr.h"
#include <mcb/mcb.h>

void compile_return(struct mcb_context *mcb, const struct yz_expr *expr)
{
	struct mcb_operand result;
	if (MCB_CALL(mcb, alloc_reg)(mcb, &result, NULL,
				get_size(expr->sum_type)))
		PANIC_MCB_CALL;
	compile_expr(mcb, &result, expr);
	print_yz_expr(expr, 0);
	if (MCB_CALL(mcb, func_ret)(mcb, &result))
		PANIC_MCB_CALL;
	if (MCB_CALL(mcb, drop_reg)(mcb, &result))
		PANIC_MCB_CALL;
}
