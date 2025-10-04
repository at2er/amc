/* This file is part of amc.
   SPDX-License-Identifier: GPL-3.0-or-later
*/
#include "compiler/expr.h"
#include "compiler/panic.h"
#include "compiler/return.h"
#include "expr.h"
#include <assert.h>
#include <mcb/mcb.h>

void compile_return(struct mcb_context *mcb, const struct yz_expr *expr)
{
	struct mcb_oprand container;
	print_yz_expr(expr, 0);
	compile_expr(mcb, &container, expr);
	if (MCB_CALL(func_ret)(mcb, &container))
		PANIC_MCB_CALL;
}
