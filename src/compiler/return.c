/* This file is part of amc.
   SPDX-License-Identifier: GPL-3.0-or-later
*/
#include "expr.h"
#include "operand.h"
#include "panic.h"
#include "return.h"
#include "../expr.h"
#include <mcb/mcb.h>

/*
 * compile_return(expr): ; return (9 + 1) + (7 + 8)
 *   alloc_reg %r0
 *   compile_expr(dst: %r0):
 *     compile_expr(dst: %r0) if lhs == expr:
 *       mov %r0, $9
 *       add %r0, $1
 *     compile_expr(dst: %r1) if rhs == expr:
 *       mov %r1, $7
 *       add %r1, $8
 *     add %r0, %r1
 *
 * compile_return(expr): ; return (8 / 4) * (1 + 2)
 *   alloc %r0
 *   compile_expr(dst: %r0) ; (8 / 4) * (func 1 2):
 *     compile_expr(dst: %r0) if lhs == expr: ; 8 / 4
 *       #gen_div(dst: %r0):
 *         mov %r0, $8
 *         alloc %r1
 *         mov %r1, $4
 *         div %r1
 *         drop %r1
 *     alloc %r1
 *     compile_expr(dst: %r1) if rhs == expr: ; 1 + 2
 *       #gen_add(dst: %r1):
 *         mov %r1, $1
 *         add %r1, $2
 *     mul %r1
 *     drop %r1
 *
 * compile_return(expr): ; return (8 / 4) * (8 / 2)
 *   alloc %r0
 *   compile_expr(dst: %r0) ; (8 / 4) * (func 1 2):
 *     compile_expr(dst: %r0) if lhs == expr: ; 8 / 4
 *       #gen_div(dst: %r0): 8 / 4 = 2
 *         mov %r0, $8
 *         alloc %r1
 *         mov %r1, $4
 *         div %r1
 *         drop %r1
 *     alloc %r1
 *     compile_expr(dst: %r1) if rhs == expr: ; 8 / 2
 *       #gen_div(dst: %r1): 8 / 2 = 4
 *         alloc[force] %r0:
 *           mov %r2, %r0
 *         mov %r1, $8
 *         alloc %r2
 *         mov %r3, $2
 *         div %r3
 *         drop %r3
 *         mov %r1, %r0
 *         .drop %r0:
 *           mov %r0, %r2
 *     mul %r1
 *     drop %r1
 */

void compile_return(struct mcb_context *mcb, const struct yz_expr *expr)
{
	struct mcb_operand result;
	if (MCB_CALL(mcb, alloc_reg, &result, NULL,
				get_size(expr->sum_type)))
		PANIC_MCB_CALL;
	print_yz_expr(expr, 0);
	compile_expr(mcb, &result, expr);
	if (MCB_CALL(mcb, func_ret, &result))
		PANIC_MCB_CALL;
	if (MCB_CALL(mcb, drop_reg, &result))
		PANIC_MCB_CALL;
}
