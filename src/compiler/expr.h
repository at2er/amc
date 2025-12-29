/* This file is part of amc.
   SPDX-License-Identifier: GPL-3.0-or-later
*/
#ifndef AMC_COMPILER_EXPR_H
#define AMC_COMPILER_EXPR_H
#include "../expr.h"
#include <mcb/mcb.h>
#include <mcb/operand.h>

/**
 * Before compile expression, you must alloc a result container
 * for `container`.
 */
void compile_expr(struct mcb_context *mcb,
		struct mcb_operand *result,
		const struct yz_expr *self);

#endif
