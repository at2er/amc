/* This file is part of amc.
   SPDX-License-Identifier: GPL-3.0-or-later
*/
#ifndef AMC_COMPILER_EXPR_H
#define AMC_COMPILER_EXPR_H
#include "../expr.h"
#include <mcb/mcb.h>
#include <mcb/oprand.h>

void compile_expr(struct mcb_context *mcb,
		struct mcb_oprand *container,
		const struct yz_expr *self);

#endif
